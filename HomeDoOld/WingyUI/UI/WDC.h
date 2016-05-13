#pragma once
#include <Windows.h>
#include "exportUI.h"

namespace wingy_ui
{

class UI_API WDC
{
public:
    WDC(HWND hWnd, HDC hdc);
    ~WDC(void);

    operator HDC()
    {
        return hdc_;
    }

protected:
    HWND hWnd_;
    HDC hdc_;
};

}