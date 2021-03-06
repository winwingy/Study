// PopupWindow.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PopupWindow.h"
#include <assert.h>
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

#define WM_CREATE_POPUP_WINDOW WM_USER+100

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
	LoadString(hInstance, IDC_POPUPWINDOW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_POPUPWINDOW));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_POPUPWINDOW));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_POPUPWINDOW);
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

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK GrandsonProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    return DefWindowProc(hWnd, message, wParam, lParam);
}



LRESULT CALLBACK ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
    case WM_PAINT:
        {
            HDC dc = GetDC(hWnd);
            RECT rect;
            GetClientRect(hWnd, &rect);
            HBRUSH brush = (HBRUSH)CreateSolidBrush(RGB(0, 100, 0));
            FillRect(dc, &rect, brush);
            DeleteObject(brush);
            ReleaseDC(hWnd, dc);
            break;
        }
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case 'B'://本进程popup window
                {
                    WNDCLASSEX wcs = {0};
                    wcs.cbSize = sizeof(wcs);
                    HBRUSH hbr = CreateSolidBrush(RGB(150, 125, 250));
                    wcs.hbrBackground = hbr;
                    wcs.hInstance = hInst;
                    wcs.lpfnWndProc = GrandsonProc;
                    wcs.lpszClassName = L"GrandsonProcPopup";
                    wcs.style = CS_VREDRAW | CS_HREDRAW;
                    ATOM at = RegisterClassEx(&wcs);
                    assert(at != 0 && L"at != 0");

                    HWND hChild = CreateWindowExW(0, wcs.lpszClassName, wcs.lpszClassName,
                        WS_CHILD|WS_VISIBLE|WS_OVERLAPPEDWINDOW, 200, 200, 500, 300, hWnd, NULL, hInst, NULL);
    
                    break;
                }
            case 'C'://其它进程的popupwindow
                {
                    HWND anotherWindow = FindWindow(NULL,
                        L"PopupWIndowAnotherProcess");
                    if (anotherWindow)
                    {
//                          SendMessage(anotherWindow, WM_CREATE_POPUP_WINDOW, 
//                              (WPARAM)hWnd, 0);
                       // anotherWindow = (HWND)((int)anotherWindow - 1);
                        WCHAR szBufA[1024];
                        _stprintf(szBufA, L"GetTickCount = %d   IsWindow(anotherWindow) = %d\n", GetTickCount(), (IsWindow(anotherWindow)));
                        OutputDebugString(szBufA);
                         SendMessageTimeout(anotherWindow, WM_CREATE_POPUP_WINDOW, (WPARAM)hWnd, 0, SMTO_BLOCK, 1000, NULL);
                         WCHAR szBuf[1024];
                         _stprintf(szBuf, L"GetTickCount = %d   IsWindow(anotherWindow) = %d\n", GetTickCount(), IsWindow(anotherWindow));
                         OutputDebugString(szBuf);
                    }
                    else
                    {
                        assert(0 &&
                            L"FindWindow(NULL, L\"PopupWindowAnotherProcessWindow\"); Fail!");
                    }
                    break;
                }
            }

            break;
        }
    }
    
    return DefWindowProc(hWnd, message, wParam, lParam);
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
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case 'A':
                {
                    WNDCLASSEX wcs = {0};
                    wcs.cbSize = sizeof(wcs);
                    //HBRUSH hbr = CreateSolidBrush(RGB(0, 125, 0));
                    //wcs.hbrBackground = hbr;
                    wcs.hInstance = hInst;
                    wcs.lpfnWndProc = ChildProc;
                    wcs.lpszClassName = L"ChildWindowPopup";
                    wcs.style = CS_VREDRAW | CS_HREDRAW;
                    ATOM at = RegisterClassEx(&wcs);
                    assert(at != 0 && L"at != 0");

                    HWND hChild = CreateWindowExW(0, wcs.lpszClassName, wcs.lpszClassName,
                        WS_BORDER|WS_VISIBLE|WS_OVERLAPPEDWINDOW, 100, 200, 1000, 800, NULL, NULL, hInst, NULL);
                    break;
                }  
            case 'B'://本进程popup window
                {
                    WNDCLASSEX wcs = {0};
                    wcs.cbSize = sizeof(wcs);
                    HBRUSH hbr = CreateSolidBrush(RGB(150, 125, 250));
                    wcs.hbrBackground = hbr;
                    wcs.hInstance = hInst;
                    wcs.lpfnWndProc = GrandsonProc;
                    wcs.lpszClassName = L"GrandsonProcPopup";
                    wcs.style = CS_VREDRAW | CS_HREDRAW;
                    RegisterClassEx(&wcs);

                    HWND hChild = CreateWindowExW(0, wcs.lpszClassName, wcs.lpszClassName,
                        WS_CHILD|WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN, 200, 200, 500, 300, hWnd, NULL, hInst, NULL);

                    break;
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
