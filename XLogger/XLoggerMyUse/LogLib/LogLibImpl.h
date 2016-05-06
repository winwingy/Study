#pragma once
#include <string>
#include <vector>
#include <tchar.h>
#include <memory>

#include "LogLibRef.h"
class LogLibImpl
{
public:
    LogLibImpl();
    ~LogLibImpl();
    void Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
              int fileSize, int fileCount);
    void Exit();
    void Log(LOG_INFO_LEVEL level, const TCHAR* fun, int line,
             const TCHAR* buf);

    void AddObserver(LogLibObserver* observer);
    void RemoveObserver(LogLibObserver* observer);
    void ClearObserver();

private:
    void FormatLog(const TCHAR* format, ...);
    bool isInit_;
    std::wstring logPath_;
    LOG_INFO_LEVEL level_;
    int fileSize_;
    int fileCount_;
    std::vector<LogLibObserver*> observerList_;
};

