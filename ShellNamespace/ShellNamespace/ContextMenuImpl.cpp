#include "StdAfx.h"
#include ".\contextmenuimpl.h"
#include "Log.h"
//STDMETHODIMP CContextMenuImpl::Initialize( LPCITEMIDLIST pidlFolder,IDataObject *pdtobj,HKEY hkeyProgID )
//{
//    TRACE_FUNCTION();
//    m_pidlItem = m_pidlManager.Copy( pidlFolder );
//#ifdef _DEBUG
//    TCHAR szDir[MAX_PATH] = {0};
//    DWORD dwLen = MAX_PATH;
//    m_pidlManager.GetFullName( m_pidlItem, szDir, &dwLen );
//    LOG(_T("%s %s"), __TFUNCTION__, szDir );
//#endif 
//    TRACE_RETURN S_OK;
//}

HRESULT CContextMenuImpl::_Init( LPCITEMIDLIST pRoot, LPCITEMIDLIST* pidls, int pidCounts )
{
    if ( !pidls
        || pidCounts <= 0 )
    {
        TRACE_RETURN E_INVALIDARG;
    }
    m_vecPath.clear();
    if ( m_pidlRoot )
    {
        m_pidlManager.Delete( m_pidlRoot );
    }
    m_pidlRoot = m_pidlManager.Copy(pRoot);
    TCHAR szDir[MAX_PATH] = {0};
    DWORD dwLen = MAX_PATH;
    LPITEMIDLIST pPidl = NULL;
    for ( int _i = 0; _i < pidCounts;  ++_i )
    {
        pPidl = m_pidlManager.Concatenate( m_pidlRoot, pidls[_i] );
        m_pidlManager.GetFullName( pPidl, szDir, &dwLen );
        m_pidlManager.Delete( pPidl );
        m_vecPath.push_back(szDir);
        dwLen = MAX_PATH;
    }
    TRACE_RETURN S_OK;
}

