#ifndef __TSTRING_H__ONE__
#define __TSTRING_H__ONE__


// tchar�ɑΉ�����string���`���Ă���
#include<string>
#include<tchar.h>

namespace std{
typedef basic_string<_TCHAR, char_traits<_TCHAR>, allocator<_TCHAR> > tstring;
}

#endif