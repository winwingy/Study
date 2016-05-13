#pragma once
#include <Windows.h>
class WStaticEx
{
public:
    WStaticEx(void);
    ~WStaticEx(void);
    HWND Create(int x, int y, int width, int height, HWND par);
    static LRESULT CALLBACK WStaticEx::StaticExProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


private:
    WNDPROC oldProc_;


};

