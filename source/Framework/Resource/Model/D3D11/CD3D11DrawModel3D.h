#pragma once
#include"Framework/Resource/Model/CDrawModelBase.h"
#include"Framework\Resource\Model\D3D11\CD3D11ConstantBuffer.h"
#include<vector>

// TODO:2Dと3Dで分けたほうがいいかも

struct s_bmdStaticMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
};

// ボーンの影響は最大４つ
struct s_bmdSkineMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
	int		boneIndex[4];
	float	boneWeight[3];
};

// モデルの定数バッファ用構造体
struct s_cbChangeEveryModel
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4X4	m_pWorldMatrix;
};

// マテリアルの定数バッファ用構造体
struct s_cbBmdMaterial
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4	m_diffuse;
	DirectX::XMFLOAT4	m_specular;
	float		m_isTex;	// boolにしたいけどシェーダにうまく渡せないので(Fuck)
	float		m_isToonTex;
	float		m_power;
	float		m_dummy;
};

// マテリアルとシェーダが1対1とは限らん(今は1対1で実装)
struct s_bmdMaterial
{
	unsigned int		m_vertexShaderIndex;
	unsigned int		m_pixelShaderIndex;
	unsigned int		m_inputLayoutIndex;

	s_cbBmdMaterial m_cbMaterialStruct;	// マテリアルごとの定数バッファ
	CD3D11ConstantBuffer	m_cbEveryMaterial;

	std::vector<unsigned int>	m_textureIndices;

	s_bmdMaterial()
	{
		m_vertexShaderIndex = -1;
		m_pixelShaderIndex = -1;
		m_inputLayoutIndex = -1;
	}
};

class CGeometoryCreater;
//class CD3D113DModelLoader;

class CD3D11DrawModel3D
	:public CDrawModelBase
{
friend CGeometoryCreater;
friend CD3D113DModelLoader;	// モデルの拡張子毎にローダー作るとここを増やす必要あるからfriendよりもセッター作ったほうがいいかも。

protected:
	std::wstring	m_filePath;
	std::wstring	m_fileName;

	std::shared_ptr<ID3D11Buffer>	m_pVertexBuffer;
	std::shared_ptr<ID3D11Buffer>	m_pIndexBuffer;

	// モデルごとに更新される定数バッファ
	s_cbChangeEveryModel m_changeEveryModelConstantBufferStruct;
	CD3D11ConstantBuffer m_changeEveryModelConstantBuffer;

	DXGI_FORMAT		m_indexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitiveTopology;

	// マテリアル
	std::vector<s_bmdMaterial>	m_materials;
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
	unsigned int	GetMaterialCBufferSlotNum(unsigned int materialIndex)const;
	std::shared_ptr<ID3D11Buffer> const	GetMaterialCBuffer(unsigned int materialIndex)const;

	// Shader.
	unsigned int	GetVertexShaderIndex(unsigned int materialIndex, unsigned int shaderIndex)const;
	unsigned int	GetPixelShaderIndex(unsigned int materialIndex, unsigned int shaderIndex)const;

	// Other.
	unsigned int	GetInputLayoutIndex(unsigned int materialIndex, unsigned int shadrIndex)const;
	D3D11_PRIMITIVE_TOPOLOGY	GetPrimitiveTopology()const;

	void	Destroy();
};

