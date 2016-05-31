#include "CD3D11TextureLoader.h"
#include<cassert>

CD3D11TextureLoader::CD3D11TextureLoader()
{
}
// TODO シェーダもリソースマネージャに

CD3D11TextureLoader::~CD3D11TextureLoader()
{
}

HRESULT CD3D11TextureLoader::Load(std::shared_ptr<ID3D11Device> pD3dDevice, std::wstring filePath, ID3D11ShaderResourceView ** ppOut)
{
	assert(pD3dDevice);

	if (filePath.empty()) {
		return COMADMIN_E_BADPATH;
	}
	if (ppOut == nullptr) {
		return E_FAIL;
	}

	HRESULT hr;
	std::wstring fileExtension = filePath.substr(filePath.find_last_of(L".") + 1);
	DirectX::TexMetadata metaData;
	DirectX::ScratchImage scratchImage;

	if (fileExtension == L"tga") {
		if (FAILED(DirectX::LoadFromTGAFile(filePath.c_str(), &metaData, scratchImage))) {
			return E_FAIL;
		}
	} else if (fileExtension == L"dds") {
		if (FAILED(DirectX::LoadFromDDSFile(filePath.c_str(), 0, &metaData, scratchImage))) {
			return E_FAIL;
		}
	} else {
		if (FAILED(DirectX::LoadFromWICFile(filePath.c_str(), 0, &metaData, scratchImage))) {
			return E_FAIL;
		}
	}

	//	hr = DirectX::CreateTexture( m_pD3dDevice, scratchImage.GetImages(), scratchImage.GetImageCount(), metaData, );
	hr = DirectX::CreateShaderResourceView(pD3dDevice.get(), scratchImage.GetImages(), scratchImage.GetImageCount(), metaData, ppOut);

	return hr;
}

HRESULT CD3D11TextureLoader::CreateSamplerState(std::shared_ptr<ID3D11Device> pD3dDevice, ID3D11SamplerState ** ppOut)
{
	assert(pD3dDevice);

	if (ppOut == nullptr) {
		return E_FAIL;
	}

	HRESULT hr;

	// よく使いそうな値を入れておく
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 2;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = pD3dDevice->CreateSamplerState(&samplerDesc, ppOut);
	if (FAILED(hr)) {
		return hr;
	}

	return hr;
}
