#include "StdAfx.h"
#include "WStaticEx.h"
#include <assert.h>
#include <WindowsX.h>
#include <tchar.h>

namespace wingy_ui
{
WStaticEx::WStaticEx(void)
{
}


WStaticEx::~WStaticEx(void)
{
}

// 
// LRESULT CALLBACK WStaticEx::StaticExProc(HWND hWnd, UINT message,
//     
//     WPARAM wParam, LPARAM lParam)
// {
//     WStaticEx* pThis = (WStaticEx*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
// 
//     switch(message)
//     {
//     case WM_PAINT:
//         {
//             PAINTSTRUCT ps;
//             HDC hdc = BeginPaint(hWnd, &ps);
// 
//             HBRUSH bru = CreateSolidBrush(RGB(0, 255, 0));
//             HBRUSH oldBru = SelectBrush(hdc, bru);
// 
//             RECT rect;
//             GetWindowRect(hWnd, &rect);
//             Rectangle(hdc, 0, 0, rect.right-rect.left, rect.bottom - rect.top);
//             TextOut(hdc, 1, 1, _T("Pint"), 4);
// 
//             SelectBrush(hdc, oldBru);
//             DeleteBrush(bru);
//             EndPaint(hWnd, &ps);
//             return TRUE;
//             break;
//         }
//     }
//     if (pThis)
//     {
//         return CallWindowProc(pThis->oldProc_, hWnd, message, wParam, lParam);
//     }
//     return TRUE;
// }
// 
// 
// HWND WStaticEx::Create(int x, int y, int width, int height, HWND par)
// {
//     HWND hwnd = CreateWindow(_T("EDIT"), _T("hi"), WS_CHILD|WS_VISIBLE, 
//         x,y, width, height, par, NULL, GetModuleHandle(NULL), this);
//     assert(hwnd);
// 
//     SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)this);
//     oldProc_ = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG)StaticExProc);
// 
// 
//     return hwnd;
// }
}