#pragma once

#include<vector>
#include<DirectXMath.h>

const float	TRANSFORM_PMD_TO_X = 0.1f;	// �傫����DirectX�ɍ��킹��(MMD�͈�ڐ���5�Ȃ̂�)


// �A���C�����g��1�o�C�g��
#pragma pack(push, 1)
// S---------- PMD�f�[�^�ꎞ�ۑ��p�\����
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
	unsigned long faceVertCnt;	// ���̍ޗ��̖ʒ��_�� �� �ޗ��ԍ�i�̃|���S���ԍ��F pmdMaterial[i - 1].face_vert_count/3 ~ pmdMaterial[i].face_vert_count/3 - 1
	char textureFileName[20];
};
struct PmdBone
{
	char boneName[20];
	unsigned short parentBoneIndex;		// �Ȃ��ꍇ��0xFFFF
	unsigned short tailPosBoneIndex;	// �Ȃ��ꍇ��0xFFFF
	unsigned char boneType;				// 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� (8, 9��MMD4.0�ȍ~)
	unsigned short ikParentBoneIndex;	// �Ȃ��ꍇ��0
	float boneHeadPos[3];
};
struct PmdIkDataWithoutArray
{
	unsigned short ikBoneIndex;			// IK�{�[���ԍ�
	unsigned short ikTargetBoneIndex;	// IK�{�[���̈ʒu�ɂ��̃{�[������v������悤��IK�������s����
	unsigned char ikChainLength;		// IK�`�F�[���̒���
	unsigned short iterations;			// �ċA���Z��
	float controlWeight;				// �{�[���̒P�ʐ����p 1.0 �� 4.0[rad]�B�܂��u�Ђ��v���܂ރ{�[���������{�[����X�������ɂ��������Ȃ�����������B
// unsigned short ik_child_bone_index[ik_chain_length]; // IK�e�����̃{�[���ԍ� (�z��̑傫�����ω�)
};
struct PmdIkData : public PmdIkDataWithoutArray
{
	std::vector<unsigned short> ikChildBoneIndex;
};
// E---------- PMD�f�[�^�ꎞ�ۑ��p�\����

// VMD�\���̒�`
// �w�b�_
struct VmdHeader
{
	char vmdHeader[30];
	char vmdModelName[20];
};

// �{�f�B(���ꂪ�A���ő��݂���)
struct VmdMotion
{
	char boneName[15];
	unsigned long frameNo;
	float location[3];	// �ړ���
	float rotation[4];	// ���f�����[�J�����W�n
	unsigned char interpolation[64];
};
#pragma pack(pop)
// �A���C�����g��߂�

//// �A���C�����g��1�o�C�g���ƕ`�掞�ɃG���[�f���̂ŁA��x������Ɉڂ�
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
	std::string			boneName;		// �{�[����
	unsigned long		frameNo;			// �t���[���ԍ�
	DirectX::XMFLOAT3	position;			// �ʒu
	DirectX::XMVECTOR	rotation;			// ��]
	DirectX::XMFLOAT2	interpolationX[2];	// position.x�̃x�W�F�⊮�p�����[�^
	DirectX::XMFLOAT2	interpolationY[2];	// position.y�̃x�W�F�⊮�p�����[�^
	DirectX::XMFLOAT2	interpolationZ[2];	// position.z�̃x�W�F�⊮�p�����[�^
	DirectX::XMFLOAT2	interpolationR[2];	// rotation�̃x�W�F�⊮�p�����[�^

	// �t���[���ԍ��Ń\�[�g���邽�߂̔�r���Z�q
	bool operator < (const _KEY_FRAME &k) const
	{
		return frameNo < k.frameNo;
	}
}KEY_FRAME;

#include"Framework\Resource\Model\D3D11\CD3D11DrawModel3D.h"

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

	bool Load(std::shared_ptr<ID3D11Device> pDevice, const wchar_t* pFilePath, std::shared_ptr<CD3D11DrawModel3D> pOut);

private:
	bool	LoadFromPMDFile(std::shared_ptr<ID3D11Device> pDevice, std::ifstream& ifStream, std::shared_ptr<CD3D11DrawModel3D> pOut);
	bool	CreateModelDataFromPMD(std::shared_ptr<ID3D11Device> pDevice, PmdVertex* pPmdVertices, USHORT* pPmdFaces, PmdMaterial* pPmdMaterials, std::shared_ptr<CD3D11DrawModel3D> pModelData);
};

