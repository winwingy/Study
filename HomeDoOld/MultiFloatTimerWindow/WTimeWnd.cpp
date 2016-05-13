#include "StdAfx.h"
#include "WTimeWnd.h"
#include <sstream>


WTimeWnd::WTimeWnd(void)
{
}


WTimeWnd::~WTimeWnd(void)
{
}

bool WTimeWnd::ProcessMessage( UINT msg, WPARAM wParam, 
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
            ss  <<time.wHour<<L":"
                <<time.wMinute<<L":"
                <<time.wSecond;

            SetWindowText(ss.str());
            InvalidateRect();
            break;
        }
    }
    return __super::ProcessMessage(msg, wParam, lParam, result);
}
