#include "StdAfx.h"
#include "resource.h"
#include ".\seshellfolder.h"
#include "PidlEnum.h"
#include "Log.h"
#include "SEShellView.h"
#include "ExtractIcon.h"
#include "ContextMenuImpl.h"

HRESULT CSEShellFolder::FinalConstruct()
{
    TRACE_FUNCTION_EX();
    m_pidlPath = NULL;
    TRACE_RETURN S_OK; 
}

void CSEShellFolder::FinalRelease()
{
    TRACE_FUNCTION_EX();
    if(m_pidlPath != NULL)
    {
        m_pidlManager.Delete(m_pidlPath);
        m_pidlPath = NULL;
    } 
    TRACE_RETURN;
}

STDMETHODIMP CSEShellFolder::GetClassID( CLSID *pClassID )
{
    TRACE_FUNCTION();
    if ( pClassID )
    {
        *pClassID = CLSID_SEShellFolder;
        TRACE_RETURN S_OK;
    }
    TRACE_RETURN E_POINTER;
}

STDMETHODIMP CSEShellFolder::Initialize( LPCITEMIDLIST pidl )
{
    LOG( _T("%s %p"), __TFUNCTION__, pidl );
    if ( pidl && (!_Module.m_pidlROOT)  )
    {
        _Module.m_pidlROOT = m_pidlManager.Copy( pidl );
    }
    TRACE_RETURN NOERROR;
}

