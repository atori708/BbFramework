#include "CD3D113DModelLoader.h"
#include<fstream>
#include"Framework\Utility\SafeDelete.h"
#include"Framework\Utility\D3d11Utility.h"
#include"Framework\Resource\CD3D11ResourceManager.h"

CD3D113DModelLoader::CD3D113DModelLoader()
{
}


CD3D113DModelLoader::~CD3D113DModelLoader()
{
}

bool CD3D113DModelLoader::Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut)
{
	assert(pDevice);
	if (pFilePath == nullptr) {
		return false;
	}

	if (pOut == nullptr) {
		return false;
	}

	std::ifstream ifs(pFilePath, std::ios::binary);
	if (!ifs) {
		return false;
	}

	pOut->m_filePath = pFilePath;
	//pOut->m_fileName = _filePath.substr( _filePath.find_last_of( L"/")+1);

	if (!this->LoadFromPMDFile(pDevice, ifs, pOut)) {
		return false;
	}

	// �V�F�[�_�̃C���f�b�N�X���Z�b�g
	for (UINT i = 0; i<pOut->GetNumMaterial(); ++i) {
		pOut->SetVertexShaderIndex(i, 0);
		pOut->SetPixelShaderIndex(i, 0);
		pOut->SetInputLayoutIndex(i, 0);
	}
	//pOut->m_canRendering = true;

	return true;
}

bool CD3D113DModelLoader::LoadFromPMDFile(std::shared_ptr<ID3D11Device> pDevice, std::ifstream & ifStream, std::shared_ptr<CD3D11DrawModel3D> pOut)
{
	PmdHeader pmdHeader;
	ifStream.read((char*)&pmdHeader, sizeof(PmdHeader));

	if (strcmp(pmdHeader.magic, "Pmd") != 0) {
		return false;
	}

	PmdVertex* pPmdVertices;	// PMD�t�@�C���ꎞ�i�[�ꏊ
	USHORT* pPmdFaces;
	PmdMaterial* pPmdMaterials;

	// ���_
	ifStream.read((char*)&pOut->m_numVertex, sizeof(UINT));
	pPmdVertices = new PmdVertex[pOut->m_numVertex];
	ifStream.read((char*)pPmdVertices, sizeof(PmdVertex) * pOut->m_numVertex);

	// ��
	ifStream.read((char*)&pOut->m_numIndex, sizeof(UINT));
	pPmdFaces = new USHORT[pOut->m_numIndex];
	ifStream.read((char*)pPmdFaces, sizeof(USHORT) * pOut->m_numIndex);

	// �}�e���A��
	ifStream.read((char*)&pOut->m_numMaterial, sizeof(UINT));
	pPmdMaterials = new PmdMaterial[pOut->m_numMaterial];
	ifStream.read((char*)pPmdMaterials, sizeof(PmdMaterial) * pOut->m_numMaterial);

	bool isCreate = this->CreateModelDataFromPMD(pDevice, pPmdVertices, pPmdFaces, pPmdMaterials, pOut);
	if (!isCreate) {
		return false;
	}

	SafeDeleteArray(pPmdVertices);
	SafeDeleteArray(pPmdFaces);
	SafeDeleteArray(pPmdMaterials);
	return true;
}

