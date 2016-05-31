#include "CWindowsApplication.h"
#include<crtdbg.h>


CWindowsApplication::CWindowsApplication(wchar_t* pApplicationName)
	:CApplicationBase(pApplicationName)
{
	wcscpy_s(m_className, m_applicationName);
	wcscat_s(m_className, L"_bbfw");
	m_windowSize.width = 256;
	m_windowSize.height = 256;
}

CWindowsApplication::~CWindowsApplication()
{
}

// 初期化
bool CWindowsApplication::Initialize()
{
	WNDCLASSEX wc;

	this->SetWndClass(&wc, m_hInstance);

	// デバッグモードであることをウィンドウに明記する
#ifdef _DEBUG
	lstrcatW(m_applicationName, L"[Debug]");
#endif // ONE_DEBUG

	m_hWnd = this->CreateMainWindow(m_hInstance, &wc);

	if (!m_hWnd) {
		//CheckBoolResult(DEBUG_LOG_TYPE_CREATE, false, L"Window");
		return false;
	} else {
		//CheckBoolResult(DEBUG_LOG_TYPE_CREATE, true, L"Window");
	}

	ShowWindow(m_hWnd, m_nCmdShow);
	UpdateWindow(m_hWnd);

	this->SetClientSize();// クライアント領域設定

	ValidateRect(m_hWnd, nullptr);	// WM_PAINTが呼ばれないようにする

	timeBeginPeriod(1);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// メモリリーク検出

	return true;
}

void CWindowsApplication::Finalize()
{
	timeEndPeriod(1);
}

void CWindowsApplication::SetWindowsCommandLine(HINSTANCE hInstance, LPSTR pCmdLine, int nCmdShow)
{
	m_hInstance = hInstance;
	m_pCmdLines = pCmdLine;
	m_nCmdShow = nCmdShow;
}

void CWindowsApplication::SetWindowSize(int width, int height, bool isFullscreen)
{
	m_windowSize.width = width;
	m_windowSize.height = height;
	m_isFullscreen = isFullscreen;
}

void CWindowsApplication::SetWindowSize(WINDOW_SIZE_TYPE sizeType, bool isFullscreen)
{
	m_windowSizeType = sizeType;
	switch (sizeType) {
	case WINDOW_SIZE_TYPE::SIZE_640x480:
		this->SetWindowSize(640, 480, isFullscreen);
		break;

	case WINDOW_SIZE_TYPE::SIZE_800x600:
		this->SetWindowSize(800, 600, isFullscreen);
		break;

	case WINDOW_SIZE_TYPE::SIZE_1280x720:
		this->SetWindowSize(1280, 720, isFullscreen);
		break;

	case WINDOW_SIZE_TYPE::SIZE_1980x1080:
		this->SetWindowSize(1980, 1080, isFullscreen);
		break;

	default:
		m_windowSizeType = WINDOW_SIZE_TYPE::SIZE_FREE;
		break;
	}
}

void CWindowsApplication::SetWindowStyle(UINT style)
{
}

// ウィンドウを作る
HWND CWindowsApplication::CreateMainWindow(HINSTANCE hInstance, WNDCLASSEX* pWndClass)
{
	HWND hWnd = 0;

	if (m_isFullscreen) {	// 仮想フルスクリーンで作る
		m_windowSize.width = GetSystemMetrics(SM_CXSCREEN);
		m_windowSize.height = GetSystemMetrics(SM_CYSCREEN);
		m_isFullscreen = true;
		m_hWnd = CreateWindow(pWndClass->lpszClassName, m_applicationName,
			WS_VISIBLE | WS_POPUP, 0, 0,
			m_windowSize.width, m_windowSize.height,
			nullptr, nullptr,
			hInstance,
			nullptr);
	} else {	// ウィンドウサイズで作る
		hWnd = CreateWindow(pWndClass->lpszClassName, m_applicationName,
			WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
			m_windowSize.width, m_windowSize.height,
			nullptr, nullptr,
			hInstance,
			nullptr);
	}

	return hWnd;
}

// クライアント座標を作る
void	CWindowsApplication::SetClientSize()
{
	RECT clientSize;
	GetClientRect(m_hWnd, &clientSize);
	m_clientSize.width = clientSize.right;
	m_clientSize.height = clientSize.bottom;
}

// ウィンドウクラスの作成
void	CWindowsApplication::SetWndClass(WNDCLASSEX* pOut, HINSTANCE hInstance)
{
	if (pOut == nullptr) {
		return;
	}

	pOut->cbSize = sizeof(WNDCLASSEX);
	pOut->style = CS_HREDRAW | CS_VREDRAW;
	pOut->hInstance = hInstance;
	pOut->lpszClassName = m_className;
	pOut->lpfnWndProc = (WNDPROC)WndProc;
	pOut->hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	pOut->hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	pOut->hCursor = LoadCursor(nullptr, IDC_ARROW);
	pOut->hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	pOut->lpszMenuName = nullptr;
	pOut->cbClsExtra = 0;
	pOut->cbWndExtra = 0;

	RegisterClassEx(pOut);
	//if (!(RegisterClassEx(pOut))) {	// 設計書をWindowsに提出
	//	CheckBoolResult(DEBUG_LOG_TYPE_CREATE, false, L"MainWindow");
	//}
}

/*!
 * @brief    メッセージの送出・WM_QUITか見る
 * @return   WM_QUIT来たらtrue
 * @note     ゲームループのwhileの条件に使ってください
 * @author   N.kaji
 * @date     2016/05/22
 */
bool	CWindowsApplication::DispatchedWMQuitMessage(void)
{
	if (PeekMessage(&m_msg, nullptr, 0, 0, PM_REMOVE)) {
		if (m_msg.message == WM_QUIT) {
			return true;
		}

		TranslateMessage(&m_msg);
		DispatchMessage(&m_msg);
	}
	return false;
}


/*!
 * @brief    ウィンドウプロシージャ
 * @param    
 * @return   
 * @note     
 * @author   N.kaji
 * @date     2016/05/22
 */
LRESULT	CWindowsApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {

#ifdef ONE_DEBUG
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			DestroyWindow(hWnd);
		break;
#endif

	case WM_CLOSE:
		if (IDYES == MessageBox(hWnd, L"終了しますか？", L"確認", MB_YESNO))
			DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

// ここから以下Getterメソッド
HWND	CWindowsApplication::GetHWnd()const
{
	return m_hWnd;
}

HINSTANCE	CWindowsApplication::GetHInstance()const
{
	return m_hInstance;
}

const RECT_SIZE&	CWindowsApplication::GetWindowSize()const
{
	return m_windowSize;
}

const RECT_SIZE&	CWindowsApplication::GetClientSize()const
{
	return m_clientSize;
}

bool CWindowsApplication::IsFullscreen() const
{
	return m_isFullscreen;
}