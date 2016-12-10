#include "StdAfx.h"
#include ".\extracticon.h"
#include "Log.h"

//解析图标
STDMETHODIMP CExtractIcon::Extract( LPCTSTR pstrName, UINT nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT /*nIconSize*/ )
{
    TRACE_FUNCTION();
    LOG(_T("%s GetIcon %s %d"), __TFUNCTION__, pstrName, nIconIndex );
    ATLASSERT(phiconLarge);
    ATLASSERT(phiconSmall);
    
    //HMODULE hLib = LoadLibrary( pstrName );
    //if ( hLib )
    //{
    //    *phiconSmall = LoadIcon( (HINSTANCE)hLib, MAKEINTRESOURCE(nIconIndex) );
    //    *phiconLarge = LoadIcon( (HINSTANCE)hLib, MAKEINTRESOURCE(nIconIndex) );
    //    FreeLibrary( hLib );
    //    return S_OK;
    //}
    SHFILEINFO   sfi = { 0 };
    HIMAGELIST hImgList = (HIMAGELIST)SHGetFileInfo( m_szPath, 0,\
        &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX|SHGFI_SMALLICON|SHGFI_ICON );
    if ( hImgList )
    {
        LOG( _T("%s Geting Smallone"), __TFUNCTION__ );
        *phiconSmall = ImageList_ExtractIcon(NULL, hImgList, nIconIndex);
    }
    hImgList = (HIMAGELIST)SHGetFileInfo( m_szPath, 0, &sfi, \
        sizeof(SHFILEINFO), SHGFI_SYSICONINDEX|SHGFI_LARGEICON|SHGFI_ICON );
    if ( hImgList )
    {
        LOG( _T("%s Geting Bigone"), __TFUNCTION__ );
        *phiconLarge = ImageList_ExtractIcon(NULL, hImgList, nIconIndex);
    }
    TRACE_RETURN S_OK;
}

HRESULT CExtractIcon::_Init(LPCITEMIDLIST pidl)
{
    TRACE_FUNCTION();
    m_pidl=m_pidlManager.Copy(pidl);
    DWORD dwLen = MAX_PATH;
    m_pidlManager.GetFullName( m_pidl, m_szPath, &dwLen );
    m_pidlManager.ReplaceRoot( m_szPath );
    LOG( _T("%s %s"), __TFUNCTION__, m_szPath );
    TRACE_RETURN S_OK;
}

// 获取图标索引
STDMETHODIMP CExtractIcon::GetIconLocation(UINT uFlags, 
                           LPTSTR pszIconFile, 
                           UINT cchMax, 
                           LPINT piIndex, 
                           LPUINT puFlags)
{
    TRACE_FUNCTION();
    assert(piIndex);
    assert(puFlags);

    if(m_pidl == NULL || m_pidl->mkid.cb == 0)
    {
        return E_FAIL;
    }

    //CRegKey regKey;
    //if( ERROR_SUCCESS == regKey.Open( HKEY_LOCAL_MACHINE, \
    //    _T("\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers"), \
    //    KEY_READ )
    //{
    //    int index = 0;
    //    TCHAR szKeyName[MAX_PATH] = {0};
    //    DWORD dwLen = MAX_PATH;
    //    TCHAR szCLSID[64] = {0};
    //    CRegKey regSubKey;
    //    ULONG ulChars = 64;
    //    
    //    HRESULT hr = E_FAIL;
    //    while( ERROR_SUCCESS == regKey.EnumKey( index, szKeyName, &dwLen ) )
    //    {
    //        do 
    //        {
    //            if( ERROR_SUCCESS != regSubKey.Open( regKey, szKeyName, KEY_READ ) )
    //            {
    //                break;
    //            }
    //            if( ERROR_SUCCESS != regSubKey.QueryStringValue( _T(""), szCLSID, &ulChars ) )
    //            {
    //                break;
    //            }
    //            CLSID clsid = {0};
    //            if( S_OK != CLSIDFromString( szCLSID, &clsid ) )
    //            {
    //                break;
    //            }
    //            IUnknown* pUnk = NULL;
    //            IShellIconOverlayIdentifier* pOverlay = NULL;
    //            hr = ::CoCreateInstance(clsid, pUnkOuter, CLSCTX_INPROC_SERVER\
    //                , IID_IShellIconOverlayIdentifier, (void**)&pUnk);
    //            if ( FAILED(hr)
    //                || NULL == pUnk )
    //            {
    //                break;
    //            }
    //            hr = pUnk->QueryInterface( IID_IShellIconOverlayIdentifier, (void**)&pOverlay )
    //            if( FAILED(hr)
    //                || NULL == pOverlay )
    //            {
    //                pUnk->Release();
    //                break;
    //            }
    //            pUnk->Release();
    //            
    //            if( S_OK != pOverlay->IsMemberOf( m_szPath, -1 ) )
    //            {
    //                pOverlay->Release();
    //                break;
    //            }
    //            pOverlay->Release();
    //        } while (0);
    //        regSubKey.Close();
    //        ulChars = 64;
    //        ++index;
    //    }
    //    
    //}

    //取文件图标列表
    SHFILEINFO   sfi = { 0 };
    SHGetFileInfo( m_szPath, 0, &sfi, sizeof(SHFILEINFO), \
        ((uFlags&GIL_OPENICON)?SHGFI_OPENICON:SHGFI_SMALLICON)|SHGFI_SYSICONINDEX );
    if ( sfi.hIcon )
    {
        DestroyIcon( sfi.hIcon );
    }
    //int _len = (int)_tcslen(sfi.szDisplayName)+1;
    //memcpy( pszIconFile, sfi.szDisplayName, min( (int)cchMax, _len )*sizeof(TCHAR) );
    //LOG(_T("%s GetIcon %d"), __TFUNCTION__, sfi.iIcon );
    //LOG(_T("%s GetIcon %s %s"), __TFUNCTION__, sfi.szDisplayName, sfi.szTypeName );
    *piIndex = sfi.iIcon;
    *puFlags = GIL_NOTFILENAME;
    TRACE_RETURN S_OK;
}