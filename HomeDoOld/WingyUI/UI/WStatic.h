#pragma once
#include "WWindow.h"
#include <Windows.h>


namespace wingy_ui
{


class UI_API WStatic : public WWindow
{
public:
    WStatic(void);
    virtual ~WStatic(void);
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth, 
        int nHeight, HWND hWndParent) override;
    virtual HRESULT ProcessMessage(UINT message, WPARAM wParam, 
        LPARAM lParam, BOOL& handle) override;
    void SetTextColor(COLORREF color);
    void SetBackColor(COLORREF color);
private:
    COLORREF textColor_;
    COLORREF backColor_;
};

}

