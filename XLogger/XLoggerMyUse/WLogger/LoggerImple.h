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
    STDMETHOD(Write)(BSTR* writeStr, LONG wantWriteLen, LONG* writedLen);
    STDMETHOD(Read)(BSTR* buf, LONG bufLen, LONG* readedLen);
    STDMETHODIMP log(BSTR text, LONG len, LONG* logged);


private:
    std::unique_ptr<ShareMem> shareMem_;
    std::unique_ptr<LogFile> logFile_;
};

