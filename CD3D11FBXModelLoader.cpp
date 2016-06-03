#include "CD3D11FBXModelLoader.h"
#include"Framework\Utility\D3d11Utility.h"
#include"Framework\Utility\SafeDelete.h"

CD3D11FBXModelLoader::CD3D11FBXModelLoader()
{
}


CD3D11FBXModelLoader::~CD3D11FBXModelLoader()
{
}

bool CD3D11FBXModelLoader::Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t * pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut)
{
	assert(pDevice);
	if (pFilePath == nullptr) {
		return false;
	}

	if (pOut == nullptr) {
		return false;
	}

	pOut->m_filePath = pFilePath;
	char pFileName[32];
	size_t ret;
	wcstombs_s(&ret, pFileName, strnlen_s(pFileName, sizeof( pFileName)), pFilePath, _TRUNCATE);
	//pOut->m_fileName = _filePath.substr( _filePath.find_last_of( L"/")+1);

	//if (!this->LoadFromPMDFile(pDevice, ifs, pOut)) {
	//	return false;
	//}

	FbxManager* fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");

	if (!importer->Initialize(pFileName, -1, fbxManager->GetIOSettings())) {
		fbxManager->Destroy();
		return false;
	}

	FbxScene* scene = FbxScene::Create(fbxManager, "myScene");
	importer->Import(scene);
	importer->Destroy();

	FbxNode* rootNode = scene->GetRootNode();
	if (rootNode) {

		// TODO:複数のノードにメッシュが入ってる場合があるので、全部まとめてからバッファを作る
		int childCount = rootNode->GetChildCount();
		for (int i = 0; i < rootNode->GetChildCount(); ++i) {
			//PrintTabs();
			FbxNode* childNode = rootNode->GetChild(i);
			FbxNodeAttribute* pAttribute = childNode->GetNodeAttribute();
			FbxNodeAttribute::EType attributeType;

			// 三角形ポリゴンだけにする
			FbxGeometryConverter converter(fbxManager);
			//pAttribute = converter.Triangulate(pAttribute, true);
			FbxMesh* mesh = childNode->GetMesh();
			const char* nodeName = childNode->GetName();

			if (mesh) {
				OutputDebugStringA(nodeName);
				this->LoadMesh(pDevice, mesh, pOut);
				mesh->Destroy();
			}
			//if (!childNode->GetNodeAttribute()) {
			//	continue;
			//}
			//pAttribute = childNode->GetNodeAttribute();
			//attributeType = pAttribute->GetAttributeType();
			//switch (attributeType) {
			//case FbxNodeAttribute::eMesh:
			//	mesh = pAttribute->get;
			//	break;

			//}

			printf("</node>\n");
		}
	}
	scene->Destroy();
	ioSettings->Destroy();
	fbxManager->Destroy();

	// シェーダのインデックスをセット
	for (UINT i = 0; i<pOut->GetNumMaterial(); ++i) {
		pOut->SetVertexShaderIndex(i, 0);
		pOut->SetPixelShaderIndex(i, 0);
		pOut->SetInputLayoutIndex(i, 0);
	}
	return true;
}

