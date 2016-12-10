// DestopItem.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DestopItem.h"
#include <CommCtrl.h>
#include <stddef.h>

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

namespace X32Use
{

typedef struct tagLVITEMKG
{
    UINT mask;
    int iItem;
    int iSubItem;
    UINT state;
    UINT stateMask;
    LPWSTR pszText;
    int placeHoders;
    int cchTextMax;
    int iImage;
    LPARAM lParam;
#if (_WIN32_IE >= 0x0300)
    int iIndent;
#endif
#if (_WIN32_WINNT >= 0x0501)
    int iGroupId;
    UINT cColumns; // tile view columns
    PUINT puColumns;
#endif
#if _WIN32_WINNT >= 0x0600
    int* piColFmt;
    int iGroup; // readonly. only valid for owner data.
#endif
} LVITEMWKG, *LPLVITEMWKG;


typedef struct _DESKTOP_ICON_INFO {
    LVITEMWKG item;
    WCHAR item_text[MAX_PATH];
    RECT rc;
} DESKTOP_ICON_INFO, *PDESKTOP_ICON_INFO;

BOOL GetDesktopIconInfo(LPCWSTR pattern, RECT &rc, HWND &desktop)
{
    HWND progman = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
    if (progman == NULL) {
        return FALSE;
    }


    HWND def_view = FindWindowEx(progman, NULL, TEXT("SHELLDLL_DefView"), NULL);
    if (def_view == NULL) {
        return FALSE;
    }

    HWND list_view = FindWindowEx(def_view, NULL, TEXT("SysListView32"), TEXT("FolderView"));
    if (list_view == NULL) {
        return FALSE;
    }
    desktop = list_view;

    ULONG process_id = 0;
    GetWindowThreadProcessId(progman, &process_id);
    if (process_id == 0) {
        return FALSE;
    }

    int count = (int)::SendMessage(list_view, LVM_GETITEMCOUNT, 0, 0);

    HANDLE process_handle = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, process_id);
    if (process_handle == NULL) {
        return FALSE;
    }

    PUCHAR remote_addr = (PUCHAR)VirtualAllocEx(process_handle, NULL, 
        sizeof(DESKTOP_ICON_INFO), MEM_COMMIT, PAGE_READWRITE);

    DESKTOP_ICON_INFO icon_info;
    icon_info.item.iItem = 0;
    icon_info.item.iSubItem = 0;
    icon_info.item.mask = LVIF_TEXT;
    icon_info.item.pszText = (WCHAR *)(remote_addr + offsetof(DESKTOP_ICON_INFO, item_text));
    icon_info.item.cchTextMax = MAX_PATH;

    for (int i = 0; i < count; i++) {
        icon_info.rc.left = LVIR_BOUNDS;
        ZeroMemory(icon_info.item_text, sizeof(icon_info.item_text));
        if (WriteProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL)) {
            ::SendMessage(list_view, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, item)));
            ::SendMessage(list_view, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, rc)));
            ReadProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL);

            if (_wcsicmp(icon_info.item_text, pattern) == 0) {
                rc = icon_info.rc;
                break;
            }
        }
    }

    VirtualFreeEx(process_handle, remote_addr, 0, MEM_RELEASE);
    CloseHandle(process_handle);
    return TRUE;
}
}



namespace X64Use
{
    typedef struct tagLVITEMKG
    {
        UINT mask;
        int iItem;
        int iSubItem;
        UINT state;
        UINT stateMask;
        BYTE holder1[4];
        LPWSTR pszText;
        BYTE holder2[4];
        int cchTextMax;
        int iImage;
        BYTE holder3[4];
        LPARAM lParam;
        BYTE holder4[4];
#if (_WIN32_IE >= 0x0300)
        int iIndent;
#endif
#if (_WIN32_WINNT >= 0x0501)
        int iGroupId;
        UINT cColumns; // tile view columns
        BYTE holder5[4];
        PUINT puColumns;
        BYTE holder6[4];
#endif
#if _WIN32_WINNT >= 0x0600
        int* piColFmt;
        BYTE holder7[4];
        int iGroup; // readonly. only valid for owner data.
        BYTE holder8[4];
#endif
    } LVITEMWKG, *LPLVITEMWKG;

