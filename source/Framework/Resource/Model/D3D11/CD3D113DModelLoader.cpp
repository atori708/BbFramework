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

bool CD3D113DModelLoader::Load(std::shared_ptr<ID3D11Device> pDevice, std::wstring filePath, std::shared_ptr<CD3D11DrawModel3D> pOut)
{
	assert(pDevice);
	if (pOut == nullptr) {
		return false;
	}

	std::ifstream ifs(filePath.c_str(), std::ios::binary);
	if (!ifs) {
		return false;
	}

	pOut->m_filePath = filePath.c_str();
	pOut->m_fileName = filePath.substr(filePath.find_last_of( L"/")+1);
	std::wstring fileExtension = filePath.substr(filePath.find_last_of(L".") + 1);
	
	SStaticModelData* pModelData = new SStaticModelData;

	if (fileExtension == L"pmd") {
		if (!this->LoadFromPMDFile(ifs, pModelData)) {
			return false;
		}
	} else if (fileExtension == L"obj") {
		if (!this->LoadFromOBJFile(ifs, pModelData)) {

		}
	}

	if (!this->ConvertD3D11DrawModel(pDevice, pModelData, pOut.get())) {
		return false;
	}
	SafeDelete(pModelData);

	//pOut->m_canRendering = true;

	return true;
}

/*!
* @brief    PMD�t�@�C����Bmd�`���ɂ���
* @param    ifStream �t�@�C���X�g���[��
* @param    pOut  ����ɏ��͂���
* @return   ���������Ȃ�
* @note     
* @author   N.kaji
* @date     2016/06/17
*/
bool CD3D113DModelLoader::LoadFromPMDFile(std::ifstream & ifStream, SStaticModelData* pOut)
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
	ifStream.read((char*)&pOut->m_vertexCount, sizeof(UINT));
	pPmdVertices = new PmdVertex[pOut->m_vertexCount];
	ifStream.read((char*)pPmdVertices, sizeof(PmdVertex) * pOut->m_vertexCount);

	// ��
	ifStream.read((char*)&pOut->m_allIndexCount, sizeof(UINT));
	pPmdFaces = new USHORT[pOut->m_allIndexCount];
	ifStream.read((char*)pPmdFaces, sizeof(USHORT) * pOut->m_allIndexCount);

	// �}�e���A��
	ifStream.read((char*)&pOut->m_materialCount, sizeof(UINT));
	pPmdMaterials = new PmdMaterial[pOut->m_materialCount];
	ifStream.read((char*)pPmdMaterials, sizeof(PmdMaterial) * pOut->m_materialCount);

	using namespace DirectX;

	// S---------- Bmd�`���ɕϊ�
	// ���_���ڂ��ւ���
	pOut->m_pVertices = new SBmdStaticMeshVertex[pOut->m_vertexCount];
	pOut->m_vertexSize = sizeof(SBmdStaticMeshVertex);
	for (int i = 0; i < pOut->m_vertexCount; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (j == 0 || j == 1) {
				pOut->m_pVertices[i].texCoord[j] = pPmdVertices[i].uv[j];
				//pBmdVertex[i].boneIndex[j] = pPmdVertices[j].boneIndex[j];
			}
			if (j != 3) {
				pOut->m_pVertices[i].pos[j] = pPmdVertices[i].pos[j];
				pOut->m_pVertices[i].normal[j] = pPmdVertices[i].normal[j];
			}
		}
		//pBmdVertex[i].boneWeight[0] = (float)pPmdVertices[i].boneWeight;
	}

	// �C���f�b�N�X��int�^��
	pOut->m_pIndices = new int[pOut->m_allIndexCount];
	for (int i = 0; i < pOut->m_allIndexCount; ++i) {
		pOut->m_pIndices[i] = (int)pPmdFaces[i];
	}

	// �}�e���A����Bmd�ɕϊ�
	pOut->m_pMaterials = new SBmdMaterial[pOut->m_materialCount];
	pOut->m_pIndexCounts = new int[pOut->m_materialCount];
	for (int i = 0; i < pOut->m_materialCount; ++i) {
		pOut->m_pMaterials[i].m_textureFileName = pPmdMaterials[i].textureFileName;
		XMStoreFloat4(&pOut->m_pMaterials[i].m_cbMaterialStruct.m_diffuse, XMVectorSet(pPmdMaterials[i].diffuse[0], pPmdMaterials[i].diffuse[1], pPmdMaterials[i].diffuse[2], 0));
		XMStoreFloat4(&pOut->m_pMaterials[i].m_cbMaterialStruct.m_specular, XMVectorSet(pPmdMaterials[i].specular[0], pPmdMaterials[i].specular[1], pPmdMaterials[i].specular[2], 0));
		pOut->m_pMaterials[i].m_cbMaterialStruct.m_specularPower = pPmdMaterials[i].specularity;

		// Index location.
		pOut->m_pIndexCounts[i] = pPmdMaterials[i].faceVertCnt;
	}
	// E---------- Bmd�`���ɕϊ�


	SafeDeleteArray(pPmdVertices);
	SafeDeleteArray(pPmdFaces);
	SafeDeleteArray(pPmdMaterials);
	return true;
}

