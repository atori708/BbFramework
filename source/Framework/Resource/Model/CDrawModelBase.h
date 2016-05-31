#pragma once
#include<memory>

class CGeometoryCreater;
class CD3D113DModelLoader;

class CDrawModelBase
{
friend CGeometoryCreater;
friend CD3D113DModelLoader;	// モデルの拡張子毎にローダー作るとここを増やす必要あるからfriendよりもセッター作ったほうがいいかも。

protected:
	unsigned int	m_numVertex;
	unsigned int	m_numIndex;
	std::shared_ptr<unsigned int>	m_pVertexStrides;
	std::shared_ptr<unsigned int>	m_pVertexOffsets;
	std::shared_ptr<unsigned int>	m_pStartIndexLocations;
	std::shared_ptr<unsigned int>	m_pIndexCounts;

	unsigned int	m_numMaterial;
public:
	CDrawModelBase();
	virtual ~CDrawModelBase();
};