    HWND GetListViewHwnd()
    {
        HWND progman = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
        if (progman == NULL) {
            return FALSE;
        }


        HWND def_view = FindWindowEx(progman, NULL, TEXT("SHELLDLL_DefView"), NULL);
        if (def_view == NULL) {
            return FALSE;
        }

        HWND list_view = FindWindowEx(def_view, NULL, TEXT("SysListView32"), TEXT("FolderView"));
        if (list_view == NULL) {
            return FALSE;
        }

        return list_view;
    }


    BOOL GetDesktopIconInfo(LPCWSTR pattern, RECT &rc, HWND &desktop)
    {
        HWND list_view = desktop = GetListViewHwnd();

        ULONG process_id = 0;
        GetWindowThreadProcessId(list_view, &process_id);
        if (process_id == 0) {
            return FALSE;
        }

        int count = (int)::SendMessage(list_view, LVM_GETITEMCOUNT, 0, 0);

        HANDLE process_handle = OpenProcess(
            PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, process_id);
        if (process_handle == NULL) {
            return FALSE;
        }

        PUCHAR remote_addr = (PUCHAR)VirtualAllocEx(process_handle, NULL, 
            sizeof(LVITEMWKG), MEM_COMMIT, PAGE_READWRITE);

        PUCHAR textBuf = (PUCHAR)VirtualAllocEx(process_handle, NULL, 
            sizeof(sizeof(TCHAR)*MAX_PATH), MEM_COMMIT, PAGE_READWRITE);


        LVITEMWKG icon_info;
        memset(&icon_info, 0, sizeof(icon_info));
        icon_info.iItem = 0;
        icon_info.iSubItem = 0;
        icon_info.mask = LVIF_TEXT;
        icon_info.pszText = (LPWSTR)textBuf;
        icon_info.cchTextMax = MAX_PATH;

        int itemSize = sizeof(icon_info);
        WCHAR buftemp[MAX_PATH] = {0};

        for (int i = 0; i < count; i++) 
        {     
            if (WriteProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL)) 
            {
                ::SendMessage(list_view, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)(remote_addr));  
                ReadProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL);                
                ReadProcessMemory(process_handle, textBuf, &buftemp, sizeof(buftemp), NULL);     
            }
        }

        VirtualFreeEx(process_handle, remote_addr, 0, MEM_RELEASE);
        VirtualFreeEx(process_handle, textBuf, 0, MEM_RELEASE);
        CloseHandle(process_handle);
        return TRUE;
    }
}




namespace X64UseSomeData
{

// 
//     typedef struct tagLVITEMKG
//     {
//         UINT mask;
//         int iItem;
//         int iSubItem;
//         UINT state;
//         UINT stateMask;
//         BYTE holderPszText[4];
//         LPWSTR pszText;
//         int cchTextMax;
//         int iImage; 
//         BYTE holderlParam[4];
//         LPARAM lParam; 
// #if (_WIN32_IE >= 0x0300)
//         int iIndent;
// #endif
// #if (_WIN32_WINNT >= 0x0501)
//         int iGroupId;
//         UINT cColumns; // tile view columns  
//         BYTE holderpuColumns[4];
//         PUINT puColumns;    
// #endif
// #if _WIN32_WINNT >= 0x0600 
//         BYTE holderpiColFmt[4];
//         int* piColFmt;       
//         int iGroup; // readonly. only valid for owner data.
// #endif
//     } LVITEMWKG, *LPLVITEMWKG;
// 

//    typedef struct _DESKTOP_ICON_INFO {
//         LVITEMWKG item;
//         WCHAR item_text[MAX_PATH];
//         RECT rc;
//     } DESKTOP_ICON_INFO, *PDESKTOP_ICON_INFO;



        typedef struct tagLVITEMKG
        {  
            UINT mask;
            int name;
            LPWSTR pszText;
            BYTE holderPszText[4];
            int cchTextMax;
            BYTE holder2[4];
        } LVITEMWKG, *LPLVITEMWKG;


        typedef struct _DESKTOP_ICON_INFO {
            LVITEMWKG item;
            WCHAR item_text[5];
            BYTE holder3[2];
            RECT rc;
            BYTE holder4[4];
        } DESKTOP_ICON_INFO, *PDESKTOP_ICON_INFO;

