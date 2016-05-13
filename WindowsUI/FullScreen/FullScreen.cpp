#include <stdio.h>
#include <tchar.h>
#include <Windows.h>


LRESULT WINAPI WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	switch(uMessage)
	{
	case WM_CREATE:
		{		
            LONG style = GetWindowLong( hWnd, GWL_STYLE );
            style&=~(WS_CAPTION|WS_SIZEBOX);//必须要去掉标题才能全屏
            ::SetWindowLong(hWnd,GWL_STYLE,style);
            int nFullWidth = GetSystemMetrics(SM_CXSCREEN);
            int nFullHeight = GetSystemMetrics(SM_CYSCREEN);
            SetWindowPos(hWnd, HWND_TOP, 0, 0, nFullWidth, nFullHeight, SWP_SHOWWINDOW);			
			break;
		}
	}
	return DefWindowProc(hWnd, uMessage, wParam, lParam);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	WNDCLASS wc = {0};
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = L"FullScreenClass";
	wc.style = CS_HREDRAW |CS_VREDRAW;
	ATOM aret = RegisterClass( &wc );
	int x = 500;
	int y = 300;
	HWND hWnd = CreateWindow( L"FullScreenClass", L"FullSCreen", WS_OVERLAPPEDWINDOW, 0, 0, x, y, NULL, NULL, hInstance, 0 );
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

}