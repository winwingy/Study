#pragma once
#include "exportUI.h"
#include "WWindow.h"
#include <functional>

namespace wingy_ui
{
class UI_API WButton : public WWindow
{
public:
    WButton(void);
    virtual ~WButton(void);
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth,
        int nHeight, HWND hWndParent) override;
    void SetLBtnDownEvent(std::function<void (WWindow*)> lBtnDownEvent)
    { *lBtnDownEvent_ = lBtnDownEvent;}

protected:
    virtual LRESULT ProcessMessage(UINT message, WPARAM wParam,
        LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL& handle);

private:
    std::function<void (WWindow*)>* lBtnDownEvent_;

};


}