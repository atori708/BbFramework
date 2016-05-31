#pragma once
#include"Framework\Application\CApplicationBase.h"

class CGraphicsBase
{
protected:
	const RECT_SIZE*	m_clientSize;

public:
	CGraphicsBase();
	~CGraphicsBase();

	virtual bool Initialize(const CApplicationBase* pApplication )abstract;
	virtual void Finalize()abstract;
};

