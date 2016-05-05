#pragma once
#include <windows.h>

class AutoLockBase
{
public:
    AutoLockBase();
    ~AutoLockBase();

private:
    BOOL TryLock();
    void Lock();
    void UnLock();

private:
    friend class AutoLock;
    CRITICAL_SECTION cs;

};

class AutoLock
{
public:
    AutoLock(const AutoLockBase& lockBase);
    AutoLock(const AutoLockBase& lockBase, bool* tryLockResult);
    ~AutoLock();

private:
    AutoLockBase* lockBase_;
};

