// LogLibApp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../LogLib/LogLib.h"
#include <atlbase.h>

#pragma comment(lib, "../Debug/LogLib.lib")
using namespace ATL;

void GeneralString(std::wstring* text)
{
    for (int i = 0; i < 26; ++i)
    {
        text->push_back(L'A' + i);
    }

    for (int i = 0; i < 26; ++i)
    {
        text->push_back(L'a' + i);
    }

    for (int i = 0; i < 10; ++i)
    {
        text->push_back(L'0' + i);
    }
}

void LogLyric()
{
    LogLib::GetInstance()->Init(L"D:\\test\\LogLyric.txt", LOG_INFO_LEVEL_INFO,
                                100, 1);

    std::wstring text;
    GeneralString(&text);
    text += text;
    text += text;   
    LOG_INFO(L"ABC");
//     for (int i = 0; i < 10; ++i)
//     {
//         LOG_INFO(L"%s\r\n", text.c_str());
//     } 
    system("pause");
    LogLib::GetInstance()->Exit();
}



int _tmain(int argc, _TCHAR* argv[])
{
    ::CoInitialize(NULL);
    LogLyric();
    ::CoUninitialize();
	return 0;
}

