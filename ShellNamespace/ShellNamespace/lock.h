#ifndef __LOCK_H__
#define __LOCK_H__
#include <Windows.h>
class CLock
{
public:
    CLock()
    {
        ::InitializeCriticalSection( &section );
    };
    ~CLock()
    {
        ::DeleteCriticalSection( &section );
    }
    void Lock()
    {
        ::EnterCriticalSection( &section );
    }
    void Unlock()
    {
        ::LeaveCriticalSection( &section );
    }
private:
    CRITICAL_SECTION section;
};

#endif