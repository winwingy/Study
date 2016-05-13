#include "StdAfx.h"
#include "WEdit.h"
#include <ShellAPI.h>

namespace wingy_ui
{



WEdit::WEdit(void)
{
}


WEdit::~WEdit(void)
{
    TRACE_UI(_T(""));
}

HWND WEdit::Create(const tstring& windowText, int x, int y, int nWidth, 
    int nHeight, HWND hWndParent)
{
    hWnd_ = CreateWindow(_T("EDIT"), windowText.c_str(), 
        WS_CHILD|WS_VISIBLE|WS_BORDER, x, y, nWidth, nHeight, hWndParent,
        NULL, GetModuleHandle(NULL), this);   
    ChangeWndProc();
    DragAcceptFiles(hWnd_, TRUE);
    return hWnd_;
}

LRESULT WEdit::OnDropFiles( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    HDROP drop = (HDROP)wParam;
    TCHAR fileName[1024] = {0};
    UINT num = DragQueryFile(drop, 0xffffffff, nullptr, 0);
    num = DragQueryFile(drop, 0, fileName, sizeof(fileName)/sizeof(TCHAR)-1);
    SetWindowText(fileName);
    return FALSE;
}

}