/*!
 * @brief    OBJ�t�@�C����Bmd�`���ɂ���
 * @param    ifStream �t�@�C���X�g���[��
 * @param    pOut  ����ɏ��͂���
 * @return   ���������Ȃ�
 * @note     �O�p�`�|���S�������Ή����ĂȂ��B�}�e���A�������Ή��B
 * @author   N.kaji
 * @date     2016/06/17
 */
bool CD3D113DModelLoader::LoadFromOBJFile(std::ifstream & ifStream, SStaticModelData* pOut)
{
	std::string readingStr;

	struct Vec3
	{
		float x, y, z;
	};
	std::vector<Vec3> positions;
	std::vector<Vec3> normals;
	std::vector<int> indices;
	std::vector<int> normalIndices;
	while (!ifStream.eof()) {
		ifStream >> readingStr;
		//// �����Ȃ��A�R�����g�s�͖���
		//if ( readingStr == "\r" || readingStr == "#") {
		//	continue;
		//}

		// ���_
		if (readingStr == "v") {
			Vec3 position;
			ifStream >> position.x >> position.y >> position.z;
			positions.push_back(position);
		}

		// �@��
		if (readingStr == "vn") {
			Vec3 normal;
			ifStream >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}

		// �C���f�b�N�X
		if (readingStr == "f") {
			// �O�p�`�|���S���̂ݑΉ�
			for (int i = 0; i < 3; ++i) {
				std::string str;
				ifStream >> readingStr;
				size_t findFirst = readingStr.find_first_of("/");
				size_t findLast = readingStr.find_last_of("/");

				// ���_�C���f�b�N�X
				str = readingStr.substr(0, findFirst);
				int index = std::stoi(str);
				if (index > 0) {
					indices.push_back(index);
				} else {
					indices.push_back((int)positions.size() + index+1);
				}

				// �e�N�X�`�����W�C���f�b�N�X
				str = readingStr.substr(findFirst + 1, findLast - findFirst-1);
				
				// �@���C���f�b�N�X
				str = readingStr.substr(findLast + 1, readingStr.size()-1);
				index = std::stoi(str);
				if (index > 0) {
					normalIndices.push_back(index);
				} else {
					normalIndices.push_back((int)normals.size() + index+1);
				}
			}
		}
	}

	// S---------- Bmd�`���ɕϊ�
	// ���_
	int vertexCount = (int)positions.size();
	pOut->m_vertexCount = vertexCount;
 	SBmdStaticMeshVertex* pVertices = new SBmdStaticMeshVertex[vertexCount];
	for (int i = 0; i < vertexCount; ++i) {
		pVertices[i].pos[0] = positions[i].x;
		pVertices[i].pos[1] = positions[i].y;
		pVertices[i].pos[2] = positions[i].z;
	}

	// �@��
	int indexCount = (int)indices.size();
	for (int i = 0; i < indexCount; ++i) {
		pVertices[indices[i]-1].normal[0] = normals[normalIndices[i]-1].x;
		pVertices[indices[i]-1].normal[1] = normals[normalIndices[i]-1].y;
		pVertices[indices[i]-1].normal[2] = normals[normalIndices[i]-1].z;
	}
	pOut->m_pVertices = pVertices;
	pOut->m_vertexSize = sizeof(SBmdStaticMeshVertex);

	// �C���f�b�N�X
	pOut->m_allIndexCount = indexCount;
	int* pIndices = new int[indexCount];
	for (int i = 0; i < indexCount; ++i) {
		pIndices[i] = indices[i]-1;
	}
	pOut->m_pIndices = pIndices;


	// �}�e���A��
	pOut->m_materialCount = 0;
	// E---------- Bmd�`���ɕϊ�

	return true;
}

/*!
 * @brief    Bmd�`���̃f�[�^��D3D11�Ŏg����悤�ɕϊ�
 * @param    prm
 * @return   ����������
 * @note     
 * @author   N.kaji
 * @date     2016/06/16
 */
