#pragma once
#include <string>
#include "WRect.h"
#include <map>
class BaseLayerWnd
{
public:
    BaseLayerWnd(void);
    ~BaseLayerWnd(void);
 
    HWND Create( HWND hWnd, int x, int y, int cx, int cy );
    BOOL ShowWindow(__in int nCmdShow);
    HWND GetHWND(){return hWnd_;}
    WRect GetClientRect();
    WRect GetWindowRect();
    WRect ClientToScreen(const WRect& rect);
    WRect ScreenToCLient(const WRect& rect);
    void SetWindowText(std::wstring text);
    std::wstring GetWindowText();
    void InvalidateRect();
    void AddDeferWindowPos(HWND hWnd);
    void BeforeDeferWindowPos();
    void AfterDeferWindowPos();
    void SetAlpha(int alpha){alpha_ = alpha;}
    void SetTextColor(COLORREF color){textColor_ = color;}
    void SetBkColor(COLORREF color){bkColor_ = color;}
    LONG_PTR GetWindowLongPtr(int nIndex);
    void SetWindowLongPtr(int nIndex, LONG_PTR dwNewLong);

protected:
    static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT msg, 
        WPARAM wParam, LPARAM lParam);

    virtual bool ProcessMessage(UINT msg, WPARAM wParam, 
        LPARAM lParam, LRESULT* result);
  

    HWND hWnd_; 
    int alpha_;
    COLORREF textColor_;
    COLORREF bkColor_;

private:
    std::map<HWND, WRect> deferPosList_;
    WRect oldWndRect_;
    POINT oldWndPoint_;

};

