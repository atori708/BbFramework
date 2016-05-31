#ifndef __TSTRING_H__ONE__
#define __TSTRING_H__ONE__


// tchar‚É‘Î‰ž‚µ‚½string‚ð’è‹`‚µ‚Ä‚¢‚é
#include<string>
#include<tchar.h>

namespace std{
typedef basic_string<_TCHAR, char_traits<_TCHAR>, allocator<_TCHAR> > tstring;
}

#endif