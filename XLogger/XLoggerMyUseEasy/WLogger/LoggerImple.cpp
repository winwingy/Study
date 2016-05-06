#include "stdafx.h"
#include "LoggerImple.h"

#include <assert.h>

#include "LogFile.h"
#include "AutoLock.h"

#pragma comment(lib, "user32.lib")

namespace
{
    void UnmapView(TCHAR* p)
    {
        if (p)
            UnmapViewOfFile(p);
    }

    void CloseMapHandle(void* p)
    {
        if (p != INVALID_HANDLE_VALUE &&
            p != nullptr)
            CloseHandle(p);
    }
}


LoggerImple::LoggerImple()
    : logFile_(new LogFile())
    , logThread_(nullptr, CloseMapHandle)
    , logSemaphore_(nullptr, CloseMapHandle)
    , exit_(false)
    , memHandle_(nullptr, CloseMapHandle)
    , mapView_(nullptr, UnmapView)
    , datas_(new std::vector<tstring>())
    , lockData_(new AutoLockBase())
{

}

LoggerImple::~LoggerImple()
{

}

unsigned __stdcall LoggerImple::WriteLogThread(void* param)
{
    LoggerImple* pThis = reinterpret_cast<LoggerImple*>(param);
    if (pThis)
    {
        pThis->WriteLogProc();
    }
    return 0;
}

STDMETHODIMP LoggerImple::InitLog(BSTR logPath, LONG level,
                                  LONG fileSize, LONG fileCount)
{
    HANDLE share = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
                                   FALSE, L"ShareMemUseEasy");
    DWORD err = GetLastError();
    memHandle_.reset(share);
    assert(memHandle_.get() != INVALID_HANDLE_VALUE);
    mapView_.reset((TCHAR*)MapViewOfFile(memHandle_.get(),
        FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0), UnmapView);

    logFile_->Init(logPath, static_cast<LOG_INFO_LEVEL>(level),
                   fileSize, fileCount);
    logThread_.reset((HANDLE)_beginthreadex(
        nullptr, 0, WriteLogThread, this, 0, nullptr));
    logSemaphore_.reset(CreateSemaphore(0, 0, 10000, nullptr));
    return S_OK;
}



STDMETHODIMP LoggerImple::Exit()
{
    exit_ = true;
    ReleaseSemaphore(logSemaphore_.get(), 3, nullptr);
    WaitForSingleObject(logThread_.get(), INFINITE);
    logFile_->Exit();
    return S_OK;
}

STDMETHODIMP LoggerImple::log(BSTR text, LONG len, LONG* logged)
{
    tstring item(mapView_.get(), mapView_.get() + len);
    {
        AutoLock l(*lockData_);
        datas_->push_back(item);
    }
    ReleaseSemaphore(logSemaphore_.get(), 1, nullptr);
    return S_OK;
}

void LoggerImple::WriteLogProc()
{
    while (true)
    {  
        WaitForSingleObject(logSemaphore_.get(), INFINITE);
        std::vector<tstring> datas;
        {
            AutoLock l(*lockData_);
            datas = *datas_;
            datas_->empty();
        }
        bool ret = logFile_->WriteToFile(&datas);
        if (exit_)
        {
            break;
        }
    }
}
