// VolumeScreenWebSpeed.cpp : 定义应用程序的入口点。
//
#include "stdafx.h"
#include "VolumeScreenWebSpeed.h"

#include "WinDefine.h"
#include "WinContorlTool.h"
#include "WMMsg.h"
#include "GlobalResource.h"

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
	LoadString(hInstance, IDC_VOLUMESCREENWEBSPEED, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    WinControlTool::GetInstance()->Release();

	return (int) msg.wParam;
}

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
	//任务切换时看到的图标（16*16）、 exe上的图标（32*32）
	//wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_VOLUMESCREENWEBSPEED));
	wcex.hIcon			= (HICON)LoadImage(NULL, GlobalResource_const::g_szIconPath,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_VOLUMESCREENWEBSPEED);
	wcex.lpszClassName	= szWindowClass;
	//窗口右上角图标、 任务栏图标、 任务管理器看到的图标（16*16）
	wcex.hIconSm		= (HICON)LoadImage(NULL, GlobalResource_const::g_szIconPath,IMAGE_ICON,0,0,LR_LOADFROMFILE);//(HICON)LoadImage(NULL,"icon_show.ico",IMAGE_ICON,0,0,LR_LOADFROMFILE);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, /*WS_OVERLAPPEDWINDOW|*/WS_DLGFRAME|WS_SYSMENU,
      500, 300, 500, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   WinControlTool::GetInstance()->Init( GlobalResource_const::g_szIconPath, hWnd);
   int value = WinDefine::GetValueFromConfig(CONFIG_SET, "IsShow", 0, CONFIG_INF_FILENAME); 
   ShowWindow(hWnd, value);
   UpdateWindow(hWnd);
   WinControlTool::GetInstance()->OnDelayCreate(hWnd);
   return TRUE;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	return (INT_PTR)FALSE;
}



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
        if (wmId == IDM_ABOUT)
        {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
        }
        else if (wmId == IDM_EXIT)
        {
            ShowWindow(hWnd, SW_HIDE);
        }
        else
        {
            WinControlTool::GetInstance()->OnCommand(hWnd, wmId, wmEvent);
        }
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_CREATE:
		{
			WinControlTool::GetInstance()->OnCreate(hWnd, message, wParam, lParam);
            int value = WinDefine::GetValueFromConfig(CONFIG_SET, 
                "IsShowTray", 0, CONFIG_INF_FILENAME); 
            if (value)
            {     
                WinControlTool::GetInstance()->ShowTrayIcon(hInst, hWnd,
                     WM_USER_MSG_ID::WM_USER_MSG_ID_TRAY_PROC,
                      GlobalResource_const::g_szIconPath, 
                      GlobalResource_const::g_szTrayTips);
            }

			break;
		}
    case WM_USER_MSG_ID::WM_USER_MSG_ID_TRAY_PROC:
        {
            WinControlTool::GetInstance()->OnTrayProc(hWnd, message, wParam, lParam);
            break;
        }
	case WM_HOTKEY:
		{
			WinControlTool::GetInstance()->OnHotKey(hWnd, message, wParam, lParam);
			break;			
		}
    case WM_TIMER:
        {
            WinControlTool::GetInstance()->OnTimer(hWnd, message, wParam, lParam);
            break;
        }
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		EndPaint(hWnd, &ps);
		break;
    case WM_CLOSE:
        {
            if (wParam != 0)
            {
                PostQuitMessage(0);
            }
            else
            {
                ShowWindow(hWnd, SW_HIDE);
            }
            break;
        }
	case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
