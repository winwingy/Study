// PowerOnTask.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <initguid.h>
#include <ole2.h>
#include <mstask.h>
#include <msterr.h>
#include <objidl.h>
#include <wchar.h>
#include <stdio.h>
#include <locale.h>


void WGetUserName(WCHAR * username, int len)
{
    // Get and display the name of the computer.
    char   szName[255]; 
    ULONG   nLen=sizeof(szName); 
    ::GetUserNameA(szName,&nLen);

    for(unsigned int i = 0; i < strlen(szName); i++)
    {
        username[i] = szName[i];
    }
    username[strlen(szName)] = '\0';
}

#define TRACE_WW

void RaiseToken()
{
    HANDLE   hToken;   
    TOKEN_PRIVILEGES   tkp;  
    BOOL bRet(FALSE);
    static bool bPrivi(false);
    if ( !bPrivi )
    {
        //定义变量   
        bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   
        char szRet[10] = {0};
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
        //GetCurrentProcess（）函数的作用是得到本进程的句柄   
        bRet = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid);  
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //LookupPrivilegeValue（）的作用是修改进程的权限   
        tkp.PrivilegeCount =  1;   
        //赋给本进程特权   
        tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;   
        bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);   
        //AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        bPrivi = true;
    }
}

int BuildTask()
{
    HRESULT hr = S_OK;
    ITaskScheduler *pITS;


    /////////////////////////////////////////////////////////////////
    // Call CoInitialize to initialize the COM library and then 
    // call CoCreateInstance to get the Task Scheduler object. 
    /////////////////////////////////////////////////////////////////
    hr = CoInitialize(NULL);
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(CLSID_CTaskScheduler,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_ITaskScheduler,
            (void **) &pITS);
        if (FAILED(hr))
        {
            CoUninitialize();
            return 1;
        }
    }
    else
    {
        return 1;
    }

    /////////////////////////////////////////////////////////////////
    /// Get user name of the computer
    /////////////////////////////////////////////////////////////////
    WCHAR username[512];
    memset(username, 0, sizeof(username));
    WGetUserName(username, sizeof(username));

    /////////////////////////////////////////////////////////////////
    // Call ITaskScheduler::NewWorkItem to create new task.
    /////////////////////////////////////////////////////////////////
    WCHAR pwszTaskName[512] = L"Veno Server WD_TEST_delay1";
    ITask *pITask;

    wcscat(pwszTaskName, username);
    pITS->Delete(pwszTaskName);
    hr = pITS->NewWorkItem(pwszTaskName,         // Name of task
        CLSID_CTask,          // Class identifier 
        IID_ITask,            // Interface identifier
        (IUnknown**)&pITask); // Address of task 
    //    interface


    pITS->Release();                               // Release object
    if (FAILED(hr))
    {
        CoUninitialize();
        fprintf(stderr, "Failed calling NewWorkItem, error = 0x%x\n",hr);
        return 1;
    }

    ///////////////////////////////////////////////////////////////////
    // Call ITask::SetApplicationName to specify the Application name
    // for Test Task.
    ///////////////////////////////////////////////////////////////////
    LPCWSTR pwszApplicationName = L"C:\\Windows\\System32\\notepad.exe "; 
    hr = pITask->SetApplicationName(pwszApplicationName);

    if (FAILED(hr))
    {
        wprintf(L"Failed calling ITask::SetApplicationName: ");
        wprintf(L"error = 0x%x\n",hr);
        pITask->Release();
        CoUninitialize();
        return 1;
    }

    /////////////////////////////////////////////////////////////
    ////// set flag Only if logged on /////
    /////////////////////////////////////////////////////////////
    hr = pITask->SetFlags(TASK_FLAG_RUN_ONLY_IF_LOGGED_ON);
    if (FAILED(hr))
    {
        wprintf(L"Failed calling ITask::EditWorkItem, ");
        wprintf(L"error = 0x%x\n",hr);
        CoUninitialize();
        return 1;
    }

    ////////////////////////////////////////////////////////////
    // set username for only for logged on
    hr = pITask->SetAccountInformation( username, NULL); //L"NT AUTHORITY\\SYSTEM"; 
    if (FAILED(hr))
    {
        wprintf(L"Failed calling ITask::SetApplicationName: ");
        wprintf(L"error = 0x%x\n",hr);
        pITask->Release();
        CoUninitialize();
        return 1;
    }


    ///////////////////////////////////////////////////////////////////
    // Call ITask::CreateTrigger to create new trigger.
    ///////////////////////////////////////////////////////////////////
    pITask->SetParameters(L"D:\\test\\testDelay.txt");

    ITaskTrigger *pITaskTrigger;
    WORD piNewTrigger;
    hr = pITask->CreateTrigger(&piNewTrigger,
        &pITaskTrigger);
    if (FAILED(hr))
    {
        wprintf(L"Failed calling ITask::CreatTrigger: ");
        wprintf(L"error = 0x%x\n",hr);
        pITask->Release();
        CoUninitialize();
        return 1;
    }


    //////////////////////////////////////////////////////
    // Define TASK_TRIGGER structure. Note that wBeginDay,
    // wBeginMonth, and wBeginYear must be set to a valid 
    // day, month, and year respectively.
    //////////////////////////////////////////////////////

    TASK_TRIGGER pTrigger;
    ZeroMemory(&pTrigger, sizeof (TASK_TRIGGER));

    // Add code to set trigger structure?
    pTrigger.wBeginDay =17;                  // Required
    pTrigger.wBeginMonth =6;                // Required
    pTrigger.wBeginYear =2016;              // Required
    pTrigger.cbTriggerSize = sizeof (TASK_TRIGGER); 
    pTrigger.wStartHour = 16;
    pTrigger.wStartMinute = 55;

  //  pTrigger.MinutesDuration = 2;
  //  pTrigger.MinutesInterval = 5;
    pTrigger.TriggerType = TASK_TIME_TRIGGER_ONCE;   ///TASK_EVENT_TRIGGER_AT_SYSTEMSTART   
    //pTrigger.Type.Daily.DaysInterval = 1;


    ///////////////////////////////////////////////////////////////////
    // Call ITaskTrigger::SetTrigger to set trigger criteria.
    ///////////////////////////////////////////////////////////////////

    hr = pITaskTrigger->SetTrigger (&pTrigger);
    if (FAILED(hr))
    {
        wprintf(L"Failed calling ITaskTrigger::SetTrigger: ");
        wprintf(L"error = 0x%x\n",hr);
        pITask->Release();
        pITaskTrigger->Release();
        CoUninitialize();
        return 1;
    }


    ///////////////////////////////////////////////////////////////////
    // Call IPersistFile::Save to save trigger to disk.
    ///////////////////////////////////////////////////////////////////

    IPersistFile *pIPersistFile;
    hr = pITask->QueryInterface(IID_IPersistFile,
        (void **)&pIPersistFile);
    hr = pIPersistFile->Save(NULL,
        TRUE);

    setlocale(LC_ALL, "chs");
    if (FAILED(hr))
    {
        DWORD err = GetLastError();
        wprintf(L"Failed calling IPersistFile::Save: ");
        wprintf(L"error = 0x%x\n",hr);
        pITask->Release();
        pITaskTrigger->Release();
        pIPersistFile->Release();
        CoUninitialize();
        wprintf(L"保存失败");
        system("pause");
        return 1;
    }
    wprintf(L"保存成功");
    system("pause");

    wprintf(L"The trigger was created and IPersistFile::Save was \n");
    wprintf(L"called to save the new trigger to disk.\n"); 

    ///////////////////////////////////////////////////////////////////
    // Release resources.
    ///////////////////////////////////////////////////////////////////

    pITask->Release();
    pITaskTrigger->Release();
    pIPersistFile->Release();
    CoUninitialize();
    printf("Created task.\n");
    return 0;
}


int main(int argc, char **argv)
{
    RaiseToken();
    BuildTask();
    return 0;
}