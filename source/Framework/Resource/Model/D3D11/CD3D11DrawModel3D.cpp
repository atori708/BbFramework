#include "CD3D11DrawModel3D.h"
#include"Framework\Utility\SafeDelete.h"

CD3D11DrawModel3D::CD3D11DrawModel3D()
{
}

CD3D11DrawModel3D::~CD3D11DrawModel3D()
{
}

void CD3D11DrawModel3D::SetWorldMatrix(std::shared_ptr<ID3D11Device> pDevice, const DirectX::XMMATRIX& worldMatrix)
{
	DirectX::XMStoreFloat4x4(&m_changeEveryModelConstantBufferStruct.m_pWorldMatrix, worldMatrix);
	m_changeEveryModelConstantBuffer.CreateConstantBuffer(pDevice, 2, sizeof(s_cbChangeEveryModel));
}

bool CD3D11DrawModel3D::Update(std::shared_ptr<ID3D11DeviceContext> pDeviceContext)
{
	if (pDeviceContext == nullptr) {
		return false;
	}

	if (FAILED(m_changeEveryModelConstantBuffer.Update(pDeviceContext, m_changeEveryModelConstantBufferStruct))) {
		return false;
	}

	return true;
}

std::shared_ptr<ID3D11Buffer> const	CD3D11DrawModel3D::GetVertexBuffer()const
{
	return m_pVertexBuffer;
}

std::shared_ptr<unsigned int>	CD3D11DrawModel3D::GetVertexStrides()const
{
	return m_pVertexStrides;
}

std::shared_ptr<unsigned int>	CD3D11DrawModel3D::GetVertexOffsets()const
{
	return m_pVertexOffsets;
}

std::shared_ptr<ID3D11Buffer> CD3D11DrawModel3D::GetIndexBuffer()const
{
	return m_pIndexBuffer;
}

DXGI_FORMAT	CD3D11DrawModel3D::GetIndexFormat()const
{
	return m_indexFormat;
}

unsigned int	CD3D11DrawModel3D::GetIndexStartLocation(unsigned int _materialIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return 0;

	return m_pStartIndexLocations.get()[_materialIndex];
}

unsigned int	CD3D11DrawModel3D::GetIndexCounts(unsigned int _materialIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return 0;

	return m_pIndexCounts.get()[_materialIndex];
}

bool	CD3D11DrawModel3D::SetVertexShaderIndex(unsigned int _materialIndex, unsigned int _shaderIndex)
{
	if (_materialIndex >= m_numMaterial)
		return false;

	m_materials[_materialIndex].m_vertexShaderIndex = _shaderIndex;

	return true;
}

bool	CD3D11DrawModel3D::SetPixelShaderIndex(unsigned int _materialIndex, unsigned int _shaderIndex)
{
	if (_materialIndex >= m_numMaterial)
		return false;

	m_materials[_materialIndex].m_pixelShaderIndex = _shaderIndex;

	return true;
}

bool	CD3D11DrawModel3D::SetInputLayoutIndex(unsigned int _materialIndex, unsigned int _inputLayoutIndex)
{
	if (_materialIndex >= m_numMaterial)
		return false;

	m_materials[_materialIndex].m_inputLayoutIndex = _inputLayoutIndex;

	return true;
}

const CD3D11ConstantBuffer& CD3D11DrawModel3D::GetChangeEveryModelConstantBuffer()
{
	return m_changeEveryModelConstantBuffer;
}


unsigned int	CD3D11DrawModel3D::GetNumMaterial()const
{
	return m_numMaterial;
}

unsigned int	CD3D11DrawModel3D::GetShaderResourceViewIndex(unsigned int _materialIndex, unsigned int _textureIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return -1;

	if (_textureIndex >= m_materials[_materialIndex].m_textureIndices.size())
		return -1;

	return m_materials[_materialIndex].m_textureIndices[_textureIndex];
}

unsigned int	CD3D11DrawModel3D::GetSamplerStateIndex(unsigned int _materialIndex, unsigned int _textureIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return -1;

	if (_textureIndex >= m_materials[_materialIndex].m_textureIndices.size())
		return -1;

	return m_materials[_materialIndex].m_textureIndices[_textureIndex];
}

unsigned int	CD3D11DrawModel3D::GetVertexShaderIndex(unsigned int _materialIndex, unsigned int _shaderIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return -1;

	return m_materials[_materialIndex].m_vertexShaderIndex;
}

HRESULT	CD3D11DrawModel3D::UpdateMaterial(std::shared_ptr<ID3D11DeviceContext> _pDeviceContext, unsigned int _materialIndex)
{
	if (_pDeviceContext == nullptr || _materialIndex >= m_numMaterial) {
		return E_INVALIDARG;
	}

	return m_materials[_materialIndex].m_cbEveryMaterial.Update(_pDeviceContext, m_materials[_materialIndex].m_cbMaterialStruct);
}

unsigned int	CD3D11DrawModel3D::GetMaterialCBufferSlotNum(unsigned int _materialIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return 16;	// 16‚Í‚ ‚è‚¦‚È‚¢’l‚È‚Ì‚Å

	return m_materials[_materialIndex].m_cbEveryMaterial.GetStartSlot();
}

std::shared_ptr<ID3D11Buffer> const CD3D11DrawModel3D::GetMaterialCBuffer(unsigned int _materialIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return nullptr;

	return m_materials[_materialIndex].m_cbEveryMaterial.GetConstantBuffer();
}

unsigned int	CD3D11DrawModel3D::GetInputLayoutIndex(unsigned int _materialIndex, unsigned int _shaderIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return -1;

	return m_materials[_materialIndex].m_inputLayoutIndex;
}

unsigned int CD3D11DrawModel3D::GetPixelShaderIndex(unsigned int _materialIndex, unsigned int _shaderIndex)const
{
	if (_materialIndex >= m_numMaterial)
		return -1;

	return m_materials[_materialIndex].m_pixelShaderIndex;
}

D3D11_PRIMITIVE_TOPOLOGY	CD3D11DrawModel3D::GetPrimitiveTopology()const
{
	return m_primitiveTopology;
}

void	CD3D11DrawModel3D::Destroy()
{
}