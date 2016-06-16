#pragma once
#include<fbxsdk.h>
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

#pragma comment(lib,"libfbxsdk-md.lib")

class CD3D11FBXModelLoader
{
private:
	std::vector<int*>	m_pIndices;		// �m�[�h���̃��b�V���̃C���f�b�N�X�z��̐擪�|�C���^
	std::vector<int>	m_indexCounts;	// �m�[�h���̃��b�V���̃C���f�b�N�X��
	std::vector<SBmdSkineMeshVertex*>	m_pVertices;	// �m�[�h���̃��b�V���̒��_�z��̐擪�|�C���^
	std::vector<int>	m_vertexCounts;	// �m�[�h���̃��b�V���̒��_�̐�

public:
	CD3D11FBXModelLoader();
	~CD3D11FBXModelLoader();

	bool Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);

private:

	bool LoadMesh(std::shared_ptr<ID3D11Device> pDevice, FbxMesh* pMesh, std::shared_ptr<CD3D11DrawModel3D> pOut);
	void GetVertex(FbxMesh* pMesh);
};

