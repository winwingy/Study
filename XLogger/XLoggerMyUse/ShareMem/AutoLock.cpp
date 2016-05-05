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

BOOL AutoLockBase::TryLock()
{
    return TryEnterCriticalSection(&cs);
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
    : lockBase_(const_cast<AutoLockBase*>(&lockBase))
{
    lockBase_->Lock();
}

AutoLock::AutoLock(const AutoLockBase& lockBase, bool* tryLockResult)
    : lockBase_(const_cast<AutoLockBase*>(&lockBase))
{
    *tryLockResult = !!lockBase_->TryLock();
}

AutoLock::~AutoLock()
{
    lockBase_->UnLock();
}
