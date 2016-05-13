#include "StdAfx.h"
#include "WHotKeyEdit.h"
#include <CommCtrl.h>

#include "WEdit.h"

namespace wingy_ui
{

bool IsIgnoreKey(BYTE vkKey)
{
    return (vkKey == VK_CONTROL) || (vkKey  == VK_SHIFT) || (vkKey == VK_MENU);
} 

bool IsNoUseKey(UINT keycode)
{
    if (keycode == VK_ESCAPE || keycode == VK_LWIN 
        || keycode == VK_RWIN ||keycode == VK_SLEEP )
    {
        return true;
    }
    return false;
}



tstring GetKeyName(UINT keycode, bool extended)
{
    if (IsNoUseKey(keycode))
    {
        keycode = 0;
    }
    long scan = MapVirtualKey(keycode, MAPVK_VK_TO_VSC) << 16;
    //if it's a extended key, add the extendedflag
    if (extended)
        scan |= 0x01000000L;

    char text[100] = {0};
    GetKeyNameText(scan, text, 100);
    return text;
}  

WORD TranslateToValue(WPARAM wParam, LPARAM lParam)
{
    BYTE vkKey = static_cast<BYTE>(wParam);
    if (IsIgnoreKey(vkKey))
    {
        vkKey = 0;
    }

    BYTE modifiers = 0;
    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        modifiers |= HOTKEYF_CONTROL;
    }
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        modifiers |= HOTKEYF_SHIFT;
    }
    if (GetKeyState(VK_MENU) & 0x8000)
    {
        modifiers |= HOTKEYF_ALT;
    }
    if (lParam & 0x01000000)
    {
        modifiers |= HOTKEYF_EXT;
    }
    return MAKEWORD(vkKey, modifiers);
}

tstring MakeHotKeyString(DWORD key)
{
    tstring str;
    if (!key)
    {
        return str;
    }
    BYTE virtualKeyCode = LOBYTE(key);
    BYTE modifiers = HIBYTE(key);
    const char* addFlag = " + ";
    if (modifiers & HOTKEYF_CONTROL)
    {
        str += GetKeyName(VK_CONTROL, false) + addFlag;
    }

    if (modifiers & HOTKEYF_SHIFT)
    {
        str += GetKeyName(VK_SHIFT, false) + addFlag;
    }

    if (modifiers & HOTKEYF_ALT)
    {
        str += GetKeyName(VK_MENU, false) + addFlag;
    }

    str += GetKeyName(virtualKeyCode, !!(modifiers&HOTKEYF_EXT));
    return str;
}


int WHotKeyEdit::GetModifiers(int globalCode)
{
    int vk_add = 0;
    if (HIBYTE(globalCode) & HOTKEYF_CONTROL)
    {
        vk_add |= VK_CONTROL;
    }
    if (HIBYTE(globalCode) & HOTKEYF_SHIFT)
    {
        vk_add |= VK_SHIFT;
    }
    if (HIBYTE(globalCode) & HOTKEYF_ALT)
    {
        vk_add |= VK_MENU;
    }
    return vk_add;
}

int WHotKeyEdit::GetVitualKey(int globalCode)
{
    return LOBYTE(globalCode);
}


WHotKeyEdit::WHotKeyEdit(void)  
    : editShow_(new WEdit)
    , keyValue_(0)
    , keyValueOld_(0)
{

}

LRESULT WHotKeyEdit::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam, BOOL& handle)
{
    return WWindow::ProcessMessage(message, wParam, lParam, handle);
}


HWND WHotKeyEdit::Create(int x, int y, int width, int heiht, HWND parent)
{  
    //最先create的会是最先给点中来编辑的目标, 最后show的会在最上显示出来
    hWnd_ = CreateWindowEx(0, "EDIT", "", WS_CHILD|WS_VISIBLE|WS_BORDER, x, y, width, heiht, parent, NULL, GetModuleHandle(NULL), this);
    assert(hWnd_);
    ChangeWndProc();
    editShow_->Create("", x, y, width, heiht, parent);
    editShow_->
    return hWnd_;
}

void WHotKeyEdit::InitHotKeyText(int globalCode)
{
    keyValueOld_ = globalCode;
    tstring hotkey = MakeHotKeyString(globalCode);
    SetWindowText(hotkey);
    editShow_->SetWindowText(hotkey);
}

WHotKeyEdit::~WHotKeyEdit(void)
{

}

LRESULT WHotKeyEdit::OnSetFocus( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
//     tstring text = MakeHotKeyString(keyValue_);
// 
//     editShow_->SetWindowText(text);
    return FALSE;
}



LRESULT WHotKeyEdit::OnKillFocus( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    tstring text = GetKeyName(LOBYTE(keyValue_), false);
    if (text.empty())
    {
        text = MakeHotKeyString(keyValueOld_);
        SetWindowText(text);
        keyValue_ = keyValueOld_;
    }
    return FALSE;
}

LRESULT WHotKeyEdit::OnKeyDown( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    OutputDebugString("WM_KEYDOWN  hWnd == hWnd_\n");
    keyValue_ = TranslateToValue(wParam, lParam);
    tstring text = MakeHotKeyString(keyValue_);
    SetWindowText(text);
    //editShow_->SetWindowText(text);
    handle = TRUE;
    return FALSE;
}

LRESULT WHotKeyEdit::OnSystemKeyDown( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    return OnKeyDown(wParam, lParam, handle);
}

LRESULT WHotKeyEdit::OnChar( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    handle = TRUE;
    return FALSE;
}

LRESULT WHotKeyEdit::OnPaste( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    return FALSE;
}

LRESULT WHotKeyEdit::OnContextMenu( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    handle = TRUE;
    return FALSE;
}

int WHotKeyEdit::GetHotKeyCode()
{
    return keyValue_;
}


}