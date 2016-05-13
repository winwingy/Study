#pragma once
#include <string>
class WinToolTip
{
public:
    static WinToolTip* GetInstance();
    void ShowTips( const std::wstring& info, void* ptrID );
    static void ClientToScreen(HWND hwnd, RECT* rect);


private: 
    WinToolTip(void);
    ~WinToolTip(void);
    bool WndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* res);
    void Create();
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static WinToolTip* winToolTip_;

    HWND hWnd_;
    std::wstring info_;
    void* ptrID_;

};

