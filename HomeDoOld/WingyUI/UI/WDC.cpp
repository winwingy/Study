#include "StdAfx.h"
#include "WDC.h"

namespace wingy_ui
{


WDC::WDC(HWND hWnd, HDC hdc)
    : hWnd_(hWnd)
    , hdc_(hdc)
{
}


WDC::~WDC(void)
{
    if (hdc_ && hWnd_)
    {
        ReleaseDC(hWnd_, hdc_);
    }
}
}