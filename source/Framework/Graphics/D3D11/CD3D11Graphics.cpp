#include "CD3D11Graphics.h"
#include"Framework\Application\CWindowsApplication.h"
#include"Framework\Graphics\D3D11\Renderer\CD3D11StaticRenderer.h"

CD3D11Graphics::CD3D11Graphics()
{
}

CD3D11Graphics::~CD3D11Graphics()
{
	this->Finalize();
}

/*!
 * @brief    初期化z
 * @param    pApplication	グラフィックの出力先になるアプリ
 * @return   成功かどうか
 * @note     
 * @author   N.kaji
 * @date     2016/05/21
 */
bool CD3D11Graphics::Initialize(const CApplicationBase* application)
{
	if (!application) {
		return false;
	}

	CWindowsApplication& windowAppliaction = (CWindowsApplication&)*application;

	if (!this->CreateDevice())						return false;
	if (!this->CreateSwapChain(windowAppliaction)) 	return false;
	if (!this->CreateRasterizerState())		return false;
	if (!this->CreateDepthStencilState())	return false;
	if (!this->CreateBlendState())			return false;

	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargets[0]->m_pRTV.get(), m_backBufferClearColor);
//	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_pSwapChain->Present(0, 0);

	// ビュー行列とプロジェクション行列の初期値を入れておく
	using namespace DirectX;
	const RECT_SIZE& pSize = windowAppliaction.GetClientSize();
	XMMATRIX viewMatrix = XMMatrixLookAtLH(XMVectorSet(0, 2, -100, 0), XMVectorSet(0, 0, 0, 0), XMVectorSet(0, 1, 0, 0));
	XMMATRIX projectionMatrix = XMMatrixOrthographicLH(static_cast<float>(pSize.width), static_cast<float>(pSize.height), 0.1f, 200.0f);
	XMStoreFloat4x4(&m_pChangeWindowResizeConstantBufferStruct.m_matProjection, projectionMatrix);
	m_changeWindowResizeConstantBuffer.CreateConstantBuffer(m_pDevice, 0, sizeof(m_pChangeWindowResizeConstantBufferStruct));
//	m_changeWindowResizeConstantBuffer.Update(m_pImmediateContext, m_pChangeWindowResizeConstantBufferStruct);

	XMStoreFloat4x4(&m_pChangeEveryFrameConstantBufferStruct.m_matView, viewMatrix);
	m_changeEveryFrameConstantBuffer.CreateConstantBuffer(m_pDevice, 1, sizeof(m_pChangeEveryFrameConstantBufferStruct));
//	m_changeEveryFrameConstantBuffer.Update(m_pImmediateContext, m_pChangeEveryFrameConstantBufferStruct);

	// レンダラを登録
	std::shared_ptr<CD3D11StaticRenderer>	pRenderer = std::make_shared<CD3D11StaticRenderer>(m_pRenderTargets, m_pDepthStencilView);
	pRenderer->SetChangeWindowResizeConstantBuffer(m_changeWindowResizeConstantBuffer.GetConstantBuffer());
	pRenderer->SetChangeEveryFrameConstatBuffer(m_changeEveryFrameConstantBuffer.GetConstantBuffer());
	m_staticRendererIndex = this->RegistRenderer(pRenderer);

	// ステートとビューポートセットしとく
	D3D11_VIEWPORT	viewPort;
	viewPort.Width = static_cast<float>(pSize.width);
	viewPort.Height = static_cast<float>(pSize.height);
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	float blendFactor[4] = { 0,0,0,0 };
	m_pImmediateContext->RSSetViewports(1, &viewPort);
	m_pImmediateContext->RSSetState(m_pRasterizerState.get());
	m_pImmediateContext->OMSetDepthStencilState(m_pDepthStencilState.get(), 0);
	m_pImmediateContext->OMSetBlendState(m_pBlendState.get(), blendFactor, 0xffffffff);
	return true;
}

