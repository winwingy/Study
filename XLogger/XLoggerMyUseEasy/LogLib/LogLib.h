#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <tchar.h>
#include <memory>

#include "LogLibRef.h"

#define LOG_DEBUG(format, ...) do \
{\
    LogLib::GetInstance()->Log(LOG_INFO_LEVEL_DEBUG, FUNCTIONNAME, __LINE__,\
    format, __VA_ARGS__); \
} while (0);

#define LOG_INFO(format, ...) do \
{\
    LogLib::GetInstance()->Log(LOG_INFO_LEVEL_INFO, FUNCTIONNAME, __LINE__,\
    format, __VA_ARGS__); \
} while (0);

#define LOG_ERROR(format, ...) do \
{\
    LogLib::GetInstance()->Log(LOG_INFO_LEVEL_ERROR, FUNCTIONNAME, __LINE__,\
    format, __VA_ARGS__); \
} while (0);

class LogLibImpl;
class ConnectionCom;
class LogLib
{
public:
    ~LogLib();

    static LogLib* GetInstance();
    void Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
              int fileSize, int fileCount);
    void Exit();
    void Log(LOG_INFO_LEVEL level, const TCHAR* fun, int line,
             const TCHAR* format, ...);

    void AddObserver(LogLibObserver* observer);
    void RemoveObserver(LogLibObserver* observer);
    void ClearObserver();

private:
    LogLib();
    static LogLib* log_;
    std::unique_ptr<LogLibImpl> impl_;
    std::unique_ptr<ConnectionCom> connect_;
};