bool CD3D11FBXModelLoader::LoadMesh(std::shared_ptr<ID3D11Device> pDevice, FbxMesh * pMesh, std::shared_ptr<CD3D11DrawModel3D> pOut)
{


	int polygonCount = pMesh->GetPolygonCount();
	int indexCount = pMesh->GetPolygonVertexCount();
	int* pIndices = pMesh->GetPolygonVertices();
	int vertexCount = pMesh->GetControlPointsCount();   // 頂点数
	FbxVector4* pVertexPoses = pMesh->GetControlPoints();    // 頂点座標配列
	for (int polygon = 0; polygon < polygonCount; ++polygon) {
		int indexCountInPolygon = pMesh->GetPolygonSize(polygon);
		if (indexCountInPolygon == 3) {	// 三角形ポリゴンだけ読み込む
			for (int i = 0; i < indexCountInPolygon; ++i) {
				int index = pMesh->GetPolygonVertex(polygon, i);
			}
		}
	}
	size_t a = sizeof(double);
	std::shared_ptr<UINT> stride(new UINT[1], std::default_delete<UINT>());
	std::shared_ptr<UINT> offset(new UINT[1], std::default_delete<UINT>());
	//std::shared_ptr<UINT> offset;
	stride.get()[0] = sizeof(s_bmdSkineMeshVertex);
	offset.get()[0] = 0;
	pOut->m_pVertexStrides = stride;
	pOut->m_pVertexOffsets = offset;

	// キャスト
	s_bmdSkineMeshVertex* pVertices = new s_bmdSkineMeshVertex[vertexCount];
	for (int i = 0; i < vertexCount; ++i) {
		pVertices[i].pos[0] = (float)pVertexPoses[i].mData[0];
		pVertices[i].pos[1] = (float)pVertexPoses[i].mData[1];
		pVertices[i].pos[2] = (float)pVertexPoses[i].mData[2];
	}

	HRESULT hr;
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA	subResourceData;

	// S---------- 頂点バッファ作成 ----------
	bufferDesc.ByteWidth = sizeof(s_bmdSkineMeshVertex) * vertexCount;
	//bufferDesc.ByteWidth = sizeof(FbxVector4) * vertexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	subResourceData.pSysMem = pVertices;
	subResourceData.SysMemPitch = 0;
	subResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* pVertexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pVertexBuffer);
	//SafeDeleteArray(_pBbxVertex);
	if (FAILED(hr)) {
		return false;
	}
	pOut->m_pVertexBuffer.reset(pVertexBuffer, D3DComDeleter());
	SafeDeleteArray(pVertices);
	// E---------- 頂点バッファ作成 ----------
	pOut->m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

	// S---------- インデックスバッファ作成 ----------
	bufferDesc.ByteWidth = sizeof(int) * indexCount;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subResourceData.pSysMem = pIndices;

	ID3D11Buffer* pIndexBuffer;
	hr = pDevice->CreateBuffer(&bufferDesc, &subResourceData, &pIndexBuffer);
	if (FAILED(hr)) {
		pOut->m_pVertexBuffer = nullptr;
		return false;
	}
	pOut->m_pIndexBuffer.reset(pIndexBuffer, D3DComDeleter());
	pOut->m_indexFormat = DXGI_FORMAT_R32_UINT;
	// E---------- インデックスバッファ作成 ----------

	// S---------- マテリアル作成 ----------
	pOut->m_numMaterial = 1;
	pOut->m_materials.resize(pOut->m_numMaterial);
	//pModelData->m_pStartIndexLocations
	//pModelData->m_indexCounts.resize( pModelData->m_numMaterial);
	std::wstring filePathTemp = pOut->m_filePath.substr(0, pOut->m_filePath.find_last_of(L"/") + 1);	// モデルのあるディレクトリのパスを取得
	std::wstring filePath;
	wchar_t textureFileName[20];

	// インデックスの区切りをマテリアル分確保
	//pModelData->m_pStartIndexLocations = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
	//pModelData->m_pIndexCounts = (UINT*)malloc(sizeof(UINT) * pModelData->m_numMaterial);
	std::shared_ptr<UINT> pStartIndexLocations(new UINT[pOut->m_numMaterial], std::default_delete<UINT>());
	std::shared_ptr<UINT> pIndexCounts(new UINT[pOut->m_numMaterial], std::default_delete<UINT>());
	pOut->m_pStartIndexLocations = pStartIndexLocations;
	pOut->m_pIndexCounts = pIndexCounts;
	for (UINT i = 0; i<pOut->m_numMaterial; ++i) {
		using namespace DirectX;
		//pModelData->m_materials[i].m_cbMaterialStruct.m_diffuse = pPmdMaterials[i].diffuse;
		//pModelData->m_materials[i].m_cbMaterialStruct.m_specular = pPmdMaterials[i].specular;
		//XMStoreFloat4(&pOut->m_materials[i].m_cbMaterialStruct.m_diffuse, XMVectorSet(pPmdMaterials[i].diffuse[0], pPmdMaterials[i].diffuse[1], pPmdMaterials[i].diffuse[2], 0));
		//XMStoreFloat4(&pOut->m_materials[i].m_cbMaterialStruct.m_specular, XMVectorSet(pPmdMaterials[i].specular[0], pPmdMaterials[i].specular[1], pPmdMaterials[i].specular[2], 0));
		//pOut->m_materials[i].m_cbMaterialStruct.m_power = pPmdMaterials[i].specularity;
		//pOut->m_materials[i].m_cbMaterialStruct.m_isToonTex = 1;


		// Index location.
		if (i == 0) {
			pOut->m_pStartIndexLocations.get()[i] = 0;
			pOut->m_pIndexCounts.get()[i] = indexCount;
		}

		// 定数バッファ作成
		hr = pOut->m_materials[i].m_cbEveryMaterial.CreateConstantBuffer(pDevice, 3, sizeof(s_cbBmdMaterial));
		// バッファ作れなくてもロードは続ける
	}
	// E---------- マテリアル作成 ----------

	return false;
}
