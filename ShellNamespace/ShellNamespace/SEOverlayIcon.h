#ifndef __SE_OVERLAY_ICON_H__
#define __SE_OVERLAY_ICON_H__
//重绘 shellitem 图标(如文件夹图标,文件图标)
#include "PidlManager.h"

class CSEOverlayIcon
    :public CComObjectRootEx<CComSingleThreadModel>
    ,public CComCoClass< CSEOverlayIcon, &CLSID_SEOverlayIcon >
    ,public IShellIconOverlayIdentifier
{
public:
    HRESULT FinalConstruct()
    {
        return S_OK;
    }
    void FinalRelease()
    {

    }

    static HRESULT WINAPI UpdateRegistry(BOOL bRegister)
    {
        CComBSTR bstrCLSID(CLSID_SEOverlayIcon);
        _ATL_REGMAP_ENTRY rm[] = { 
            { OLESTR("DESCRIPTION"), _T("SE_OVERLAY_1_0") },
            { OLESTR("CLSID"), bstrCLSID },
            { NULL,NULL } };
            return _Module.UpdateRegistryFromResource(IDR_REG_OVERLAYICON, bRegister, rm);
    }

    DECLARE_PROTECT_FINAL_CONSTRUCT()
    BEGIN_COM_MAP(CSEOverlayIcon)
        COM_INTERFACE_ENTRY_IID( IID_IShellIconOverlayIdentifier, IShellIconOverlayIdentifier )
    END_COM_MAP()

public:
    STDMETHOD(GetOverlayInfo)(LPWSTR pwszIconFile,int cchMax,int *pIndex,DWORD* pdwFlags);
    STDMETHOD(GetPriority)(int* pPriority);
    STDMETHOD(IsMemberOf)(LPCWSTR pwszPath,DWORD dwAttrib);

private:
    CPidlManager m_pidlManager;
};
#endif