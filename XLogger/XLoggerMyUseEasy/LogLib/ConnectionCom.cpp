#include "stdafx.h"
#include "ConnectionCom.h"
#import "../Debug/WLogger.exe" no_namespace

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


ConnectionCom::ConnectionCom()
    : logger_()
    , memHandle_(nullptr, CloseMapHandle)
    , write_(nullptr, UnmapView)
{
}


ConnectionCom::~ConnectionCom()
{
}

void ConnectionCom::Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
                         int fileSize, int fileCount)
{
    HRESULT hr = CoCreateInstance(__uuidof(Logger), 0, CLSCTX_LOCAL_SERVER,
                                  __uuidof(ILogger), 
                                  reinterpret_cast<void**>(&logger_));
    if (!SUCCEEDED(hr))
    {
        assert(0);
        return;
    }  
    
    memHandle_.reset(CreateFileMapping((HANDLE)-1, nullptr, PAGE_READWRITE, 0,
        10 * sizeof(TCHAR), L"ShareMemUseEasy"));
    assert(memHandle_.get() != INVALID_HANDLE_VALUE);
    write_.reset((TCHAR*)MapViewOfFile(memHandle_.get(),
        FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0), UnmapView);

    BSTR logPathAtl = ::SysAllocString(logPath);
    logger_->InitLog(logPathAtl, static_cast<LONG>(level), fileSize, fileCount);
    ::SysFreeString(logPathAtl);


    return;
}

void ConnectionCom::Exit()
{
    if (logger_)
    {
        logger_->Exit();
    }
}

bool ConnectionCom::log(const TCHAR* text, int len, int* logged)
{
    if (logger_)
    {
        memcpy(write_.get(), text, len * sizeof(TCHAR));
        BSTR textAtl = ::SysAllocStringLen(L"0", 1);
        *logged = logger_->log(textAtl, len);
        ::SysFreeString(textAtl);
        return true;
    }
    return false;
}
