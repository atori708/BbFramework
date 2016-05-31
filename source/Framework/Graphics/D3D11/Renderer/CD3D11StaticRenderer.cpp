#include "CD3D11StaticRenderer.h"
#include"Framework\Resource\Shader\CD3D11ShaderManager.h"
#include"Framework\Resource\CD3D11ResourceManager.h"

CD3D11StaticRenderer::CD3D11StaticRenderer(std::vector< std::shared_ptr<s_d3d11RenderTarget>> pRenderTargets, std::shared_ptr< ID3D11DepthStencilView> pDepthStencilView)
{
	// S---------- Graphics����󂯎���������_�[�^�[�Q�b�g��z��Ɋi�[����
	m_useRenderTargetCount = pRenderTargets.size();

	/// �Ǝ��Ŏg�������_�[�^�[�Q�b�g���Ȃ炱����

	m_ppRenderTargetViews.reset(new ID3D11RenderTargetView*[m_useRenderTargetCount], std::default_delete<ID3D11RenderTargetView*[]>());
	m_ppDepthStencilViews.reset(new ID3D11DepthStencilView*[m_useRenderTargetCount], std::default_delete<ID3D11DepthStencilView*[]>());
	//m_ppDepthStencilViews = new ID3D11DepthStencilView*[m_useRenderTargetCount];

	int i = 0;
	for (auto pRenderTarget : pRenderTargets) {
		m_ppRenderTargetViews.get()[i] = pRenderTarget->m_pRTV.get();
		++i;
	}

	m_ppDepthStencilViews.get()[0] = pDepthStencilView.get();
	// E---------- Graphics����󂯎���������_�[�^�[�Q�b�g��z��Ɋi�[����
}

CD3D11StaticRenderer::~CD3D11StaticRenderer()
{
}


int CD3D11StaticRenderer::ResistDrawObject(std::shared_ptr<CDrawModelBase> pDrawObject)
{
	if (pDrawObject == nullptr) {
		return -1;
	}

	m_pDrawObjects.push_back(std::dynamic_pointer_cast<CD3D11DrawModel3D>(pDrawObject));
	return m_pDrawObjects.size()-1;
}

void CD3D11StaticRenderer::SetChangeWindowResizeConstantBuffer(std::weak_ptr<ID3D11Buffer>  pConstantBuffer)
{
	if (!pConstantBuffer.expired()) {

		m_pConstantBuffers[0] = pConstantBuffer._Get();
	}
}

void CD3D11StaticRenderer::SetChangeEveryFrameConstatBuffer(std::weak_ptr<ID3D11Buffer>  pConstantBuffer)
{
	if (!pConstantBuffer.expired()) {
		m_pConstantBuffers[1] = pConstantBuffer._Get();
	}
}

void CD3D11StaticRenderer::Render(std::shared_ptr<ID3D11DeviceContext> pDeviceContext)
{
	pDeviceContext->OMSetRenderTargets(m_useRenderTargetCount, m_ppRenderTargetViews.get(), m_ppDepthStencilViews.get()[0]);
	pDeviceContext->VSSetConstantBuffers(0, 2, m_pConstantBuffers);

	for (auto drawModel : m_pDrawObjects) {
		// ���f�����ɍX�V�����萔�o�b�t�@�ݒ�
		drawModel->Update(pDeviceContext);
		const CD3D11ConstantBuffer& pCBuffer = drawModel->GetChangeEveryModelConstantBuffer();
		ID3D11Buffer* pBuffer[1] = { pCBuffer.GetConstantBuffer().get() };
		pDeviceContext->VSSetConstantBuffers(pCBuffer.GetStartSlot(), 1, pBuffer);

		// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�ݒ�
		pBuffer[0] = drawModel->GetVertexBuffer().get();
		pDeviceContext->IASetVertexBuffers(0, 1, pBuffer, drawModel->GetVertexStrides().get(), drawModel->GetVertexOffsets().get());
		pDeviceContext->IASetPrimitiveTopology(drawModel->GetPrimitiveTopology());
		pDeviceContext->IASetIndexBuffer(drawModel->GetIndexBuffer().get(), drawModel->GetIndexFormat(), 0);

		// �}�e���A�����ɕ`��
		for (int i = 0; i < drawModel->GetNumMaterial(); ++i) {
			const CD3D11ShaderManager& shaderManager = D3D11ResourceManager.GetShaderManager();
			pDeviceContext->IASetInputLayout(shaderManager.GetInputLayout(drawModel->GetInputLayoutIndex(i, 0)).get());
			pDeviceContext->VSSetShader(shaderManager.GetVertexShader(drawModel->GetVertexShaderIndex(i, 0)).get(), nullptr, 0);
			pDeviceContext->PSSetShader(shaderManager.GetPixelShader(drawModel->GetPixelShaderIndex(i, 0)).get(), nullptr, 0);
			ID3D11ShaderResourceView* pSRVs[1] = { D3D11ResourceManager.GetShaderResourceView(drawModel->GetShaderResourceViewIndex(i, 0)).get() };
			ID3D11SamplerState*	pSamplerStates[1] = { D3D11ResourceManager.GetSamplerState(drawModel->GetSamplerStateIndex(i, 0)).get() };
			pDeviceContext->PSSetShaderResources(0, 1, pSRVs);
			pDeviceContext->PSSetSamplers(0, 1, pSamplerStates);
			pDeviceContext->DrawIndexed(drawModel->GetIndexCounts(i), drawModel->GetIndexStartLocation(i), 0);
		}
	}
}
