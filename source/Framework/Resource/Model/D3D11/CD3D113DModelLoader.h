#pragma once

#include<vector>
#include<DirectXMath.h>
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"
#include"Framework\Resource\Model\PmdDataStruct.h"

// TODO �f�[�^���[�_������āAD3D11�̌`���ɕϊ�����p�̃N���X������ق����ǂ�
/*!
 * @brief    3D���f���̃��[�_
 * @note     ����.pmd�̂݁B���̊g���q�̓N���X�������鎖
 * @author   N.kaji
 * @date     2016/05/27
 */
class CD3D113DModelLoader
{
public:
	CD3D113DModelLoader();
	~CD3D113DModelLoader();

	bool Load(std::shared_ptr<ID3D11Device> pDevice, std::wstring pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);
	bool ConvertD3D11DrawModel(std::shared_ptr<ID3D11Device> pDevice, SStaticModelData* pModelData, CD3D11DrawModel3D* pOut);

private:
	bool	LoadFromPMDFile(std::ifstream& ifStream, SStaticModelData* pOut);
	bool	LoadFromOBJFile(std::ifstream& ifStream, SStaticModelData* pOut);
};



