#pragma once
#include <Windows.h>

namespace wingy_ui
{


class WDC
{
public:
    WDC(HWND hWnd, HDC hdc);
    ~WDC(void);

private:
    HWND hWnd_;
    HDC hdc_;
};

}