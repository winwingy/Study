// PreReadDisk.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string>
#include <windows.h>
#include <sstream>
#include <assert.h>

void PreReadFile(const std::wstring& path, std::wstring* out)
{
    HANDLE hFile = CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ,
                              nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN,
                              nullptr);
    assert(hFile);
    int len = 1000 * 1000;
    TCHAR* buf = new TCHAR[len];
    DWORD readFinally = 0;
    BOOL ret = FALSE;
    do 
    {
        ret = ReadFile(hFile, buf, len, &readFinally, NULL);
        if (out && ret && readFinally)
        {
            out->append(buf, readFinally);
        }
    } while (ret && readFinally);
    delete [] buf;
    CloseHandle(hFile);
}

void PreReadDiskUseTime()
{
    // 事实证明， 事先读一次的确提高下次读文件的时间， 由1482 降人低到 682
    PreReadFile(L"D:\\Kugou\\童丽 - 月满西楼.ape", nullptr);
    PreReadFile(L"D:\\Kugou\\许美静 - 倾城.ape", nullptr);
    PreReadFile(L"D:\\Kugou\\许美静 - 阳光总在风雨后.ape", nullptr);
    PreReadFile(L"D:\\Kugou\\郑融、薛凯琪 - 红绿灯.ape", nullptr);
    //Sleep(3000);
    std::wstring text;
    DWORD beg = GetTickCount();
    PreReadFile(L"D:\\Kugou\\童丽 - 月满西楼.ape", &text);
    PreReadFile(L"D:\\Kugou\\许美静 - 倾城.ape", &text);
    PreReadFile(L"D:\\Kugou\\许美静 - 阳光总在风雨后.ape", &text);
    PreReadFile(L"D:\\Kugou\\郑融、薛凯琪 - 红绿灯.ape", &text);
    DWORD end = GetTickCount();
    std::wstringstream ss;
    ss << L"使用了多少时间: " << end - beg << L"\n";
    std::wstring output = ss.str();
    setlocale(LC_ALL, "chs");
    printf("Use:: %ls", output.c_str());
}


int _tmain(int argc, _TCHAR* argv[])
{
    PreReadDiskUseTime();
    system("pause");
    PreReadFile(L"E:\\0工作资料\\HIFI\\FormatSound\\wong.wing.tsan.黄永灿.-九寨沟.水之歌.森之眠.专辑.tta", NULL);
    PreReadFile(L"E:\\0工作资料\\HIFI\\FormatSound\\Daniel Shafran - Kabalevsky-Davidov-Tchaikovsky.wv", NULL);
	return 0;
}

