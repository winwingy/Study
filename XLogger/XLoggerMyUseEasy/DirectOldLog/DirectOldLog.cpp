// DirectOldLog.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Log.h"
#include "../LogLib/LogLibRef.h"
#include <windows.h>

void GeneralString(tstring* text)
{
    for (int i = 0; i < 26; ++i)
    {
        text->push_back(_T('A') + i);
    }

    for (int i = 0; i < 26; ++i)
    {
        text->push_back(_T('a') + i);
    }

    for (int i = 0; i < 10; ++i)
    {
        text->push_back(_T('0') + i);
    }
    *text += *text;
}


int _tmain(int argc, _TCHAR* argv[])
{
    CLog::Instance()->Init("D:\\test\\LogDirect.txt", 2, 10000);

    tstring text;
    GeneralString(&text);
    DWORD beg = GetTickCount();
    for (int i = 0; i < 1000000; ++i)
    {
        LOG_INFO((_T("%s\r\n"), text.c_str()));
    } 
    DWORD end = GetTickCount();
    TCHAR useTime[100];
    _stprintf_s(useTime, _T("useTime: %d \n"), end - beg);
    OutputDebugString(useTime);
	return 0;
}

