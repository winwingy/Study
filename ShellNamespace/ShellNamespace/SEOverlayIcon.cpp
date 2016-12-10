#include "StdAfx.h"
#include ".\seoverlayicon.h"
#include "Log.h"
STDMETHODIMP CSEOverlayIcon::GetOverlayInfo(
    LPWSTR pwszIconFile,
    int cchMax,int* pIndex,
    DWORD* pdwFlags)
{
    TRACE_FUNCTION();
    LOG( _T("%s Icon Path"), __TFUNCTION__, pwszIconFile );
    GetModuleFileNameW(_Module.GetModuleInstance(), pwszIconFile, cchMax);
    *pIndex = 0;
    *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEOverlayIcon::GetPriority(int* pPriority)
{
    // highest priority
    *pPriority=0;
    return S_OK;
}

STDMETHODIMP CSEOverlayIcon::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
    TRACE_FUNCTION();
    if ( m_pidlManager.IsValidDir( pwszPath ) )
    {
        LOG( _T("%s %s need Overlay"), __TFUNCTION__, pwszPath );
        TRACE_RETURN S_OK;
    }
    TRACE_RETURN S_FALSE;
}
