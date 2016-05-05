#include "stdafx.h"
#include "LogLib.h"
#include <assert.h>
#include <windows.h>
#include <memory>

namespace
{

std::pair<LOG_INFO_LEVEL, TCHAR*> LogLevelToString[] =
{
    { LOG_INFO_LEVEL_DEBUG, _T("DEBUG") },
    { LOG_INFO_LEVEL_INFO, _T("INFO") },
    { LOG_INFO_LEVEL_ERROR, _T("ERROR") },
};

}

LogLib* LogLib::log_ = nullptr;

LogLib::LogLib()
    : isInit_(false)
    , logPath_()
    , level_(LOG_INFO_LEVEL_INFO)
    , fileSize_(FileSize)
    , fileCount_(FileCount)
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
    logPath_ = logPath;
    level_ = level;
    fileSize_ = fileSize;
    fileCount_ = fileCount;
    isInit_ = true;
    for (auto& per : observerList_)
    {
        if (per)
        {
            per->Init(logPath, level, fileSize, fileCount);
        }
    }
}

void LogLib::Exit()
{
    if (log_)
    {
        delete log_;
    }
    for (auto& per : observerList_)
    {
        if (per)
        {
            per->Exit();
        }
    }
}

void LogLib::Log(LOG_INFO_LEVEL level, const TCHAR* fun, int line, 
                 const TCHAR* format, ...)
{
    if (!isInit_)
    {
        assert(isInit_);
        return;
    }

    if (level > level_)
    {
        return;
    }

    tstringstream ss;
    SYSTEMTIME st = { 0 };
    GetLocalTime(&st);
    TCHAR timeStr[64] = { 0 };
    _stprintf_s(timeStr, _T("%04d-%02d-%02d  %02d:%02d:%02d.%03d"), 
                st.wYear, st.wMonth, st.wDay,
                st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    TCHAR levetStr[30] = { 0 };
    _stprintf_s(levetStr, _T("%s"), LogLevelToString[level].second);

    try
    {
        va_list list;
        va_start(list, format);
        int bufSize = _vsctprintf(format, list);
        bufSize += 1;
        std::unique_ptr<TCHAR, void(*)(void*)> bufPr(
            new TCHAR[bufSize], [](void* p){ delete[] p; });
        _vsntprintf_s(bufPr.get(), bufSize, bufSize-1, format, list);
        (bufPr.get())[bufSize - 1] = 0;

        FormatLog(_T("%s [%s] %s : %d  %s  %s"),
            timeStr, levetStr, fun, line, bufPr);
    }
    catch (...)
    {
        FormatLog(_T("%s [%s] %s : %d [LogException] %s\n"),
                  timeStr, levetStr, fun, line);
    }
    

}

void LogLib::FormatLog(const TCHAR* format, ...)
{
    va_list list;
    va_start(list, format);
    int bufSize = _vsctprintf(format, list);
    bufSize += 1;
    std::unique_ptr<TCHAR, void(*)(void*)> bufPr(
        new TCHAR[bufSize], [](void* p){ delete[] p; });
    _vsntprintf_s(bufPr.get(), bufSize, bufSize-1, format, list);
    (bufPr.get())[bufSize - 1] = 0;

    for (auto& item : observerList_)
    {
        if (item)
        {
            int writted = 0;
            bool ret = item->log(bufPr.get(), bufSize - 1, &writted);
        }
    }
}

void LogLib::AddObserver(LogLibObserver* observer)
{
    for (auto& item : observerList_)
    {
        if (item == observer)
        {
            assert(false);
            return;
        }
    }
    observerList_.push_back(observer);
}

void LogLib::RemoveObserver(LogLibObserver* observer)
{
    for (auto& i = observerList_.begin(); i != observerList_.end(); ++i)
    {
        if (*i == observer)
        {
            observerList_.erase(i);
            return;
        }
    }
}

void LogLib::ClearObserver()
{
    observerList_.clear();
}
