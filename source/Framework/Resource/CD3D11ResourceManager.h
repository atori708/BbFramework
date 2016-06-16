#pragma once

#include<memory>
#include<vector>
#include"Framework\Resource\Texture\CD3D11TextureLoader.h"
#include"Framework\Resource\Shader\CD3D11ShaderManager.h"

// �e�N�X�`���\����
struct s_TextureData
{
	std::wstring m_fileName;
	std::shared_ptr<ID3D11Resource>				m_pTexture;
	std::shared_ptr<ID3D11ShaderResourceView>	m_pSRV;
	std::shared_ptr<ID3D11SamplerState>			m_pSamplerState;

	s_TextureData()
	{
		m_pTexture = nullptr;
		m_pSRV = nullptr;
		m_pSamplerState = nullptr;
	}
};

class CD3D11ResourceManager
{
private:
	CD3D11ResourceManager();
	~CD3D11ResourceManager();

	// �e�N�X�`���֘A
	CD3D11TextureLoader	m_texLoader;
	std::vector<std::shared_ptr<s_TextureData>>			m_pTextureDatas;
	//std::vector<std::shared_ptr<ID3D11SamplerState>>	m_pSamplerStates;

	// �V�F�[�_
	CD3D11ShaderManager	m_shaderManager;

public:
	static CD3D11ResourceManager& GetInstance(){
		static CD3D11ResourceManager instance;
		return instance;
	}
	CD3D11ResourceManager(const CD3D11ResourceManager&) = delete;
	CD3D11ResourceManager& operator=(const CD3D11ResourceManager&) = delete;
	CD3D11ResourceManager(CD3D11ResourceManager&&) = delete;
	CD3D11ResourceManager& operator=(CD3D11ResourceManager&&) = delete;

	// �e�N�X�`���֘A
	UINT	LoadTexture(std::shared_ptr<ID3D11Device> pDevice, std::wstring filePath, const wchar_t* pFileName);
	const std::shared_ptr<ID3D11ShaderResourceView>	GetShaderResourceView( UINT _index)const;
	const std::shared_ptr<ID3D11SamplerState>	GetSamplerState( UINT _index)const;

	// �V�F�[�_
	const CD3D11ShaderManager& GetShaderManager();
	int CompileVertexShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pShaderFile, const char* pEntryPoint);
	int CompilePixelShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pShaderFile, const char* pEntryPoint);


};

#define D3D11ResourceManager (CD3D11ResourceManager::GetInstance())

