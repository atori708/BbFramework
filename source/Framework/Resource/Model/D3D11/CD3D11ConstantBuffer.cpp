#include"CD3D11ConstantBuffer.h"
#include"Framework\Utility\SafeDelete.h"
#include"Framework\Utility\D3d11Utility.h"

CD3D11ConstantBuffer::CD3D11ConstantBuffer()
{
	m_size = 0;
	m_startSlot = 0;
	m_pCbuffer = nullptr;
}

CD3D11ConstantBuffer::~CD3D11ConstantBuffer()
{
	//SafeRelease( m_pCbuffer);
}

HRESULT CD3D11ConstantBuffer::CreateConstantBuffer(std::shared_ptr<ID3D11Device> pDevice, UINT slotNum, UINT dataSize)
{
	if (pDevice == nullptr) {
		return false;
	}
	
	HRESULT hr;
	ID3D11Buffer* pConstantBuffer;
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = dataSize;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	hr = pDevice->CreateBuffer( &bufferDesc, nullptr, &pConstantBuffer);
	if (FAILED(hr)) {
		return E_FAIL;
	}
	m_pCbuffer.reset(pConstantBuffer, D3DComDeleter());

	m_startSlot = slotNum;
	m_size = dataSize;

	return S_OK;
}

/*------------------------------
* @brief	ConstantBufferçXêV
* @param
* @note
* @author   N.Kaji
* @date		2014/07/27
------------------------------*/
HRESULT	CD3D11ConstantBuffer::Update(std::shared_ptr<ID3D11DeviceContext> pDeviceContext, SConstantBufferStruct& _pUpdateData)
{
	if (pDeviceContext == nullptr || m_pCbuffer == nullptr ) {
		return false;
	}

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	hr = pDeviceContext->Map( m_pCbuffer.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hr)) {
		return E_FAIL;
	}
	CopyMemory( mappedResource.pData, &_pUpdateData, m_size);
	pDeviceContext->Unmap( m_pCbuffer.get(), 0);
	
	return S_OK;
}

/*------------------------------
* @brief
* @param
* @note
* @author   N.Kaji
* @date
------------------------------*/
std::shared_ptr<ID3D11Buffer> const	CD3D11ConstantBuffer::GetConstantBuffer()const
{
	return m_pCbuffer;
}

/*------------------------------
* @brief
* @param
* @note
* @author   N.Kaji
* @date		
------------------------------*/
UINT CD3D11ConstantBuffer::GetStartSlot()const
{
	return m_startSlot;
}

void CD3D11ConstantBuffer::DestroyConstantBuffer()
{
	m_pCbuffer = nullptr;
}
