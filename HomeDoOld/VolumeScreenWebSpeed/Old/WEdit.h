#pragma once
#include <Windows.h>
#include "WWindow.h"
namespace wingy_ui
{


class WEdit : public wingy_ui::WWindow
{
public:
    WEdit(void);
    ~WEdit(void);
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth, int nHeight, HWND hWndParent) override;
};

}

