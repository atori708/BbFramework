#include "CGameObject3D.h"

CGameObject3D::CGameObject3D()
{
	m_worldMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity());
}

CGameObject3D::~CGameObject3D()
{
}

bool CGameObject3D::SetDrawObject( std::shared_ptr<CDrawModelBase> pDrawObject)
{
	m_pDrawObject = pDrawObject;

	return true;
}

const std::shared_ptr<CDrawModelBase> CGameObject3D::GetDrawObject()
{
	return m_pDrawObject;
}

DirectX::XMMATRIX& CGameObject3D::GetWorldMatrix()
{
	return m_worldMatrix;
}
