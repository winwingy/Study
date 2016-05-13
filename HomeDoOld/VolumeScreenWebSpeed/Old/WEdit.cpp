#include "StdAfx.h"
#include "WEdit.h"

namespace wingy_ui
{



WEdit::WEdit(void)
{
}


WEdit::~WEdit(void)
{
}

HWND WEdit::Create(const tstring& windowText, int x, int y, int nWidth, int nHeight, HWND hWndParent)
{
    hWnd_ = CreateWindow("EDIT", windowText.c_str(), WS_CHILD|WS_VISIBLE|WS_BORDER, x, y, nWidth, nHeight, hWndParent, NULL, GetModuleHandle(NULL), this);   
    ChangeWndProc();
    return hWnd_;
}

}