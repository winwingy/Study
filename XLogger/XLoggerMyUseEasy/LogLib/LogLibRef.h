#pragma once
#include <tchar.h>

#define LogFileSize 5 * 1024
#define LogFileCount 1

#ifdef _UNICODE
#define FUNCTIONNAME __FUNCTIONW__
#define tstring std::wstring
#define tstringstream std::wstringstream
#else
#define FUNCTIONNAME __FUNCTION__
#define tstring std::string
#define tstringstream std::stringstream
#endif

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
