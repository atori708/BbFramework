#pragma once
#include"Framework/Resource/Model/CDrawModelBase.h"
#include"Framework\Resource\Model\D3D11\CD3D11ConstantBuffer.h"
#include<vector>

// TODO:2D��3D�ŕ������ق�����������

struct s_bmdStaticMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
};

// �{�[���̉e���͍ő�S��
struct s_bmdSkineMeshVertex
{
	float	pos[3];
	float	normal[3];
	float	texCoord[2];
	int		boneIndex[4];
	float	boneWeight[3];
};

// ���f���̒萔�o�b�t�@�p�\����
struct s_cbChangeEveryModel
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4X4	m_pWorldMatrix;
};

// �}�e���A���̒萔�o�b�t�@�p�\����
struct s_cbBmdMaterial
	:public s_constantBufferStruct
{
	DirectX::XMFLOAT4	m_diffuse;
	DirectX::XMFLOAT4	m_specular;
	float		m_isTex;	// bool�ɂ��������ǃV�F�[�_�ɂ��܂��n���Ȃ��̂�(Fuck)
	float		m_isToonTex;
	float		m_power;
	float		m_dummy;
};

// �}�e���A���ƃV�F�[�_��1��1�Ƃ͌����(����1��1�Ŏ���)
struct s_bmdMaterial
{
	unsigned int		m_vertexShaderIndex;
	unsigned int		m_pixelShaderIndex;
	unsigned int		m_inputLayoutIndex;

	s_cbBmdMaterial m_cbMaterialStruct;	// �}�e���A�����Ƃ̒萔�o�b�t�@
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
friend CD3D113DModelLoader;	// ���f���̊g���q���Ƀ��[�_�[���Ƃ����𑝂₷�K�v���邩��friend�����Z�b�^�[������ق������������B

protected:
	std::wstring	m_filePath;
	std::wstring	m_fileName;

	std::shared_ptr<ID3D11Buffer>	m_pVertexBuffer;
	std::shared_ptr<ID3D11Buffer>	m_pIndexBuffer;

	// ���f�����ƂɍX�V�����萔�o�b�t�@
	s_cbChangeEveryModel m_changeEveryModelConstantBufferStruct;
	CD3D11ConstantBuffer m_changeEveryModelConstantBuffer;

	DXGI_FORMAT		m_indexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_primitiveTopology;

	// �}�e���A��
	std::vector<s_bmdMaterial>	m_materials;
	//s_material*		m_pMaterials;

public:
	CD3D11DrawModel3D();
	~CD3D11DrawModel3D();

	void SetWorldMatrix(std::shared_ptr<ID3D11Device> pDevice, const DirectX::XMMATRIX& worldMatrix);
	bool Update(std::shared_ptr<ID3D11DeviceContext>);	// ���f�����Ƃɕς��l�̍X�V

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
	HRESULT			UpdateMaterial(std::shared_ptr<ID3D11DeviceContext> pDeviceContext, unsigned int materialIndex);// �}�e���A�����ɕς��l�̍X�V
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

