#pragma once
#include"Framework\Resource\Model\CDrawModelBase.h"
#include<DirectXMath.h>

class CGameObject3D
{
private:
	DirectX::XMMATRIX	m_worldMatrix;
	std::shared_ptr< CDrawModelBase>	m_pDrawObject;

public:
	CGameObject3D();
	~CGameObject3D();

	bool	SetDrawObject(const std::shared_ptr<CDrawModelBase> pDrawObject);
	const std::shared_ptr<CDrawModelBase> GetDrawObject();
	DirectX::XMMATRIX& GetWorldMatrix();
};

