// PolyCtrl.h : CPolyCtrl 的声明
#pragma once
#include "resource.h"       // 主符号
#include <atlctl.h>
#include "Polygon_i.h"
#include "_IPolyCtrlEvents_CP.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Windows CE 平台(如不提供完全 DCOM 支持的 Windows Mobile 平台)上无法正确支持单线程 COM 对象。定义 _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA 可强制 ATL 支持创建单线程 COM 对象实现并允许使用其单线程 COM 对象实现。rgs 文件中的线程模型已被设置为“Free”，原因是该模型是非 DCOM Windows CE 平台支持的唯一线程模型。"
#endif


// CPolyCtrl
class ATL_NO_VTABLE CPolyCtrl :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CStockPropImpl<CPolyCtrl, IPolyCtrl>,
	public IPersistStreamInitImpl<CPolyCtrl>,
	public IOleControlImpl<CPolyCtrl>,
	public IOleObjectImpl<CPolyCtrl>,
	public IOleInPlaceActiveObjectImpl<CPolyCtrl>,
	public IViewObjectExImpl<CPolyCtrl>,
	public IOleInPlaceObjectWindowlessImpl<CPolyCtrl>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CPolyCtrl>,
	public CProxy_IPolyCtrlEvents<CPolyCtrl>,
	public IPersistStorageImpl<CPolyCtrl>,
	public ISpecifyPropertyPagesImpl<CPolyCtrl>,
	public IQuickActivateImpl<CPolyCtrl>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CPolyCtrl>,
#endif
	public IProvideClassInfo2Impl<&CLSID_PolyCtrl, &__uuidof(_IPolyCtrlEvents), &LIBID_PolygonLib>,
#ifdef _WIN32_WCE // 要在 Windows CE 上正确加载该控件，要求 IObjectSafety
	public IObjectSafetyImpl<CPolyCtrl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComCoClass<CPolyCtrl, &CLSID_PolyCtrl>,
	public CComControl<CPolyCtrl>
{
public:


	CPolyCtrl()
	{
		m_nSides = 3;
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_POLYCTRL)


BEGIN_COM_MAP(CPolyCtrl)
	COM_INTERFACE_ENTRY(IPolyCtrl)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY(IPersistStreamInit)
	COM_INTERFACE_ENTRY2(IPersist, IPersistStreamInit)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY(IQuickActivate)
	COM_INTERFACE_ENTRY(IPersistStorage)
#ifndef _WIN32_WCE
	COM_INTERFACE_ENTRY(IDataObject)
#endif
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
#ifdef _WIN32_WCE // 要在 Windows CE 上正确加载该控件，要求 IObjectSafety
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
#endif
END_COM_MAP()

BEGIN_PROP_MAP(CPolyCtrl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
#ifndef _WIN32_WCE
	PROP_ENTRY_TYPE("FillColor", DISPID_FILLCOLOR, CLSID_StockColorPage, VT_DISPATCH)
#endif
	// 示例项
	// PROP_ENTRY_TYPE("属性名", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()

BEGIN_CONNECTION_POINT_MAP(CPolyCtrl)
	CONNECTION_POINT_ENTRY(__uuidof(_IPolyCtrlEvents))
END_CONNECTION_POINT_MAP()

BEGIN_MSG_MAP(CPolyCtrl)
	CHAIN_MSG_MAP(CComControl<CPolyCtrl>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// 处理程序原型:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid)
	{
		static const IID* arr[] =
		{
			&IID_IPolyCtrl,
		};

		for (int i=0; i<sizeof(arr)/sizeof(arr[0]); i++)
		{
			if (InlineIsEqualGUID(*arr[i], riid))
				return S_OK;
		}
		return S_FALSE;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IPolyCtrl
public:
		HRESULT OnDraw(ATL_DRAWINFO& di)
		{
		RECT& rc = *(RECT*)di.prcBounds;
		// 将剪辑区域设置为 di.prcBounds 指定的矩形
		HRGN hRgnOld = NULL;
		if (GetClipRgn(di.hdcDraw, hRgnOld) != 1)
			hRgnOld = NULL;
		bool bSelectOldRgn = false;

		HRGN hRgnNew = CreateRectRgn(rc.left, rc.top, rc.right, rc.bottom);

		if (hRgnNew != NULL)
		{
			bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
		}

		Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
		SetTextAlign(di.hdcDraw, TA_CENTER|TA_BASELINE);
		LPCTSTR pszText = _T("ATL 8.0 : PolyCtrl");
#ifndef _WIN32_WCE
		TextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			pszText,
			lstrlen(pszText));
#else
		ExtTextOut(di.hdcDraw,
			(rc.left + rc.right) / 2,
			(rc.top + rc.bottom) / 2,
			ETO_OPAQUE,
			NULL,
			pszText,
			ATL::lstrlen(pszText),
			NULL);
#endif

		if (bSelectOldRgn)
			SelectClipRgn(di.hdcDraw, hRgnOld);

		return S_OK;
	}

	OLE_COLOR m_clrFillColor;
	short m_nSides;
	void OnFillColorChanged()
	{
		ATLTRACE(_T("OnFillColorChanged\n"));
	}

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}
	STDMETHOD(get_Sides)(SHORT* pVal);
	STDMETHOD(put_Sides)(SHORT newVal);
};

OBJECT_ENTRY_AUTO(__uuidof(PolyCtrl), CPolyCtrl)
