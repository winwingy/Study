#include "stdafx.h"
#include "ConnectionCom.h"
#import "../Debug/WLogger.exe" no_namespace


ConnectionCom::ConnectionCom()
    : logger_()
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
    if (SUCCEEDED(hr))
    {
        assert(0);
        return;
    }
    BSTR logPathAtl = ::SysAllocString(logPath);

    logger_->InitLog(logPathAtl, static_cast<LONG>(level), fileSize, fileCount);
    ::SysFreeString(logPathAtl);
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
        BSTR textAtl = ::SysAllocStringLen(text, len);
        *logged = logger_->log(textAtl, len);
        ::SysFreeString(textAtl);
        return true;
    }
    return false;
}
