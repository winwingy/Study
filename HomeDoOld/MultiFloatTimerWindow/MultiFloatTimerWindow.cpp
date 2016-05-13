// MultiFloatTimerWindow.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MultiFloatTimerWindow.h"
#include "MultiWnd.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: 在此放置代码。
 	MSG msg;
 	HACCEL hAccelTable;
 	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MULTIFLOATTIMERWINDOW));

    MultiWnd wnd;
    wnd.Show();

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


