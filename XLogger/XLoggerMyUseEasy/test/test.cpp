// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <tchar.h>
#include <locale.h>

int _tmain(int argc, _TCHAR* argv[])
{
    FILE* fp = nullptr;
    errno_t err = _tfopen_s(&fp, L"D:\\test\\testun.txt", L"a+,ccs=UNICODE");
    TCHAR* p = L"abc我\r\n";
    int len = _tcslen(p);
    fwrite(p, 2, len, fp);
    fclose(fp);
	return 0;
}

