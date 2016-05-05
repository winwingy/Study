#include "stdafx.h"
#include "LoggerImple.h"
#include "../ShareMem/ShareMem.h"
#include "LogFile.h"
#include <functional>
#pragma comment(lib, "../Debug/ShareMem.lib")

using std::placeholders::_1;
using std::placeholders::_2;


void HANDLEClose(HANDLE h)
{
    if (h != nullptr && h != INVALID_HANDLE_VALUE)
    {
        CloseHandle(h);
    }
}

LoggerImple::LoggerImple()
    : shareMem_(new ShareMem())
    , logFile_(new LogFile())
    , logThread_(nullptr, HANDLEClose)
    , logSemaphore_(nullptr, HANDLEClose)
{

}

LoggerImple::~LoggerImple()
{
}

unsigned __stdcall LoggerImple::WriteLogThread(void* param)
{
    LoggerImple* pThis = reinterpret_cast<LoggerImple*>(param);
    if (pThis)
    {
        pThis->WriteLogProc();
    }
    return 0;
}

STDMETHODIMP LoggerImple::InitLog(BSTR logPath, LONG level,
                                  LONG fileSize, LONG fileCount)
{
    shareMem_->Init();
    shareMem_->SetReadEvent(std::bind(&LoggerImple::OnMemRead, this, _1, _2));
    logFile_->Init(logPath, static_cast<LOG_INFO_LEVEL>(level),
                   fileSize, fileCount);
    logThread_.reset((HANDLE)_beginthreadex(
        nullptr, 0, WriteLogThread, this, 0, nullptr));
    logSemaphore_.reset(CreateSemaphore(0, 0, 10000, nullptr));
    return S_OK;
}



STDMETHODIMP LoggerImple::Exit()
{
    shareMem_->TryExit();
    shareMem_->Exit();
    return S_OK;
}

STDMETHODIMP LoggerImple::log(BSTR text, LONG len, LONG* logged)
{
    shareMem_->Write(text, len, reinterpret_cast<int*>(logged));
    return S_OK;
}

void LoggerImple::WriteLogProc()
{
    while (true)
    {
        WaitForSingleObject(logSemaphore_.get(), INFINITE);
        std::unique_ptr<TCHAR, void(*)(void*)> text(
            new TCHAR[1024], [](void* p){delete[] p; });
        int readed = 0;
        do 
        {
            Sleep(2000);
            shareMem_->Read(text.get(), 1024, &readed);
            int writted = 0;
            bool ret = logFile_->WriteToFile(text.get(), readed, &writted);
        } while (readed > 0);
    }
}

void LoggerImple::OnMemRead(bool isMemPer, float per)
{
    ReleaseSemaphore(logSemaphore_.get(), 1, nullptr);
}