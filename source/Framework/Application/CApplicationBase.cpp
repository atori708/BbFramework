#include "CApplicationBase.h"
#include<wchar.h>

CApplicationBase::CApplicationBase(wchar_t* pApplicationName)
{
	if( wcslen( pApplicationName) > WINDOW_NAME_STR_LEN-6){	// "_DEBUG"‚ÆI’[•¶š‚ğl—¶‚µ‚Ä-6‚·‚é
		return;
	}

	wcscpy_s(m_applicationName, pApplicationName);
}

CApplicationBase::~CApplicationBase()
{
}
