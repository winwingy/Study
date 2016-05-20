#include "stdafx.h"
#include "Log.h"
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_FILE_NAME   300
#define MAX_FUNC_NAME   50

//////////////////////////////////////////////////////////////////////////
// CLog实例
CLog _logger;

CLog::CLog()
: m_nLevel(CLog::LEVEL_INFO)
, m_nMaxLogFileSize(MAX_LOG_FILE_SIZE)
, m_nMode(CLog::CIRCULAR)
, m_nFileCount(2)
, m_nLine(0)
, m_pSrcFileName(NULL)
, m_pFuncName(NULL)
, m_pPath(NULL)
, m_pLogFile(NULL)
{
    m_pSrcFileName = (char *)malloc(MAX_FILE_NAME);
    m_pFuncName = (char *)malloc(MAX_FUNC_NAME);
    assert(NULL != m_pSrcFileName && NULL != m_pFuncName);
}

CLog::~CLog()
{
    if (m_pSrcFileName)
    {
        free(m_pSrcFileName);
    }

    if (m_pFuncName)
    {
        free(m_pFuncName);
    }

    if (m_pPath)
    {
        free(m_pPath);
    }

    if (m_pLogFile)
    {
        fclose(m_pLogFile);
    }
}

CLog * CLog::Instance()
{
    return &_logger;
}

int CLog::Init(const char *pPath, 
               int nLevel /* = CLog::LEVEL_INFO */, 
               int nMaxLogFileSize /* = MAX_LOG_FILE_SIZE */, 
               int nMode /* = CLog::ARCHIVAL */, 
               int nFileCount /* = 2 */)
{
    assert(NULL != pPath);

    m_nLevel = nLevel;
    m_nMaxLogFileSize = nMaxLogFileSize;
    m_pPath = (char *)malloc(strlen(pPath) + 1);
    strcpy(m_pPath, pPath);
    if (m_nLevel < CLog::LEVEL_TRACE || m_nLevel > CLog::LEVEL_FATAL)
    {
        m_nLevel = CLog::LEVEL_UNKNWN;
    }

    if (NULL == (m_pLogFile = fopen(pPath, "a")))
    {
        return  -1;
    }
    return 0;
}

void CLog::SetLevel(int nLevel)
{
    assert(nLevel >= CLog::LEVEL_TRACE && nLevel <= CLog::LEVEL_UNKNWN);
    if (nLevel < CLog::LEVEL_TRACE || nLevel > CLog::LEVEL_UNKNWN)
    {
        m_nLevel = LEVEL_UNKNWN;
    } 
    else
    {
        m_nLevel = nLevel;
    }    
}

void CLog::SetMaxLogFileSize(int nSize)
{
    m_nMaxLogFileSize = nSize;
}

void CLog::SetMode(int nMode)
{
    assert(CLog::CIRCULAR == nMode || CLog::ARCHIVAL == nMode);
    if (CLog::CIRCULAR != nMode || CLog::ARCHIVAL != nMode)
    {
        m_nMode = CLog::ARCHIVAL;
    } 
    else
    {
        m_nMode = nMode;
    }    
}

void CLog::SetFileCount(int nFileCount)
{
    assert(nFileCount >= 2);
    if (nFileCount < 2)
    {
        m_nFileCount = 2;
    } 
    else
    {
        m_nFileCount = nFileCount;
    }    
}

void CLog::SetLine(int nLine)
{
    m_nLine = nLine;
}

void CLog::SetSourceFileName(const char *pFileName)
{
    strncpy(m_pSrcFileName, pFileName, MAX_FILE_NAME);
}

void CLog::SetFunctionName(const char *pFuncName)
{
    strncpy(m_pFuncName, pFuncName, MAX_FUNC_NAME);
}

void CLog::Trace(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_TRACE, fmt, p);
    va_end(p);
}

void CLog::Debug(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_DEBUG, fmt, p);
    va_end(p);
}

