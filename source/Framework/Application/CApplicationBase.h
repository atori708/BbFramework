#pragma once
#include"Framework\Utility\BBFWStructs.h"

class CApplicationBase abstract
{
protected:
	static const int WINDOW_NAME_STR_LEN = 32;

protected:
	wchar_t m_applicationName[WINDOW_NAME_STR_LEN];

public:
	CApplicationBase(wchar_t* pApplicationName);
	virtual ~CApplicationBase();

	virtual bool Initialize()abstract;
	virtual void Finalize()abstract;
};

