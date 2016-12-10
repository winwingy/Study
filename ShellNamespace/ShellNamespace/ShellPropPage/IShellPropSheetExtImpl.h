// IShellPropSheetExtImpl.h

#include <AtlCom.h>

#include <ShlObj.h>

class ATL_NO_VTABLE IShellPropSheetExtImpl : public IShellPropSheetExt

{

public:

    TCHAR m_szFile[MAX_PATH];

    // IUnknown

    STDMETHOD(QueryInterface)(REFIID riid, void** ppvObject) = 0;

    _ATL_DEBUG_ADDREF_RELEASE_IMPL(IShellPropSheetExtImpl)

        // IShellPropSheetExt

        STDMETHOD(AddPages)(LPFNADDPROPSHEETPAGE, LPARAM)

    {

        return S_FALSE;

    }

    STDMETHOD(ReplacePage)(UINT, LPFNADDPROPSHEETPAGE, LPARAM)

    {

        return E_NOTIMPL;

    }

};

