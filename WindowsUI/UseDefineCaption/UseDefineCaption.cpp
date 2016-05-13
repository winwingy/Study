// UseDefineCaption.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "UseDefineCaption.h"
#include <WindowsX.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_USEDEFINECAPTION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_USEDEFINECAPTION));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_USEDEFINECAPTION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+2);
	wcex.lpszMenuName	= nullptr;//MAKEINTRESOURCE(IDC_USEDEFINECAPTION);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, 
       WS_POPUP|WS_CLIPCHILDREN | WS_CLIPSIBLINGS|WS_MINIMIZEBOX,
      100, 100, 500, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
    static bool lbtnDown = false;
    static POINT pointCursor = {0};
    int space = 10;
    static BOOL mouseEvent = FALSE;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
        {

		    hdc = BeginPaint(hWnd, &ps);
		    //Draw My Caption
            RECT rect = {0};
            GetClientRect(hWnd, &rect);

            {
                HBRUSH brush = CreateSolidBrush(RGB(0, 100, 125));
                HBRUSH oldBrush = SelectBrush(hdc, brush);
                HPEN pen = CreatePen(PS_NULL, 1, 0);
                HPEN oldPen = SelectPen(hdc, pen);

                Rectangle(hdc, 0, 0, rect.right, 30);

                SelectPen(hdc, oldPen);
                SelectBrush(hdc, oldBrush);
                DeleteBrush(brush);
                DeletePen(pen);
            }

            {
                HBRUSH brush = GetStockBrush(NULL_BRUSH);
                HBRUSH oldBrush = SelectBrush(hdc, brush);

                TCHAR* szText = L"Hello Cpation!";
                SIZE sizeText = {0};
                GetTextExtentPoint32(hdc, szText, _tcslen(szText), &sizeText); 

                HPEN penText = CreatePen(PS_NULL, 1, 0);
                RECT rectText = {5, (30 - sizeText.cy)/2,
                    5 + sizeText.cx, (30 - sizeText.cy)/2 + sizeText.cy};
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 255, 255));
                DrawText(hdc, szText, _tcslen(szText),
                    &rectText, DT_LEFT|DT_VCENTER|DT_CENTER);

                SelectBrush(hdc, oldBrush);            
            }

            EndPaint(hWnd, &ps);
		    break;
        }
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_LBUTTONDOWN:
        {
            lbtnDown = true;
            pointCursor.x = GET_X_LPARAM(lParam);
            pointCursor.y = GET_Y_LPARAM(lParam);
            break;
        }
    case WM_MOUSEHOVER:
        {
            OutputDebugString(L"WM_MOUSEHOVER\n");
            break;
        }
    case WM_MOUSELEAVE:
        {
            OutputDebugString(L"WM_MOUSELEAVE\n");
            mouseEvent = FALSE;
            break;
        }
    case WM_MOUSEMOVE:
        {
            OutputDebugString(L"WM_MOUSEMOVE\n");

            if (mouseEvent == FALSE)
            {
                TRACKMOUSEEVENT tse = {0};
                tse.cbSize = sizeof(tse);
                tse.dwFlags = TME_HOVER|TME_LEAVE;
                tse.hwndTrack = hWnd;
                mouseEvent = TrackMouseEvent(&tse);
            }

            if (wParam & MK_LBUTTON)
            {
                POINT ptNow = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};  
                POINT ptWinNow = ptNow;
                ClientToScreen(hWnd, &ptWinNow);
                RECT rect = {0};
                GetWindowRect(hWnd, &rect);
                POINT movept = {ptNow.x - pointCursor.x,
                    ptNow.y - pointCursor.y};
                ::OffsetRect(&rect, movept.x, movept.y);
                MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left,
                    rect.bottom - rect.top, TRUE);
            }
            break;
        }
    case WM_NCLBUTTONDOWN:
        {
            OutputDebugString(L"WM_NCLBUTTONDOWN\n");
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
    case WM_NCLBUTTONUP:
        {
            OutputDebugString(L"WM_NCLBUTTONUP\n");
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
        }
    case WM_NCMOUSEMOVE:
        {
            OutputDebugString(L"WM_NC  MOUSEMOVE\n");
            break;
        }
    case WM_NCHITTEST:
        {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT rect = {0};
            GetWindowRect(hWnd, &rect);
        
            if (::PtInRect(&rect, pt))
            {
                if (pt.x > rect.left && pt.x < rect.left + space)
                {
                    if (pt.y > rect.top && pt.y < rect.top + space)
                    {
                        return HTTOPLEFT;
                    }
                    else if (pt.y < rect.bottom && pt.y > rect.bottom - space)
                    {
                        return HTBOTTOMLEFT;
                    }
                    else
                    {
                        OutputDebugString(L"HTLEFT\n");
                        return HTLEFT;
                    }
                }
                else if (pt.x < rect.right && pt.x > rect.right - space)
                {
                    if (pt.y > rect.top && pt.y < rect.top + space)
                    {
                        return HTTOPRIGHT;
                    }
                    else if (pt.y < rect.bottom && pt.y > rect.bottom - space)
                    {
                        return HTBOTTOMRIGHT;
                    }
                    else
                    {
                        return HTRIGHT;
                    }
                }
                else if (pt.y > rect.top && pt.y < rect.top + space)
                {
                    return HTTOP;
                }
                else if (pt.y < rect.bottom && pt.y > rect.bottom - space)
                {
                    return HTBOTTOM;
                }
                else
                {
                    return HTCLIENT;
                }
            }
            break;
        }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
