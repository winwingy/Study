// PropPage.h : CPropPage 的声明

#pragma once
#include "resource.h"       // 主符号
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")

#include "IShellExtInitImpl.h" // IShellExtInit

#include "IShellPropSheetExtImpl.h" // IShellPropSheetExt

BOOL CALLBACK PropPage_DlgProc(HWND, UINT, WPARAM, LPARAM);

#include "ShellPropPage_i.h"



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif

using namespace ATL;


// CPropPage

class ATL_NO_VTABLE CPropPage :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPropPage, &CLSID_PropPage>,
    public IShellExtInitImpl,
    public IShellPropSheetExtImpl,
	public IDispatchImpl<IPropPage, &IID_IPropPage, &LIBID_ShellPropPageLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CPropPage()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PROPPAGE)


BEGIN_COM_MAP(CPropPage)
	COM_INTERFACE_ENTRY(IPropPage)
	COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IShellExtInit)

    COM_INTERFACE_ENTRY(IShellPropSheetExt)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

    // IPropPage

public:

    STDMETHOD(Initialize)(LPCITEMIDLIST, LPDATAOBJECT, HKEY);

    STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE, LPARAM);


};

OBJECT_ENTRY_AUTO(__uuidof(PropPage), CPropPage)