void CLog::Info(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_INFO, fmt, p);
    va_end(p);
}

void CLog::Warn(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_WARN, fmt, p);
    va_end(p);
}

void CLog::Error(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_ERROR, fmt, p);
    va_end(p);
}

void CLog::Fatal(const char *fmt, ...)
{
    va_list p;
    va_start(p, fmt);
    Log(CLog::LEVEL_FATAL, fmt, p);
    va_end(p);
}

void CLog::Log(int nLevel, const char *fmt, va_list p)
{
    static const char *levelName[] = 
    {
        "[TRACE]",
        "[DEBUG]",
        "[INFO]",
        "[WARN]",
        "[ERROR]",
        "[FATAL]"
        "[UNKNWN]"
    };

    // 写日志级别大于或者等于设置的日志级别时才写到日志文件
    if (m_nLevel <= nLevel)
    {
        char *pBuf = NULL;
        int nBufSize = 0;
        char szTime[20];
        struct _stat st = {0};
        time_t now = time(NULL);
        strftime(szTime, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
        size_t nPathLen = strlen(m_pPath);

        nBufSize = _vscprintf(fmt, p);
        pBuf = (char *)malloc(nBufSize + 1);
        vsprintf(pBuf, fmt, p);
        if (m_nLevel <= CLog::LEVEL_DEBUG)
        {
            // 日志级别是TRACE，DEBUG时格式：
            // yyyy-mm-dd HH:MM:SS [Level] [ThreadID] [FuncName] [File:Line] logmessage
            fprintf(m_pLogFile, "%s %s %s %s:%d %s\n", szTime, 
                levelName[nLevel], m_pFuncName, m_pSrcFileName, m_nLine, pBuf);
        }
        else
        {
            // 日志级别大于DEBUG时格式：
            // yyyy-mm-dd HH:MM:SS [Level] [ThreadID] logmessage
            fprintf(m_pLogFile, "%s %s %s\n", szTime, levelName[nLevel], pBuf);
        }
        free(pBuf);
        fflush(m_pLogFile);

        if (_fstat(_fileno(m_pLogFile), &st) == 0 
            && st.st_size >= (m_nMaxLogFileSize * 1024 * 1024))
        {
            fclose(m_pLogFile);
            char *pNewPath = (char *)malloc(nPathLen + 20);

            if (CLog::ARCHIVAL == m_nMode) // 归档日志模式
            {
                strftime(szTime, 20, "%Y-%m-%d", localtime(&now));
                sprintf(pNewPath, "%s.%s", m_pPath, szTime);
            } 
            else // 循环日志模式
            {
                bool bFound = false;
                struct _stat sttmp;

                // 查找是否有空闲的文件可写
                for (int i = 1; i < m_nFileCount; i++)
                {
                    sprintf(pNewPath, "%s.%d", m_pPath, i);
                    if (_stat(pNewPath, &sttmp) == -1 && ENOENT == errno)
                    {
                        bFound = true;
                        break;
                    }
                }
                
                // 找不到就删除最旧的日志文件，如xxx.log.1
                if (!bFound)
                {
                    sprintf(pNewPath, "%s.%d", m_pPath, 1);
                    remove(pBuf);
                    char *pNew = (char *)malloc(nPathLen + 20);
                    char *pOld = (char *)malloc(nPathLen + 20);
                    for (int i = 2; i < m_nFileCount; i++)
                    {
                        sprintf(pNew, "%s.%d", m_pPath, i - 1);
                        sprintf(pOld, "%s.%d", m_pPath, i);
                        rename(pOld, pNew);
                    }
                    free(pOld);
                    free(pNew);
                    sprintf(pNewPath, "%s.%d", m_pPath, m_nFileCount - 1);
                }                
            }
            rename(m_pPath, pNewPath);
            free(pNewPath);
            m_pLogFile = fopen(m_pPath, "a");
            assert(NULL != m_pLogFile);
        }        
    }
}