STDMETHODIMP CContextMenuImpl::QueryContextMenu(
                            HMENU hmenu,
                            UINT indexMenu,
                            UINT idCmdFirst,
                            UINT idCmdLast,
                            UINT uFlags)
{
    TRACE_FUNCTION();
    //注释掉:允许空白区域菜单
    //if ( m_vecPath.empty() )
    //{
    //    TRACE_RETURN S_OK;
    //}
    TCHAR szDir[MAX_PATH] = {0};
    DWORD dwLen = MAX_PATH;
    HRESULT hr = E_FAIL;
    //查询Shell菜单
    IShellFolder* pDesktop = NULL;
    hr = SHGetDesktopFolder( &pDesktop );
    if ( FAILED(hr)
        || !pDesktop )
    {
        LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
        TRACE_RETURN hr;
    }

    //获取当前操作目录的Folder接口
    IShellFolder* pCurFolder = NULL;
    m_pidlManager.GetFullName( m_pidlRoot, szDir, &dwLen );
    //替换为系统目录
    m_pidlManager.ReplaceRoot( szDir );

    LPITEMIDLIST pPidlTmp = NULL;
    hr = pDesktop->ParseDisplayName( NULL, 0,szDir, NULL, &pPidlTmp, NULL );
    if ( FAILED(hr)
        || NULL == pPidlTmp )
    {
        LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
        pDesktop->Release();
        TRACE_RETURN hr;
    }

    //LOG( _T("%s root Folder_Get %s, %d"), __TFUNCTION__, bstr, __LINE__ );
    UINT nCount = m_pidlManager.CountItem( pPidlTmp );
    if ( 0 == nCount )
    {
        pDesktop->Release();
        m_pidlManager.Delete( pPidlTmp );
        LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, E_FAIL, __LINE__ );
        TRACE_RETURN E_FAIL;
    }
    if ( nCount == 1 )
    {
        hr = pDesktop->QueryInterface(IID_IShellFolder, (void**)&pCurFolder );
        m_pidlManager.Delete( pPidlTmp );
        if( FAILED( hr )
            || NULL == pCurFolder )
        {
            m_pidlManager.Delete( pPidlTmp );
            pDesktop->Release();
            LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
            TRACE_RETURN hr;
        }
    }
    else
    {
        LPITEMIDLIST pItemParent = m_pidlManager.Copy( pPidlTmp );
        if ( !pItemParent )
        {
            LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, E_FAIL, __LINE__ );
            pDesktop->Release();
            m_pidlManager.Delete( pPidlTmp );
            TRACE_RETURN E_FAIL;
        }
        IShellFolder* pShell = NULL;
        hr = pDesktop->BindToObject( pItemParent, NULL, IID_IShellFolder, (void**)&pShell );
        m_pidlManager.Delete( pItemParent );
        if( FAILED(hr)
            || NULL == pShell )
        {
            m_pidlManager.Delete( pPidlTmp );
            pDesktop->Release();
            LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
            TRACE_RETURN hr;
        }
        hr = pShell->QueryInterface( IID_IShellFolder, (void**)&pCurFolder );
        pShell->Release();
        if ( FAILED( hr )
            || NULL == pCurFolder )
        {
            m_pidlManager.Delete( pPidlTmp );
            pDesktop->Release();
            LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
            TRACE_RETURN hr;
        }
    }
    //解析PIDL 列表
    int ncount = (int)m_vecPath.size();
    LPITEMIDLIST* pIdls = (LPITEMIDLIST*)_Module.m_Allocator.Alloc( sizeof(LPITEMIDLIST)*ncount );
    ZeroMemory( pIdls, sizeof(LPITEMIDLIST)*ncount );
    TCHAR szPath[MAX_PATH] = {0};
    LPITEMIDLIST lp = NULL;
    for ( int _i = 0; _i < ncount; ++_i )
    {
        CopyMemory( szPath, m_vecPath[_i].c_str(), (m_vecPath[_i].length()+1)*sizeof(TCHAR) );
        m_pidlManager.ReplaceRoot( szPath );
        hr = pDesktop->ParseDisplayName( NULL, NULL, szPath, NULL, &lp, NULL );
        pIdls[_i] = m_pidlManager.Copy( m_pidlManager.GetLastItem(lp) );
        m_pidlManager.Delete( lp );
        if ( FAILED(hr) )
        {
            LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
        }
    }
    if ( m_pProxyMenu )
    {
        m_pProxyMenu->Release();
    }
    m_pProxyMenu = NULL;
    //取菜单接口
    LPCONTEXTMENU pTmp = NULL;
    hr = pCurFolder->GetUIObjectOf (NULL, ncount, (LPCITEMIDLIST*) pIdls, \
        IID_IContextMenu, NULL, (void**) &pTmp);
    if ( FAILED(hr)
        || NULL == pTmp )
    {
        _Module.m_Allocator.Free( pIdls );
        m_pidlManager.Delete( pPidlTmp );
        pCurFolder->Release();
        pDesktop->Release();
        LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
        return hr;
    }
    hr = pTmp->QueryInterface( IID_IContextMenu, (void**)&m_pProxyMenu );
    pTmp->Release();
    if ( FAILED(hr)
        || NULL == m_pProxyMenu )
    {
        _Module.m_Allocator.Free( pIdls );
        m_pidlManager.Delete( pPidlTmp );
        pCurFolder->Release();
        pDesktop->Release();
        LOG( _T("%s Error 0x%08x, line %d"), __TFUNCTION__, hr, __LINE__ );
        return hr;
    }
    //取菜单项
    m_pProxyMenu->QueryContextMenu ( hmenu, indexMenu, \
        idCmdFirst, idCmdLast, uFlags );

    for ( int _i = 0; _i < (int)m_vecPath.size(); ++_i )
    {
        m_pidlManager.Delete( pIdls[_i] );
    }
    _Module.m_Allocator.Free( pIdls );
    m_pidlManager.Delete( pPidlTmp );
    pCurFolder->Release();
    pDesktop->Release();
    TRACE_RETURN S_OK;
}

STDMETHODIMP CContextMenuImpl::InvokeCommand(LPCMINVOKECOMMANDINFO lpici)
{
    TRACE_FUNCTION();
    if ( m_pProxyMenu )
    {
        TRACE_RETURN m_pProxyMenu->InvokeCommand( lpici );
    }
    TRACE_RETURN S_OK;
}

STDMETHODIMP CContextMenuImpl::GetCommandString(
                            UINT_PTR    idCmd,
                            UINT        uType,
                            UINT      * pwReserved,
                            LPSTR       pszName,
                            UINT        cchMax)
{
    TRACE_FUNCTION();
    if ( m_pProxyMenu )
    {
        TRACE_RETURN m_pProxyMenu->GetCommandString( idCmd, uType, pwReserved, pszName, cchMax );
    }
    TRACE_RETURN S_OK;
}

HRESULT CContextMenuImpl::FinalConstruct()
{
    m_vecPath.clear();
    m_pidlRoot = NULL;
    m_pProxyMenu = NULL;
    TRACE_RETURN S_OK;
}

void CContextMenuImpl::FinalRelease()
{
    if( m_pidlRoot )
    {
        m_pidlManager.Delete( m_pidlRoot );
        m_pidlRoot = NULL;
    }
    if( m_pProxyMenu )
    {
        m_pProxyMenu->Release();
        m_pProxyMenu = NULL;
    }
    m_vecPath.clear();
}