bool CD3D113DModelLoader::ConvertD3D11DrawModel(std::shared_ptr<ID3D11Device> pDevice, SStaticModelData * pModelData, CD3D11DrawModel3D * pOut)
{
	if (pModelData == nullptr || pOut == nullptr) {
		return false;
	}

	std::shared_ptr<UINT> stride(new UINT[1], std::default_delete<UINT>());
	//std::shared_ptr<UINT> stride;
	std::shared_ptr<UINT> offset(new UINT[1], std::default_delete<UINT>());
	//std::shared_ptr<UINT> offset;
	stride.get()[0] = (UINT)pModelData->m_vertexSize;
	offset.get()[0] = 0;
	pOut->m_pVertexStrides = stride;
	pOut->m_pVertexOffsets = offset;

	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA	subResourceData;
	// S---------- ���_�o�b�t�@�쐬
	bufferDesc.ByteWidth = (UINT)pModelData->m_vertexSize * pModelData->m_vertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	subResourceData.pSysMem = pModelData->m_pVertices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* pVertexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pVertexBuffer);
	if (FAILED(hr)) {
		return false;
	}
	pOut->m_pVertexBuffer.reset(pVertexBuffer, D3DComDeleter());
	// E---------- ���_�o�b�t�@�쐬
	pOut->m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	pOut->m_numVertex = pModelData->m_vertexCount;

	// S---------- �C���f�b�N�X�o�b�t�@�쐬 ----------
	bufferDesc.ByteWidth = sizeof(int) * pModelData->m_allIndexCount;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResourceData.pSysMem = pModelData->m_pIndices;

	ID3D11Buffer* pIndexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pIndexBuffer);
	if (FAILED(hr)) {
		pOut->m_pVertexBuffer = nullptr;
		return false;
	}
	pOut->m_pIndexBuffer.reset(pIndexBuffer, D3DComDeleter());
	pOut->m_indexFormat = DXGI_FORMAT_R32_UINT;
	// E---------- �C���f�b�N�X�o�b�t�@�쐬 ----------
	pOut->m_numIndex = pModelData->m_allIndexCount;

	// S---------- �}�e���A���쐬 ----------
	struct textureFileInfo
	{
		wchar_t m_fileName[20];
		int	m_resourceIndex;
	};

	using namespace DirectX;

	// �}�e���A�����Ȃ������甒����1����Ƃ�
	if (pModelData->m_materialCount == 0) {
		pOut->m_numMaterial = 1;
		pOut->m_materials.resize(1);

		pOut->SetVertexShaderIndex(0, 1);
		pOut->SetPixelShaderIndex(0, 1);
		pOut->SetInputLayoutIndex(0, 0);

		std::shared_ptr<UINT> pStartIndexLocations(new UINT[1], std::default_delete<UINT>());
		std::shared_ptr<UINT> pIndexCounts(new UINT[1], std::default_delete<UINT>());
		pOut->m_pStartIndexLocations = pStartIndexLocations;
		pOut->m_pIndexCounts = pIndexCounts;
		pOut->m_pStartIndexLocations.get()[0] = 0;
		pOut->m_pIndexCounts.get()[0] = pModelData->m_allIndexCount;

		XMStoreFloat4(&pOut->m_materials[0].m_cbMaterialStruct.m_diffuse, XMVectorSet(1, 1, 1, 0));
		XMStoreFloat4(&pOut->m_materials[0].m_cbMaterialStruct.m_specular, XMVectorSet(1, 1, 1, 0));
		pOut->m_materials[0].m_cbMaterialStruct.m_specularPower = 1;
		pOut->m_materials[0].m_cbMaterialStruct.m_isToonTex = 1;

		hr = pOut->m_materials[0].m_cbEveryMaterial.CreateConstantBuffer(pDevice, 3, sizeof(SConstantBufferBmdMaterial));
	} else {
		pOut->m_numMaterial = pModelData->m_materialCount;
		pOut->m_materials.resize(pModelData->m_materialCount);
		//pModelData->m_pStartIndexLocations
		//pModelData->m_indexCounts.resize( pModelData->m_numMaterial);
		std::wstring filePathTemp = pOut->m_filePath.substr(0, pOut->m_filePath.find_last_of(L"/") + 1);	// ���f���̂���f�B���N�g���̃p�X���擾
		std::wstring filePath;
		wchar_t textureFileName[20];
		textureFileInfo fileInfo;
		std::vector<textureFileInfo> fileInfos;	// ���̃��f���Ɏg���Ă�e�N�X�`�������ꎞ�ۑ�(�����e�N�X�`����ǂݍ��܂Ȃ�����)

		// �C���f�b�N�X�̋�؂���}�e���A�����m��
		//pModelData->m_pStartIndexLocations = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
		//pModelData->m_pIndexCounts = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
		std::shared_ptr<UINT> pStartIndexLocations(new UINT[pModelData->m_materialCount], std::default_delete<UINT>());
		std::shared_ptr<UINT> pIndexCounts(new UINT[pModelData->m_materialCount], std::default_delete<UINT>());
		pOut->m_pStartIndexLocations = pStartIndexLocations;
		pOut->m_pIndexCounts = pIndexCounts;

		for (int i = 0; i < pModelData->m_materialCount; ++i) {
			//pModelData->m_materials[i].m_cbMaterialStruct.m_diffuse = pPmdMaterials[i].diffuse;
			//pModelData->m_materials[i].m_cbMaterialStruct.m_specular = pPmdMaterials[i].specular;
			pOut->m_materials[i].m_cbMaterialStruct.m_diffuse = pModelData->m_pMaterials[i].m_cbMaterialStruct.m_diffuse;
			pOut->m_materials[i].m_cbMaterialStruct.m_specular = pModelData->m_pMaterials[i].m_cbMaterialStruct.m_specular;
			pOut->m_materials[i].m_cbMaterialStruct.m_specularPower = pModelData->m_pMaterials[i].m_cbMaterialStruct.m_specularPower;
			pOut->m_materials[i].m_cbMaterialStruct.m_isToonTex = 1;

			// S---------- �e�N�X�`���ǂݍ���
			// ���C�h�����ɕϊ�
			mbstowcs_s(nullptr, textureFileName, pModelData->m_pMaterials[i].m_textureFileName.c_str(), sizeof(textureFileName));
			wcsncpy_s(textureFileName, textureFileName, wcschr(textureFileName, L'*') - textureFileName);	// PMD�ɂ�'*'�ŕʃe�N�X�`��������@�\�����邽�ߍ폜

			// ���[�h�ς̂��̂Ɠ������O�̃e�N�X�`�����Ȃ������ׂ�
			bool isTextureAlreadyExist = false;
			int matchTextureIndex = 0;
			for (auto textureFileInfo : fileInfos) {
				if (lstrcmpW(textureFileInfo.m_fileName, textureFileName) == 0) {
					isTextureAlreadyExist = true;
					matchTextureIndex = textureFileInfo.m_resourceIndex;
					break;
				}
			}

			bool isTextureExist = false;
			if (isTextureAlreadyExist) {
				// ���łɃ��[�h�ς݂̃e�N�X�`���Ȃ̂ŃC���f�b�N�X�����炤
				pOut->m_materials[i].m_textureIndices.push_back(matchTextureIndex);
				isTextureExist = true;
			} else {
				// �V�����e�N�X�`���Ȃ̂œǂݍ���
				filePath = filePathTemp;
				if (lstrcmpW(textureFileName, L"") == 0) {
					// �e�N�X�`�����Ȃ�
					isTextureExist = false;
				} else {
					isTextureExist = true;
					filePath = filePath.substr(0, filePath.find_last_of(L"*"));
					UINT index = D3D11ResourceManager.LoadTexture(pDevice, filePath, textureFileName);
					if (index == -1) {
						return false;
					}

					pOut->m_materials[i].m_textureIndices.push_back(index);
					lstrcpyW(fileInfo.m_fileName, textureFileName);
					fileInfo.m_resourceIndex = index;
					fileInfos.push_back(fileInfo);
				}
			}
			// E---------- �e�N�X�`���ǂݍ���

			// �����o�[�g�g�U�݂̂̃V�F�[�_���Z�b�g
			if (isTextureExist) {
				pOut->SetVertexShaderIndex(i, 0);
				pOut->SetPixelShaderIndex(i, 0);
			} else {
				pOut->SetVertexShaderIndex(i, 1);
				pOut->SetPixelShaderIndex(i, 1);
			}
			pOut->SetInputLayoutIndex(i, 0);

			// Index location.
			if (i == 0) {
				pOut->m_pStartIndexLocations.get()[i] = 0;
				pOut->m_pIndexCounts.get()[i] = pModelData->m_pIndexCounts[i];
			} else {
				pOut->m_pStartIndexLocations.get()[i] = pModelData->m_pIndexCounts[i - 1] + pOut->m_pStartIndexLocations.get()[i - 1];
				pOut->m_pIndexCounts.get()[i] = pModelData->m_pIndexCounts[i];
			}

			// �萔�o�b�t�@�쐬(�o�b�t�@���Ȃ��Ă����[�h�͑�����)
			hr = pOut->m_materials[i].m_cbEveryMaterial.CreateConstantBuffer(pDevice, 3, sizeof(SConstantBufferBmdMaterial));
		}
	}
	// E---------- �}�e���A���쐬 ----------


	return true;
}
