#ifndef	__SAFE_DELETE_H__ONE__
#define	__SAFE_DELETE_H__ONE__

#include<Windows.h>

// Instが、Tを継承しているかを判定する
// (Javaのinstanceofと同機能)
template<typename T, typename Inst>
inline bool InstanceOf( const Inst& inst)
{
	return dynamic_cast<const T*>(inst) != 0;
}

// newされたものをdeleteする
template<typename T>
inline void	SafeDelete(T& ptr)
{
	if(ptr != nullptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

// newした配列をdeleteする
template<typename T>
inline void	SafeDeleteArray(T& ptr)
{
	if(ptr != nullptr)
	{
		delete[] ptr;
		ptr = nullptr;
	}
}

// mallocされたものをfreeする
template<typename T>
inline void  SafeFree(T& ptr)
{
	if(ptr != nullptr)
	{
		free(ptr);
		ptr = nullptr;
	}
}

// DirectXのCOMインスタンスをReleaseする
template<typename T>
inline void SafeRelease(T& ptr)
{
	if( InstanceOf<IUnknown>(ptr) )
	{
		if(ptr != nullptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
}

// shared_ptr用のデリータ
struct com_deleter
{
    void operator()( IUnknown* p )
    {
        p->Release();
    }
};

// HANDLEをcloseする
//template<typename T>
//inline BOOL	SafeCloseHandle(T& handle)
//{
//	BOOL ok = false;
//
//	if( handle != 0 )
//	{
//		ok = CloseHandle(handle);
//		if( ok )
//			handle = 0;
//		else
//			return false;
//	}
//
//	return ok;
//}

#endif