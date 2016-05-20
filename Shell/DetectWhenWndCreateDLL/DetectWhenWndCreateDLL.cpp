// DetectWhenWndCreateDLL.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#define IN_DLL
#include "DetectWhenWndCreateDLL.h"
#include "HookDLLDefine.h"
#include <assert.h>



LRESULT CALLBACK hookProc(int code, WPARAM w, LPARAM l)
{
    if (code < 0)
    {
        return CallNextHookEx(g_hook, code, w, l);
    }
    if (code == HCBT_CREATEWND)
    {
        TCHAR szBuf[1024];
        HWND wnd = reinterpret_cast<HWND>(w);
        GetWindowText(wnd, szBuf, 1024);
    }
    return CallNextHookEx(g_hook, code, w, l);

}



void Hook()
{
    // 只能勾到本程序 的
    g_hook = SetWindowsHookEx(WH_CBT, hookProc, g_module, 0);
    DWORD err = GetLastError();
    assert(g_hook);
}

void UnHook()
{
    UnhookWindowsHookEx(g_hook);
}
