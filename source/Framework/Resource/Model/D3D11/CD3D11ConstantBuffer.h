#pragma once

#include<d3d11.h>
#include<DirectXMath.h>
#include<memory>
#define CONSTANT_BUFFER_NONE	(0xffffffff)

struct s_constantBufferStruct
{
};

struct s_cbChangeWindowResizing
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4X4 m_matProjection;
};

struct s_cbChangeEveryFrame
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4X4	m_matView;
	DirectX::XMFLOAT4	m_directionalLight;		// 平行光源向き(wはライトの強さ)
	DirectX::XMFLOAT4	m_directionalLightColor;	// 平行光源色

	s_cbChangeEveryFrame()
	{

	}
	s_cbChangeEveryFrame(DirectX::XMFLOAT4X4& matView, DirectX::XMFLOAT4& directionLight, DirectX::XMFLOAT4& lightColor)
	{
		m_matView = matView;
		m_directionalLight = directionLight;
		m_directionalLightColor = lightColor;
	}
};

struct s_cbChangesAtEveryObject
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4X4	m_matWorld;
	DirectX::XMFLOAT4X4	m_matWorldInv;
};

class CD3D11ConstantBuffer
{
private:
	UINT	m_startSlot;
	size_t	m_size;
	std::shared_ptr<ID3D11Buffer>	m_pCbuffer;

public:
	CD3D11ConstantBuffer();
	~CD3D11ConstantBuffer();

	HRESULT	CreateConstantBuffer( std::shared_ptr<ID3D11Device> pDevice, UINT slotNum, UINT dataSize);
	HRESULT	Update(std::shared_ptr<ID3D11DeviceContext> pDeviceContext, s_constantBufferStruct& _pUpdateData);
	std::shared_ptr<ID3D11Buffer> const	GetConstantBuffer()const;
	UINT GetStartSlot()const;
	void DestroyConstantBuffer();
};

