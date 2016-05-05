#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <tchar.h>
#include <memory>

#include "LogLibRef.h"

#ifdef _UNICODE
#define FUNCTIONNAME __FUNCTIONW__
#define tstring std::wstring
#define tstringstream std::wstringstream
#else
#define FUNCTIONNAME __FUNCTION__
#define tstring std::string
#define tstringstream std::stringstream
#endif

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

class LogLib
{
public:

    LogLib();
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
    void FormatLog(const TCHAR* format, ...);
    static LogLib* log_;
    bool isInit_;
    std::wstring logPath_;
    LOG_INFO_LEVEL level_;
    int fileSize_;
    int fileCount_;
    std::vector<LogLibObserver*> observerList_;
};

