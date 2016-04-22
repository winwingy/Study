#pragma once
#include <windows.h>

class AutoLockBase
{
public:
    AutoLockBase();
    ~AutoLockBase();
    void Lock();
    void UnLock();


private:
    CRITICAL_SECTION cs;

};

class AutoLock
{
public:
    AutoLock(const AutoLockBase& lockBase);
    ~AutoLock();

private:
    AutoLockBase lockBase_;
};

