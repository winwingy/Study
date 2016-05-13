// LogFactory.cpp : CLogFactory 的实现

#include "stdafx.h"
#include "LogFactory.h"
#include "Parameter.h"
#include "ConfigReader.h"

// CLogFactory

LONG LockServer();
LONG UnlockServer();
void LogEvent(LPCTSTR pszFormat, ...);

CDataArray<MONITOR> CLogFactory::m_arrMonitor;
LOGSERVER CLogFactory::m_sLogServer[MAX_SERVER_COUNT];

BOOL CLogFactory::IsFileExisted(CString p_strFileName) 
{
	DWORD dwAttricutes = ::GetFileAttributes(p_strFileName);
	return dwAttricutes != DWORD(-1) && ((dwAttricutes & FILE_ATTRIBUTE_DIRECTORY) == 0) ;
}

BOOL CLogFactory::CreateLogName(BSTR AppName, 
								BSTR Configue, 
								BSTR Section, 
								LONG FileType, 
								CString &strLogName)
{
	CString strError;
	CString strAppPath(AppName);
	CString strFileName(Configue);
	CString strSection(Section);

	if (!IsFileExisted(strAppPath))
	{
		strError.Format(_T("AppName:%s is not existed. CLogFactory::CreateLogName return FALSE."), strAppPath);
		LOGGER_WRITE(strError.GetBuffer());
		LogEvent(strError);
		return FALSE;
	}

	strError.Format(_T("AppName:%s"), strAppPath);
	LOGGER_WRITE(strError.GetBuffer());

	int iIndex = strAppPath.ReverseFind(_T('\\'));
	ATLASSERT(-1 != iIndex);
	strAppPath = strAppPath.Left(iIndex + 1);

	CParameter Parameter;
	CConfigReader Reader;
	Reader.Read(strAppPath, strFileName, strSection, FileType, Parameter);

	CHAR Buffer[MAX_PATH] = { 0 };
#pragma warning(push)
#pragma warning(disable: 4996)
	strcat(Buffer, Parameter.m_cFixFolder);
	strcat(Buffer, Parameter.m_cCycleFolder);
	strcat(Buffer, Parameter.m_cCycleName);
	strcat(Buffer, Parameter.m_cCreateTime);
	strcat(Buffer, Parameter.m_cSaveTime);
	strcat(Buffer, Parameter.m_cExeName);
#pragma warning(pop)
	strLogName = CString(Buffer);
	strLogName.Remove(_T('\\'));
	strLogName.Remove(_T(':'));
	strLogName.Remove(_T('.'));
	strLogName.Remove(_T(' '));

	LOGGER_WRITE2(_T("LogName:") << strLogName.GetBuffer() << _T(" FileType:") << FileType);

	return TRUE;
}

