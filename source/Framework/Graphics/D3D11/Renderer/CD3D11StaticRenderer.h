#pragma once
#include"Framework\Graphics\D3D11\Renderer\ID3D11Renderer.h"
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"
#include<vector>

class CD3D11ShaderManager;

/*!
 * @brief    静的なオブジェクトを描画する
 * @note     
 * @author   N.kaji
 * @date     2016/05/22
 */
class CD3D11StaticRenderer
	:public ID3D11Renderer
{
private:
	std::vector<std::shared_ptr<CD3D11DrawModel3D>> m_pDrawObjects;
	ID3D11Buffer* m_pConstantBuffers[2];	// changeWindowResizeとchangeEveryFrameの定数バッファまとめた

	std::shared_ptr<ID3D11RenderTargetView*>	m_ppRenderTargetViews;
	std::shared_ptr<ID3D11ShaderResourceView*>	m_ppShaderResourceViews;
	std::shared_ptr<ID3D11DepthStencilView*>	m_ppDepthStencilViews;

	std::vector<s_shaderLabel>	m_shaderLabels;

	//int m_backBufferRenderTargetIndex = 0;
	int m_useRenderTargetCount;	// このレンダラで使うレンダーターゲットの数(DepthStencilViewの数も同じである必要あり)
	
public:
	CD3D11StaticRenderer() = delete;
	CD3D11StaticRenderer(std::vector< std::shared_ptr<s_d3d11RenderTarget>> pRenderTargets, std::shared_ptr< ID3D11DepthStencilView> pDepthStencilView);
	~CD3D11StaticRenderer();

	int ResistDrawObject(std::shared_ptr<CDrawModelBase> pDrawObject)override;
	void SetChangeWindowResizeConstantBuffer(std::weak_ptr<ID3D11Buffer> pConstantBuffer);
	void SetChangeEveryFrameConstatBuffer(std::weak_ptr<ID3D11Buffer> pConstantBuffer);
	void Render(std::shared_ptr<ID3D11DeviceContext> pDeviceContext);
};

