#include "stdafx.h"
#include "AutoLock.h"

AutoLockBase::AutoLockBase()
{
    InitializeCriticalSection(&cs);
}

AutoLockBase::~AutoLockBase()
{
    DeleteCriticalSection(&cs);
}

void AutoLockBase::Lock()
{
    EnterCriticalSection(&cs);
}

void AutoLockBase::UnLock()
{
    LeaveCriticalSection(&cs);
}



AutoLock::AutoLock(const AutoLockBase& lockBase)
    : lockBase_(lockBase)
{
    lockBase_.Lock();
}


AutoLock::~AutoLock()
{
    lockBase_.UnLock();
}
