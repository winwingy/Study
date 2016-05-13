// test.cpp : 定义控制台应用程序的入口点。
//

// #include "stdafx.h"
// #include <tchar.h>
// #include <locale.h>
// #include <comutil.h>
// #pragma comment(lib, "comsuppw.lib")
// 
// int _tmain(int argc, _TCHAR* argv[])
// {
//     BSTR bs = ::SysAllocString(L"abc");
//     char* ch = _com_util::ConvertBSTRToString(bs);
//     FILE* fp = nullptr;
//     errno_t err = _tfopen_s(&fp, _T("D:\\test\\testun.txt"), _T("w,ccs=UNICODE"));
//     TCHAR* p = _T("abc我\r\n");
//     int len = _tcslen(p);
//     fwrite(p, 2, len, fp);
//     fclose(fp);
// 	return 0;
// }

// Mutex_release.cpp : 定义控制台应用程序的入口点。
//
//
#include "stdafx.h"
#include "iostream"
#include "windows.h"

using namespace std;

DWORD WINAPI ThreadProc1(LPVOID lpParam);
DWORD WINAPI ThreadProc2(LPVOID lpParam);
HANDLE hEvent = NULL;
HANDLE hThread1 = NULL;
HANDLE hThread2 = NULL;
// int main(int argc, char *args[])
// {
//    // hEvent = CreateEvent(NULL, FALSE, FALSE, NULL); // 使用 *重置为无信号状态，初始化时*信号状态
//         hEvent = CreateMutex(NULL, FALSE, NULL); // FALSE: 创建线程没有获得互斥对象的所有权 TRUE: 创建线程获得初始所有权的互斥对象
//     hThread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc1, NULL, 0, NULL);
//     Sleep(200);
//     hThread2 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc2, NULL, 0, NULL);
//     Sleep(200);
//     if (NULL == hThread1 || NULL == hThread2)
//     {
//         cout << "create thread fail!";
//     }
//     DWORD dReturn = WaitForSingleObject(hEvent,INFINITE);
//     cout<< dReturn << endl;
//     ReleaseMutex(hEvent);
//     ReleaseMutex(hEvent);
//     while (1){
// 
//         Sleep(100);
//         ReleaseMutex(hEvent);
//        // SetEvent(hEvent);
//     }
//     return 0;
// }
// DWORD WINAPI ThreadProc1(LPVOID lpParam)
// {
//     cout << "in thread1@!" << endl;
//     DWORD dReturn = WaitForSingleObject(hEvent, INFINITE);
//     if (WAIT_OBJECT_0 == dReturn)
//     {
//         cout << "thread1 signaled ! " << endl;
//     }
// 
//     dReturn = WaitForSingleObject(hEvent, INFINITE);
//     if (WAIT_OBJECT_0 == dReturn)
//     {
//         cout << "thread1 signaled*&* ! " << endl;
//     }
// 
//     cout << "in thread1 --signal" << endl;
//     //SetEvent(hEvent);
// 
//     ReleaseMutex(hEvent);
//     ReleaseMutex(hEvent);
//     while (1){
//         Sleep(100);
//     }
//     return 0;
// }
// DWORD WINAPI ThreadProc2(LPVOID lpParam)
// {
//     cout << "in thread2@!" << endl;
//     DWORD dReturn = WaitForSingleObject(hEvent, INFINITE);
//     if (WAIT_OBJECT_0 == dReturn)
//     {
//         cout << "thread2 signaled ! " << endl;
//     }
//     cout << "in thread2--signal" << endl;
//     //SetEvent(hEvent);
//     //SetEvent(hEvent);
//     ReleaseMutex(hEvent);
//     while (1){
//         Sleep(100);
//     }
//     return 0;
// }

int _tmain(int argc, _TCHAR* argv[])
{

    HANDLE m_hMutex = CreateEvent(NULL,FALSE,TRUE,NULL);// 检查错误代码
 //  HANDLE m_hMutex = CreateMutex(NULL, TRUE, NULL);
   if(m_hMutex == NULL)
   {
       printf("create no!\n");
       return -1;
   }

   printf("create yes!\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes\n");

    SetEvent(m_hMutex);
    SetEvent(m_hMutex);
    SetEvent(m_hMutex);


   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 2\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 3\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 4\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 5\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 6\n");
   WaitForSingleObject(m_hMutex, INFINITE);
   printf("wait yes 7\n");
   return 0;
}