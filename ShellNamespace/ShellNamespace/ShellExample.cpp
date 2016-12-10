#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "SEShellFolder.h"
#include "SEShellView.h"
#include "SEOverlayIcon.h"
#include "Log.h"
CSEModule _Module;

const TCHAR g_szRoot[MAX_PATH] = {_T("–Èƒ‚Õ¯≈Ã")};
TCHAR g_szRealRoot[MAX_PATH] = {_T("C:\\")};

// {588991C0-1E9A-4ecc-A556-88B8EED13556}
static const CLSID CLSID_SEShellFolder = 
{ 0x588991c0, 0x1e9a, 0x4ecc, { 0xa5, 0x56, 0x88, 0xb8, 0xee, 0xd1, 0x35, 0x56 } };

// {C219EAAF-8E03-443e-8A64-648CF130EA7E}
static const CLSID CLSID_SEShellView = 
{ 0xc219eaaf, 0x8e03, 0x443e, { 0x8a, 0x64, 0x64, 0x8c, 0xf1, 0x30, 0xea, 0x7e } };

// {453C849D-2961-43e0-BE10-343DE3DF0BDD}
static const CLSID CLSID_SEOverlayIcon = 
{ 0x453c849d, 0x2961, 0x43e0, { 0xbe, 0x10, 0x34, 0x3d, 0xe3, 0xdf, 0xb, 0xdd } };

//
BEGIN_OBJECT_MAP(ObjectMap)
    OBJECT_ENTRY( CLSID_SEShellFolder, CSEShellFolder )
    OBJECT_ENTRY( CLSID_SEShellView,   CSEShellView )
    OBJECT_ENTRY( CLSID_SEOverlayIcon, CSEOverlayIcon )
END_OBJECT_MAP()
 
/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
    TRACE_FUNCTION();
   // MessageBox(nullptr, L"DllMain", L"DllMain", MB_OK);
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init (ObjectMap,hInstance);
        _Module.m_ImageLists.Init();
        DisableThreadLibraryCalls(hInstance);

#ifdef _ATL_DEBUG_INTERFACES
        _Module.m_nIndexBreakAt = 1;
#endif

    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        _Module.Term();
    }
    TRACE_RETURN TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI  DllCanUnloadNow(void)
{
    MessageBox(nullptr, L"DllCanUnloadNow", L"DllCanUnloadNow", MB_OK);
    if(_Module.GetLockCount()==0)
    {
        SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
        return S_OK;
    }
    else
    {
        return S_FALSE;
    }
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    MessageBox(nullptr, L"DllGetClassObject", L"DllGetClassObject", MB_OK);
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    MessageBox(nullptr, L"DllRegisterServer", L"DllRegisterServer", MB_OK);
    // registers object, not include typelib and all interfaces in typelib
    HRESULT Hr;
    Hr=_Module.RegisterServer(FALSE);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

    if(_Module.m_wWinMajor == 5 && _Module.m_wWinMinor == 1) //XP can't auto refresh explore
    {
        //RefreshShellViewWndsExcept(NULL);
    }
    return Hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{ 
    HRESULT Hr;
    Hr= _Module.UnregisterServer(FALSE);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    if(_Module.m_wWinMajor == 5 && _Module.m_wWinMinor == 1) //XP can't auto refresh explore
    {
        ::SHChangeNotify( SHCNE_RMDIR , SHCNF_IDLIST | SHCNF_FLUSH, NULL, NULL);
    }
    ::CoFreeUnusedLibraries();
    return Hr;
}