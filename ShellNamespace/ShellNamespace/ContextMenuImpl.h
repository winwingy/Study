#pragma once
#include "PidlManager.h"
#include <vector>
using std::vector;
#include <string>
using std::string;
using std::wstring;

class CContextMenuImpl
    :public IContextMenu3
/*    ,public IShellExtInit*/
    ,public CComObjectRootEx<CComSingleThreadModel>
{
public:
    HRESULT FinalConstruct();
    void FinalRelease();

    BEGIN_COM_MAP( CContextMenuImpl )
        COM_INTERFACE_ENTRY_IID( IID_IContextMenu , IContextMenu  )
        COM_INTERFACE_ENTRY_IID( IID_IContextMenu2 , IContextMenu2  )
        COM_INTERFACE_ENTRY_IID( IID_IContextMenu3 , IContextMenu3  )
/*        COM_INTERFACE_ENTRY_IID( IID_IShellExtInit, IShellExtInit )*/
    END_COM_MAP()

public:
/*    STDMETHOD(Initialize)( LPCITEMIDLIST pidlFolder,IDataObject *pdtobj,HKEY hkeyProgID);*/
    HRESULT _Init( LPCITEMIDLIST pRoot, LPCITEMIDLIST* pidls, int pidCounts );
    STDMETHOD(QueryContextMenu)(
        HMENU hmenu,
        UINT indexMenu,
        UINT idCmdFirst,
        UINT idCmdLast,
        UINT uFlags);
    STDMETHOD(InvokeCommand)(LPCMINVOKECOMMANDINFO lpici);
    STDMETHOD(GetCommandString)(
        UINT_PTR    idCmd,
        UINT        uType,
        UINT      * pwReserved,
        LPSTR       pszName,
        UINT        cchMax);

    STDMETHOD(HandleMenuMsg)(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam)
    {
        return S_OK;
    }
    STDMETHOD(HandleMenuMsg2)(
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam,
        LRESULT* plResult)
    {
        return S_OK;
    }
private:
#if defined(_UNICODE) || defined(UNICODE)
    typedef vector<wstring> VECFILE;
#else
    typedef vector<string> VECFILE;
#endif
    VECFILE      m_vecPath;     //菜单操作的文件列表
    LPITEMIDLIST m_pidlRoot;    //根目录
    CPidlManager m_pidlManager;
    LPCONTEXTMENU3 m_pProxyMenu;//系统默认菜单
};
