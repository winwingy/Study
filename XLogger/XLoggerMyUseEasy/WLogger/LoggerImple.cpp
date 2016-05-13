#include "stdafx.h"
#include "LoggerImple.h"

#include <assert.h>
#include <comutil.h>

#include "LogFile.h"
#include "AutoLock.h"
#include "../ShareWriter/ShareWriter.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "ShareWriter.lib")

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


    tstring BSTR2TString(BSTR bs)
    {
        tstring str;
#ifdef _UNICODE
        str.assign(bs);
#else
        char* tc = _com_util::ConvertBSTRToString(bs);
        str.assign(tc);
        delete[] tc;
#endif
        return str;
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
    , shareWriter_(new ShareWriter())
{

}

LoggerImple::~LoggerImple()
{
    int a = 5;
}

unsigned __stdcall LoggerImple::WriteLogThread(void* param)
{
    LoggerImple* pThis = reinterpret_cast<LoggerImple*>(param);
    if (pThis)
    {
        pThis->WriteLogProcShareMem();
    }
    return 0;
}

STDMETHODIMP LoggerImple::InitLog(BSTR logPath, LONG level,
                                  LONG fileSize, LONG fileCount)
{
    HANDLE share = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,
                                   FALSE, _T("ShareMemUseEasy"));
    DWORD err = GetLastError();
    memHandle_.reset(share);
    assert(memHandle_.get() != INVALID_HANDLE_VALUE);
    mapView_.reset((TCHAR*)MapViewOfFile(memHandle_.get(),
        FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0), UnmapView);

    logFile_->Init(BSTR2TString(logPath).c_str(),
                   static_cast<LOG_INFO_LEVEL>(level),
                   fileSize, fileCount); 
    shareWriter_->Create(_T("LogShareMem"));
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

STDMETHODIMP LoggerImple::log(LONG len, LONG* logged)
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
            datas.assign(datas_->begin(), datas_->end());
            datas_->clear();
        }
        bool ret = logFile_->WriteToFile(&datas);
        if (exit_)
        {
            break;
        }
        //Sleep(500);
    }
}

void LoggerImple::WriteLogProcShareMem()
{
    std::unique_ptr<TCHAR, void(*)(void*)> szBuf
        (new TCHAR[ItemBuffSize], [](void*p){ delete [] p; });
    while (true)
    {
        int getted = 0;
        bool readRet = shareWriter_->Read(szBuf.get(), ItemBuffSize, &getted);
        if (readRet && getted)
        {
            bool ret = logFile_->WriteToFile(szBuf.get(), getted);
        }
        if (exit_ && !readRet)
        {
            break;
        }
    }
}