STDMETHODIMP CSEShellFolder::Initialize( LPCITEMIDLIST pidlFolder,IDataObject *pdtobj,HKEY hkeyProgID )
{
    TRACE_FUNCTION();
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEShellFolder::BindToObject(LPCITEMIDLIST pidl,
                                            LPBC pbcReserved, 
                                            REFIID riid, 
                                            LPVOID *ppRetVal)
{
    TRACE_FUNCTION();
    *ppRetVal = NULL;
    HRESULT hr=S_OK;
    if (!IsEqualIID(riid, IID_IShellFolder))
    {
        TRACE_RETURN E_NOINTERFACE;

    }

    CComObject<CSEShellFolder>* pFolder=NULL;
    hr=CComObject<CSEShellFolder>::CreateInstance(&pFolder);
    if(FAILED(hr))
    {
        TRACE_RETURN hr;
    }
    pFolder->AddRef();
    LPITEMIDLIST pidlNew = m_pidlManager.Concatenate(m_pidlPath,pidl);
    pFolder->m_pidlPath = m_pidlManager.Copy( pidlNew );
    m_pidlManager.Delete(pidlNew);
    hr=pFolder->QueryInterface(riid, ppRetVal);
    if ( !FAILED(hr) )
    {
#ifdef _DEBUG 
        TCHAR szPath[MAX_PATH] = {0};
        DWORD dwLen = MAX_PATH;
        m_pidlManager.GetFullName( pFolder->m_pidlPath, szPath, &dwLen );
        LOG( szPath );
        m_pidlManager.GetFullName( m_pidlPath, szPath, &dwLen );
        LOG( _T("parent: %s"), szPath );
        LOG(_T("%s %p m_pidlPath == %p"), __TFUNCTION__, pFolder, pFolder->m_pidlPath );
#endif
    }
    pFolder->Release();
    TRACE_RETURN hr;
}
STDMETHODIMP CSEShellFolder::BindToStorage(	LPCITEMIDLIST pidl, 
                                             LPBC pbcReserved, 
                                             REFIID riid, 
                                             LPVOID *ppvOut)
{
    *ppvOut = NULL;
    TRACE_RETURN E_NOTIMPL;
}

STDMETHODIMP CSEShellFolder::CreateViewObject(HWND hWnd, 
                                                REFIID riid, 
                                                LPVOID* ppRetVal)
{
    TRACE_FUNCTION();
    if( !ppRetVal )
    {
        TRACE_RETURN E_POINTER;
    }
 
    HRESULT hr = E_FAIL;
    if( riid == IID_IShellView ) 
    {
        CComObject<CSEShellView> *pView = NULL;
        hr= CComObject<CSEShellView>::CreateInstance(&pView);
        if(FAILED(hr))
        {
            TRACE_RETURN hr;
        }
        pView->AddRef();
        pView->_Init(this,m_pidlPath) ;
        hr=pView->QueryInterface(IID_IShellView, ppRetVal);
        pView->Release();
        TRACE_RETURN hr;
    }
    TRACE_RETURN E_NOINTERFACE;
}

// EnumObjects() creates a COM object that implements IEnumIDList.
STDMETHODIMP CSEShellFolder::EnumObjects(  HWND hWnd, 
                                           DWORD dwFlags, 
                                           LPENUMIDLIST* ppEnumIDList )
{
    TRACE_FUNCTION_EX();
    if ( !ppEnumIDList )
    {
        TRACE_RETURN E_POINTER;
    }

    HRESULT hr;
    CComObject<CPidlEnum>* pEnum;
    hr = CComObject<CPidlEnum>::CreateInstance(&pEnum);
    if (!FAILED(hr))
    {
        LOG(_T("%s %p m_pidlPath == %p"), __TFUNCTION__, this, m_pidlPath );
        pEnum->AddRef();
        hr = pEnum->_Init(m_pidlPath, dwFlags);
        if (!FAILED(hr))
        {
            hr = pEnum->QueryInterface(IID_IEnumIDList, (void**)ppEnumIDList);
        }
        pEnum->Release();
    }
    TRACE_RETURN hr;
}

STDMETHODIMP CSEShellFolder::GetAttributesOf(UINT uCount, 
                                               LPCITEMIDLIST pidls[], 
                                               LPDWORD pdwAttribs)
{
    TRACE_FUNCTION();
    *pdwAttribs = (DWORD)-1;
    if(( uCount==0 )||(pidls[0]->mkid.cb == 0)) 
    {
        DWORD dwAttr;
        dwAttr |= SFGAO_FOLDER | SFGAO_HASSUBFOLDER;
        *pdwAttribs &= dwAttr;
    }
    else
    {
        for( UINT i=0; i<uCount; i++ ) 
        {
            DWORD dwAttr = 0;
            switch( m_pidlManager.GetItemType(pidls[i]) ) 
            {
            case CPidlManager::FOLDER:
                {
                    dwAttr |=SFGAO_FOLDER;
                    //get the full pidl
                    LPITEMIDLIST   tmpPidl = m_pidlManager.Concatenate(m_pidlPath, pidls[i]);				
                    //decide whether current folder object has subfolder
                    if( m_pidlManager.HasSubFolder(tmpPidl) )
                    { 
                        dwAttr |= SFGAO_HASSUBFOLDER;
                    }
                    m_pidlManager.Delete(tmpPidl);
                }
                break;
            }
            *pdwAttribs &= dwAttr;
        }
    }

    TRACE_RETURN S_OK;
}

#define GET_SHGDN_FOR(dwFlags)         ((DWORD)dwFlags & (DWORD)0x0000FF00)
#define GET_SHGDN_RELATION(dwFlags)    ((DWORD)dwFlags & (DWORD)0x000000FF)

//GetDisplayNameOf() Retrieves the display name for the specified file object 
//                   or subfolder, returning it in a STRRET structure. 

STDMETHODIMP CSEShellFolder::GetDisplayNameOf(LPCITEMIDLIST pidl, 
                                              DWORD dwFlags, 
                                              LPSTRRET lpName)
{
    TRACE_FUNCTION();
    DWORD dwLen = MAX_PATH;
    TCHAR szText[MAX_PATH] = {0};
    
    if (NULL==pidl || pidl->mkid.cb == 0)//root folder
    {
        const TCHAR* pszRet = m_pidlManager.GetItemName( pidl );
        memcpy( szText, pszRet, _tcslen(pszRet)*sizeof(TCHAR) );
    }
    else
    {
        switch(GET_SHGDN_FOR(dwFlags))
        {
        case SHGDN_FORADDRESSBAR:
        case SHGDN_NORMAL:
            switch(GET_SHGDN_RELATION(dwFlags))
            {
            case SHGDN_INFOLDER:
                {
                    const TCHAR* pszRet = m_pidlManager.GetItemName( pidl );
                    memcpy( szText, pszRet, _tcslen(pszRet)*sizeof(TCHAR) );
                }
                break;
            case SHGDN_NORMAL:
                {
                    LPITEMIDLIST   tmpPidl;
                    tmpPidl = m_pidlManager.Concatenate(m_pidlPath, pidl);    
                    m_pidlManager.GetFullName( tmpPidl, szText, &dwLen );
                    m_pidlManager.Delete(tmpPidl);
                }
                break;
            default:
                return E_INVALIDARG;
            }
            break;
        default:
            return E_INVALIDARG;
        }
    }
    LOG( _T("%s %s"), __TFUNCTION__, szText );
    
    USES_CONVERSION;
#if defined(_UNICODE) || defined(UNICODE)
    // Allocate the wide character string 
    int cchOleStr = (int)_tcslen(szText) + 1;  
    lpName->pOleStr = (LPWSTR)_Module.m_Allocator.Alloc(cchOleStr * sizeof(OLECHAR)); 
    if( lpName->pOleStr==NULL ) 
    {
        TRACE_RETURN E_OUTOFMEMORY;  
    }
    lpName->uType = STRRET_WSTR;
    memcpy( lpName->pOleStr, szText, cchOleStr*sizeof(TCHAR));
#else
    lpName->uType = STRRET_CSTR;
    _tcscpy(lpName->cStr, T2CA(szText)); //cStr had allocated space
#endif
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEShellFolder::GetUIObjectOf(HWND hWnd, 
                                             UINT nCount, 
                                             LPCITEMIDLIST* pidls, 
                                             REFIID riid, LPUINT, 
                                             LPVOID* ppRetVal)
{
    TRACE_FUNCTION();
    if( NULL == ppRetVal ) 
    {
        TRACE_RETURN E_INVALIDARG;
    }

    HRESULT hr;

    if( IsEqualGUID(riid, IID_IExtractIcon) ) 
    {
        if ( nCount == 0 )
        {
            TRACE_RETURN E_INVALIDARG;
        }
        CComObject< CExtractIcon > *pExtractIcon = NULL;
        hr = CComObject< CExtractIcon >::CreateInstance(&pExtractIcon);
        if ( FAILED(hr) )
        {
            LOG( _T("%s query extractIcon failed"), __TFUNCTION__ );
            TRACE_RETURN hr;
        }
        pExtractIcon->AddRef();
        LPITEMIDLIST pId = m_pidlManager.Concatenate( m_pidlPath, *pidls );
        pExtractIcon->_Init(pId);
        m_pidlManager.Delete( pId );
        hr=pExtractIcon->QueryInterface(IID_IExtractIcon, ppRetVal);
        pExtractIcon->Release();
        TRACE_RETURN hr;
    }
    
    if ( IsEqualGUID( riid, IID_IContextMenu ) )
    {
        CComObject< CContextMenuImpl > *pContextMenu = NULL;
        hr = CComObject< CContextMenuImpl >::CreateInstance(&pContextMenu);
        if ( FAILED(hr) )
        {
            LOG( _T("%s query menu failed"), __TFUNCTION__ );
            TRACE_RETURN hr;
        }
        pContextMenu->AddRef();
        pContextMenu->_Init( m_pidlPath, pidls, nCount );
        hr=pContextMenu->QueryInterface(IID_IContextMenu, ppRetVal);
        if ( FAILED(hr) )
        {
            LOG( _T("%s query menu failed"), __TFUNCTION__ );
        }
        LOG( _T("%s query menu failed"), __TFUNCTION__ );
        pContextMenu->Release();
        TRACE_RETURN hr;
    }
    //if( riid == IID_IDropTarget ) 
    //{

    //    if( nCount !=1)
    //        TRACE_RETURN E_FAIL;

    //    CComObject<CNSFDropTarget>* pDropTarget;
    //    HR( CComObject<CNSFDropTarget>::CreateInstance(&pDropTarget) );

    //    pDropTarget->AddRef();	    

    //    HR( pDropTarget->_Init(this, *pidls) );

    //    hr=pDropTarget->QueryInterface(IID_IDropTarget, ppRetVal);

    //    pDropTarget->Release();

    //    TRACE_RETURN hr;
    //}

    TRACE_RETURN E_NOINTERFACE;
}


STDMETHODIMP CSEShellFolder::ParseDisplayName( HWND hwndOwner, 
                                                LPBC pbcReserved, 
                                                LPOLESTR lpDisplayName, 
                                                LPDWORD pdwEaten, 
                                                LPITEMIDLIST *ppidl, 
                                                LPDWORD pdwAttributes)
{
    if ( !ppidl )
    {
        TRACE_RETURN E_INVALIDARG;
    }
    if ( pdwEaten )
    {
        *pdwEaten = 0;
    }
    if ( pdwAttributes )
    {
        *pdwAttributes = 0;
    }
    if ( !lpDisplayName )
    {
        *ppidl =  m_pidlManager.Copy( m_pidlPath );
        TRACE_RETURN S_OK;
    }
    *ppidl = NULL;
#if defined(UNICODE)||defined(_UNICODE)
    LPTSTR pszDir = lpDisplayName;
    LPTSTR pszIter = pszDir;
#else
    TCHAR szDir[MAX_PATH] = {0};
    WideCharToMultiByte( CP_ACP, WC_COMPOSITECHECK, \
        lpDisplayName, -1, szDir, MAX_PATH, NULL, NULL );
    LPTSTR pszDir = szDir;
    LPTSTR pszIter = pszDir;
#endif
    TCHAR szFolderDir[MAX_PATH] = {0};
    DWORD dwLen;
    m_pidlManager.GetFullName( m_pidlPath,  szFolderDir, &dwLen );
    if ( _tcsncicmp( szFolderDir, pszDir, dwLen ) )
    {
        TRACE_RETURN S_OK;
    }
    LPITEMIDLIST pIdl = NULL;
    LPITEMIDLIST pIdlTmp = NULL;
    pszIter += _tcslen(szFolderDir);
    ++pszIter;
    LPTSTR pszEnum = pszIter;
    pszEnum = StrChr( pszIter, _T('\\') );
    while( pszEnum
        && *pszEnum != 0 )
    {
        *pszEnum = 0;
        pIdlTmp = m_pidlManager.Create( CPidlManager::FOLDER, pszDir, NULL );
        pIdl = m_pidlManager.Concatenate( pIdl, pIdlTmp );
        m_pidlManager.Delete( pIdlTmp );
        ++pszEnum;
        pszIter = pszEnum;
        pszEnum = StrChr( pszIter, _T('\\') );
    }
    if ( NULL == pszEnum
        && *pszIter != 0 )
    {
        CPidlManager::ITEM_TYPE type = CPidlManager::FILE;
        WIN32_FIND_DATA FindFileData;  
        HANDLE hFind=::FindFirstFile(pszDir,&FindFileData);  
        if(INVALID_HANDLE_VALUE != hFind
            && hFind != NULL )
        {
            if( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            {
                type = CPidlManager::FOLDER;
            }
            FindClose( hFind );
        }
        pIdlTmp = m_pidlManager.Create( type, pszIter, NULL );
        *ppidl = m_pidlManager.Concatenate( pIdl, pIdlTmp );
        m_pidlManager.Delete( pIdlTmp );
    }
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEShellFolder::SetNameOf(HWND, 
                                         LPCITEMIDLIST pidlOld, 
                                         LPCOLESTR pstrName, 
                                         DWORD, 
                                         LPITEMIDLIST* ppidlOut)
{
    TRACE_RETURN E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IQueryInfo
//
STDMETHODIMP CSEShellFolder::GetInfoFlags(DWORD* pdwFlags)
{
    TRACE_RETURN E_NOTIMPL;
}

//
STDMETHODIMP CSEShellFolder::GetInfoTip(DWORD, LPWSTR* ppwszTip)
{
    TRACE_FUNCTION();
    *ppwszTip = (LPWSTR)_Module.m_Allocator.Alloc(((ULONG)wcslen(L"虚拟实例提示")+1) * sizeof(WCHAR)); 
    if( *ppwszTip==NULL ) 
    {
        TRACE_RETURN E_OUTOFMEMORY;  
    }
    ::ZeroMemory(*ppwszTip,(wcslen(L"虚拟实例提示")+1) * sizeof(WCHAR));
    wcscpy(*ppwszTip, L"虚拟实例提示");
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEShellFolder::CompareIDs( LPARAM lParam, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2 )
{
    TRACE_FUNCTION();
    CPidlManager::ITEM_TYPE it1,it2;
    it1=m_pidlManager.GetItemType(pidl1);
    it2=m_pidlManager.GetItemType(pidl2);
    if( it1 == it2 )
    {
        TCHAR szName1[MAX_PATH]=_T("");
        TCHAR szName2[MAX_PATH]=_T("");
        DWORD dwLen1=MAX_PATH;
        DWORD dwLen2=MAX_PATH;
        m_pidlManager.GetFullName(pidl1,szName1,&dwLen1);
        m_pidlManager.GetFullName(pidl2,szName2,&dwLen2);
        TRACE_RETURN _tcscmp(szName1, szName2);
    }
    else
    {
        if( CPidlManager::FOLDER == it1)//pidl1 correspond a folder object
        {
            TRACE_RETURN -1;
        }
        else
        {
            TRACE_RETURN 1;
        }
    }
}

STDMETHODIMP CSEShellFolder::AddPages( LPFNSVADDPROPSHEETPAGE pfnAddPage, LPARAM lParam )
{
    TRACE_FUNCTION();
    PROPSHEETPAGE psp;
    ZeroMemory( &psp, sizeof(PROPSHEETPAGE) );
    psp.dwSize = sizeof( PROPSHEETPAGE );
    psp.dwFlags = PSP_USEREFPARENT|PSP_USETITLE|PSP_DEFAULT|PSP_USEHEADERSUBTITLE|PSP_HIDEHEADER;
    psp.hInstance = _Module.GetModuleInstance();
    psp.pszTemplate = MAKEINTRESOURCE(IDD_DIALOG_PROP);
    psp.pszTitle = __TEXT("常规");
    psp.pszHeaderSubTitle = _T("sub title");
    psp.pfnDlgProc = (DLGPROC)::DefWindowProc;
    psp.pszHeaderTitle = _T("title");
    psp.pcRefParent = (UINT*)(&_Module.m_nLockCnt);
    HPROPSHEETPAGE hPage = ::CreatePropertySheetPage(&psp);
    if ( hPage )
    {
        if ( !pfnAddPage(hPage, lParam) )
        {
            ::DestroyPropertySheetPage(hPage);
            hPage = NULL;
        }
        LOG( _T("%s %p"), __TFUNCTION__, hPage );
        TRACE_RETURN NOERROR;
    }
    LOG( _T("%s %p Failed"), __TFUNCTION__, hPage );
    TRACE_RETURN E_INVALIDARG;
}

STDMETHODIMP CSEShellFolder::ReplacePage( EXPPS uPageID,LPFNSVADDPROPSHEETPAGE pfnReplaceWith, LPARAM lParam )
{
    TRACE_FUNCTION();
    TRACE_RETURN E_NOTIMPL;
}