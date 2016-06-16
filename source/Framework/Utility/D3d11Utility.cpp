#include"D3d11Utility.h"
#include"SafeDelete.h"


HRESULT	GetD3D11Texture2DDescFromTexture( ID3D11Texture2D* _pTexture, D3D11_TEXTURE2D_DESC* _pOut)
{
	if( _pTexture == nullptr || _pOut == nullptr )
		return E_INVALIDARG;

	_pTexture->GetDesc( _pOut);

	return S_OK;
}

HRESULT	GetD3D11Texture2DDescFromSRV( ID3D11ShaderResourceView* _pSRV, D3D11_TEXTURE2D_DESC* _pOut)
{
	if( _pSRV == nullptr || _pOut == nullptr )
		return E_INVALIDARG;

	ID3D11Resource* pResource;
	_pSRV->GetResource( &pResource);
	((ID3D11Texture2D*)pResource)->GetDesc( _pOut);

	SafeRelease( pResource);

	return S_OK;
}

HRESULT	GetD3D11Texture2DDescFromRTV( ID3D11RenderTargetView* _pRTV, D3D11_TEXTURE2D_DESC* _pOut)
{
	if( _pRTV == nullptr || _pOut == nullptr )
		return E_INVALIDARG;

	ID3D11Resource* pResource;
	_pRTV->GetResource( &pResource);
	((ID3D11Texture2D*)pResource)->GetDesc( _pOut);

	SafeRelease( pResource);

	return S_OK;
}

/*!
 * @brief    �Q�̃C���v�b�g�G�������g�����S�Ɉ�v���Ă��邩��r����
 * @param    prm
 * @return   ��v���Ă��邩�ǂ���
 * @note     �P�ł��������false�Ԃ��܂�
 * @author   N.kaji
 * @date     2016/??
 */
bool	CompareD3D11InputElementDesc( D3D11_INPUT_ELEMENT_DESC* _pElementDescA,  D3D11_INPUT_ELEMENT_DESC* _pElementDescB )
{
    if( _pElementDescA == nullptr || _pElementDescB == nullptr )
        return false;

    if( _pElementDescA->AlignedByteOffset != _pElementDescA->AlignedByteOffset )	return false;
    if( _pElementDescA->Format != _pElementDescB->Format )							return false;
    if( _pElementDescA->InputSlot != _pElementDescB->InputSlot )					return false;
    if( _pElementDescA->InputSlotClass != _pElementDescB->InputSlotClass )			return false;
    if( _pElementDescA->InstanceDataStepRate != _pElementDescB->InstanceDataStepRate )	return false;
    if( _pElementDescA->SemanticIndex != _pElementDescB->SemanticIndex )				return false;
    if( strcmp( _pElementDescA->SemanticName, _pElementDescB->SemanticName) != 0)		return false;

    return true;	
}

/*!
 * @brief    �f�v�X�X�e���V���̃t�H�[�}�b�g����A�f�v�X�X�e���V���p�̃e�N�X�`���̃t�H�[�}�b�g���擾����
 * @param    depthStencilFormat �f�v�X�X�e���V���Ŏg�������t�H�[�}�b�g
 * @return   �f�v�X�X�e���V���p�̃e�N�X�`���Ŏg���t�H�[�}�b�g
 * @note     �����t�H�[�}�b�g���ƃe�N�X�`�����Ȃ������̂ō����
 * @author   N.kaji
 * @date     2016/05/21
 */
DXGI_FORMAT GetD3D11DepthStencilSRVFormat(DXGI_FORMAT depthStencilFormat)
{
	switch (depthStencilFormat) {
		case DXGI_FORMAT_D16_UNORM: return DXGI_FORMAT_R16_FLOAT;
		case DXGI_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_D32_FLOAT:	return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		default: return DXGI_FORMAT_UNKNOWN;
	}
}

/*!
* @brief    �f�v�X�X�e���V���̃t�H�[�}�b�g����A�f�v�X�X�e���V����SRV�p�̃e�N�X�`���̃t�H�[�}�b�g���擾����
* @param    depthStencilFormat �f�v�X�X�e���V���Ŏg�������t�H�[�}�b�g
* @return   �f�v�X�X�e���V����SRV�p�p�̃e�N�X�`���Ŏg���t�H�[�}�b�g
* @note     �����t�H�[�}�b�g���ƃe�N�X�`�����Ȃ������̂ō����
* @author   N.kaji
* @date     2016/05/21
*/
DXGI_FORMAT GetD3D11DepthStencilTextureFormat(DXGI_FORMAT depthStencilFormat)
{
	switch (depthStencilFormat) {
	case DXGI_FORMAT_D16_UNORM: return DXGI_FORMAT_R16_TYPELESS;
	case DXGI_FORMAT_D24_UNORM_S8_UINT: return DXGI_FORMAT_R24G8_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT:	return DXGI_FORMAT_R32_TYPELESS;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT: return DXGI_FORMAT_R32G8X24_TYPELESS;
	default: return DXGI_FORMAT_UNKNOWN;
	}
}
