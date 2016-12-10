#pragma once
#include "PidlManager.h"
class CExtractIcon
    :public IExtractIcon
    ,public CComObjectRootEx<CComSingleThreadModel>
{
public:
    HRESULT FinalConstruct()
    {
        m_pidl = NULL;

        return S_OK;
    }

    void FinalRelease() 
    {
        if ( m_pidl )
        {
            m_pidlManager.Delete( m_pidl );
        }
    }

    BEGIN_COM_MAP(CExtractIcon)
        COM_INTERFACE_ENTRY_IID(IID_IExtractIcon,IExtractIcon)
    END_COM_MAP()

public:
    HRESULT _Init(LPCITEMIDLIST pidl);
    // GetIconLocation:Retrieves the location and index of an icon. 
    STDMETHOD(GetIconLocation)(UINT uFlags, 
        LPTSTR pszIconFile, 
        UINT /*cchMax*/, 
        LPINT piIndex, 
        LPUINT puFlags);

    STDMETHOD(Extract)(LPCTSTR /*pstrName*/, 
        UINT nIconIndex, 
        HICON *phiconLarge, 
        HICON *phiconSmall, 
        UINT /*nIconSize*/);;

private:
    TCHAR        m_szPath[MAX_PATH];
    LPITEMIDLIST m_pidl;
    CPidlManager m_pidlManager;
};
