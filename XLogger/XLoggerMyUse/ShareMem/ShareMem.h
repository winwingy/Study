#pragma once
#include <tchar.h>
#include <windows.h>
class AutoLockBase;
class ShareMem
{
public:
    ShareMem();
    ~ShareMem();

    bool Init();
    bool Write(const TCHAR* writeText, int writeLen, int* writedLen);
    bool Read(TCHAR* buf, int bufLen, int* readedLen);
private:
    HANDLE memHandle_;
    TCHAR* write_;
    TCHAR* read_;
    int beg_;
    int end_;
    AutoLockBase* dataLock_;

    
};