/*!
 * @brief    終了・片付け
 * @param    
 * @return   
 * @note     
 * @author   N.kaji
 * @date     2016/05/21
 */
void CD3D11Graphics::Finalize()
{
	m_changeEveryFrameConstantBuffer.DestroyConstantBuffer();
	m_changeWindowResizeConstantBuffer.DestroyConstantBuffer();
	//m_shaderManager.DestroyAll();

#ifdef _DEBUG
	if (m_pD3D11Debug) {
		m_pD3D11Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
#endif
	m_pD3D11Debug.reset();	// ID3D11Deviceより先に解放しておく
}

/*!
* @brief    テクスチャを元にレンダーターゲットの作成
* @param    pSize	レンダーターゲットのサイズ
* @param    format	フォーマット
* @param    isWitchSRV	SRVも一緒に作るかどうか
* @return   配列の添字  失敗なら-1
* @note     RTV(RenderTargetView)とSRV(ShaderResourceView)を作れます
* @author   N.kaji
* @date     2016/05/21
*/
int CD3D11Graphics::CreateRenderTarget(const RECT_SIZE* pSize, DXGI_FORMAT format, bool isWithSRV)
{
	if (m_pDevice == nullptr || pSize == nullptr)
		return -1;

	UINT bindFlag;

	if (isWithSRV) {
		bindFlag = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	} else {
		bindFlag = D3D11_BIND_RENDER_TARGET;
	}

	// レンダーターゲットのテクスチャ作成
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = pSize->width;
	textureDesc.Height = pSize->height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 2;
	textureDesc.SampleDesc.Quality = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = bindFlag;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* pTexture2D;
	HRESULT hr = m_pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture2D);
	if (FAILED(hr)) {
		return -1;
	}

	this->CreateRenderTarget(pTexture2D, isWithSRV);
	SafeRelease(pTexture2D);

	return this->CreateRenderTarget(pTexture2D, isWithSRV);
}

/*!
 * @brief    テクスチャを元にレンダーターゲットの作成
 * @param    pTexture2D	元になるテクスチャ
 * @param    isWitchSRV	SRVも一緒に作るかどうか
 * @return   配列の添字  失敗なら-1
 * @note     RTV(RenderTargetView)とSRV(ShaderResourceView)を作れます 元のpTexture2Dを解放してないので注意
 * @author   N.kaji
 * @date     2016/05/21
 */
int CD3D11Graphics::CreateRenderTarget(ID3D11Texture2D* pTexture2D, bool isWithSRV)
{
	if (m_pDevice == nullptr || pTexture2D == nullptr) {
		return -1;
	}

	// RTV作成
	std::shared_ptr<s_d3d11RenderTarget> pRenderTarget(new s_d3d11RenderTarget);
	ID3D11RenderTargetView* pRenderTargetView;
	
	HRESULT hr = m_pDevice->CreateRenderTargetView(pTexture2D, nullptr, &pRenderTargetView);
	if (FAILED(hr)) {
		return -1;
	}
	pRenderTarget->m_pRTV.reset(pRenderTargetView, D3DComDeleter());

	// SRV作成
	if (isWithSRV) {
		ID3D11ShaderResourceView* pShaderResourceView;
		hr = m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pShaderResourceView);
		if (FAILED(hr)) {
			return -1;
		}
		pRenderTarget->m_pSRV.reset(pShaderResourceView, D3DComDeleter());
	}

	m_pRenderTargets.push_back(pRenderTarget);
	return m_pRenderTargets.size() - 1;
}

/*!
 * @brief    デバイスの作成
 * @param    
 * @return   
 * @note     
 * @author   N.kaji
 * @date     2016/05/21
 */
