#include "StdAfx.h"
#include "WButton.h"

namespace wingy_ui
{

WButton::WButton(void)
{
    lBtnDownEvent_ = new std::function<void (WWindow*)>;
}


WButton::~WButton(void)
{
    if (lBtnDownEvent_)
    {
        delete lBtnDownEvent_;
        lBtnDownEvent_ = nullptr;
    }
}

HWND WButton::Create( const tstring& windowText, int x, int y, 
    int nWidth, int nHeight, HWND hWndParent )
{
    hWnd_ = CreateWindow(_T("BUTTON"), windowText.c_str(), 
        WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON, x, y, nWidth, nHeight, hWndParent,
        NULL, GetModuleHandle(NULL), this);   
    ChangeWndProc();
    return hWnd_;
}

LRESULT WButton::OnLButtonDown( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    if (*lBtnDownEvent_)
    {
        (*lBtnDownEvent_)(this);
    }
    return FALSE;
}

LRESULT WButton::ProcessMessage( UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    if (message == WM_LBUTTONDOWN)
    {
        OnLButtonDown(wParam, lParam, handle);
    }

    return __super::ProcessMessage(message, wParam, lParam, handle);
}

}