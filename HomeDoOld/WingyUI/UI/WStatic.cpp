#include "StdAfx.h"
#include "WStatic.h"
#include <assert.h>
#include <WindowsX.h>
#include <tchar.h>


namespace wingy_ui
{


WStatic::WStatic(void)
    : textColor_(0)
    , backColor_(RGB(255, 255, 255))
{
}


WStatic::~WStatic(void)
{
}

HRESULT WStatic::ProcessMessage(UINT message, WPARAM wParam, 
    LPARAM lParam, BOOL& handle)
{
    switch(message)
    {
    case WM_CREATE:
        {
            break;
        }
    case WM_KEYDOWN:
        {
            OutputDebugString(_T("WM_KEYDOWN\n"));
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps = {0};
            HDC hdc = BeginPaint(hWnd_, &ps);

            WRect rect = GetClientRect();
            HBRUSH backBrush = CreateSolidBrush(backColor_);
            RECT fillRect = {0, 0, rect.Width(), rect.Height()};
            FillRect(hdc, &fillRect, backBrush);

            TCHAR text[200];
            ::GetWindowText(hWnd_, text, _countof(text));
            ::SetBkMode(hdc, TRANSPARENT);
            ::SetTextColor(hdc, textColor_);
            ::DrawText(hdc, text, _tcslen(text), &fillRect, 
                DT_VCENTER|DT_CENTER);
  
            EndPaint(hWnd_, &ps);           
            break;
        }
    }
    return __super::ProcessMessage(message, wParam, lParam, handle);
}

HWND WStatic::Create(const tstring& windowText, int x, int y,
    int nWidth, int nHeight, HWND hWndParent)
{
    hWnd_ = CreateWindow(_T("STATIC"), windowText.c_str(),        
        WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, 
        x, y, nWidth, nHeight, hWndParent, (HMENU)NULL,
        GetModuleHandle(NULL), this);
    ChangeWndProc();
    assert(hWnd_);
    return hWnd_;
}

void WStatic::SetTextColor( COLORREF color )
{
    textColor_ = color;
}

void WStatic::SetBackColor( COLORREF color )
{
    backColor_ = color;
}

}