BOOL CLogFactory::FindObject(CString strLogName, INT &iServer)
{
	iServer = -1;

	for (INT i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &LogServer = m_sLogServer[i];

		if (LogServer.strLogName == strLogName
			&& 0 != LogServer.iClient)
		{
			iServer = i;
			LogServer.iClient++;
			LOGGER_WRITE(_T("使用已存在的存储线程."));
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CLogFactory::CreateObject(CString strLogName,
							   LONG FileType,
							   BSTR AppName, 
							   BSTR Configue, 
							   BSTR Section,
							   INT &iServer)
{
	iServer = -1;

	for (INT i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &LogServer = m_sLogServer[i];
		if (0 != LogServer.iClient)
			continue;

		LogServer.strLogName = strLogName;
		if (!LogServer.oLogServer.Open(FileType, 
			AppName, Configue, Section, strLogName))
			return FALSE;

		DWORD Begin = GetTickCount();
		if (!LogServer.oLogServer.ThreadRunning())
		{
			LogServer.oLogServer.Close();
			LOGGER_WRITE2(_T("启动存储线程失败!!!"));
			return FALSE;
		}
		
		iServer = i;
		LogServer.iClient++;
		LOGGER_WRITE(_T("存储线程创建成功(") << (GetTickCount() - Begin) << _T(")."));		
		return TRUE;
	}

	CString strError;
	strError.Format(_T("Client maximal number is: %d. CLogFactory::Create return E_FAIL."), MAX_SERVER_COUNT);
	LOGGER_WRITE(strError.GetBuffer());
	LogEvent(strError);

	return FALSE;
}

STDMETHODIMP CLogFactory::Create(LONG FileType, 
								 BSTR AppName, 
								 BSTR Configue, 
								 BSTR Section, 
								 BSTR* LogName,
								 BSTR* XFile)
{
    //MessageBox(NULL, _T("CLogFactory::Create"), _T("CLogFactory::Create"), MB_OK);
	ATLASSERT(NULL != AppName);
	ATLASSERT(NULL != Configue);
	ATLASSERT(NULL != Section);
	if (NULL == AppName
		|| NULL == Configue
		|| NULL == Section
		|| NULL == LogName)
	{
		CString strError = _T("AppName(or Configue? Section? LogName?) is NULL. CLogFactory::Create return E_FAIL.");
		LOGGER_WRITE(strError.GetBuffer());
		LogEvent(strError);
		return E_FAIL;
	}

	CString strLogName;
	if (!CreateLogName(AppName, Configue, Section, FileType, strLogName))
		return E_FAIL;

	int iSerIndex = -1;
	if (!FindObject(strLogName, iSerIndex))
	{
		if (!CreateObject(strLogName, FileType, AppName, Configue, Section, iSerIndex))
			return E_FAIL;
	}

	ATLASSERT(-1 != iSerIndex);
	LOGSERVER &LogServer = m_sLogServer[iSerIndex];
	LogServer.oClientName.Add(CString(AppName));

	LONG Count = LockServer();
	LOGGER_WRITE(_T("增加客户端, 服务计数:") << Count);
	*LogName = strLogName.AllocSysString();

	TCHAR Buffer[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(NULL), Buffer, MAX_PATH - 1);
	CString ModuleFile(Buffer);
	*XFile = ModuleFile.AllocSysString();

	return S_OK;
}

STDMETHODIMP CLogFactory::Close(BSTR AppName,
								BSTR LogName)
{
	if (NULL == LogName)
		return E_FAIL;

	for (INT i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &LogServer = m_sLogServer[i];
		if (LogServer.strLogName == LogName
			&& 0 < LogServer.iClient)
		{
			if (0 == --LogServer.iClient)
			{
				LogServer.oLogServer.Close();
				LogServer.strLogName.Empty();
			}

			CString strAppName(AppName);
			LogServer.oClientName.Delete(strAppName);

			LONG Count = UnlockServer();
			LOGGER_WRITE(_T("进程(") << strAppName.GetBuffer() << _T(")已关闭."));
			LOGGER_WRITE2(_T("删除客户端, 服务计数:") << Count);
			break;
		}
	}

	return S_OK;
}

STDMETHODIMP CLogFactory::LogMonitor(LONG Handle, BSTR FileName)
{
	MONITOR Data;
	Data.Handle = Handle;
	Data.FileName = CString(FileName);

	if (0 != Handle)
	{
		m_arrMonitor.Add(Data);
		LONG Count = LockServer();
		LOGGER_WRITE(_T("增加监控, 服务计数:") << Count);
	}
	else
	{
		m_arrMonitor.Delete(Data);
		LONG Count = UnlockServer();
		LOGGER_WRITE(_T("删除监控, 服务计数:") << Count);
	}

	for (INT i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &LogServer = m_sLogServer[i];
		if (0 != LogServer.iClient)
		{
			CLogWriter &oLogServer = LogServer.oLogServer;
			oLogServer.SetMonitor(Handle, CString(FileName));
			return S_OK;
		}
	}

	return S_OK;
}
