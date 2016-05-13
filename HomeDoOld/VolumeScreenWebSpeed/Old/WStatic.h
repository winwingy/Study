#pragma once
#include "WWindow.h"
#include <Windows.h>


namespace wingy_ui
{


class WStatic : public WWindow
{
public:
    WStatic(void);
    ~WStatic(void);
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth, int nHeight, HWND hWndParent) override;
    virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle) override;
private:

};

}

