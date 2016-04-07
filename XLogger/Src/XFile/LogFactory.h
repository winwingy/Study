// LogFactory.h : CLogFactory 的声明

#pragma once
#include "resource.h"       // 主符号

#include "XFile_i.h"
#include "LogWriter.h"
#include "CLientName.h"
#include "DataArray.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif


struct LOGSERVER {
	CLogWriter oLogServer;
	CString strLogName;
	CCLientName oClientName;
	int iClient;
	LOGSERVER() { 
		iClient = 0; 
	}
};

struct MONITOR {
	LONG Handle;
	CString FileName;
	BOOL operator == (const MONITOR& Data) const {
		return (FileName.CompareNoCase(Data.FileName) == 0);
	}
};

// CLogFactory

class ATL_NO_VTABLE CLogFactory :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CLogFactory, &CLSID_LogFactory>,
	public IDispatchImpl<ILogFactory, &IID_ILogFactory, &LIBID_XFileLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CLogFactory()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_LOGFACTORY)


BEGIN_COM_MAP(CLogFactory)
	COM_INTERFACE_ENTRY(ILogFactory)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	static CDataArray<MONITOR> m_arrMonitor;
	static LOGSERVER m_sLogServer[MAX_SERVER_COUNT];
	BOOL CreateLogName(BSTR AppName, BSTR Configue, BSTR Section, LONG FileType, CString &strLogName);
	BOOL IsFileExisted(CString strFileName);
	BOOL FindObject(CString strLogName, int &iServer);
	BOOL CreateObject(CString strLogName, LONG FileType, BSTR AppName, BSTR Configue, BSTR Section, INT &iServer);

public:

	STDMETHOD(Create)(LONG FileType, BSTR AppName, BSTR Configue, BSTR Section, BSTR* LogName, BSTR* XFile);
	STDMETHOD(LogMonitor)(LONG Handle, BSTR FileName);
	STDMETHOD(Close)(BSTR AppName, BSTR LogName);
};

OBJECT_ENTRY_AUTO(__uuidof(LogFactory), CLogFactory)
