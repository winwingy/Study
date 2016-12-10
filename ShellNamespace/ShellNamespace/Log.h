#ifndef __LOG_H__
#define __LOG_H__
#include "lock.h"
#ifdef _DEBUG
    inline void LOGFUN( const TCHAR* pszFormat, ... )
    {
        static CLock lock;
        lock.Lock();
        va_list vl;
        va_start( vl, pszFormat );
        TCHAR szBuffer[1024] = {0};
        _vsntprintf( szBuffer, 1024, pszFormat, vl );
        va_end( vl );
        //Êä³ö
        OutputDebugString( szBuffer );
        lock.Unlock();
    }
#define LOG LOGFUN
#else
#define LOG
#endif
#define  _TRACE_FUN_
#ifdef _TRACE_FUN_
#if defined(_UNICODE)||defined(UNICODE)
#define __STR2WSTR(str)    L##str
#define _STR2WSTR(str)     __STR2WSTR(str)
#define __FUNCTIONW__      _STR2WSTR(__FUNCTION__)
#define __TFUNCTION__  __FUNCTIONW__
#else
#define __TFUNCTION__ __FUNCTION__
#endif
#define  TRACE_FUNCTION() LOG( _T("enter function: %s\n"), __TFUNCTION__ )
#define  TRACE_FUNCTION_EX() LOG( _T("enter function: %s  %p\n"), __TFUNCTION__, this )
#define  TRACE_RETURN     LOG( _T("exit function: %s line:%d\n"), __TFUNCTION__, __LINE__ );return
#else
#define  TRACE_FUNCTION()
#define  TRACE_FUNCTION_EX()
#define  TRACE_RETURN return
#endif
#endif