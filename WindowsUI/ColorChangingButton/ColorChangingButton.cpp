#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <WindowsX.h>

#include <sstream>

using std::wstringstream;
using std::stringstream;
using std::endl;

wchar_t* g_ClassName = L"ColorChangingButton";
HINSTANCE g_instance = nullptr;
const int ID_BUTTON = 1001;
COLORREF  g_clrButton = RGB(255, 255, 0);
WNDPROC g_loldBtnProc = 0;
bool g_bMouseTrack = true;
#define WM_REFRESHCHILD WM_USER+100


LRESULT onCtlColorBtn(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( (HWND)lParam == GetDlgItem(hWnd, ID_BUTTON) )
	{
		wstringstream ss;
		ss<<__FUNCTION__<<__LINE__;
		OutputDebugString(ss.str().c_str());
		HWND hbn = (HWND)lParam;
		HDC hdc = (HDC)(wParam);
		RECT rc;
		TCHAR text[64];
		GetWindowText(hbn, text, 63);
		GetClientRect(hbn, &rc);
		SetTextColor(hdc, RGB(0, 0, 255));//设置按钮上文本的颜色
		SetBkMode(hdc, TRANSPARENT);
		DrawText(hdc, text, _tcslen(text), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		return (INT_PTR)CreateSolidBrush(g_clrButton);
	}
	return 0;
}

void onMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int x = GET_X_LPARAM(lParam);
	int y = GET_Y_LPARAM(lParam);
	HWND hBtn = GetDlgItem(hWnd, ID_BUTTON);
	RECT rect;
	GetWindowRect(hBtn, &rect);	
	POINT point = {x, y};
	stringstream ssA;
	ssA<<point.x<<"  "<<point.y;
	OutputDebugStringA(ssA.str().c_str());
	BOOL bRet = ClientToScreen(hWnd, &point);
	stringstream ss;
	ss<<"Point: "<<point.x<<"  "<<point.y<< "  rect: "<<rect.left<<" "<<rect.top<<" "<<rect.right<<"  "<<rect.bottom<<endl;
	OutputDebugStringA(ss.str().c_str());
// 	if ( PtInRect( &rect, point) )	//当在 button里面时， 消息是在button的处理函数里边的， 不会到主界面
// 	{
// 		if ( MK_LBUTTON == wParam )
// 		{
// 			g_clrButton = RGB( 0, 255, 255 );
// 		}
// 		else
// 		{
// 			g_clrButton = RGB(200, 0, 0);
// 		}	
// 	}
// 	else
	{
		g_clrButton = RGB(255, 255, 0);
	}
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_MOUSEMOVE )
	{		
		if ( MK_LBUTTON == wParam )
		{
			g_clrButton = RGB( 0, 255, 255 );
		}
		else
		{
			g_clrButton = RGB(200, 0, 0);
		}
	}
	else
	{
		return g_loldBtnProc(hWnd, message, wParam, lParam);
	}
	return g_loldBtnProc(hWnd, message, wParam, lParam);

}


LRESULT CALLBACK wndproc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_REFRESHCHILD:
		{
			HWND hChild = (HWND)GetDlgItem(hWnd, ID_BUTTON);
			if ( hChild )
			{
				InvalidateRect(hChild, NULL, TRUE);
			}
			break;
		}
	case WM_CREATE:
		{
			HWND hChild = CreateWindow( L"BUTTON", L"K", WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON, 20, 20, 50, 30, hWnd, (HMENU)ID_BUTTON, g_instance, NULL );			
			LONG lOld = SetWindowLong( hChild, GWL_STYLE, GetWindowLong(hChild, GWL_STYLE)|BS_OWNERDRAW );	
			//WNDPROC lOldProc = (WNDPROC)SetWindowLong( hChild, GWL_WNDPROC, (LONG)ChildProc);
			break;
		}
    case WM_COMMAND:
		{
			if ( (HANDLE)lParam == GetDlgItem(hWnd, ID_BUTTON) )
			{
				OutputDebugStringA("ss");
			}
			break;
		}
	case WM_CTLCOLORBTN:
		{
			LRESULT ret = onCtlColorBtn(hWnd, message, wParam, lParam);		
			if ( ret )
			{
				return ret;
			}
			break;
		}
	case WM_MOUSELEAVE:
		{
			stringstream ss;
			ss<<"   "<<__FUNCTION__<<"  "<<__LINE__<<" ";
			OutputDebugStringA(ss.str().c_str());
			break;
		}
	case WM_MOUSEMOVE:
		{
			onMouseMove(hWnd, message, wParam, lParam);			
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	WNDCLASSW WndClass = {0};
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	WndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	WndClass.hIcon = NULL;
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = wndproc;	
	WndClass.lpszClassName = g_ClassName;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	ATOM aret = RegisterClass( &WndClass );
	DWORD dwErr = GetLastError();

	HWND hWin = CreateWindow( g_ClassName, L"Hi", WS_OVERLAPPEDWINDOW,/* CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,*/ 100, 500, 500, 300, NULL, NULL, hInstance, 0 );
	dwErr = GetLastError();

	ShowWindow(hWin, SW_SHOW);
	UpdateWindow(hWin);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if ( msg.message == WM_MOUSEMOVE )
		{
			if ( g_bMouseTrack )
			{				
				TRACKMOUSEEVENT csTME;
				csTME.cbSize = sizeof(csTME);
				csTME.dwFlags = TME_LEAVE|TME_HOVER;
				csTME.hwndTrack = hWin;
				csTME.dwHoverTime = 10;
				TrackMouseEvent(&csTME);//开启windows的 WM_MOUSELEAVE WM_MOUSEHOVER 事件支持
				g_bMouseTrack = false;
			}
		}
		else if ( msg.message == WM_MOUSELEAVE )
		{
			g_bMouseTrack = true;
			g_clrButton = RGB(0, 255, 0);
			SendMessage(hWin, WM_REFRESHCHILD,0, 0);			
		}
		else if ( msg.message == WM_MOUSEHOVER )
		{
			stringstream ss;
			ss<<__FUNCTION__<<"  "<<__LINE__<<endl;
			OutputDebugStringA(ss.str().c_str());
			g_clrButton = RGB(255, 255, 0);
			SendMessage(hWin, WM_REFRESHCHILD,0, 0);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}