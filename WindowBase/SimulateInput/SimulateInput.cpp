// SimulateInput.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <atlconv.h>


void SendAsciiCtrlAlt(wchar_t vkData, BOOL shift, BOOL ctrl, BOOL alt)
{
    INPUT input[2] = { 0 };
    if (shift)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_SHIFT;
        SendInput(1, input, sizeof(INPUT));
    }

    if (ctrl)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_CONTROL;
        SendInput(1, input, sizeof(INPUT));
    }

    if (alt)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_MENU;
        SendInput(1, input, sizeof(INPUT));
    }

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = vkData;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = vkData;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, input, sizeof(INPUT));

    if (shift)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_SHIFT;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, input, sizeof(INPUT));
    }

    if (ctrl)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_CONTROL;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, input, sizeof(INPUT));
    }

    if (alt)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_MENU;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, input, sizeof(INPUT));
    }
}


void SendAscii(wchar_t vkData, BOOL shift)
{
    INPUT input[2] = { 0 };
    if (shift)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_SHIFT;
        SendInput(1, input, sizeof(INPUT));
    }

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = vkData;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = vkData;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, input, sizeof(INPUT));

    if (shift)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_SHIFT;
        input[0].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(1, input, sizeof(INPUT));
    }
}

void SendUnicode(wchar_t data)
{
    INPUT input[2] = { 0 };

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = 0;
    input[0].ki.wScan = data;
    input[0].ki.dwFlags = KEYEVENTF_UNICODE;

    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = 0;
    input[1].ki.wScan = data;
    input[1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;

    SendInput(2, input, sizeof(INPUT));

}

void SendKeys(TCHAR* msg)
{
    short vk = 0;
    BOOL shift = FALSE;
    USES_CONVERSION;
    wchar_t* data = T2W(msg);
    int len = _tcslen(data);
    for (int i = 0; i < len; ++i)
    {
        if (data[i] >= 0 && data[i] < 256) // ascii 字符
        {
            vk = VkKeyScanW(data[i]);
            if ((HIBYTE(vk) == -1) && (LOBYTE(vk) == -1))
            {
                SendUnicode(data[i]);
            }
            else
            {
                if (vk < 0)
                {
                    vk = ~vk + 0x1;  // ??
                }
                shift = HIBYTE(vk) & 0x1;
                if (GetKeyState(VK_CAPITAL) & 0x1)
                {
                    if (data[i] >= 'a' && data[i] <= 'z' || 
                        data[i] >= 'A' && data[i] <= 'Z')
                    {
                        shift = !shift;
                    }
                }
                SendAscii(LOBYTE(vk), shift);
            }
        }
        else // unicode
        {
            SendUnicode(data[i]);
        }
    }

}


#include <windows.h>
void TestSendInput()
{
    ShellExecute(NULL, NULL, _T("notepad.exe"), NULL, NULL, SW_SHOWNORMAL);
    Sleep(5000); //等下记事本打开
    HWND note = FindWindow(NULL, _T("无标题 - 记事本"));
    if (note)
    {
        SetForegroundWindow(note);
        SetFocus(note);
        SendKeys(_T("我是sway，我爱中国！\nI love China!\nEmail: xmujava@163.com\t\n2010-05-21  \b\b"));
    }
}

void TestHotKey()
{
    SendAsciiCtrlAlt(L'U', FALSE, TRUE, TRUE);
}


int _tmain(int argc, _TCHAR* argv[])
{
    TestSendInput();

    TestHotKey();
	return 0;
}

