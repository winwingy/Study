#include "stdafx.h"
#include "LogLibImpl.h"

#include <assert.h>
#include <windows.h>
#include <memory>
#include <sstream>

namespace
{
    std::pair<LOG_INFO_LEVEL, TCHAR*> LogLevelToString[] =
    {
        { LOG_INFO_LEVEL_DEBUG, _T("DEBUG") },
        { LOG_INFO_LEVEL_INFO, _T("INFO") },
        { LOG_INFO_LEVEL_ERROR, _T("ERROR") },
    };
}

LogLibImpl::LogLibImpl()
    : isInit_(false)
    , logPath_()
    , level_(LOG_INFO_LEVEL_INFO)
    , fileSize_(LogFileSize)
    , fileCount_(LogFileCount)
{

}


LogLibImpl::~LogLibImpl()
{
}

void LogLibImpl::Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
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

void LogLibImpl::Exit()
{
    for (auto& per : observerList_)
    {
        if (per)
        {
            per->Exit();
        }
    }
}

void LogLibImpl::Log(LOG_INFO_LEVEL level, const TCHAR* fun, int line,
                 const TCHAR* buf)
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

 //   try
    {
        FormatLog(_T("%s [%s] %s : %d  %s"),
                  timeStr, levetStr, fun, line, buf);
    }
//     catch (...)
//     {
//         FormatLog(_T("%s [%s] %s : %d [LogException]\n"),
//                   timeStr, levetStr, fun, line);
//     }
}

void LogLibImpl::FormatLog(const TCHAR* format, ...)
{
    va_list list;
    va_start(list, format);
    int bufSize = _vsctprintf(format, list);
    bufSize += 1;
    std::unique_ptr<TCHAR, void(*)(void*)> bufPr(
        new TCHAR[bufSize], [](void* p){ delete[] p; });
    _vsntprintf_s(bufPr.get(), bufSize, bufSize - 1, format, list);
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

void LogLibImpl::AddObserver(LogLibObserver* observer)
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

void LogLibImpl::RemoveObserver(LogLibObserver* observer)
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

void LogLibImpl::ClearObserver()
{
    observerList_.clear();
}
