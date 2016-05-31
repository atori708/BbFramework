#include "CD3D11ResourceManager.h"
#include<cassert>
#include"Framework\Utility\D3d11Utility.h"

CD3D11ResourceManager::CD3D11ResourceManager()
{
}

CD3D11ResourceManager::~CD3D11ResourceManager()
{
	m_pTextureDatas.clear();
}

UINT CD3D11ResourceManager::LoadTexture( std::shared_ptr<ID3D11Device> pDevice, const wchar_t * pFileName)
{
	assert(pDevice);
	if (pFileName == nullptr) {
		return -1;
	}

	HRESULT hr;

	// SRV作成
	std::shared_ptr<s_TextureData> pTexData = std::make_shared<s_TextureData>();
	ID3D11ShaderResourceView* pSRV;
	hr = m_texLoader.Load(pDevice, pFileName, &pSRV);
	if (FAILED(hr)) {
		return -1;
	}
	pTexData->m_pSRV.reset(pSRV, D3DComDeleter());

	// サンプラーステート作成
	ID3D11SamplerState* pSamplerState;
	hr = m_texLoader.CreateSamplerState(pDevice, &pSamplerState);
	if (FAILED(hr)) {
		return -1;
	}
	pTexData->m_pSamplerState.reset(pSamplerState, D3DComDeleter());

	m_pTextureDatas.push_back(pTexData);

	return m_pTextureDatas.size() - 1;
}

const std::shared_ptr<ID3D11ShaderResourceView> CD3D11ResourceManager::GetShaderResourceView(UINT _index) const
{
	if (_index >= m_pTextureDatas.size())
		return nullptr;

	return m_pTextureDatas[_index]->m_pSRV;
}

const std::shared_ptr<ID3D11SamplerState> CD3D11ResourceManager::GetSamplerState(UINT _index) const
{
	if (_index >= m_pTextureDatas.size())
		return nullptr;

	return m_pTextureDatas[_index]->m_pSamplerState;
}
//
//UINT CD3D11ResourceManager::CreateSamplerState(D3D11_FILTER filter)
//{
//	assert(m_pD3dDevice);
//
//	HRESULT hr;
//	ID3D11SamplerState* pSamplerState;
//	hr = m_texLoader.CreateSamplerState(m_pD3dDevice, filter, &pSamplerState);
//	if (FAILED(hr)) {
//		return -1;
//	}
//
//	m_pSamplerStates.push_back(pSamplerState);
//
//	return m_pSamplerStates.size() - 1;
//}

const CD3D11ShaderManager & CD3D11ResourceManager::GetShaderManager()
{
	return m_shaderManager;
}

int CD3D11ResourceManager::CompileVertexShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t * pShaderFile, const char* pEntryPoint)
{
	assert(pDevice);
	if (pShaderFile == nullptr || pEntryPoint == nullptr) {
		return -1;
	}

	return m_shaderManager.CompileVS(pDevice, pShaderFile, pEntryPoint);
}

int CD3D11ResourceManager::CompilePixelShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t * pShaderFile, const char* pEntryPoint)
{
	assert(pDevice);
	if (pShaderFile == nullptr || pEntryPoint == nullptr) {
		return -1;
	}

	return m_shaderManager.CompilePS(pDevice, pShaderFile, pEntryPoint);
}
