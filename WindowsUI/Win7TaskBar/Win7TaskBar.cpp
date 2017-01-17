// Win7TaskBar.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win7TaskBar.h"
#include <ShlObj.h>
#include <dwmapi.h>

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
	LoadString(hInstance, IDC_WIN7TASKBAR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN7TASKBAR));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN7TASKBAR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN7TASKBAR);
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


int WM_TASKBARBUTTONCREATED = 0;
ITaskbarList4* g_taskBar = nullptr;
enum TBUTTON
{
    TBUTTON_LEFT,
    TBUTTON_MIDDLE,
    TBUTTON_RIGHT,
};

void OnThumbnailClicked(int wmId)
{
    switch(wmId)
    {
    case TBUTTON_LEFT:
        {
            OutputDebugString(L"TBUTTON_LEFT\n");
            break;
        }
    case TBUTTON_MIDDLE:
        {
            OutputDebugString(L"TBUTTON_MIDDLE\n");
            break;
        }
    case TBUTTON_RIGHT:
        {
            OutputDebugString(L"TBUTTON_RIGHT\n");
            break;
        }
    }
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

    if (message == WM_TASKBARBUTTONCREATED)
    {
        ::CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&g_taskBar));
        if (g_taskBar)
        {
            g_taskBar->HrInit();
        }
        return 0;
    }

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
        if (wmEvent == THBN_CLICKED)
        {
            OnThumbnailClicked(wmId);
            return 0;
        }
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
    case WM_CREATE:
        {
            WM_TASKBARBUTTONCREATED = 
                ::RegisterWindowMessage(L"TaskbarButtonCreated");
            break;
        }
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case 'A':
                {
//                     HICON icon = reinterpret_cast<HICON>(
//                         LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), 
//                         IMAGE_ICON, 0, 0, LR_SHARED));
                    HICON icon = reinterpret_cast<HICON>(
                        LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)));
                    g_taskBar->SetProgressState(hWnd, TBPF_PAUSED);
                    g_taskBar->SetProgressValue(hWnd, 50, 100);
                    g_taskBar->SetOverlayIcon(hWnd, icon, L"TestIcon");
                    break;
                }
            case 'B':
                {
                    THUMBBUTTONMASK mask = THB_ICON|THB_TOOLTIP;
                    THUMBBUTTON buttons[3];
                    {
                        buttons[0].dwMask = mask;
                        buttons[0].iId = TBUTTON_LEFT;
                        HICON icon = reinterpret_cast<HICON>(
                            LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_LEFT), 
                            IMAGE_ICON, 32, 32, LR_SHARED));
                        buttons[0].hIcon = icon;
//                         buttons[0].hIcon = LoadIcon(
//                             hInst, MAKEINTRESOURCE(IDI_ICON_LEFT));
        
                        _tcscpy_s(buttons[0].szTip, L"Left");
                    }
         
                    {
                        buttons[1].dwMask = mask;
                        buttons[1].iId = TBUTTON_MIDDLE;
                        HICON icon = reinterpret_cast<HICON>(
                            LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_MIDDLE), 
                            IMAGE_ICON, 128, 128, LR_SHARED));
                        buttons[1].hIcon = icon;
//                         buttons[1].hIcon = LoadIcon(
//                             hInst, MAKEINTRESOURCE(IDI_ICON_MIDDLE));
                        _tcscpy_s(buttons[1].szTip, L"Middle");
                    }
                    {
                        buttons[2].dwMask = mask;
                        buttons[2].iId = TBUTTON_RIGHT;
                        HICON icon = reinterpret_cast<HICON>(
                            LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_RIGHT), 
                            IMAGE_ICON, 128, 128, LR_SHARED));
                        buttons[2].hIcon = icon;
//                         buttons[2].hIcon = LoadIcon(
//                             hInst, MAKEINTRESOURCE(IDI_ICON_RIGHT));
                        _tcscpy_s(buttons[2].szTip, L"Right");
                    }
                    g_taskBar->ThumbBarAddButtons(hWnd, _countof(buttons), 
                        buttons);
                    break;
                }
            case 'C':
                {
                    g_taskBar->SetThumbnailTooltip(hWnd, L"Hello Taskbar");
                    break;
                }
            case 'D':
                {
                    BOOL enable = TRUE;
                    DwmSetWindowAttribute(hWnd, DWMWA_HAS_ICONIC_BITMAP,
                        &enable, sizeof(enable));
                    DwmSetWindowAttribute(hWnd, DWMWA_FORCE_ICONIC_REPRESENTATION,
                        &enable, sizeof(enable));

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
