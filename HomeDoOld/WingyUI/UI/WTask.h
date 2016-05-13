#pragma once
#include "exportUI.h"
#include <functional>
#include <map>

#include "WWindow.h"


namespace wingy_ui
{

class UI_API WTask
{
public:
    static WTask* GetInstance();
    bool PostTask( std::function<void (int)> taskFunction );
    bool PostTaskB( std::function<void ()> taskFunction );

protected:
    virtual HWND Create(DWORD stleyEx, const tstring& windowText, DWORD dwStyle,
        int x, int y, int nWidth, int nHeight, HWND hWndParent);
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
    virtual LRESULT ProcessMessage(UINT message, WPARAM wParam,
        LPARAM lParam, BOOL& handle);

private:   
    WTask(void);
    ~WTask(void);
    static WTask* pThis_;

    HWND hWnd_;
    WNDPROC oldProc_;
    std::map<int, std::function<void(int)>>* idFunction_;
    std::map<int, std::function<void()>>* idFunctionB_;
    int id_;
};

}