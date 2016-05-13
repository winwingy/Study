#pragma once
#include <Windows.h>
#include "WWindow.h"

namespace wingy_ui
{


class UI_API WEdit : public wingy_ui::WWindow
 {
public:
    WEdit(void);
    virtual ~WEdit(void);
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth, 
        int nHeight, HWND hWndParent) override;

 protected:
     virtual LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam, 
         BOOL& handle) override;

};

}

