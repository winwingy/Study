#pragma once
#include <Windows.h>
#include <string>
#include "WWindow.h"

namespace wingy_ui
{

class WEdit;


class WHotKeyEdit : public WWindow
{
public:
    WHotKeyEdit(void);

    static int WHotKeyEdit::GetModifiers(int globalCode);

    static int WHotKeyEdit::GetVitualKey(int globalCode);

    //static std::string WHotKeyEdit::MakeHotKeyString(DWORD key);

    HWND Create(int x, int y, int width, int heiht, HWND parent);
    ~WHotKeyEdit(void);

    void InitHotKeyText(int globalCode);

    int GetHotKeyCode();

    bool ApplyHotKey();

    virtual LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnKillFocus(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnSetFocus(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnChar(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnSystemKeyDown(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnPaste(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnContextMenu(WPARAM wParam, LPARAM lParam, BOOL& handle) override;

    

private:
    virtual LRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle) override;   

    WEdit* editShow_;
    DWORD keyValue_;
    DWORD keyValueOld_;
};

}