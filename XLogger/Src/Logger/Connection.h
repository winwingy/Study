#include <comdef.h>
#include <atltrace.h>
#include "MemWriter.h"

#pragma once
#pragma pack(push, 8)

struct /* coclass */ LogFactory;
struct __declspec(uuid("1b9ff0a9-ccfc-4e4f-9051-213566e483ae"))
	/* dual interface */ ILogFactory;

_COM_SMARTPTR_TYPEDEF(ILogFactory, __uuidof(ILogFactory));

struct __declspec(uuid("1b9ff0a9-ccfc-4e4f-9051-213566e483ae"))
ILogFactory : IDispatch
{
	virtual HRESULT __stdcall Create (
		/*[in]*/ long FileType,
		/*[in]*/ BSTR AppName,
		/*[in]*/ BSTR Configue,
		/*[in]*/ BSTR Section,
		/*[out]*/ BSTR * LogName,
		/*[out]*/ BSTR * XFile) = 0;
	virtual HRESULT __stdcall LogMonitor (
		/*[in]*/ long Handle,
		/*[in]*/ BSTR FileName ) = 0;
	virtual HRESULT __stdcall Close (
		/*[in]*/ BSTR AppName,
		/*[in]*/ BSTR LogName ) = 0;
};

extern "C" const GUID __declspec(selectany) CLSID_LogFactory =
{0x742a6ed8,0x2816,0x498c,{0x86,0x8e,0xc1,0x8e,0xf8,0x12,0xfd,0x87}};

#pragma pack(pop)

#define MAX_CONFIG_VALUE_SIZE      1024
#define TRACE_FUNCTION_INFO        ATLTRACE("%s %ld [%s %d]", __FUNCTION__, GetCurrentThreadId(), __FILE__, __LINE__);

struct XCONNECTDATA {
	CMemWriter MemWriter;
	TCHAR strLogName[MAX_PATH];
	TCHAR cConfigFile[MAX_PATH];
	TCHAR cSection[MAX_PATH];
	TCHAR TempFile[MAX_PATH];
	TCHAR cConfigValue[MAX_CONFIG_VALUE_SIZE];
	TCHAR XFile[MAX_PATH];
	BOOL bLogOpened;
	LONG lOpenCount;
	UINT iCodePage;
	LONG iFileType;
	INT iLevel;
	XCONNECTDATA() { 
		TRACE_FUNCTION_INFO;
		Init(); 
	}
	~XCONNECTDATA()
	{
		TRACE_FUNCTION_INFO;
		Init();
	}
	void Init() {
		TRACE_FUNCTION_INFO;
		int csize = sizeof(TCHAR);
		memset(strLogName, 0, csize * MAX_PATH);
		memset(cConfigFile, 0, csize * MAX_PATH);
		memset(cSection, 0, csize * MAX_PATH);
		memset(TempFile, 0, csize * MAX_PATH);
		memset(XFile, 0, csize * MAX_PATH);
		memset(cConfigValue, 0, csize * MAX_CONFIG_VALUE_SIZE);
		bLogOpened = FALSE;
		lOpenCount = 0;
		iCodePage = CP_ACP;
		iFileType = 0;
		iLevel = 0;
	}
};

BOOL Open2(const TCHAR *FileName, const TCHAR *Section, INT FileType, const TCHAR *Value, XCONNECTDATA *ConnectInfo);
void Close2(XCONNECTDATA *ConnectInfo);
