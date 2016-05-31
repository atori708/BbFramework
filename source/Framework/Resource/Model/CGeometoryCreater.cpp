#include "CGeometoryCreater.h"
#include"Framework\Utility\SafeDelete.h"
#include"Framework\Utility\D3d11Utility.h"

CGeometoryCreater::CGeometoryCreater(std::shared_ptr<ID3D11Device> pDevice)
{
	m_pDevice = pDevice;
}

CGeometoryCreater::~CGeometoryCreater()
{
}

std::shared_ptr<CD3D11DrawModel3D> CGeometoryCreater::CreatePlanePolygon(float size, UINT numDivision)
{
	if (m_pDevice == nullptr ) {
		return nullptr;
	}

	std::shared_ptr<CD3D11DrawModel3D> pOut = std::make_shared<CD3D11DrawModel3D>();

	using namespace DirectX;
	HRESULT hr;
	int	num = numDivision + 2;	// ある一辺上の頂点の数

	// S---------- 頂点バッファ作成 ----------
	UINT *strides = new UINT[1];
	UINT *offsets = new UINT[1];
	strides[0] = sizeof(s_simplePrimitiveVertex);
	offsets[0] = 0;
	pOut->m_pVertexStrides.reset(strides);
	pOut->m_pVertexOffsets.reset(offsets);
	pOut->m_numVertex = num * num;
	s_simplePrimitiveVertex* pVertices = new s_simplePrimitiveVertex[pOut->m_numVertex];
	// 頂点定義
	for (int i = 0; i<num; ++i) {
		for (int j = 0; j<num; ++j) {
			UINT index = i * num + j;
			float _size = size / (num - 1);	// 頂点と頂点の間の長さ
			float uv = 1 / _size * _size;
			// 座標
			pVertices[index].pos.x = (j - (num - 3)) * _size;
			pVertices[index].pos.y = 0;
			pVertices[index].pos.z = -(i - (num - 3)) * _size;
			// 法線(真上)
			pVertices[index].normal = XMFLOAT3(0, 1, 0);
			// UV
			pVertices[index].uv.x = j * uv;
			pVertices[index].uv.y = i * uv;
		}
	}

	ID3D11Buffer* pVertexBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferDesc.ByteWidth = sizeof(s_simplePrimitiveVertex) * pOut->m_numVertex;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	subResource.pSysMem = pVertices;

	hr = m_pDevice->CreateBuffer(&bufferDesc, &subResource, &pVertexBuffer);
	if (FAILED(hr)) {
		return nullptr;
	}
	pOut->m_pVertexBuffer.reset(pVertexBuffer, D3DComDeleter());
	SafeDeleteArray(pVertices);
	// E----------頂点バッファ作成  ----------

	// S---------- インデックスバッファ作成 ----------
	ID3D11Buffer* pIndexBuffer;
	pOut->m_numIndex = (num - 1) * (num - 1) * 2 * 3;
	UINT* pIndices = new UINT[pOut->m_numIndex];
	for (int i = 0; i<num; ++i) {
		for (int j = 0; j<num; ++j) {
			if (j == (num - 1) || i == (num - 1))
				break;
			UINT index = i * (6 * (num - 1)) + j * 6;
			UINT work = i*num + j;
			pIndices[index] = work;
			pIndices[index + 1] = work + 1;
			pIndices[index + 2] = work + num;
			pIndices[index + 3] = work + 1;
			pIndices[index + 4] = pIndices[index + 1] + num;
			pIndices[index + 5] = pIndices[index + 2];
		}
	}

	bufferDesc.ByteWidth = sizeof(UINT) * pOut->m_numIndex;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResource.pSysMem = pIndices;

	hr = m_pDevice->CreateBuffer(&bufferDesc, &subResource, &pIndexBuffer);
	if (FAILED(hr)) {
		pOut->Destroy();
		return nullptr;
	}
	pOut->m_pIndexBuffer.reset(pIndexBuffer, D3DComDeleter());
	SafeDeleteArray(pIndices);
	// E---------- インデックスバッファ作成 ----------

	pOut->m_numMaterial = 1;

	pOut->m_indexFormat = DXGI_FORMAT_R32_UINT;
	pOut->m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//_pOut->m_startIndexLocations.push_back(0);
	//_pOut->m_indexCounts.push_back( _pOut->m_numIndex);

	pOut->m_pStartIndexLocations.reset((UINT*)malloc(sizeof(UINT) * pOut->m_numMaterial));
	pOut->m_pStartIndexLocations.get()[0] = 0;
	pOut->m_pIndexCounts.reset((UINT*)malloc(sizeof(UINT) * pOut->m_numMaterial));
	pOut->m_pIndexCounts.get()[0] = pOut->m_numIndex;

	// マテリアル、シェーダの設定
	pOut->m_materials.resize(1);
	pOut->m_materials[0].m_inputLayoutIndex = 0;
	pOut->m_materials[0].m_vertexShaderIndex = 0;
	pOut->m_materials[0].m_pixelShaderIndex = 0;

	// マテリアルの定数バッファ
	pOut->m_materials[0].m_cbEveryMaterial.CreateConstantBuffer(m_pDevice, 3, sizeof(pOut->m_materials[0].m_cbMaterialStruct));
	pOut->m_materials[0].m_cbMaterialStruct.m_diffuse = DirectX::XMFLOAT4(0.7f, 0.7f, 2.0f, 1);
	pOut->m_materials[0].m_cbMaterialStruct.m_specular = DirectX::XMFLOAT4(1, 1, 1, 1);
	pOut->m_materials[0].m_cbMaterialStruct.m_isTex = 0;
	pOut->m_materials[0].m_cbMaterialStruct.m_isToonTex = 0;
	pOut->m_materials[0].m_cbMaterialStruct.m_power = 0;

	return pOut;
}

bool CGeometoryCreater::CreateRenderTargetPrimitive(CD3D11DrawModel3D * _pOut)
{
	if (m_pDevice == nullptr || _pOut == nullptr) {
		return false;
	}

	using namespace DirectX;

	HRESULT hr;

	s_postProcessVertex vertices[4] = {
		{ XMFLOAT3(-1, 1, 0), XMFLOAT2(0,0) },
		{ XMFLOAT3(1, 1, 0), XMFLOAT2(1.0f, 0) },
		{ XMFLOAT3(-1, -1, 0), XMFLOAT2(0, 1.0f) },
		{ XMFLOAT3(1, -1, 0), XMFLOAT2(1.0f, 1.0f) }
	};

	// 頂点バッファ作成
	ID3D11Buffer* pVertexBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA subResource;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(s_postProcessVertex) * 4;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	subResource.pSysMem = vertices;
	subResource.SysMemPitch = 0;
	subResource.SysMemSlicePitch = 0;

	hr = m_pDevice->CreateBuffer(&bufferDesc, &subResource, &pVertexBuffer);
	if (FAILED(hr)) {
		return false;
	}
	_pOut->m_pVertexBuffer.reset(pVertexBuffer, D3DComDeleter());

	// インデックスバッファ作成
	ID3D11Buffer* pIndexBuffer;
	UINT indices[6] = { 0, 1, 2, 1, 3, 2 };
	bufferDesc.ByteWidth = sizeof(UINT) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResource.pSysMem = indices;

	hr = m_pDevice->CreateBuffer(&bufferDesc, &subResource, &pIndexBuffer);
	if (FAILED(hr)) {
		return false;
	}
	_pOut->m_pIndexBuffer.reset(pIndexBuffer, D3DComDeleter());

	return true;
}
