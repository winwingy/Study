// Logger.cpp : CLogger 的实现

#include "stdafx.h"
#include "Logger.h"
#include "LoggerImple.h"


// CLogger
CLogger::CLogger()
    : impl_(new LoggerImple())
{
}

CLogger::~CLogger()
{
}


STDMETHODIMP CLogger::Exit()
{
    impl_->Exit();
    return S_OK;
}

STDMETHODIMP CLogger::Write(BSTR* writeStr, LONG wantWriteLen, LONG* writedLen)
{
    impl_->Write(writeStr, wantWriteLen, writedLen);
    return S_OK;
}

STDMETHODIMP CLogger::Read(BSTR* buf, LONG bufLen, LONG* readedLen)
{
    impl_->Read(buf, bufLen, readedLen);
    return S_OK;
}


STDMETHODIMP CLogger::InitLog(BSTR logPath, LONG level, 
                              LONG fileSize, LONG fileCount)
{
    // TODO:  在此添加实现代码
    impl_->InitLog(logPath, level, fileSize, fileCount);
    return S_OK;
}


STDMETHODIMP CLogger::log(BSTR text, LONG len, LONG* logged)
{
    // TODO:  在此添加实现代码
    return impl_->log(text, len, logged);
}
