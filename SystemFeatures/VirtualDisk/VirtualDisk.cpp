// VirtualDisk.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <assert.h>

void CreateVirtualDisk()
{
    BOOL ret = DefineDosDevice(DDD_RAW_TARGET_PATH, L"I:",
                               L"\\GLOBAL??\\D:\\test");
    assert(ret);
    HANDLE hFile = CreateFileW(L"\\\\.\\I:", GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ|FILE_SHARE_WRITE,
                              NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,
                              NULL);

    assert(hFile);
    CloseHandle(hFile);

    // 删除盘符
    ret = DefineDosDevice(
        DDD_RAW_TARGET_PATH | DDD_REMOVE_DEFINITION | DDD_EXACT_MATCH_ON_REMOVE,
        L"I:", L"\\GLOBAL??\\D:\\test");
    assert(ret);

}

int _tmain(int argc, _TCHAR* argv[])
{
    CreateVirtualDisk();
	return 0;
}

