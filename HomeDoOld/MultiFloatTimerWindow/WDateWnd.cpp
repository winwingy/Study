#include "StdAfx.h"
#include <sstream>
#include "WDateWnd.h"


WDateWnd::WDateWnd(void)
{

}


WDateWnd::~WDateWnd(void)
{
}


bool WDateWnd::ProcessMessage( UINT msg, WPARAM wParam,
    LPARAM lParam, LRESULT* result )
{

    switch (msg)
    {
    case WM_CREATE:
        {
            SetTimer(hWnd_, 1000, 500, nullptr);
            break;
        }
    case WM_TIMER:
        {
            SYSTEMTIME time;
            GetLocalTime(&time);
            std::wstringstream ss;
            ss  <<time.wYear<<L"Äê"
                <<time.wMonth<<L"ÔÂ"
                <<time.wDay<<L"ÈÕ";

            SetWindowText(ss.str());
            InvalidateRect();
            break;
        }
    }
    return __super::ProcessMessage(msg, wParam, lParam, result);
}