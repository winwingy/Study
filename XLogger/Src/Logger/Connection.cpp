
#include <atlbase.h>
#include <OBJBASE.H>
#include <comdef.h>
#include <assert.h>
#include <tchar.h>
#include <WINNLS.H>

#include "Connection.h"
#include "SimpleLogger.h"
#include "XLogUtil.h"

BOOL CreateFactory2(ILogFactoryPtr &ipFactory)
{
	HRESULT hrResult = ipFactory.CreateInstance(CLSID_LogFactory);
	if (S_OK == hrResult)
		return TRUE;

	if (CO_E_NOTINITIALIZED == hrResult)
	{
		//CoInitialize(NULL);
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
		hrResult = ipFactory.CreateInstance(CLSID_LogFactory);
		if (S_OK == hrResult)
			return TRUE;
	}

	TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 }; 
	GetErrorMessage(GetLastError(), Buffer); 
	LOGGER_WRITE2(_T("创建日志组件失败:") << Buffer);

	return FALSE;
}

BOOL CreateFactory(ILogFactoryPtr &ipFactory)
{
#define ATLTRACE_X(Format) ATLTRACE("%s %s %ld [%s %d]", Format, __FUNCTION__, \
	GetCurrentThreadId(), __FILE__, __LINE__);

	ATLTRACE_X("!!! ->");
	if (CreateFactory2(ipFactory))
	{
		ATLTRACE_X("!!! <- OK");
		return TRUE;
	}

	ATLTRACE_X("!!! <- FALSE");
	return FALSE;
}

BOOL GetName(const TCHAR *Config, 
			 const TCHAR *Section, 
			 INT FileType,
			 UINT iCodePage,
			 TCHAR Buffer[MAX_PATH],
			 TCHAR XFile[MAX_PATH])
{
	TRACE_FUNCTION_INFO;
	ILogFactoryPtr ipFactory = NULL;
	if (!CreateFactory(ipFactory))
		return FALSE;

	const TCHAR *Value[2] = { _T(""), _T("") };
	if (NULL != Config)  Value[0] = Config;
	if (NULL != Section) Value[1] = Section;

	TCHAR AppName[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(NULL), AppName, MAX_PATH);

	CComBSTR bstrLogName;
	CComBSTR bstrXFile;
	if (S_OK != ipFactory->Create(FileType, CComBSTR(AppName), 
		CComBSTR(Value[0]), CComBSTR(Value[1]), &bstrLogName, &bstrXFile))
	{
		LOGGER_WRITE(_T("从日志组件查询日志名称失败."));
		return FALSE;
	}

#ifdef UNICODE
	wmemset(Buffer, 0, MAX_PATH);
	wmemcpy(Buffer, bstrLogName, bstrLogName.Length());
	wmemset(XFile, 0, MAX_PATH);
	wmemcpy(XFile, bstrXFile, bstrXFile.Length());
#else
	memset(Buffer, 0, MAX_PATH);
	WideCharToMultiByte(iCodePage, 0, bstrLogName, -1, Buffer, MAX_PATH - 1, NULL, NULL);
	memset(XFile, 0, MAX_PATH);
	WideCharToMultiByte(iCodePage, 0, bstrXFile, -1, XFile, MAX_PATH - 1, NULL, NULL);
#endif

	return TRUE;
}

void Close2(const TCHAR *LogName)
{
	TRACE_FUNCTION_INFO;
	ILogFactoryPtr ipFactory = NULL;
	if (CreateFactory(ipFactory))
	{
		TCHAR AppName[MAX_PATH] = { 0 };
		GetModuleFileName(GetModuleHandle(NULL), AppName, MAX_PATH);
		ipFactory->Close(CComBSTR(AppName), CComBSTR(LogName));
		ipFactory.Release();
	}
}

BOOL Open3(const TCHAR *FileName, 
		   const TCHAR *Section,
		   INT FileType, 
		   const TCHAR *Value,
		   UINT iCodePage,
		   TCHAR Name[MAX_PATH],
		   TCHAR TempFile[MAX_PATH],
		   TCHAR XFile[MAX_PATH])
{
	TRACE_FUNCTION_INFO;
	BOOL bRetVal = FALSE;
	if (NULL != Value)
	{
		if (CreateTempConfig(FileName, Section, Value, TempFile))
			bRetVal = GetName(TempFile, Section, FileType, iCodePage, Name, XFile);
	}
	else
	{
		bRetVal = GetName(FileName, Section, FileType, iCodePage, Name, XFile);
	}

	return bRetVal;
}

BOOL Open2(const TCHAR *FileName, 
		   const TCHAR *Section,
		   INT FileType, 
		   const TCHAR *Value,
		   XCONNECTDATA *ConnectInfo)
{
	TRACE_FUNCTION_INFO;
	if (ConnectInfo->bLogOpened)
		return FALSE;

	ConnectInfo->Init();
	ConnectInfo->iCodePage = GetCodePage();
	if (!Open3(FileName, Section, FileType, Value, ConnectInfo->iCodePage, 
		ConnectInfo->strLogName, ConnectInfo->TempFile, ConnectInfo->XFile))
		return FALSE;

	if (!ConnectInfo->MemWriter.Create(ConnectInfo->strLogName))
		return FALSE;

	StringCopy(ConnectInfo->cConfigValue, MAX_CONFIG_VALUE_SIZE, Value);
	StringCopy(ConnectInfo->cConfigFile, MAX_PATH, FileName);
	StringCopy(ConnectInfo->cSection, MAX_PATH, Section);

	ConnectInfo->iFileType  = FileType;
	ConnectInfo->iLevel     = logDebug;
	ConnectInfo->bLogOpened = TRUE;

	return TRUE;
}

void Close2(XCONNECTDATA *ConnectInfo)
{
	TRACE_FUNCTION_INFO;
	if (ConnectInfo->bLogOpened)
	{
		Close2(ConnectInfo->strLogName);
		ConnectInfo->MemWriter.Close();
		if (0 != ConnectInfo->TempFile[0])
			DeleteFile(ConnectInfo->TempFile); 
		ConnectInfo->Init();
	}
}
