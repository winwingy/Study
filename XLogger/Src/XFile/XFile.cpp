// XFile.cpp : WinMain 的实现


#include "stdafx.h"
#include "resource.h"
#include "XFile_i.h"
#include "HManager.h"
#include "StateMonitor.h"
#include "..\Logger\Constants.h"
#include "..\Logger\XLogUtil.h"


class CXFileModule : public CAtlExeModuleT< CXFileModule >
{
public :
	DECLARE_LIBID(LIBID_XFileLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_XFILE, "{02C1A732-1767-4D4C-ADD6-9597A52E6030}")
};

CXFileModule _AtlModule;
CHistoryManager g_oFileManager;

void LogEvent(LPCTSTR pszFormat, ...)
{
	const int LOG_EVENT_MSG_SIZE = 512;

	TCHAR chMsg[LOG_EVENT_MSG_SIZE];
	HANDLE hEventSource;
	LPTSTR lpszStrings[1];
	va_list pArg;

	va_start(pArg, pszFormat);
	_vsntprintf_s(chMsg, LOG_EVENT_MSG_SIZE, LOG_EVENT_MSG_SIZE-1, pszFormat, pArg);
	va_end(pArg);

	chMsg[LOG_EVENT_MSG_SIZE - 1] = 0;
	lpszStrings[0] = chMsg;

	hEventSource = RegisterEventSource(NULL, _T("XFile"));
	if (hEventSource != NULL)
	{
		ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
		DeregisterEventSource(hEventSource);
	}
}

LONG LockServer()
{
	return _AtlModule.Lock();
}

LONG UnlockServer()
{
	return _AtlModule.Unlock();
}

LONG GetLockCount()
{
	return _AtlModule.GetLockCount();
}

void AddHistryInfo(const CParameter &Parameter)
{
	g_oFileManager.Add(Parameter);
}

void DeleteHistryInfo(const CParameter &Parameter)
{
	g_oFileManager.Delete(Parameter);
}

CString GetFileVersion()
{
	CString strVersion;	
	VS_FIXEDFILEINFO FileInfo;

	if (GetAppVersion(FileInfo))
	{
		strVersion.Format(_T("%d.%d.%d.%d"), 
			HIWORD(FileInfo.dwProductVersionMS),
			LOWORD(FileInfo.dwProductVersionMS),
			HIWORD(FileInfo.dwProductVersionLS),
			LOWORD(FileInfo.dwProductVersionLS));
	}

	return strVersion;
}

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, 
								HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, 
								int nShowCmd)
{
	CString FileVersion = GetFileVersion();
	LOGGER_WRITE(_T("启动(Compilation:") << __DATE__ << _T(" ") << __TIME__ 
		<< _T(", Version:") << FileVersion.GetBuffer() <<  _T(")")); 
	g_oFileManager.CreateThread();
	CStateMonitor Monitor; Monitor.InitMonitor();
	int iRetVal = _AtlModule.WinMain(nShowCmd);
	g_oFileManager.CloseThread();
	LOGGER_WRITE2(_T("退出"));
	return iRetVal;
}
