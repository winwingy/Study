#include "stdafx.h"
#include "LoggerImple.h"
#include "../ShareMem/ShareMem.h"
#include "LogFile.h"
#pragma comment(lib, "../Debug/ShareMem.lib")

LoggerImple::LoggerImple()
    : shareMem_(new ShareMem())
    , logFile_(new LogFile())
{
}
LoggerImple::~LoggerImple()
{
}

STDMETHODIMP LoggerImple::InitLog(BSTR logPath, LONG level,
                                  LONG fileSize, LONG fileCount)
{
    shareMem_->Init();
    logFile_->Init(logPath, static_cast<enum LOG_INFO_LEVEL>(level),
                   fileSize, fileCount);
    return S_OK;
}

STDMETHODIMP LoggerImple::Exit()
{
    shareMem_->TryExit();
    shareMem_->Exit();
    return S_OK;
}

STDMETHODIMP LoggerImple::Write(BSTR* writeStr, LONG wantWriteLen,
                                LONG* writedLen)
{
    return S_OK;
}

STDMETHODIMP LoggerImple::Read(BSTR* buf, LONG bufLen, LONG* readedLen)
{
    return S_OK;
}

STDMETHODIMP LoggerImple::log(BSTR text, LONG len, LONG* logged)
{
    bool ret = logFile_->WriteToFile(text, len, reinterpret_cast<int*>(logged));
    return ret ? S_OK : S_FALSE;
}