#include "WContainer.h"

namespace wingy_UI
{

WContainer::WContainer(void)
{

}


WContainer::~WContainer(void)
{
   
}

LRESULT CALLBACK WContainer::WndProc(HWND hWnd, UINT message, WPARAM wParam, 
                                     LPARAM lParam)
{
    switch(message)
    {
    case WM_CREATE:
    {
        OutputDebugString(L"adsdsadsads\n");
        break;
    }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


bool WContainer::Create(HWND hwnd)
{
    WNDCLASSEX wndClassEx = {0};
    wndClassEx.cbClsExtra = 0;
    wndClassEx.cbSize = sizeof(wndClassEx);
    wndClassEx.cbWndExtra = 0;
    wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClassEx.hIcon = NULL;
    wndClassEx.hIconSm = NULL;
    wndClassEx.hInstance = (HINSTANCE)GetModuleHandle(NULL);
    wndClassEx.lpfnWndProc = (WNDPROC)WndProc;
    wndClassEx.lpszClassName = _SN(WContainer);
    wndClassEx.lpszMenuName = NULL;
    wndClassEx.style = CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS;

    WNDCLASS classExist = {0};
    BOOL reg = ::GetClassInfoW(wndClassEx.hInstance, wndClassEx.lpszClassName,
                               &classExist);
    if (reg && wndClassEx.lpfnWndProc != classExist.lpfnWndProc)
    {
        ::UnregisterClassW(wndClassEx.lpszClassName, wndClassEx.hInstance);
    }
    ATOM ret = RegisterClassExW(&wndClassEx);

    DWORD style = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    DWORD exStyle = WS_EX_LEFT | WS_EX_LTRREADING;
    HWND hwndWin = hwnd_ = CreateWindowExW(exStyle, wndClassEx.lpszClassName, 
        _SN(WContainer), style, 100, 100, 500, 500, hwnd, NULL, 
        wndClassEx.hInstance, NULL);
    DWORD dw = GetLastError();
    ShowWindow(hwndWin, SW_SHOW);
    UpdateWindow(hwndWin);

    return hwndWin == nullptr?false:true;
}
}