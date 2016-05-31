#pragma once

#include<d3d11.h>
#include<DirectXTex.h>
#include<memory>

#pragma comment( lib, "DirectXTex.lib")

/*!
 * @brief    direct3d11用のテクスチャローダ
 * @note     正しくはSRVとサンプラーステート作ってる
 * @author   N.kaji
 * @date     2016/05/29
 */
// TODO: サンプラーステートのフィルターとか今はいじれない。必要なら追加すること。
class CD3D11TextureLoader
{
public:
	CD3D11TextureLoader();
	~CD3D11TextureLoader();

	HRESULT	Load(std::shared_ptr<ID3D11Device> pD3dDevice, std::wstring filePath, ID3D11ShaderResourceView** ppOut);
	HRESULT	CreateSamplerState(std::shared_ptr<ID3D11Device> pD3dDevice, ID3D11SamplerState** ppOut);
};

