#include "CApplicationBase.h"
#include<wchar.h>

CApplicationBase::CApplicationBase(wchar_t* pApplicationName)
{
	if( wcslen( pApplicationName) > WINDOW_NAME_STR_LEN-6){	// "_DEBUG"と終端文字を考慮して-6する
		return;
	}

	wcscpy_s(m_applicationName, pApplicationName);
}

CApplicationBase::~CApplicationBase()
{
}
