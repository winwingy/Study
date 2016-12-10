#pragma once
#include "SEShellFolder.h"
#include "PidlManager.h"

class CSEShellView
    : public CComObjectRootEx<CComSingleThreadModel>
    , public CComCoClass<CSEShellView, &CLSID_SEShellView>
    , public IShellView
{
public:
    HRESULT FinalConstruct();
    void FinalRelease();
    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CSEShellView)
        COM_INTERFACE_ENTRY_IID(IID_IShellView,IShellView)
        COM_INTERFACE_ENTRY(IOleWindow)
    END_COM_MAP()
    
    //
    //ON_NOTIFY (NM_DBLCLK, IDC_FILELIST, OnDblclkFileList)
    //ON_NOTIFY(NM_RCLICK, IDC_FILELIST, OnRClickFileList)

public:
    static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
    {
        CComBSTR bstrCLSID(CLSID_SEShellView);
        _ATL_REGMAP_ENTRY rm[] = { 
            { OLESTR("DESCRIPTION"), L"SE ShellView 1.0" },
            { OLESTR("CLSID"), bstrCLSID },
            { NULL,NULL } };

        return _Module.UpdateRegistryFromResource(IDR_REG_VIEW, bRegister, rm);
    }

    //IOleWindow
    STDMETHOD(GetWindow)(HWND* phWnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL);

    //IShellView
    STDMETHOD (TranslateAccelerator) (LPMSG);
    STDMETHOD (EnableModeless) (BOOL);
    STDMETHOD (UIActivate) (UINT);
    STDMETHOD (Refresh) (void);
    STDMETHOD (CreateViewWindow) (LPSHELLVIEW, LPCFOLDERSETTINGS, LPSHELLBROWSER, LPRECT, HWND*);
    STDMETHOD (DestroyViewWindow) (void);
    STDMETHOD (GetCurrentInfo) (LPFOLDERSETTINGS);
    STDMETHOD (AddPropertySheetPages) (DWORD, LPFNADDPROPSHEETPAGE, LPARAM);
    STDMETHOD (SaveViewState) (void);
    STDMETHOD (SelectItem) (LPCITEMIDLIST, UINT);
    STDMETHOD (GetItemObject) (UINT, REFIID, LPVOID*);
    HRESULT _Init(CSEShellFolder* pFolder,LPCITEMIDLIST pidlRoot);
protected:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
    LRESULT OnCommand( DWORD dwCmdID, DWORD dwCmd, HWND hwndCmd );
    LRESULT OnNotify( LPNMHDR pNmhdr );
    void InitTooBar();
    void InitMenu();
    void UninitMenu();
    void Clear();
private:
    HWND              m_hWnd;
    HMENU             m_hMenu;
    CSEShellFolder*   m_pFolder;
    CPidlManager      m_pidlManager;
    LPITEMIDLIST      m_pidlRoot;
    FOLDERSETTINGS    m_folderSettings;
    LPSHELLBROWSER    m_pShellBrowser;
};