bool CD3D11Graphics::CreateDevice()
{
	HRESULT hr;
	D3D11_CREATE_DEVICE_FLAG createDeviceFlags = (D3D11_CREATE_DEVICE_FLAG)0;
	D3D_DRIVER_TYPE driverType;

#ifdef _DEBUG
	createDeviceFlags = (D3D11_CREATE_DEVICE_FLAG)(createDeviceFlags | D3D11_CREATE_DEVICE_DEBUG);
	//driverType = D3D_DRIVER_TYPE_REFERENCE;	// めちゃおもい
#endif // _DEBUG

	driverType = D3D_DRIVER_TYPE_HARDWARE;

	// デバイス作成
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	hr = D3D11CreateDevice(nullptr,
		driverType,
		nullptr,
		createDeviceFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&pDevice,
		//&m_featureLevel,
		nullptr,
		&pDeviceContext);

	if (FAILED(hr)) {
		return false;
	}
	m_pDevice.reset(pDevice, D3DComDeleter());
	m_pImmediateContext.reset(pDeviceContext, D3DComDeleter());

	// デバッグ
#ifdef _DEBUG
	ID3D11Debug* pDebug;
	hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug));
	if (FAILED(hr)) {
		this->Finalize();
		return false;
	}
	std::unique_ptr<ID3D11Debug, D3DComDeleter> _pDebug(pDebug, D3DComDeleter());
	m_pD3D11Debug = std::move(_pDebug);
	//m_pD3dDevice->Release();// QueryInterface()でAddRefされてるので→いらないっぽい
#endif

	// マルチスレッドをサポートしているかチェック
	//D3D11_FEATURE_DATA_THREADING featureThread;
	//hr = m_pD3dDevice->CheckFeatureSupport( D3D11_FEATURE_THREADING, &featureThread, sizeof( featureThread));

	//if( featureThread.DriverConcurrentCreates )

	return true;
}

/*!
 * @brief    スワップチェインの作成
 * @param    pApplication　グラフィックの出力先になるアプリ
 * @return   
 * @note     バックバッファのレンダーターゲットとデプスステンシルバッファもついでに作ってる
 * @author   N.kaji
 * @date     2016/05/21
 */
bool CD3D11Graphics::CreateSwapChain(const CWindowsApplication& application)
{
	HRESULT hr;
	IDXGIFactory *pFactory;
	//CWindowsApplication* pWindowsApplication = (CWindowsApplication*)pApplication;

	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory);
	//if (FAILED(CheckHRESULT(DEBUG_LOG_TYPE_CREATE, hr, L"DXGI Factory")))
	//	return E_FAIL;
	if (FAILED(hr)) {
		return false;
	}

	m_clientSize = &application.GetClientSize();

	// スワップチェイン作成
	IDXGISwapChain* pSwapChain;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = m_clientSize->width;
	swapChainDesc.BufferDesc.Height = m_clientSize->height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.OutputWindow = application.GetHWnd();
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.Windowed = !application.IsFullscreen();

	hr = pFactory->CreateSwapChain(m_pDevice.get(), &swapChainDesc, &pSwapChain);
	SafeRelease(pFactory);

	//if (FAILED(CheckHRESULT(DEBUG_LOG_TYPE_CREATE, hr, L"Swap chain")))
	//	return false;
	if (FAILED(hr)) {
		return false;
	}
	m_pSwapChain.reset(pSwapChain, D3DComDeleter());

	// バックバッファのレンダーターゲットビュー作成
	ID3D11Texture2D* pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
	m_backBufferRenderTargetIndex = this->CreateRenderTarget(pBackBuffer, false);

	if (m_backBufferRenderTargetIndex == -1) {
		SafeRelease(pBackBuffer);
		return false;
	}

	// デプスステンシルバッファの作成
	RECT_SIZE depthStencilSize;
	D3D11_TEXTURE2D_DESC backBufferTextureDesc;

	GetD3D11Texture2DDescFromTexture(pBackBuffer, &backBufferTextureDesc);
	depthStencilSize.width = backBufferTextureDesc.Width;
	depthStencilSize.height = backBufferTextureDesc.Height;
	bool result = this->CreateDepthStencilView(&depthStencilSize, DXGI_FORMAT_D32_FLOAT_S8X24_UINT, true);
	SafeRelease(pBackBuffer);

	return true;
}

