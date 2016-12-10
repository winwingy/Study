#ifndef __SE_SHELL_FOLDER_H__
#define __SE_SHELL_FOLDER_H__
#include "PidlManager.h"
class ATL_NO_VTABLE CSEShellFolder
    : public CComObjectRootEx<CComSingleThreadModel>
    , public CComCoClass<CSEShellFolder, &CLSID_SEShellFolder>
    , public IShellFolder
    , public IPersistFolder
    , public IShellPropSheetExt
    , public IShellExtInit
    , public IQueryInfo
{
public:
    HRESULT FinalConstruct();
    void FinalRelease();

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CSEShellFolder)
        COM_INTERFACE_ENTRY_IID(IID_IShellFolder,IShellFolder)
        COM_INTERFACE_ENTRY_IID(IID_IPersistFolder,IPersistFolder)
        COM_INTERFACE_ENTRY_IID(IID_IQueryInfo,IQueryInfo)
        COM_INTERFACE_ENTRY_IID(IID_IShellExtInit, IShellExtInit )
        COM_INTERFACE_ENTRY_IID(IID_IShellPropSheetExt, IShellPropSheetExt )
        COM_INTERFACE_ENTRY(IPersist)
    END_COM_MAP()

public:
    static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
    {
        CComBSTR bstrCLSID(CLSID_SEShellFolder);
        CComBSTR bstrCLSIDOverlay(CLSID_SEOverlayIcon);
        _ATL_REGMAP_ENTRY rm[] = { 
            { OLESTR("DESCRIPTION"), g_szRoot },
            { OLESTR("PROJECTNAME"), g_szRoot },
            { OLESTR("CLSID"), bstrCLSID },
            { OLESTR("INFOTIP"), _T("–Èƒ‚ µ¿˝Ã· æ") },
            { NULL,NULL } };
       return _Module.UpdateRegistryFromResource(IDR_REG_FOLDER, bRegister, rm);
    }

public:
    STDMETHOD(GetClassID)(CLSID *pClassID);
    STDMETHOD(Initialize)(LPCITEMIDLIST pidl);
    STDMETHOD(AddPages)( LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam);
    STDMETHOD(ReplacePage)(EXPPS uPageID,LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam);
    STDMETHOD(Initialize)( LPCITEMIDLIST pidlFolder,IDataObject *pdtobj,HKEY hkeyProgID);
    //IShellFolder methods
    STDMETHOD(BindToObject)(LPCITEMIDLIST, LPBC, REFIID, LPVOID*);
    STDMETHOD(BindToStorage) (LPCITEMIDLIST, LPBC, REFIID, LPVOID*);
    STDMETHOD(CreateViewObject)(HWND, REFIID, LPVOID*);
    STDMETHOD(EnumObjects)(HWND, DWORD, LPENUMIDLIST*);
    STDMETHOD(GetAttributesOf)(UINT, LPCITEMIDLIST*, LPDWORD);
    STDMETHOD(GetDisplayNameOf)(LPCITEMIDLIST pidl, DWORD, LPSTRRET);
    STDMETHOD(GetUIObjectOf)(HWND, UINT, LPCITEMIDLIST*, REFIID, LPUINT, LPVOID*);
    STDMETHOD(ParseDisplayName) (HWND, LPBC, LPOLESTR, LPDWORD, LPITEMIDLIST*, LPDWORD);
    STDMETHOD(SetNameOf)(HWND, LPCITEMIDLIST, LPCOLESTR, DWORD, LPITEMIDLIST*);
    STDMETHOD(CompareIDs)(LPARAM lParam, LPCITEMIDLIST, LPCITEMIDLIST);;
    //IQueryInfo
    STDMETHOD(GetInfoTip)(DWORD dwFlags,LPWSTR *ppwszTip);
    STDMETHOD(GetInfoFlags)(DWORD *pdwFlags);

public:
    LPCITEMIDLIST GetRootPath()const
    {
        return m_pidlPath;
    }
private:
    LPITEMIDLIST m_pidlPath;
    CPidlManager m_pidlManager;
};

#endif