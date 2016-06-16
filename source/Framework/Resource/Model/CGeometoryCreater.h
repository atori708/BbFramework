#pragma once


/*!
 * @brief    簡易的なジオメトリを作るクラス
 * @note     nt
 * @author   N.kaji
 * @date     2016年5月21日
 */
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

struct s_simplePrimitiveVertex
{
	DirectX::XMFLOAT3	pos;
	DirectX::XMFLOAT3	normal;
	DirectX::XMFLOAT2	uv;
};

// フルスクリーンの板ポリ用
struct s_screenPolygonVertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 uv;
};

class CGeometoryCreater
{
private:
	std::shared_ptr<ID3D11Device> m_pDevice;

public:
	CGeometoryCreater() = delete;
	CGeometoryCreater(std::shared_ptr<ID3D11Device> pDevice);

	~CGeometoryCreater();

	std::shared_ptr<CD3D11DrawModel3D> 	CreatePlanePolygon(float size, UINT divisionCount);
//	std::shared_ptr<CD3D11DrawModel3D>	CreateSphere(float r, UINT divisionCount);
	bool	CreateRenderTargetPrimitive(CD3D11DrawModel3D* pOut);
};

