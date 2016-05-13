// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#define ENABLE_XLOGGER
#include "../../Include/XLogger2.h"
#include "../Logger/XLogger.h"
#pragma comment(lib, "../../Output/lib/Debug32/XLogger.lib")

#ifdef _UNICODE
#define FUNCTIONNAME __FUNCTIONW__
#define tstring std::wstring
#define tstringstream std::wstringstream
#else
#define FUNCTIONNAME __FUNCTION__
#define tstring std::string
#define tstringstream std::stringstream
#endif

class City
{
public:
    void ShowFun()
    {
        XLOGGER2_FUNCT_INFO;
    }

    void LoveCity()
    {
        XLOGGER2_FUNCT_INFO;
        ShowFun();
        ShowFun();
    }
};

void log2()
{

    XLOGGER2_OPEN(_T("UpgradeXLog"));
    {
        XLOGGER2_FUNCT_INFO;
    }

    City city;
    city.ShowFun();
    city.LoveCity();
    XLOGGER2_CLOSE
}

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

class log1Fun
{
public:
    void Show()
    {
        tstring text;
        GeneralString(&text);
        DWORD beg = GetTickCount();
        for (int i = 0; i < 100000; ++i)
        {
            XLOGGER_INFO(text);
        }
        DWORD end = GetTickCount();
        TCHAR useTime[100];
        _stprintf_s(useTime, _T("useTime: %d \n"), end - beg);
        OutputDebugString(useTime);
        MessageBox(nullptr, useTime, useTime, MB_OK);
    }

    void Eat()
    {
        XLOGGER_ERROR(_T("Error Now"));
    }
};

void log1()
{
    XLOGGER_OPEN;
    log1Fun log1fun;
    log1fun.Show();
    XLOGGER_CLOSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    ::CoInitialize(0);
    log1();
    ::CoUninitialize();
	return 0;
}

