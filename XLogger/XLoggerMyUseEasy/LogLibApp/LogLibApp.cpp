// LogLibApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../LogLib/LogLib.h"
#include <atlbase.h>

#pragma comment(lib, "../Debug/LogLib.lib")
using namespace ATL;

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

void LogComAyscToFile()
{
    LogLib::GetInstance()->Init(_T("D:\\test\\LogLyric.txt"), LOG_INFO_LEVEL_INFO,
                                100, 1);

    tstring text;
    GeneralString(&text);
    DWORD beg = GetTickCount();
    for (int i = 0; i < 1000000; ++i)
    {
        LOG_INFO(_T("%s\r\n"), text.c_str());
    }
    DWORD end = GetTickCount();

    LogLib::GetInstance()->Exit();
    DWORD ExitEnd = GetTickCount();
    TCHAR useTime[100];
    _stprintf_s(useTime, 
                _T("useTime: %d \n plus Exit total useTime: %d \n"),
                end - beg, ExitEnd - beg);
    OutputDebugString(useTime);
}

void LogByOldDirectToFile()
{

}

int _tmain(int argc, _TCHAR* argv[])
{
    ::CoInitialize(NULL);
    LogComAyscToFile();
    ::CoUninitialize();
    return 0;
}
