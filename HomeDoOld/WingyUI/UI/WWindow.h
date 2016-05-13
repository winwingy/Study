#pragma once
#include <Windows.h>
#include "WRect.h"
#include "UIDefine.h"
#include "WDC.h"


namespace wingy_ui
{


class UI_API WWindow
{
public:
    WWindow(void);
    virtual ~WWindow(void);

    
    virtual HWND Create(DWORD stleyEx, const tstring& windowText, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent);

	virtual HWND Create(const tstring& windowText, int x, int y, int nWidth,
        int nHeight, HWND hWndParent);
 
    void InvalidateRect(const WRect& rect);
    void InvalidateRect();
    void SetPos(const WRect& rect);
    BOOL SetWindowText(const tstring& text);
    tstring GetWindowText();
    void SetWindowVisible(bool show);
    bool GetWindowVisible();

    WNDPROC GetOldProc(){return oldProc_;}
    HWND GetHWND() const {return hWnd_;}
    operator HWND() const {return GetHWND();}
    WRect GetWindowRect() const;
    WRect GetPos() const;
    WRect GetClientRect() const;
    WDC GetDC();

protected:
    virtual void ChangeWndProc();
    virtual LRESULT OnCreate(WPARAM wParam, LPARAM lParam, BOOL& handle);
    virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}    
    virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnPaste(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnSystemKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& handle)
    {return FALSE;}
    virtual LRESULT ProcessMessage(UINT message, WPARAM wParam,
        LPARAM lParam, BOOL& handle);

    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam, BOOL& handle);
    HWND hWnd_;   


    tstring* windowText_;
    WNDPROC oldProc_;
    int backGroundColor_;
    int frameColor_;
 };
}

