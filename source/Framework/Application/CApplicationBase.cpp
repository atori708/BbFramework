#include "CApplicationBase.h"
#include<wchar.h>

CApplicationBase::CApplicationBase(wchar_t* pApplicationName)
{
	if( wcslen( pApplicationName) > WINDOW_NAME_STR_LEN-6){	// "_DEBUG"�ƏI�[�������l������-6����
		return;
	}

	wcscpy_s(m_applicationName, pApplicationName);
}

CApplicationBase::~CApplicationBase()
{
}
