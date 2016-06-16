#pragma once
#include<fbxsdk.h>
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

#pragma comment(lib,"libfbxsdk-md.lib")

class CD3D11FBXModelLoader
{
private:
	std::vector<int*>	m_pIndices;		// ノード毎のメッシュのインデックス配列の先頭ポインタ
	std::vector<int>	m_indexCounts;	// ノード毎のメッシュのインデックス数
	std::vector<SBmdSkineMeshVertex*>	m_pVertices;	// ノード毎のメッシュの頂点配列の先頭ポインタ
	std::vector<int>	m_vertexCounts;	// ノード毎のメッシュの頂点の数

public:
	CD3D11FBXModelLoader();
	~CD3D11FBXModelLoader();

	bool Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);

private:

	bool LoadMesh(std::shared_ptr<ID3D11Device> pDevice, FbxMesh* pMesh, std::shared_ptr<CD3D11DrawModel3D> pOut);
	void GetVertex(FbxMesh* pMesh);
};