/*!
 * @brief    デプスステンシルステート作成
 * @param    
 * @return   成功かどうか
 * @note     
 * @author   N.kaji
 * @date     2016/05/21
 */
bool CD3D11Graphics::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = FALSE;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	//dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	ID3D11DepthStencilState* pDepthStencilState;
	HRESULT hr = m_pDevice->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
	if (FAILED(hr)) {
		return false;
	}
	m_pDepthStencilState.reset(pDepthStencilState, D3DComDeleter());

	return true;
}

/*!
* @brief    ラスタライザステート作成
* @param
* @return   成功かどうか
* @note
* @author   N.kaji
* @date     2016/05/21
*/
bool CD3D11Graphics::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rsDesc;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = FALSE;
	rsDesc.DepthBias = 0;
	rsDesc.DepthBiasClamp = 0;
	rsDesc.SlopeScaledDepthBias = 0;
	rsDesc.DepthClipEnable = TRUE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.MultisampleEnable = FALSE;
	rsDesc.AntialiasedLineEnable = FALSE;

	ID3D11RasterizerState* pRasterizerState;
	HRESULT hr = m_pDevice->CreateRasterizerState(&rsDesc, &pRasterizerState);
	if (FAILED(hr)) {
		return false;
	}
	m_pRasterizerState.reset(pRasterizerState, D3DComDeleter());

	return true;
}

/*!
* @brief    ブレンドステート作成
* @param
* @return   成功かどうか
* @note
* @author   N.kaji
* @date     2016/05/21
*/
bool CD3D11Graphics::CreateBlendState()
{
	D3D11_BLEND_DESC blendDesc;
	float blendFactor[4] = { 0,0,0,0 };
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;

	ID3D11BlendState* pBlendState;
	HRESULT hr = m_pDevice->CreateBlendState(&blendDesc, &pBlendState);
	if (FAILED(hr)) {
		return false;
	}
	m_pBlendState.reset(pBlendState, D3DComDeleter());

	return true;
}

/*!
 * @brief    デプスステンシルバッファの作成
 * @param    pSize	サイズ
 * @param    format	フォーマット(DXGI_FORMAT_Dxxのものを指定する)
 * @param    isWithSRV	SRVも作るか
 * @return   成功かどうか
 * @note     
 * @author   N.kaji
 * @date     2016/05/21
 */
