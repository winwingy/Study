// Logger.cpp : CLogger 的实现

#include "stdafx.h"
#include "Logger.h"
#pragma comment(lib, "../Debug/ShareMem.lib")
#include "../ShareMem/ShareMem.h"


// CLogger
CLogger::CLogger()
    :shareMem_(new ShareMem())
{
}

CLogger::~CLogger()
{
}


STDMETHODIMP CLogger::Init()
{
    // TODO:  在此添加实现代码
    shareMem_->Init();
    return S_OK;
}


STDMETHODIMP CLogger::Exit()
{
    // TODO:  在此添加实现代码
    shareMem_->TryExit();
    shareMem_->Exit();
    return S_OK;
}



STDMETHODIMP CLogger::Write(BSTR* writeStr, LONG wantWriteLen, LONG* writedLen)
{
    CComBSTR bstr(*writeStr);
    TCHAR* szBuf = bstr;
    shareMem_->Write(szBuf, wantWriteLen, reinterpret_cast<int*>(writedLen));
    return S_OK;
}