    HWND GetListViewHwnd()
    {
        HWND progman = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
        if (progman == NULL) {
            return FALSE;
        }


        HWND def_view = FindWindowEx(progman, NULL, TEXT("SHELLDLL_DefView"), NULL);
        if (def_view == NULL) {
            return FALSE;
        }

        HWND list_view = FindWindowEx(def_view, NULL, TEXT("SysListView32"), TEXT("FolderView"));
        if (list_view == NULL) {
            return FALSE;
        }
        return list_view;
    }

    HWND GetMyOwnerDrawHwnd()
    {
        HWND progman = FindWindow(nullptr, TEXT("Choose an icon"));
        if (progman == NULL) {
            return FALSE;
        }


//         HWND list_view = FindWindowEx(progman, NULL, TEXT("SysListView32"), TEXT("ListView!!!"));
//         if (list_view == NULL) {
//             return FALSE;
//         }
        return progman;
    }




    BOOL GetSomeDataFormX64Info(LPCWSTR pattern, RECT &rc, HWND &desktop)
    {
        desktop = GetMyOwnerDrawHwnd();

        ULONG process_id = 0;
        GetWindowThreadProcessId(desktop, &process_id);
        if (process_id == 0) 
        {
            return FALSE;
        }

        int count = 1;//(int)::SendMessage(desktop, LVM_GETITEMCOUNT, 0, 0);

        HANDLE process_handle = OpenProcess(
            PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, 
            FALSE, process_id);
        if (process_handle == NULL) {
            return FALSE;
        }

        int x64Add = 16;
        PUCHAR remote_addr = (PUCHAR)VirtualAllocEx(process_handle, NULL, 
            sizeof(DESKTOP_ICON_INFO), MEM_COMMIT, PAGE_READWRITE);

         int test = offsetof(DESKTOP_ICON_INFO, item_text);
//         int testa = offsetof(LVITEMWKG, mask);
//         int testb = offsetof(LVITEMWKG, iItem);
//         remote_addr += 1;
//         remote_addr = (PUCHAR)((double*)remote_addr + 1);
   
        int allSize = sizeof(DESKTOP_ICON_INFO);


        DESKTOP_ICON_INFO icon_info;
        memset(&icon_info, 0, sizeof(icon_info));
        icon_info.item.mask = LVIF_TEXT;
//         icon_info.item.iItem = 0x12345678;
//         icon_info.item.iSubItem = 0x11111111;
//         icon_info.item.holderPszText[0] = 0x22;
//         icon_info.item.holderPszText[1] = 0x22;
//         icon_info.item.holderPszText[2] = 0x22;
//         icon_info.item.holderPszText[3] = 0x22;      
        icon_info.item.pszText = (WCHAR *)(remote_addr + offsetof(DESKTOP_ICON_INFO, item_text));
        icon_info.item.cchTextMax = MAX_PATH;

        for (int i = 0; i < count; i++) 
        {
            icon_info.rc.left = LVIR_BOUNDS;
            ZeroMemory(icon_info.item_text, sizeof(icon_info.item_text));
            if (WriteProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL)) 
            {
                ::SendMessage(desktop, 123456, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, item)));
                //::SendMessage(desktop, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, rc)));
                ReadProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL);

                if (_wcsicmp(icon_info.item_text, pattern) == 0) {
                    rc = icon_info.rc;
                    break;
                }
            }
        }

        VirtualFreeEx(process_handle, remote_addr, 0, MEM_RELEASE);
        CloseHandle(process_handle);
        return TRUE;
    }


}

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
	LoadString(hInstance, IDC_DESTOPITEM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DESTOPITEM));

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DESTOPITEM));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DESTOPITEM);
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
            case _T('A'):
                {
                    LPCWSTR pattern = L"ABD";
                    RECT rc = {0};
                    HWND desktop = nullptr;
                    X32Use::GetDesktopIconInfo( pattern, rc, desktop);
                    break;
                }
            case _T('B'):
                {
                    LPCWSTR pattern = L"ABD";
                    RECT rc = {0};
                    HWND desktop = nullptr;
                    X64UseSomeData::GetSomeDataFormX64Info(pattern, rc, desktop);
                    break;
                }
            case _T('C'):
                {
                    LPCWSTR pattern = L"ABD";
                    RECT rc = {0};
                    HWND desktop = nullptr;
                    X64Use::GetDesktopIconInfo(pattern, rc, desktop);
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
