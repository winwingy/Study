#pragma once
#include <tchar.h>

const int FileSize = 5 * 1024;
const int FileCount = 1;

enum LOG_INFO_LEVEL
{
    LOG_INFO_LEVEL_DEBUG,
    LOG_INFO_LEVEL_INFO,
    LOG_INFO_LEVEL_ERROR,
};

class LogLibObserver
{
public:
    virtual void Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
                      int fileSize, int fileCount) = 0;
    virtual void Exit() = 0;
    virtual bool log(const TCHAR* text, int len, int* logged) = 0;
};

class LogLibRef
{
public:
    LogLibRef();
    ~LogLibRef();
};

