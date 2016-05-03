#pragma once
#include <tchar.h>
#include <windows.h>
#include <functional>
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
    HANDLE memHandleA_;
    HANDLE memHandleB_;
    TCHAR* mapPtrA_;
    TCHAR* mapPtrB_;
    TCHAR* write_;
    int wBeg_;
    int wEnd_;
    TCHAR* read_;
    int rBeg_;
    int rEnd_;
    ReadEvent readEvent_;
    AutoLockBase* dataLockWrite_;
    AutoLockBase* dataLockRead_;
};