// �X�L�����b�V���p�̃f�[�^������Ă�(���_�C���f�b�N�X�Ƃ��A�u�����h�W���Ƃ�)
bool CD3D113DModelLoader::CreateModelDataFromPMD(std::shared_ptr<ID3D11Device> pDevice, PmdVertex* pPmdVertices, USHORT* pPmdFaces, PmdMaterial* pPmdMaterials, std::shared_ptr<CD3D11DrawModel3D> pModelData)
{
	if (pPmdVertices == nullptr || pPmdFaces == nullptr || pModelData == nullptr) {
		return false;
	}

	HRESULT hr;

	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA	subResourceData;

	std::shared_ptr<UINT> stride(new UINT[1], std::default_delete<UINT>());
	//std::shared_ptr<UINT> stride;
	std::shared_ptr<UINT> offset(new UINT[1], std::default_delete<UINT>());
	//std::shared_ptr<UINT> offset;
	stride.get()[0] = sizeof(s_bmdSkineMeshVertex);
	offset.get()[0] = 0;
	pModelData->m_pVertexStrides = stride;
	pModelData->m_pVertexOffsets = offset;

	// ���_���ڂ��ւ���
	s_bmdSkineMeshVertex* _pBbxVertex;
	_pBbxVertex = new s_bmdSkineMeshVertex[pModelData->m_numVertex];
	for (UINT i = 0; i < pModelData->m_numVertex; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (j == 0 || j == 1) {
				_pBbxVertex[i].texCoord[j] = pPmdVertices[i].uv[j];
				_pBbxVertex[i].boneIndex[j] = pPmdVertices[j].boneIndex[j];
			}

			if (j != 3) {
				_pBbxVertex[i].pos[j] = pPmdVertices[i].pos[j];
				_pBbxVertex[i].normal[j] = pPmdVertices[i].normal[j];
			}
		}

		_pBbxVertex[i].boneWeight[0] = (float)pPmdVertices[i].boneWeight;
	}

	// S---------- ���_�o�b�t�@�쐬 ----------
	bufferDesc.ByteWidth = sizeof(s_bmdSkineMeshVertex) * pModelData->m_numVertex;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	subResourceData.pSysMem = _pBbxVertex;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* pVertexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pVertexBuffer);
	SafeDeleteArray(_pBbxVertex);
	if (FAILED(hr)) {
		return false;
	}
	pModelData->m_pVertexBuffer.reset(pVertexBuffer, D3DComDeleter());
	// E---------- ���_�o�b�t�@�쐬 ----------
	pModelData->m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// S---------- �C���f�b�N�X�o�b�t�@�쐬 ----------
	bufferDesc.ByteWidth = sizeof(USHORT) * pModelData->m_numIndex;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResourceData.pSysMem = pPmdFaces;

	ID3D11Buffer* pIndexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pIndexBuffer);
	if (FAILED(hr)) {
		pModelData->m_pVertexBuffer = nullptr;
		return false;
	}
	pModelData->m_pIndexBuffer.reset(pIndexBuffer, D3DComDeleter());

	pModelData->m_indexFormat = DXGI_FORMAT_R16_UINT;
	// E---------- �C���f�b�N�X�o�b�t�@�쐬 ----------


	// S---------- �}�e���A���쐬 ----------
	pModelData->m_materials.resize(pModelData->m_numMaterial);
	//pModelData->m_pStartIndexLocations
	//pModelData->m_indexCounts.resize( pModelData->m_numMaterial);
	std::wstring filePathTemp = pModelData->m_filePath.substr(0, pModelData->m_filePath.find_last_of(L"/") + 1);	// ���f���̂���f�B���N�g���̃p�X���擾
	std::wstring filePath;
	wchar_t textureFileName[20];

	// �C���f�b�N�X�̋�؂���}�e���A�����m��
	//pModelData->m_pStartIndexLocations = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
	//pModelData->m_pIndexCounts = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
	std::shared_ptr<UINT> pStartIndexLocations(new UINT[pModelData->m_numMaterial], std::default_delete<UINT>());
	std::shared_ptr<UINT> pIndexCounts(new UINT[pModelData->m_numMaterial], std::default_delete<UINT>());
	pModelData->m_pStartIndexLocations = pStartIndexLocations;
	pModelData->m_pIndexCounts = pIndexCounts;

	for (UINT i = 0; i<pModelData->m_numMaterial; ++i) {
		using namespace DirectX;
		//pModelData->m_materials[i].m_cbMaterialStruct.m_diffuse = pPmdMaterials[i].diffuse;
		//pModelData->m_materials[i].m_cbMaterialStruct.m_specular = pPmdMaterials[i].specular;
		XMStoreFloat4(&pModelData->m_materials[i].m_cbMaterialStruct.m_diffuse, XMVectorSet(pPmdMaterials[i].diffuse[0], pPmdMaterials[i].diffuse[1], pPmdMaterials[i].diffuse[2], 0));
		XMStoreFloat4(&pModelData->m_materials[i].m_cbMaterialStruct.m_specular, XMVectorSet(pPmdMaterials[i].specular[0], pPmdMaterials[i].specular[1], pPmdMaterials[i].specular[2], 0));
		pModelData->m_materials[i].m_cbMaterialStruct.m_power = pPmdMaterials[i].specularity;
		pModelData->m_materials[i].m_cbMaterialStruct.m_isToonTex = 1;

		// �e�N�X�`���ǂݍ���
		filePath = filePathTemp;

		mbstowcs_s(nullptr, textureFileName, pPmdMaterials[i].textureFileName, sizeof(textureFileName));
		if (lstrcmpW(textureFileName, L"") == 0) {
			pModelData->m_materials[i].m_cbMaterialStruct.m_isTex = 0;
		} else {
			pModelData->m_materials[i].m_cbMaterialStruct.m_isTex = 1;

			filePath = filePath + textureFileName;
			filePath = filePath.substr(0, filePath.find_last_of(L"*"));

			UINT index = D3D11ResourceManager.LoadTexture(pDevice, filePath.c_str());
			if (index == -1) {
				return false;
			}
			pModelData->m_materials[i].m_textureIndices.push_back(index);

			// Sampler state
			// SamplerState��Texture�\���̂ɂ܂Ƃ߂�
			//index = CD3D11ResourceManager.CreateSamplerState(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
			//if (index == -1) {
			//	return false;
			//}
			//pModelData->m_materials[i].m_samplerStateIndices.push_back(index);
		}

		// Index location.
		if (i == 0) {
			pModelData->m_pStartIndexLocations.get()[i] = 0;
			pModelData->m_pIndexCounts.get()[i] = pPmdMaterials[i].faceVertCnt - 3;
		} else {
			pModelData->m_pStartIndexLocations.get()[i] = pPmdMaterials[i - 1].faceVertCnt + pModelData->m_pStartIndexLocations.get()[i - 1];
			pModelData->m_pIndexCounts.get()[i] = pPmdMaterials[i].faceVertCnt;
		}

		// �萔�o�b�t�@�쐬
		hr = pModelData->m_materials[i].m_cbEveryMaterial.CreateConstantBuffer(pDevice, 3, sizeof(s_cbBmdMaterial));
		// �o�b�t�@���Ȃ��Ă����[�h�͑�����
	}
	// E---------- �}�e���A���쐬 ----------


	return true;
}
