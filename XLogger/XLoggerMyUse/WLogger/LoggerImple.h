#pragma once
#include <memory>
using namespace ATL;

class ShareMem;
class LogFile;
class LoggerImple
{
public:
    LoggerImple();
    ~LoggerImple();

    STDMETHOD(InitLog)(BSTR logPath, LONG level, LONG fileSize, LONG fileCount);
    STDMETHOD(Exit)();
    STDMETHODIMP log(BSTR text, LONG len, LONG* logged);

private:
    void WriteLogProc();
    void OnMemRead(bool isMemPer, float per);
    static unsigned __stdcall WriteLogThread(void* param);
    std::unique_ptr<ShareMem> shareMem_;
    std::unique_ptr<LogFile> logFile_;
    std::unique_ptr<void, void(*)(HANDLE h)> logThread_;
    std::unique_ptr<void, void(*)(HANDLE h)> logSemaphore_;
    bool exit_;
};

