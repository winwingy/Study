#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include "WWindow.h"

namespace wingy_ui
{

class WEdit;


class UI_API WHotKeyEdit : public WWindow
{
public:
    WHotKeyEdit(void);
    virtual ~WHotKeyEdit();

    static int WHotKeyEdit::GetModifiers(int globalCode);
    static int WHotKeyEdit::GetVitualKey(int globalCode);
    //static std::string WHotKeyEdit::MakeHotKeyString(DWORD key);
    HWND Create(int x, int y, int width, int heiht, HWND parent);   
    void InitHotKeyText(int globalCode);
    int GetHotKeyCode();
    bool ApplyHotKey();
    void SetKillFocusEvent(std::function <void (WWindow*)> focusEvent)
    { *killFocusEvent_ = focusEvent;}
    void ShowOldKey();

private: 
    virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnSystemKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnPaste(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam, BOOL& handle) override;     
    virtual LRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle) override;  
    BOOL SetWindowText(const tstring& text);

    WEdit* editShow_;
    DWORD keyValue_;
    DWORD keyValueOld_;
    std::function <void (WWindow*)>* killFocusEvent_;
};

}