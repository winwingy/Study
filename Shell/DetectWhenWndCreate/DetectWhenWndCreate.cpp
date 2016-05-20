// DetectWhenWndCreate.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>

HHOOK g_hook = nullptr;

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

int _tmain(int argc, _TCHAR* argv[])
{
    HMODULE mod = GetModuleHandle(nullptr);
    g_hook = SetWindowsHookEx(WH_CBT, hookProc, mod, 0);
    DWORD err = GetLastError();
    system("pause");
    UnhookWindowsHookEx(g_hook);

	return 0;
}

