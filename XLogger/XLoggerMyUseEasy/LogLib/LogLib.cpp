#include "stdafx.h"
#include "LogLib.h"

#include <stdarg.h>

#include "LogLibImpl.h"
#include "ConnectionCom.h"

LogLib* LogLib::log_ = nullptr;

LogLib::LogLib()
    : impl_(new LogLibImpl())
    , connect_(new ConnectionCom())
{

}


LogLib::~LogLib()
{
}

LogLib* LogLib::GetInstance()
{
    if (!log_)
    {
        log_ = new LogLib();
    }
    return log_;
}

void LogLib::Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
                  int fileSize, int fileCount)
{
    impl_->AddObserver(connect_.get());
    impl_->Init(logPath, level, fileSize, fileCount);    
}

void LogLib::Exit()
{
    impl_->ClearObserver();
    impl_->Exit();
    if (log_)
    {
        delete log_;
    }
}

void LogLib::Log(LOG_INFO_LEVEL level, const TCHAR* fun, int line, 
                 const TCHAR* format, ...)
{
    va_list list;
    va_start(list, format);
    int bufSize = _vsctprintf(format, list);
    bufSize += 1;
    std::unique_ptr<TCHAR, void(*)(void*)> bufPr(
        new TCHAR[bufSize], [](void* p){ delete[] p; });
    _vsntprintf_s(bufPr.get(), bufSize, bufSize - 1, format, list);
    (bufPr.get())[bufSize - 1] = 0;
    impl_->Log(level, fun, line, bufPr.get());

}

void LogLib::AddObserver(LogLibObserver* observer)
{
    impl_->AddObserver(observer);
}

void LogLib::RemoveObserver(LogLibObserver* observer)
{
    impl_->AddObserver(observer);
}

void LogLib::ClearObserver()
{
    impl_->ClearObserver();
}
