// CreateFont.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "CreateFont.h"
#include <string>
#include <sstream>
#include <WindowsX.h>
#include <assert.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

std::wstring g_text = L"一场游戏一场梦，我不害怕。";
std::wstring g_fontName = L"微软雅黑";
int g_fontSize = 72;
bool g_fontDirectHori = false;

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
	LoadString(hInstance, IDC_CREATEFONT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CREATEFONT));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CREATEFONT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CREATEFONT);
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


int FontSizeToPixel(HDC hDC, int fontSize)
{
    return -GetDeviceCaps(hDC, LOGPIXELSY)*fontSize/72;
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
    bool fresh = false;

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
        case 1000:
            {
                if (wmEvent == EN_UPDATE)
                {
                    HWND editHwnd = GetDlgItem(hWnd, 1000);
                    TCHAR szBuf[1024];
                    GetWindowText(editHwnd, szBuf, 
                        sizeof(szBuf)/sizeof(szBuf[0])-1);
                    if (_tcslen(szBuf) > 0)
                    {
                        g_text = szBuf;
                    }
                    fresh = true;
                }
                break;
            }
        case ID_FONTNAME_YAHEI:
            {
                g_fontName = L"微软雅黑";
                fresh = true;
                break;
            }
        case ID_FONTNAME_SONGTI:
            {
                g_fontName = L"宋体";
                fresh = true;
                break;
            }
        case ID_FONTNAME_BLACK:
            {
                g_fontName = L"黑体";
                fresh = true;
                break;
            }
        case ID_FONTNAME_HUAWENCAIYUN:
            {
                g_fontName = L"华文彩云";
                fresh = true;
                break;
            }
        case ID_FONTNAME_JITI:
            {
                g_fontName = L"楷体";
                fresh = true;
                break;
            }
        case ID_FONTSIZE_36:
            {
                g_fontSize = 36;
                fresh = true;
                break;
            }
        case ID_FONTSIZE_72:
            {
                g_fontSize = 72;
                fresh = true;
                break;
            }
        case ID_DIRECT_HORI:
            {
                g_fontDirectHori = true;
                fresh = true;
                break;
            }
        case ID_DIRECT_VERT:
            {
                g_fontDirectHori = false;
                fresh = true;
                break;
            }
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
        if (fresh)
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            InvalidateRect(hWnd, &rect, TRUE);
        }
		break;
	case WM_PAINT:        
        {
            hdc = BeginPaint(hWnd, &ps);

            {
                TCHAR szTip[100] = L"请输入： ";
                BOOL ret = ::TextOut(hdc, 400, 30, szTip, _tcslen(szTip));
            }

            // TODO: 在此添加任意绘图代码...
            {
                LOGFONT logFont = {0};
                std::wstring fontName = g_fontName;
                if (!g_fontDirectHori)
                {
                    fontName = L"@" + g_fontName;
                }
                _tcscpy_s(logFont.lfFaceName, 32-1, fontName.c_str());
                logFont.lfCharSet = GB2312_CHARSET;
                logFont.lfHeight = FontSizeToPixel(hdc, g_fontSize);
                logFont.lfEscapement = g_fontDirectHori ? 0 : 2700;
                logFont.lfOrientation = logFont.lfEscapement;
                if (logFont.lfWeight > 610 && !wcscmp(L"@微软雅黑", logFont.lfFaceName))
                {
                    logFont.lfWeight = 610;
                }
                HFONT newFont = CreateFontIndirect(&logFont);
                HFONT oldFont = SelectFont(hdc, newFont);

                RECT rect = {50, 50, 1200, 200};
                BOOL ret = ::TextOut(hdc, 200, 100, g_text.c_str(), g_text.length());
                SelectFont(hdc, oldFont);
                DeleteFont(newFont);
            }

            {
                LOGFONT logFont = {0};
                std::wstring fontName = g_fontName;
                if (!g_fontDirectHori)
                {
                    fontName = L"@" + g_fontName;
                }
                _tcscpy_s(logFont.lfFaceName, 32-1, fontName.c_str());
                logFont.lfCharSet = GB2312_CHARSET;
                logFont.lfHeight = FontSizeToPixel(hdc, 20);
                HFONT newFont = CreateFontIndirect(&logFont);
                HFONT oldFont = SelectFont(hdc, newFont);

                {
                    SIZE allSize = {0};
                    ::GetTextExtentPoint32W(
                        hdc, g_text.c_str(), g_text.length(), &allSize);
                    std::wstringstream allString;
                    allString << L"总长度:" << allSize.cx << L", " << allSize.cy;
                    BOOL ret = ::TextOut(hdc, 100, 0,
                        allString.str().c_str(), allString.str().length());
                }

                {
                    int perx = 300;
                    for (UINT i = 0; i < g_text.size(); ++i)
                    {
                        SIZE perSize = {0};
                        std::wstring perString(1, g_text[i]);
                        BOOL ret = ::TextOut(hdc, perx, 400, 
                            perString.c_str(), perString.length());

                        {
                            ::GetTextExtentPoint32W(
                                hdc, perString.c_str(), 1, &perSize);
                            std::wstringstream allString;
                            allString << perSize.cx
                                << L", " << perSize.cy;
                            BOOL retPer = ::TextOut(hdc, perx, 500, 
                                allString.str().c_str(), allString.str().length());
                        }

                        perx += 150;
                    }

                }
                SelectFont(hdc, oldFont);
                DeleteFont(newFont);
            }

   

            EndPaint(hWnd, &ps);
            break;
        }
    case WM_CREATE:
        {
            ShowWindow(hWnd, SW_SHOWMAXIMIZED);
            HWND editHwnd = CreateWindow(L"EDIT", L"", 
                WS_VISIBLE|WS_BORDER|WS_CHILD, 
                400, 50, 500, 50, hWnd, (HMENU)1000, hInst, 0);
            assert(editHwnd);
            break;
        }
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
