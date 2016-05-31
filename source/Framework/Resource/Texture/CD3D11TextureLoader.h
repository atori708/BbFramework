#pragma once

#include<d3d11.h>
#include<DirectXTex.h>
#include<memory>

#pragma comment( lib, "DirectXTex.lib")

/*!
 * @brief    direct3d11�p�̃e�N�X�`�����[�_
 * @note     ��������SRV�ƃT���v���[�X�e�[�g����Ă�
 * @author   N.kaji
 * @date     2016/05/29
 */
// TODO: �T���v���[�X�e�[�g�̃t�B���^�[�Ƃ����͂�����Ȃ��B�K�v�Ȃ�ǉ����邱�ƁB
class CD3D11TextureLoader
{
public:
	CD3D11TextureLoader();
	~CD3D11TextureLoader();

	HRESULT	Load(std::shared_ptr<ID3D11Device> pD3dDevice, std::wstring filePath, ID3D11ShaderResourceView** ppOut);
	HRESULT	CreateSamplerState(std::shared_ptr<ID3D11Device> pD3dDevice, ID3D11SamplerState** ppOut);
};

