#pragma once

#include<vector>
#include<DirectXMath.h>
#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"
#include"Framework\Resource\Model\PmdDataStruct.h"

// TODO データリーダを作って、D3D11の形式に変換する用のクラス作ったほうが良い
/*!
 * @brief    3Dモデルのローダ
 * @note     今は.pmdのみ。他の拡張子はクラス毎分ける事
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



