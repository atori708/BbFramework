#pragma once

#include<memory>
#include<vector>
#include"Framework\Resource\Texture\CD3D11TextureLoader.h"
#include"Framework\Resource\Shader\CD3D11ShaderManager.h"

// テクスチャ構造体
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

	// テクスチャ関連
	CD3D11TextureLoader	m_texLoader;
	std::vector<std::shared_ptr<s_TextureData>>			m_pTextureDatas;
	//std::vector<std::shared_ptr<ID3D11SamplerState>>	m_pSamplerStates;

	// シェーダ
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

	// テクスチャ関連
	UINT	LoadTexture(std::shared_ptr<ID3D11Device> pDevice, std::wstring filePath, const wchar_t* pFileName);
	const std::shared_ptr<ID3D11ShaderResourceView>	GetShaderResourceView( UINT _index)const;
	const std::shared_ptr<ID3D11SamplerState>	GetSamplerState( UINT _index)const;

	// シェーダ
	const CD3D11ShaderManager& GetShaderManager();
	int CompileVertexShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pShaderFile, const char* pEntryPoint);
	int CompilePixelShader(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pShaderFile, const char* pEntryPoint);


};

#define D3D11ResourceManager (CD3D11ResourceManager::GetInstance())

