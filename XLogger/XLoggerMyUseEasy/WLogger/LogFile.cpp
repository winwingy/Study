#include "stdafx.h"
#include "LogFile.h"
#include <assert.h>
#include "../LogLib/LogLibRef.h"

#ifdef _UNICODE
#define OPEN_CCS L",ccs=UNICODE"
#else 
#define OPEN_CCS ""
#endif // _UNICODE



LogFile::LogFile()
    : isInit_(false)
    , fp_(nullptr, [](FILE* p){ fclose(p); })
    , logPath_()
    , fileSize_(LogFileSize)
    , fileCount_(LogFileCount)
{
}


LogFile::~LogFile()
{
}

void LogFile::Init(const TCHAR* logPath, LOG_INFO_LEVEL level,
                  int fileSize, int fileCount)
{
    logPath_ = logPath;
    fileSize_ = fileSize;
    fileCount_ = fileCount;
    isInit_ = true;

    FILE* fp = nullptr;
    errno_t err = _tfopen_s(&fp, logPath_.c_str(),
                            tstring(_T("r") + tstring(OPEN_CCS)).c_str());
    if (err != 0)
        err = _tfopen_s(&fp, logPath_.c_str(), 
        tstring(_T("w") + tstring(OPEN_CCS)).c_str());
    else
    {
        fclose(fp);
        err = _tfopen_s(&fp, logPath_.c_str(),
                        tstring(_T("a") + tstring(OPEN_CCS)).c_str());
    }

    if (0 != err)
    {
       // assert(0);
        return;
    }
    fp_.reset(fp);
}

void LogFile::Exit()
{
    fp_.reset();
}

bool LogFile::WriteToFile(std::vector<tstring>* datas)
{
    if (!isInit_)
    {
        return false;
    }

    for (auto& item : *datas)
    {
        int writted = fwrite(item.c_str(), sizeof(TCHAR),
                             item.size(), fp_.get());
        //OutputDebugString(item.c_str());
        assert(writted > 0);
    }
    fflush(fp_.get());
    return true;
}

bool LogFile::WriteToFile(const TCHAR* text, int lenth)
{
    if (!isInit_)
    {
        return false;
    }
    int writted = fwrite(text, sizeof(TCHAR),
                         lenth, fp_.get());
        //OutputDebugString(item.c_str());
    assert(writted > 0);
    fflush(fp_.get());
    return true;
}
