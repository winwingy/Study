#include "StdAfx.h"
#include "WStatic.h"
#include <assert.h>
#include <WindowsX.h>


namespace wingy_ui
{


WStatic::WStatic(void)
{
}


WStatic::~WStatic(void)
{
}

HRESULT WStatic::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle)
{
    switch(message)
    {
    case WM_CREATE:
        {
            break;
        }
    case WM_KEYDOWN:
        {
            OutputDebugString("WM_KEYDOWN\n");
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps = {0};
            HDC hdc = BeginPaint(hWnd_, &ps);

            HBRUSH bru = CreateSolidBrush(RGB(0, 255, 0));
            HBRUSH oldbru = SelectBrush(hdc, bru);

            WRect rect = GetWindowRect();
            HBRUSH nullBrush = GetStockBrush(LTGRAY_BRUSH);
            RECT fillRect = {0, 0, rect.Width(), rect.Height()};
            FillRect(hdc, &fillRect, nullBrush);
            


            TCHAR text[200];
            ::GetWindowText(hWnd_, text, _countof(text));
            ::SetBkMode(hdc, TRANSPARENT);
            //TextOut(hdc, 2, 1, text, _tcslen(text));
            ::DrawText(hdc, text, _tcslen(text), &fillRect, DT_VCENTER|DT_BOTTOM|DT_RIGHT);

            SelectBrush(hdc, oldbru);
            DeleteBrush(bru);
            EndPaint(hWnd_, &ps);
           
            handle = TRUE;
            break;
        }
    }
    return FALSE;
}

HWND WStatic::Create(const tstring& windowText, int x, int y, int nWidth, int nHeight, HWND hWndParent)
{

    hWnd_ = CreateWindow("STATIC", windowText.c_str(), WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, 
        x, y, nWidth, nHeight, hWndParent, (HMENU)NULL, GetModuleHandle(NULL), this);
    ChangeWndProc();
    assert(hWnd_);
    return hWnd_;
}

}