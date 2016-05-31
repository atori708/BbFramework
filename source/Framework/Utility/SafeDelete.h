#ifndef	__SAFE_DELETE_H__ONE__
#define	__SAFE_DELETE_H__ONE__

#include<Windows.h>

// Inst���AT���p�����Ă��邩�𔻒肷��
// (Java��instanceof�Ɠ��@�\)
template<typename T, typename Inst>
inline bool InstanceOf( const Inst& inst)
{
	return dynamic_cast<const T*>(inst) != 0;
}

// new���ꂽ���̂�delete����
template<typename T>
inline void	SafeDelete(T& ptr)
{
	if(ptr != nullptr)
	{
		delete ptr;
		ptr = nullptr;
	}
}

// new�����z���delete����
template<typename T>
inline void	SafeDeleteArray(T& ptr)
{
	if(ptr != nullptr)
	{
		delete[] ptr;
		ptr = nullptr;
	}
}

// malloc���ꂽ���̂�free����
template<typename T>
inline void  SafeFree(T& ptr)
{
	if(ptr != nullptr)
	{
		free(ptr);
		ptr = nullptr;
	}
}

// DirectX��COM�C���X�^���X��Release����
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

// shared_ptr�p�̃f���[�^
struct com_deleter
{
    void operator()( IUnknown* p )
    {
        p->Release();
    }
};

// HANDLE��close����
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