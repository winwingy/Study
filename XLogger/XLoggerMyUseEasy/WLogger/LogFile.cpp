#include "stdafx.h"
#include "LogFile.h"
#include <assert.h>
#include "../LogLib/LogLibRef.h"


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
    errno_t err = _tfopen_s(&fp, logPath_.c_str(), _T("a,ccs=UNICODE"));
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
        const WCHAR* p = item.c_str();
        FILE* fp = nullptr;
        errno_t err = _tfopen_s(&fp, L"D:\\test\\logfile.txt", _T("a,ccs=UNICODE"));
        int writted = fwrite(item.c_str(), 2,
                             item.size(), fp);
        fclose(fp);
        OutputDebugString(item.c_str());
        assert(writted > 0);
    }
    fflush(fp_.get());
    return true;
}
