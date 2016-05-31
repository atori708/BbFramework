#include<d3d11.h>

HRESULT	GetD3D11Texture2DDescFromTexture( ID3D11Texture2D* _pTexture, D3D11_TEXTURE2D_DESC* _pOut);
HRESULT	GetD3D11Texture2DDescFromSRV( ID3D11ShaderResourceView* _pSRV, D3D11_TEXTURE2D_DESC* _pOut);
HRESULT	GetD3D11Texture2DDescFromRTV( ID3D11RenderTargetView* _pRTV, D3D11_TEXTURE2D_DESC* _pOut);

bool CompareD3D11InputElementDesc( D3D11_INPUT_ELEMENT_DESC* _pElementDescA, D3D11_INPUT_ELEMENT_DESC* _pElementDescB);

DXGI_FORMAT	GetD3D11DepthStencilSRVFormat(DXGI_FORMAT depthStencilFormat);
DXGI_FORMAT	GetD3D11DepthStencilTextureFormat(DXGI_FORMAT depthStencilFormat);

struct D3DComDeleter
{
	void operator()(IUnknown* p)
	{
		p->Release();
	}
};