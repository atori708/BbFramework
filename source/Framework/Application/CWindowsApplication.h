#include<Windows.h>
#include "CApplicationBase.h"

#pragma comment (lib, "winmm.lib")
#pragma once

class CWindowsApplication
	:public CApplicationBase
{
private:
	HWND m_hWnd;
	// WinMainから受け取る変数
	HINSTANCE m_hInstance;
	int m_nCmdShow;
	LPSTR m_pCmdLines;

	// ウィンドウの情報
	WINDOW_SIZE_TYPE m_windowSizeType = WINDOW_SIZE_TYPE::SIZE_FREE;
	wchar_t m_className[WINDOW_NAME_STR_LEN + 5];	//"_bbfw"の5文字確保
	RECT_SIZE m_windowSize;
	RECT_SIZE m_clientSize;
	bool m_isFullscreen;

	//　ウィンドウメッセージ
	tagMSG m_msg;


public:
	CWindowsApplication(wchar_t* pApplicationName);
	~CWindowsApplication();

	bool Initialize()override;
	void Finalize()override;

	bool DispatchedWMQuitMessage(void);

	void SetWindowsCommandLine(HINSTANCE hInstance, LPSTR pCmdLine, int nCmdShow);
	void SetWindowSize( int width, int height, bool isFullscreen = false);
	void SetWindowSize( WINDOW_SIZE_TYPE sizeType, bool isFullscreen = false);
	void SetWindowStyle( UINT style);

	// ゲッター
	HWND GetHWnd() const;
	HINSTANCE GetHInstance() const;
	const RECT_SIZE& GetWindowSize() const;
	const RECT_SIZE& GetClientSize() const;
	bool IsFullscreen() const;

private:
	HWND CreateMainWindow(HINSTANCE hInstance, WNDCLASSEX * pWndClass);

	void SetClientSize();
	void SetWndClass(WNDCLASSEX * pOut, HINSTANCE hInstance);

	static LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	

};

