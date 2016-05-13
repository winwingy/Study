#include "StdAfx.h"
#include "WWindow.h"
#include <assert.h>

#include <cassert>
#include <ShellAPI.h>
#pragma comment(lib, "shell32.lib")

namespace wingy_ui
{

WWindow::WWindow(void)
    : hWnd_(NULL)
    , oldProc_(NULL)
    , backGroundColor_(RGB(255, 255, 255))
    , frameColor_(0)
    , windowText_(new tstring)
{
}


WWindow::~WWindow(void)
{
    if (::IsWindow(hWnd_))
    {
        ::DestroyWindow(hWnd_);
    }

    if (windowText_)
    {
        delete windowText_;
        windowText_ = nullptr;
    }
    TRACE_UI(_T(""));
}

void WWindow::SetPos(const WRect& rect)
{
    MoveWindow(hWnd_, rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
}

BOOL WWindow::SetWindowText(const tstring& text)
{
    return ::SetWindowText(hWnd_, text.c_str());
}


tstring WWindow::GetWindowText()
{
    TCHAR szBuf[1024] = {0};
    ::GetWindowText(hWnd_, szBuf, 1023);
    return szBuf;
}

void WWindow::SetWindowVisible(bool show)
{
    if (show)
    {
         ::ShowWindow(hWnd_, SW_SHOW);
    }
    else
    {
         ::ShowWindow(hWnd_, SW_HIDE);
    }
}

bool WWindow::GetWindowVisible()
{
    return !!::IsWindowVisible(hWnd_);
}

WRect WWindow::GetWindowRect() const
{
    RECT rect = {0};
    ::GetWindowRect(hWnd_, &rect);
    return rect;
}

WRect WWindow::GetPos() const
{
    WRect rect = GetWindowRect();
    if (::GetParent(hWnd_))
    {
        POINT ptLeftTop = {rect.left, rect.top};
        ::ScreenToClient(::GetParent(hWnd_), &ptLeftTop);
        POINT ptRightBottom = {rect.right, rect.bottom};
        ::ScreenToClient(::GetParent(hWnd_), &ptRightBottom);
        rect.left = ptLeftTop.x;
        rect.top = ptLeftTop.y;
        rect.right = ptRightBottom.x;
        rect.bottom = ptRightBottom.y;
    }
    else
    {
        rect = GetClientRect();
    }
    return rect;
}

 WRect  WWindow::GetClientRect() const
 {
     RECT rect = {0};
     ::GetClientRect(hWnd_, &rect);
     return rect;
 }

 WDC WWindow::GetDC()
 {
     WDC wdc(hWnd_, ::GetDC(hWnd_));
     return wdc;
 }

LRESULT WWindow::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle)
{
    switch(message)
    {
    case WM_CREATE:
        {
            OnCreate(wParam, lParam, handle);
            break;
        }
    case WM_CLOSE:
        {
            break;
        }
    case WM_PAINT:
        {
            OnPaint(wParam, lParam, handle);
            break;
        }
    case WM_KEYDOWN:
        {
            OnKeyDown(wParam, lParam, handle);
            break;
        }
    case WM_SYSKEYDOWN:
        {
            OnSystemKeyDown(wParam, lParam, handle);
            break;
        }
    case WM_SETFOCUS:
        {
            OnSetFocus(wParam, lParam, handle);
            break;
        }
    case WM_KILLFOCUS:
        {
            OnKillFocus(wParam, lParam, handle);
            break;
        }
    case WM_DROPFILES:
        {
            OnDropFiles(wParam, lParam, handle);
            break;
        }
    case WM_CHAR:
        {
            OnChar(wParam,lParam, handle);
            break;
        }
    case WM_PASTE:
        {
            OnPaste(wParam, lParam, handle);
            break;
        }
    case WM_RBUTTONDOWN:
        {
            OnRButtonDown(wParam, lParam, handle);
            break;
        }
    case WM_CONTEXTMENU:
        {
            OnContextMenu(wParam, lParam, handle);
            break;
        }
    case WM_SIZE:
        {
            OnSize(wParam, lParam, handle);
            break;
        }
    }
    return handle;    
}




LRESULT CALLBACK WWindow::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    BOOL handle = FALSE;
    WWindow* thisClass = (WWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);   
    if (thisClass)
    {
        thisClass->hWnd_ = hWnd; 
    }    
    if (message == WM_NCCREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)cs->lpCreateParams);
              
    }
    else
    {
        if (thisClass)
        {
            thisClass->ProcessMessage(message, wParam, lParam, handle);
        }
    }

    if (handle == FALSE)
    {
        if (thisClass && thisClass->oldProc_)
        {
            return CallWindowProc(thisClass->oldProc_, hWnd, message, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }        
    }
    return 0;
}

HWND WWindow::Create(const tstring& windowText, int x, int y, int nWidth, int nHeight, HWND hWndParent)
{
	return Create(0, windowText, WS_CHILD|WS_VISIBLE, x, y, nWidth, nHeight, hWndParent);
}

HWND WWindow::Create(DWORD stleyEx, const tstring& windowText, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent)
{
    WNDCLASSEX wcse = {0};
    wcse.cbSize = sizeof(wcse);
    wcse.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcse.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
    wcse.hInstance = GetModuleHandle(NULL);
    wcse.lpfnWndProc = StaticWndProc;
    wcse.lpszClassName = _T("WWindowClass");
    if(GetClassInfoEx(wcse.hInstance, wcse.lpszClassName, &wcse))
    {
        ::UnregisterClass(wcse.lpszClassName, wcse.hInstance);
    }
    ATOM at = RegisterClassEx(&wcse);
    assert(at);
    if (at == NULL)
    {
        return NULL;
    }
    *windowText_ = windowText;
    hWnd_ = CreateWindowEx(stleyEx, wcse.lpszClassName, windowText.c_str(), dwStyle, x, y, nWidth, nHeight, hWndParent, NULL, wcse.hInstance, (VOID*)this);
    assert(hWnd_);
    if (hWnd_ == NULL)
    {
        return hWnd_;
    }
    //SendMessage(hwnd_, WM_SETICON, 0, 0);
    return hWnd_;
}

void WWindow::ChangeWndProc()
{
    SetWindowLongPtr(hWnd_, GWLP_USERDATA, (LONG)this);
    if (oldProc_ == NULL)
    {
        oldProc_ = (WNDPROC)SetWindowLong(hWnd_, GWL_WNDPROC, (LONG)StaticWndProc); 
    }    
}

LRESULT WWindow::OnPaint( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    return FALSE;
}

void WWindow::InvalidateRect( const WRect& rect )
{
    ::InvalidateRect(hWnd_, &(rect.GetRECT()), TRUE);
}

void WWindow::InvalidateRect()
{
    WRect rect = GetWindowRect();
    InvalidateRect(rect);
}

LRESULT WWindow::OnCreate( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    DragAcceptFiles(hWnd_, TRUE);
    return FALSE;
}


}