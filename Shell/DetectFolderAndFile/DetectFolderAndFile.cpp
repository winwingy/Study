// DetectFolderAndFile.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <assert.h>


void RegesterFolderAndFileDetect()
{
    HANDLE notify = FindFirstChangeNotification(
        L"D:\\test", TRUE,
        FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_ATTRIBUTES |
        FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE);

    if (notify == INVALID_HANDLE_VALUE)
    {
        assert(notify);
        return;
    }

    while(1)
    {
        WaitForSingleObject(notify, INFINITE);
        OutputDebugString(L"目录有改变\n");
        FindNextChangeNotification(notify);
    }

    FindCloseChangeNotification(notify);

}


int _tmain(int argc, _TCHAR* argv[])
{
    RegesterFolderAndFileDetect();
	return 0;
}

