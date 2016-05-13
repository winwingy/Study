#pragma once
#include <memory>
#include <vector>
#include "../LogLib/LogLibRef.h"

using namespace ATL;
class LogFile;
class AutoLockBase;
class ShareWriter;
class LoggerImple
{
public:
    LoggerImple();
    ~LoggerImple();

    STDMETHOD(InitLog)(BSTR logPath, LONG level, LONG fileSize, LONG fileCount);
    STDMETHOD(Exit)();
    STDMETHODIMP log(LONG len, LONG* logged);

private:
    void WriteLogProc();
    void WriteLogProcShareMem();
    static unsigned __stdcall WriteLogThread(void* param);
    std::unique_ptr<LogFile> logFile_;
    std::unique_ptr<void, void(*)(HANDLE h)> logThread_;
    std::unique_ptr<void, void(*)(HANDLE h)> logSemaphore_;
    bool exit_;
    std::unique_ptr<void, void(*)(void*)> memHandle_;
    std::shared_ptr<TCHAR> mapView_;
    std::vector<tstring>* datas_;
    std::unique_ptr<AutoLockBase> lockData_;
    std::unique_ptr<ShareWriter> shareWriter_;
};

