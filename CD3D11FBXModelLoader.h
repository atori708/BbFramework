#pragma once
#include<fbxsdk.h>
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

#pragma comment(lib,"libfbxsdk-mt.lib")

class CD3D11FBXModelLoader
{
public:
	CD3D11FBXModelLoader();
	~CD3D11FBXModelLoader();

	bool Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);

private:

	bool LoadMesh(std::shared_ptr<ID3D11Device> pDevice, FbxMesh* pMesh, std::shared_ptr<CD3D11DrawModel3D> pOut);

};