bool CD3D11Graphics::CreateDepthStencilView(RECT_SIZE* pSize, DXGI_FORMAT format, bool isWithSRV)
{
	if (m_pDevice == nullptr || pSize == nullptr) {
		return false;
	}

	HRESULT hr;
	UINT bindFlag;

	if (isWithSRV) {
		bindFlag = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	} else {
		bindFlag = D3D11_BIND_DEPTH_STENCIL;
	}

	// デプスステンシル用のテクスチャ作成
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc;
	ID3D11Texture2D* pDepthStencilTexture;
	ZeroMemory(&depthStencilTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilTextureDesc.Width = pSize->width;
	depthStencilTextureDesc.Height = pSize->height;
	depthStencilTextureDesc.Format = GetD3D11DepthStencilTextureFormat(format);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 1;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = bindFlag;
	depthStencilTextureDesc.CPUAccessFlags = 0;
	depthStencilTextureDesc.MiscFlags = 0;

	hr = m_pDevice->CreateTexture2D(&depthStencilTextureDesc, nullptr, &pDepthStencilTexture);
	if (FAILED(hr)) {
		return false;
	}

	// デプスステンシルバッファ作成
	ID3D11DepthStencilView* pDepthStencilView;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	m_pDevice->CreateDepthStencilView(pDepthStencilTexture, &dsvDesc, &pDepthStencilView);
	if (FAILED(hr)) {
		SafeRelease(pDepthStencilTexture);
		return false;
	}
	m_pDepthStencilView.reset(pDepthStencilView, D3DComDeleter());

	// 必要ならSRVも作成する
	if (isWithSRV) {
		ID3D11ShaderResourceView* pSRV;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = GetD3D11DepthStencilSRVFormat(format);
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = depthStencilTextureDesc.MipLevels;

		m_pDevice->CreateShaderResourceView(pDepthStencilTexture, &srvDesc, &pSRV);
		if (FAILED(hr)) {
			SafeRelease(pDepthStencilTexture);
			return false;
		}
		m_pDepthStencilSRV.reset(pSRV, D3DComDeleter());

	}

	SafeRelease(pDepthStencilTexture);

	return true;
}

int CD3D11Graphics::RegistRenderer(std::shared_ptr<ID3D11Renderer> pRenderer)
{
	if (pRenderer == nullptr) {
		return -1;
	}

	m_pRenderers.push_back(pRenderer);
	return m_pRenderers.size() - 1;
}

bool CD3D11Graphics::RegistObject2StaticRenerer(std::shared_ptr<CGameObject3D> pDrawObject)
{
	std::shared_ptr<CD3D11DrawModel3D> pObject = std::dynamic_pointer_cast<CD3D11DrawModel3D>(pDrawObject->GetDrawObject());
	pObject->SetWorldMatrix(m_pDevice, pDrawObject->GetWorldMatrix());
	m_pRenderers[m_staticRendererIndex]->ResistDrawObject(pObject);
	return true;
}


int CD3D11Graphics::ResistShaderToStaticRenderer(D3D11_SHADER_TYPE shaderType, int shaderIndex)
{
	return 0;
}

int CD3D11Graphics::ResistShaderToStaticRenderer(D3D11_SHADER_TYPE shaderType, const wchar_t * pShaderFile, const char* pEntryPoint)
{
	if (!pShaderFile) {
		return -1;
	}

	switch (shaderType) {
	case D3D11_SHADER_TYPE::VERTEX_SHADER:
		//m_shaderManager.GetVertexShader(pShaderFile, pEntryPoint);
		break;
	}
	return 0;
}


/*!
 * @brief    定数バッファなどを更新する
 * @param    
 * @return   ret
 * @note     行列がnullだと対応してる定数バッファは更新しない
 * @author   N.kaji
 * @date     2016/05/22
 */
 bool CD3D11Graphics::Update(const DirectX::XMMATRIX& pProjectionMatrix, const s_cbChangeEveryFrame& changeEveryFrame)
{
	using namespace DirectX;
	XMStoreFloat4x4(&m_pChangeWindowResizeConstantBufferStruct.m_matProjection, pProjectionMatrix);
	m_changeWindowResizeConstantBuffer.Update(m_pImmediateContext, m_pChangeWindowResizeConstantBufferStruct);

	m_pChangeEveryFrameConstantBufferStruct = changeEveryFrame;
	m_changeEveryFrameConstantBuffer.Update(m_pImmediateContext, m_pChangeEveryFrameConstantBufferStruct);

	//m_pRenderers[0]->SetChangeEveryFrameConstatBuffer(m_changeEveryFrameConstantBuffer.GetConstantBuffer());
	//m_pRenderer->SetChangeWindowResizeConstantBuffer(m_changeWindowResizeConstantBuffer.GetConstantBuffer());
	return false;
}

bool CD3D11Graphics::Render()
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargets[0]->m_pRTV.get(), m_backBufferClearColor);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for each (std::shared_ptr<ID3D11Renderer> pRenderer  in m_pRenderers){
		pRenderer->Render(m_pImmediateContext);
	}

	m_pSwapChain->Present(0, 0);
	return false;
}

std::shared_ptr<ID3D11Device> CD3D11Graphics::GetD3D11Device()
 {
	 return m_pDevice;
 }

