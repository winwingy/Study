#pragma once
#include <tchar.h>
#include <windows.h>
#include <functional>
#include <memory>
class AutoLockBase;
class ShareMem
{
public:
    typedef std::function<void(bool isMemPer, float per)> ReadEvent;
    ShareMem();
    ~ShareMem();

    bool Init();
    bool Write(const TCHAR* writeText, int writeLen, int* writedLen);
    bool Read(TCHAR* buf, int bufLen, int* readedLen);
    void SetReadEvent(const ReadEvent& event);
    void TryExit();
    void Exit();

private:
    void ChangeWriteReadPtr();
    std::unique_ptr<void, void(*)(void*)> memHandleA_;
    std::unique_ptr<void, void(*)(void*)> memHandleB_;
    std::shared_ptr<TCHAR> mapPtrA_;
    std::shared_ptr<TCHAR> mapPtrB_;
    std::shared_ptr<TCHAR> write_;
    int wBeg_;
    int wEnd_;
    std::shared_ptr<TCHAR> read_;
    int rBeg_;
    int rEnd_;
    ReadEvent readEvent_;
    std::unique_ptr<AutoLockBase> dataLockWrite_;
    std::unique_ptr<AutoLockBase> dataLockRead_;
};

