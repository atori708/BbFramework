#pragma once

#include<vector>
#include<DirectXMath.h>

const float	TRANSFORM_PMD_TO_X = 0.1f;	// 大きさをDirectXに合わせる(MMDは一目盛り5なので)


// アライメントを1バイトに
#pragma pack(push, 1)
// S---------- PMDデータ一時保存用構造体
struct PmdHeader
{
	char magic[3];
	float version;
	char modelName[20];
	char comment[256];
};
struct PmdVertex
{
	float pos[3];
	float normal[3];
	float uv[2];
	unsigned short boneIndex[2];
	unsigned char boneWeight;
	unsigned char isEdge;
};
struct PmdMaterial
{
	float diffuse[3];
	float alpha;
	float specularity;
	float specular[3];
	float mirror[3];
	unsigned char toonIndex;
	unsigned char isEdge;
	unsigned long faceVertCnt;	// この材料の面頂点数 → 材料番号iのポリゴン番号： pmdMaterial[i - 1].face_vert_count/3 ~ pmdMaterial[i].face_vert_count/3 - 1
	char textureFileName[20];
};
struct PmdBone
{
	char boneName[20];
	unsigned short parentBoneIndex;		// ない場合は0xFFFF
	unsigned short tailPosBoneIndex;	// ない場合は0xFFFF
	unsigned char boneType;				// 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 (8, 9はMMD4.0以降)
	unsigned short ikParentBoneIndex;	// ない場合は0
	float boneHeadPos[3];
};
struct PmdIkDataWithoutArray
{
	unsigned short ikBoneIndex;			// IKボーン番号
	unsigned short ikTargetBoneIndex;	// IKボーンの位置にこのボーンを一致させるようにIK処理が行われる
	unsigned char ikChainLength;		// IKチェーンの長さ
	unsigned short iterations;			// 再帰演算回数
	float controlWeight;				// ボーンの単位制限角 1.0 → 4.0[rad]。また「ひざ」を含むボーン名をもつボーンはX軸方向にしか動かない制限がある。
// unsigned short ik_child_bone_index[ik_chain_length]; // IK影響下のボーン番号 (配列の大きさが変化)
};
struct PmdIkData : public PmdIkDataWithoutArray
{
	std::vector<unsigned short> ikChildBoneIndex;
};
// E---------- PMDデータ一時保存用構造体

// VMD構造体定義
// ヘッダ
struct VmdHeader
{
	char vmdHeader[30];
	char vmdModelName[20];
};

// ボディ(これが連続で存在する)
struct VmdMotion
{
	char boneName[15];
	unsigned long frameNo;
	float location[3];	// 移動量
	float rotation[4];	// モデルローカル座標系
	unsigned char interpolation[64];
};
#pragma pack(pop)
// アライメントを戻す

//// アライメントが1バイトだと描画時にエラー吐くので、一度こちらに移す
//struct _PmdVertex{
//	float pos[3];
//	float normal[3];
//	float uv[2];
//	unsigned short boneIndex[2];
//	unsigned char boneWeight;
//	unsigned char isEdge;
//};

typedef struct _KEY_FRAME
{
	std::string			boneName;		// ボーン名
	unsigned long		frameNo;			// フレーム番号
	DirectX::XMFLOAT3	position;			// 位置
	DirectX::XMVECTOR	rotation;			// 回転
	DirectX::XMFLOAT2	interpolationX[2];	// position.xのベジェ補完パラメータ
	DirectX::XMFLOAT2	interpolationY[2];	// position.yのベジェ補完パラメータ
	DirectX::XMFLOAT2	interpolationZ[2];	// position.zのベジェ補完パラメータ
	DirectX::XMFLOAT2	interpolationR[2];	// rotationのベジェ補完パラメータ

	// フレーム番号でソートするための比較演算子
	bool operator < (const _KEY_FRAME &k) const
	{
		return frameNo < k.frameNo;
	}
}KEY_FRAME;

#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

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

	bool Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);

private:
	bool	LoadFromPMDFile(std::shared_ptr<ID3D11Device> pDevice, std::ifstream& ifStream, std::shared_ptr<CD3D11DrawModel3D> pOut);
	bool	CreateModelDataFromPMD(std::shared_ptr<ID3D11Device> pDevice, PmdVertex* pPmdVertices, USHORT* pPmdFaces, PmdMaterial* pPmdMaterials, std::shared_ptr<CD3D11DrawModel3D> pModelData);
};

