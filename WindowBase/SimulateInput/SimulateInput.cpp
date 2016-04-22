// SimulateInput.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>

void SendAscii(wchar_t data, BOOL shift)
{
    INPUT input[2] = { 0 };
    if (shift)
    {
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_SHIFT;
        SendInput(1, input, sizeof(input[0]));
    }

    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = data;
    
    input[1]
}






int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

