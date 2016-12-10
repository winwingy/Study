#include "StdAfx.h"
#include ".\semodule.h"
#include "Log.h"
HRESULT CSEModule::Init( _ATL_OBJMAP_ENTRY* p, HINSTANCE h, const GUID* plibid /*= NULL*/ )
{
    TRACE_FUNCTION();
    ::OleInitialize(NULL);
#ifdef INITCOMMONCONTROLSEX
    INITCOMMONCONTROLSEX iccex;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    iccex.dwICC =	ICC_LISTVIEW_CLASSES;
    ::InitCommonControlsEx(&iccex);
#else
    ::InitCommonControls();
#endif // INITCOMMONCONTROLSEX

#ifndef	_NO_CLIPFORMATS
#define	CFSTR_OLECLIPBOARDPERSISTONFLUSH TEXT("OleClipboardPersistOnFlush")
#ifndef	CFSTR_LOGICALPERFORMEDDROPEFFECT 
#define CFSTR_LOGICALPERFORMEDDROPEFFECT	TEXT("Logical Performed	DropEffect")
#endif // CFSTR_LOGICALPERFORMEDDROPEFFECT
    m_CFSTR_FILEDESCRIPTOR			   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_FILEDESCRIPTOR);
    m_CFSTR_FILECONTENTS			   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_FILECONTENTS);
    m_CFSTR_PASTESUCCEEDED			   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_PASTESUCCEEDED);
    m_CFSTR_LOGICALPERFORMEDDROPEFFECT = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_LOGICALPERFORMEDDROPEFFECT);
    m_CFSTR_PERFORMEDDROPEFFECT		   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_PERFORMEDDROPEFFECT);
    m_CFSTR_PREFERREDDROPEFFECT		   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT);
    m_CFSTR_SHELLIDLIST				   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_SHELLIDLIST);
    m_CFSTR_OLECLIPBOARDPERSISTONFLUSH = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_OLECLIPBOARDPERSISTONFLUSH);
    // Register Self-defined clipformat
    m_CFSTR_DRAGDROP = (CLIPFORMAT)::RegisterClipboardFormat(_T("Shell Example"));
#if	(_WIN32_WINNT >= 0x0500)
    m_CFSTR_TARGETCLSID				   = (CLIPFORMAT)	::RegisterClipboardFormat(CFSTR_TARGETCLSID);
#endif // WIN32_WINNT
#endif // _NO_CLIPFORMATS

    //ªÒ»°∞Ê±æ
    OSVERSIONINFO	ovi	= {	0 };
    ovi.dwOSVersionInfoSize =	sizeof(ovi);
    ::GetVersionEx(&ovi);
    m_wWinMajor =	(WORD) ovi.dwMajorVersion;
    m_wWinMinor =	(WORD) ovi.dwMinorVersion;

    GetModuleFileName( h, m_szInstallPath, MAX_PATH );
    TCHAR* pszFileName = _tcsrchr(m_szInstallPath, _T('\\'));
    *pszFileName = _T('\0');
    SetResourceInstance( h );
    TRACE_RETURN CComModule::Init(p,h,plibid);
}