#pragma once
#include"Framework/Resource/Model/CDrawModelBase.h"
#include"Framework\Resource\Model\D3D11\CD3D11ConstantBuffer.h"
#include"Framework\Utility\SafeDelete.h"
#include<vector>


struct SBmdStaticMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
};

// ボーンの影響は最大４つ
struct SBmdSkineMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
	int		boneIndex[4];
	float	boneWeight[3];
};

// モデルの定数バッファ用構造体
struct SConstantBufferChangeEveryModel
	:public SConstantBufferStruct
{
	DirectX::XMFLOAT4X4	m_pWorldMatrix;
};

// マテリアルの定数バッファ用構造体
struct SConstantBufferBmdMaterial
	:public SConstantBufferStruct
{
	DirectX::XMFLOAT4	m_diffuse;
	DirectX::XMFLOAT4	m_specular;
	float		m_isToonTex;
	float		m_specularPower;
	float		m_dummy[2];
};

// Bmdファイルでのマテリアル(シェーダは載ってない)
struct SBmdMaterial
{
	std::string		m_textureFileName;
	SConstantBufferBmdMaterial m_cbMaterialStruct;	// マテリアルごとの定数バッファ
	//CD3D11ConstantBuffer	m_cbEveryMaterial;

};

// 実際に描画に使われるマテリアル
// マテリアルとシェーダが1対1とは限らん(今は1対1で実装)
struct SMaterial
{
	unsigned int		m_vertexShaderIndex;
	unsigned int		m_pixelShaderIndex;
	unsigned int		m_inputLayoutIndex;

	SConstantBufferBmdMaterial m_cbMaterialStruct;	// マテリアルごとの定数バッファ
	CD3D11ConstantBuffer	m_cbEveryMaterial;

	std::vector<unsigned int>	m_textureIndices;

	SMaterial()
	{
		m_vertexShaderIndex = -1;
		m_pixelShaderIndex = -1;
		m_inputLayoutIndex = -1;
	}
};

struct SStaticModelData
{
	SBmdStaticMeshVertex* m_pVertices;	// 頂点配列の先頭ポインタ
	int m_vertexCount;				// 頂点数
	size_t m_vertexSize;			// 頂点1つのサイズ
	int* m_pIndices;				// インデックス配列の先頭ポインタ
	int m_allIndexCount;			// インデックス数
	int* m_pIndexCounts;			// マテリアル毎のインデックスの数
	SBmdMaterial* m_pMaterials;		// マテリアル配列の先頭ポインタ
	int m_materialCount;			// マテリアルの数

	SStaticModelData()
	{
		m_vertexCount = 0;
		m_allIndexCount = 0;
		m_materialCount = 0;
		m_vertexSize = 0;
		m_pVertices = nullptr;
		m_pIndices = nullptr;
		m_pIndexCounts = nullptr;
		m_pMaterials = nullptr;
	}

	~SStaticModelData(){
		SafeDeleteArray(m_pVertices);
		SafeDeleteArray(m_pIndices);
		SafeDeleteArray(m_pIndexCounts);
		SafeDeleteArray(m_pMaterials);
	}
};

class CD3D11DrawModel3D
	:public CDrawModelBase
{
friend CGeometoryCreater;
friend CD3D113DModelLoader;	// モデルの拡張子毎にローダー作るとここを増やす必要あるからfriendよりもセッター作ったほうがいいかも。
friend CD3D11FBXModelLoader;

protected:
	std::wstring	m_filePath;
	std::wstring	m_fileName;

	std::shared_ptr<ID3D11Buffer>	m_pVertexBuffer;
	std::shared_ptr<ID3D11Buffer>	m_pIndexBuffer;

	// モデルごとに更新される定数バッファ
	SConstantBufferChangeEveryModel m_changeEveryModelConstantBufferStruct;
	CD3D11ConstantBuffer m_changeEveryModelConstantBuffer;

	DXGI_FORMAT		m_indexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitiveTopology;

	// マテリアル
	std::vector<SMaterial>	m_materials;
	//s_material*		m_pMaterials;

public:
	CD3D11DrawModel3D();
	~CD3D11DrawModel3D();

	void SetWorldMatrix(std::shared_ptr<ID3D11Device> pDevice, const DirectX::XMMATRIX& worldMatrix);
	bool Update(std::shared_ptr<ID3D11DeviceContext>);	// モデルごとに変わる値の更新

	// Vertex buffer.
	std::shared_ptr<ID3D11Buffer> const	GetVertexBuffer()const;
	std::shared_ptr<unsigned int>	GetVertexStrides()const;
	std::shared_ptr<unsigned int>	GetVertexOffsets()const;

	// Index buffer.
	std::shared_ptr<ID3D11Buffer>	GetIndexBuffer()const;
	DXGI_FORMAT		GetIndexFormat()const;
	unsigned int	GetIndexStartLocation(unsigned int _materialIndex)const;
	unsigned int	GetIndexCounts(unsigned int _materialIndex)const;

	bool	SetVertexShaderIndex(unsigned int materialIndex, unsigned int shaderIndex);
	bool	SetPixelShaderIndex(unsigned int materialIndex, unsigned int shaderIndex);
	bool	SetInputLayoutIndex(unsigned int materialIndex, unsigned int inputLayoutIndex);

	// Constant Buffer
	const CD3D11ConstantBuffer&	GetChangeEveryModelConstantBuffer();

	// Material.
	unsigned int	GetNumMaterial()const;
	unsigned int	GetShaderResourceViewIndex(unsigned int materialIndex, unsigned int textureIndex)const;
	unsigned int	GetSamplerStateIndex(unsigned int _materialIndex, unsigned int textureIndex)const;
	HRESULT			UpdateMaterial(std::shared_ptr<ID3D11DeviceContext> pDeviceContext, unsigned int materialIndex);// マテリアル毎に変わる値の更新
	unsigned int	GetMaterialCBufferStartSlot(unsigned int materialIndex)const;
	std::shared_ptr<ID3D11Buffer> const	GetMaterialCBuffer(unsigned int materialIndex)const;

	// Shader.
	unsigned int	GetVertexShaderIndex(unsigned int materialIndex, unsigned int shaderIndex)const;
	unsigned int	GetPixelShaderIndex(unsigned int materialIndex, unsigned int shaderIndex)const;

	// Other.
	unsigned int	GetInputLayoutIndex(unsigned int materialIndex, unsigned int shadrIndex)const;
	D3D11_PRIMITIVE_TOPOLOGY	GetPrimitiveTopology()const;

	void	Destroy();
};

