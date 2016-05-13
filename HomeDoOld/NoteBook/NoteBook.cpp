// NoteBook.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include <CommDlg.h>
#include "DefineRef.h"
#include "NoteBook.h"


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
	LoadString(hInstance, IDC_NOTEBOOK, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEBOOK));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEBOOK));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_NOTEBOOK);
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

BOOL GetClipFileName(LPTSTR szName)
{
	OPENFILENAME ofn={0};

	ofn.lStructSize       = sizeof(OPENFILENAME);
	//	ofn.hwndOwner         = ghApp;
	ofn.lpstrFilter       = NULL;
	//ofn.lpstrFilter       = "Video files (*.mpg; *.mpeg; *.mp4)\0*.mpg; *.mpeg; *.mp4\0\0";
	ofn.lpstrFilter       = L"Chess files (*.cnd)\0*.cnd\0\0";
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex      = 1;	
	ofn.lpstrFile         = szName;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.lpstrFileTitle    = NULL;
	//ofn.lpstrDefExt       = "MPG";
	ofn.lpstrDefExt       = NULL;
	//ofn.Flags             = OFN_FILEMUSTEXIST|OFN_READONLY|OFN_PATHMUSTEXIST;
	ofn.Flags             = OFN_PATHMUSTEXIST;
	//bn(GetOpenFileName((LPOPENFILENAME)&ofn));
	return GetOpenFileName((LPOPENFILENAME)&ofn);

} // GetClipFileName

LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
	HDC hDC;
	PAINTSTRUCT ps;
	hDC = BeginPaint(hWnd, &ps);
// 	SetTextColor(hDC,RGB(255,0,0));
// 	SetBkColor(hDC,RGB(0,255,0));
// 	SetBkMode(hDC,TRANSPARENT);
// 	HFONT hFont = CreateFont(30,0,45,0,900,TRUE,
// 		TRUE,TRUE,0,0,0,0,0,"黑体");
// 	HFONT hOldFont = (HFONT)SelectObject(hDC,hFont);
// 
// 	CHAR szText[256] = "hello text LONG long long long long long longlong longl onglongl onglonglon glonglong";
// 	TextOut(hDC,100,100,szText,strlen(szText));
// 
// 	RECT rcRect = {0};
// 	rcRect.top = 150;
// 	rcRect.left = 100;
// 	rcRect.bottom = 200;
// 	rcRect.right = 200;
// 	Rectangle(hDC,100,150,200,200);
// 
// 	DrawText(hDC,szText,strlen(szText),&rcRect,
// 		DT_WORDBREAK|DT_RIGHT|DT_VCENTER|DT_NOCLIP);
// 	CHAR szExtText[] =  "A中国人民";
// 	rcRect.left = 100;
// 	rcRect.right = 200;
// 	rcRect.top = 300;
// 	rcRect.bottom = 350;
// 	INT nExtText[] = {20,0,20,0,20,0,20,0};
// 	ExtTextOut(hDC,100,300,ETO_OPAQUE,&rcRect,szExtText,
// 		strlen(szExtText),nExtText);
// 	SelectObject(hDC,hOldFont);
// 	DeleteObject(hFont);
// 	
	EndPaint(hWnd, &ps);

	return S_OK;
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
		case IDM_OPEN:
			{
				//打开系统选择框
				GetClipFileName(NULL);

				break;
			}
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_CREATE:
		{
			//创建一个编辑框
			RECT rect;
			GetClientRect(hWnd, &rect);			
			CreateWindow(L"EDIT", L"", ES_AUTOHSCROLL|WS_CHILD|WS_VISIBLE|ES_AUTOVSCROLL|ES_LEFT|ES_MULTILINE,
				rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top, hWnd, (HMENU)ID_NOTE_EDIT, hInst, 0);
		}
	case WM_PAINT:
		OnPaint(hWnd, message, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
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
