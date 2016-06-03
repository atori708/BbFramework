#pragma once
#include"../CGraphicsBase.h"
#include"Framework\Graphics\D3D11\Renderer\ID3D11Renderer.h"
#include"Framework\Game\Object\CGameObject3D.h"
#include"Framework\Resource\Shader\CD3D11ShaderManager.h"
#include"Framework\Resource\Model\D3D11\CD3D11ConstantBuffer.h"
#include"Framework\Utility\D3d11Utility.h"
#include<vector>
#include<memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment( lib, "dxgi.lib")

class CWindowsApplication;

class CD3D11Graphics :
	public CGraphicsBase
{
private:
	std::shared_ptr< ID3D11Device> m_pDevice;
	std::shared_ptr<ID3D11DeviceContext>  m_pImmediateContext;
	std::shared_ptr<IDXGISwapChain>	m_pSwapChain;	// dxgiだから分離したほうがいいかも？

	// デバッグ用
	std::unique_ptr<ID3D11Debug, D3DComDeleter> m_pD3D11Debug;

	// ステートオブジェクト
	std::shared_ptr<ID3D11RasterizerState>	m_pRasterizerState;
	std::shared_ptr<ID3D11DepthStencilState> m_pDepthStencilState;
	std::shared_ptr<ID3D11BlendState> m_pBlendState;

	// レンダーターゲット(バックバッファなどよく使うものだけ)
	// TODO 独自のレンダーターゲットはレンダラ毎に持つ？
	// TODO レンダーターゲットを管理するクラス作ったほうがいいかも
	int m_backBufferRenderTargetIndex = 0;
	std::vector<std::shared_ptr<s_d3d11RenderTarget>> m_pRenderTargets;
	std::shared_ptr<ID3D11DepthStencilView> m_pDepthStencilView;
	std::shared_ptr<ID3D11ShaderResourceView> m_pDepthStencilSRV;

	const float m_backBufferClearColor[4] = { 1.0f, 0.0f, 1.0f, 0.0f };

	// MSAA使うかと、クオリティ(変更するならスワップチェインとデプスステンシルバッファ、全てのレンダーターゲット作り直し)
	//bool m_isMSAA;
	//int m_msaaQuality;

	// レンダラ
	std::vector<std::shared_ptr<ID3D11Renderer>>	m_pRenderers;
	int m_staticRendererIndex;


	// 定数バッファ
	s_cbChangeWindowResizing	m_pChangeWindowResizeConstantBufferStruct;
	s_cbChangeEveryFrame		m_pChangeEveryFrameConstantBufferStruct;
	CD3D11ConstantBuffer		m_changeWindowResizeConstantBuffer;
	CD3D11ConstantBuffer		m_changeEveryFrameConstantBuffer;

public:
	CD3D11Graphics();
	~CD3D11Graphics();

	bool Initialize(const CApplicationBase* application)override;
	void Finalize()override;

	// レンダーターゲット作成
	int	CreateRenderTarget(const RECT_SIZE* pSize, DXGI_FORMAT format, bool isWithSRV = true);
	int	CreateRenderTarget(ID3D11Texture2D* pTexture, bool isWithSRV);	// テクスチャを元に作る

	bool CreateDepthStencilView(RECT_SIZE* pSize, DXGI_FORMAT format, bool isWithSRV);

	// レンダラ登録
	int RegistRenderer(std::shared_ptr<ID3D11Renderer>);

	// レンダラにDrawObject登録
	bool RegistObject2StaticRenerer(std::shared_ptr<CGameObject3D> pDrawObject);

	// シェーダをレンダラに登録
	int ResistShaderToStaticRenderer(D3D11_SHADER_TYPE shaderType, int shaderIndex);
	int ResistShaderToStaticRenderer(D3D11_SHADER_TYPE shaderType, const wchar_t* pShaderFile, const char* pEntryPoint);

	bool Update(const DirectX::XMMATRIX& pProjectionMatrix, const s_cbChangeEveryFrame& changeEveryFrame);
	bool Render();

	std::shared_ptr<ID3D11Device>	GetD3D11Device();

private:
	bool	CreateDevice();
	bool	CreateSwapChain(const CWindowsApplication& pApplication);

	bool	CreateDepthStencilState();
	bool	CreateRasterizerState();
	bool	CreateBlendState();
};

