#pragma once
//#pragma warning( push  )
#pragma warning( disable:4996 )
#include <stdio.h>
#include <stdarg.h>

#define MAX_LOG_FILE_SIZE    5   // 默认最大日志文件5M

#define LOG_TRACE(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Trace X; } while (0);

#define LOG_DEBUG(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Debug X; } while (0);

#define LOG_INFO(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Info X; } while (0);

#define LOG_WARN(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Warn X; } while (0);

#define LOG_ERROR(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Error X; } while (0);

#define LOG_FATAL(X) do { \
    CLog::Instance()->SetSourceFileName(__FILE__); \
    CLog::Instance()->SetLine(__LINE__); \
    CLog::Instance()->SetFunctionName(__FUNCTION__); \
    CLog::Instance()->Fatal X; } while (0);

class CLog
{
public:
    enum
    {
        LEVEL_TRACE   = 0,      // TRACE
        LEVEL_DEBUG,            // DEBUG
        LEVEL_INFO,             // INFO
        LEVEL_WARN,             // WARN
        LEVEL_ERROR,            // ERROR
        LEVEL_FATAL,            // FATAL
        LEVEL_UNKNWN            // UNKNWN
    };

    enum
    {
        ARCHIVAL = 0,           // 归档日志模式
        CIRCULAR                // 循环日志模式
    };

public:
    CLog();
    ~CLog();
    static CLog *  Instance();

public:
    int     Init(const char *pPath, int nLevel = CLog::LEVEL_INFO, 
        int nMaxLogFileSize = MAX_LOG_FILE_SIZE, int nMode = CLog::CIRCULAR, int nFileCount = 2);
    void    SetMaxLogFileSize(int nSize);
    void    SetLevel(int nLevel);
    void    SetMode(int nMode);
    void    SetFileCount(int nFileCount);
    void    SetLine(int nLine);
    void    SetSourceFileName(const char *pFileName);
    void    SetFunctionName(const char *pFuncName);
    void    Trace(const char *fmt, ...);
    void    Debug(const char *fmt, ...);
    void    Info(const char *fmt, ...);
    void    Warn(const char *fmt, ...);
    void    Error(const char *fmt, ...);
    void    Fatal(const char *fmt, ...);

private:
    void    Log(int nLevel, const char *fmt, va_list p);

private:
    int     m_nLevel;           // 日志级别
    int     m_nMaxLogFileSize;  // 最大日志文件
    int     m_nMode;            // 日志模式，ARCHIVAL或者CIRCULAR
    int     m_nFileCount;       // 日志文件个数（循环日志才有意义）
    int     m_nLine;            // 行数
    char *  m_pSrcFileName;     // 源文件名
    char *  m_pFuncName;        // 函数名
    char *  m_pPath;            // 当前日志文件路径
    FILE *  m_pLogFile;         // 当前日志文件FILE指针
};
//#pragma warning( pop )