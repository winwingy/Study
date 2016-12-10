#include "StdAfx.h"
#include ".\seshellview.h"
#include "PidlEnum.h"
#include "Log.h"
#include <WindowsX.h>
#include "ContextMenuImpl.h"
#include "DlgSet.h"
WNDPROC orgProc;
HRESULT CSEShellView::FinalConstruct()
{
    TRACE_FUNCTION();

    m_pShellBrowser=NULL;
    m_hWnd=NULL;   
    m_hMenu = NULL;
    m_pFolder = NULL;
    m_pidlRoot = NULL;
    TRACE_RETURN S_OK;
}

void CSEShellView::FinalRelease()
{
    TRACE_FUNCTION();
    if ( m_pFolder )
    {
        m_pFolder->Release();
    }
    m_pShellBrowser=NULL;
    m_hWnd=NULL;
    if(m_pidlRoot != NULL)
    {
        m_pidlManager.Delete(m_pidlRoot);
        m_pidlRoot = NULL;
    }
    TRACE_RETURN;
}

/////////////////////////////////////////////////////////////////////////////
// IOleWindow

HRESULT CSEShellView::GetWindow(HWND* phWnd)
{
    TRACE_FUNCTION();
    if(::IsWindow(m_hWnd)==0)
    {
        TRACE_RETURN E_FAIL;
    }
    *phWnd = m_hWnd;
    TRACE_RETURN S_OK;
}

HRESULT CSEShellView::ContextSensitiveHelp(BOOL)
{
    TRACE_FUNCTION();
    TRACE_RETURN E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////
// IShellView
HRESULT CSEShellView::AddPropertySheetPages(
    DWORD /*dwReserved*/,
    LPFNADDPROPSHEETPAGE /*lpfn*/, 
    LPARAM /*lParam*/)
{
    TRACE_FUNCTION();
    TRACE_RETURN E_NOTIMPL;
}

STDMETHODIMP CSEShellView::CreateViewWindow(
    LPSHELLVIEW lpPrevView,
    LPCFOLDERSETTINGS lpFS, 
    LPSHELLBROWSER pSB,
    LPRECT prcView, 
    HWND* phWnd)
{
    TRACE_FUNCTION();
    assert(m_pFolder);
    if( m_pFolder==NULL ) 
    {
        LOG( _T("IShellFolder is NULL\n") );
        TRACE_RETURN E_FAIL;
    }
    assert(prcView);
    assert(pSB);
    assert(lpFS);
    assert(phWnd);

    WNDCLASS wc = { 0 };
    *phWnd = NULL;

    // If our window class has not been registered, then do so
    if( !::GetClassInfo(_Module.GetModuleInstance(), _T("SE_VIEW"), &wc) ) 
    {
        wc.style             = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc     = (WNDPROC)WndProc;
        wc.cbClsExtra      = 0;
        wc.cbWndExtra      = 0;
        wc.hInstance        = _Module.GetModuleInstance();
        wc.hIcon             = NULL;
        wc.hCursor          = ::LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName    = NULL;
        wc.lpszClassName  = _T("SE_VIEW"); 
        if( !::RegisterClass(&wc) ) 
        {
            TRACE_RETURN HRESULT_FROM_WIN32(::GetLastError());
        }
    }

    // Set up the member variables
    m_pShellBrowser = pSB;
    m_folderSettings = *lpFS;

    // Load the ListView settings from the stream
    // FIX: This fixes a problem where XP doesn't relay this attribute
    CComPtr<IStream> spStream;
    if( SUCCEEDED( m_pShellBrowser->GetViewStateStream(STGM_READ, &spStream) ) ) 
    {
        long lVersion = 0;
        DWORD dwRead = 0;
        spStream->Read(&lVersion, sizeof(lVersion), &dwRead);
        spStream->Read(&m_folderSettings.ViewMode, sizeof(m_folderSettings.ViewMode), &dwRead);
    }
    // Get our parent window
    HWND hwndShell = NULL;
    m_pShellBrowser->GetWindow(&hwndShell);

    // Create host window
    m_hWnd= ::CreateWindowEx(WS_EX_CLIENTEDGE|WS_EX_CONTROLPARENT,
        WC_LISTVIEW,
        NULL,
        WS_CHILD | WS_VISIBLE | LVS_REPORT \
        | LVS_SHAREIMAGELISTS | LVS_SHOWSELALWAYS,
        prcView->left,
        prcView->top,
        prcView->right - prcView->left,
        prcView->bottom - prcView->top,
        hwndShell,
        NULL,
        _Module.GetModuleInstance(),
        NULL);
    if( m_hWnd == NULL ) 
    {
        TRACE_RETURN HRESULT_FROM_WIN32(::GetLastError());
    }
    SetWindowLong(m_hWnd, GWLP_USERDATA, (LONG)(LONG_PTR)this);
    orgProc = (WNDPROC)(LONG_PTR)SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)CSEShellView::WndProc );
    *phWnd = m_hWnd;
    LVCOLUMN column;
    column.mask = LVCF_TEXT|LVCF_FMT;
    column.pszText = (LPTSTR)TEXT ("名称");
    column.fmt = LVCFMT_LEFT;
    column.mask |= LVCF_WIDTH;
    column.cx = 300;
    column.mask |= LVCF_SUBITEM;
    column.iSubItem = 0;
    ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, 0, (LPARAM)&column);

    column.pszText = TEXT ("大小");
    column.fmt = LVCFMT_RIGHT;
    column.cx = 60;
    column.iSubItem = 1;
    ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, 1, (LPARAM)&column);
    column.pszText = TEXT ("类型");
    column.fmt = LVCFMT_LEFT;
    column.cx = 50;
    column.iSubItem = 2;
    ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, 2, (LPARAM)&column);
    column.pszText = TEXT ("修改日期");
    column.fmt = LVCFMT_LEFT;
    column.cx = 80;
    column.iSubItem = 3;
    ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, 3, (LPARAM)&column);

    SHFILEINFO sfi;
    ListView_SetImageList( m_hWnd\
        , (HIMAGELIST) SHGetFileInfo (m_pidlManager.GetRootDir(), NULL, &sfi, sizeof (sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)\
        , LVSIL_SMALL );
    ListView_SetImageList( m_hWnd\
        , (HIMAGELIST) SHGetFileInfo (m_pidlManager.GetRootDir(), NULL, &sfi, sizeof (sfi), SHGFI_SYSICONINDEX | SHGFI_LARGEICON)\
        , LVSIL_NORMAL );
    ListView_SetExtendedListViewStyleEx (m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

    m_pShellBrowser->AddRef();
    InitTooBar();
    Refresh();
    TRACE_RETURN S_OK;
}

STDMETHODIMP CSEShellView::DestroyViewWindow(void)
{
    TRACE_FUNCTION();
    UIActivate(SVUIA_DEACTIVATE);
    ::DestroyWindow(m_hWnd);
    m_pShellBrowser->Release();
    TRACE_RETURN S_OK;
}

HRESULT CSEShellView::EnableModeless(BOOL /*fEnable*/)
{
    TRACE_FUNCTION();
    TRACE_RETURN E_NOTIMPL;
}

//GetCurrentInfo(): Retrieves the current folder settings. 
HRESULT CSEShellView::GetCurrentInfo(LPFOLDERSETTINGS lpFS)
{
    TRACE_FUNCTION();
    *lpFS = m_folderSettings;
    TRACE_RETURN S_OK;
}
HRESULT CSEShellView::Refresh(void)
{
    TRACE_FUNCTION();
    Clear(); 
    if ( !m_pFolder )
    {
        return E_NOTIMPL;
    }
    LPENUMIDLIST pEnumIDList = NULL;
    HRESULT hr = m_pFolder->EnumObjects(
        m_hWnd, SHCONTF_NONFOLDERS | SHCONTF_FOLDERS, &pEnumIDList);
    if ( FAILED(hr) )
    {
        return hr;
    }
    {
        // 更新状态栏
        LRESULT lResult;
        INT nPartArray[1] = { -1 };
        m_pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETPARTS, 1, (LPARAM)nPartArray, &lResult);
        TCHAR szInfo[128] = { 0 };
        TCHAR* pszInfo = (TCHAR*)szInfo;

        CPidlEnum* pEnumInstance = (CPidlEnum*)((DWORD_PTR)pEnumIDList-offsetofclass( IEnumIDList, CPidlEnum ));

        int nPos = _sntprintf( pszInfo, 128, _T("虚拟网盘"), pEnumInstance->GetAllItemCount() );
        //if ( pEnumInstance->GetHideItemCount() > 0
        //    && !pEnumInstance->IsIncludeHide() )
        //{
        //    _sntprintf( pszInfo+nPos, 128-nPos, _T("(加 %d 个隐藏对象)"), pEnumInstance->GetHideItemCount() );   
        //}
        m_pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXT, 0, (LPARAM)szInfo, &lResult);
    }
    pEnumIDList->Reset();
    LPITEMIDLIST lpItem = NULL;
    while( S_OK == pEnumIDList->Next( 1, &lpItem, NULL ) )
    {
        const TCHAR* pszName = m_pidlManager.GetItemName( m_pidlManager.GetLastItem( lpItem ) );
        if ( pszName )
        {
            LVITEM item;
            item.mask = LVIF_TEXT|LVIF_IMAGE|LVIF_PARAM;
            item.iItem = ListView_GetItemCount( m_hWnd );
            item.iSubItem = 0;
            item.pszText = (LPTSTR)pszName;
            item.state = 0;
            item.stateMask = 0;
            item.lParam = (LPARAM)(LONG_PTR)lpItem;
            SHFILEINFO sfi;
            if( m_pidlManager.GetItemType(lpItem) == CPidlManager::FOLDER )
            {
                LPITEMIDLIST pidl = NULL;
                pidl = m_pidlManager.Concatenate( m_pFolder->GetRootPath(), lpItem );
                TCHAR szPath[MAX_PATH] = {0};
                DWORD dwLen = MAX_PATH;
                m_pidlManager.GetFullName( pidl, szPath, &dwLen );
                m_pidlManager.ReplaceRoot( szPath );
                m_pidlManager.Delete( pidl );
                SHGetFileInfo (szPath,\
                    NULL, &sfi, sizeof (sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
                item.iImage = sfi.iIcon;
            }
            else
            {
                SHGetFileInfo (pszName,\
                    FILE_ATTRIBUTE_NORMAL, &sfi, sizeof (sfi), SHGFI_USEFILEATTRIBUTES|SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
                item.iImage = sfi.iIcon;
            }
            int nItem = ListView_InsertItem( m_hWnd, &item );
            
            if( m_pidlManager.GetItemType(lpItem) == CPidlManager::FOLDER )
            {
                ListView_SetItemText( m_hWnd, nItem, 2, _T("文件夹") );
            }
            else
            {
                LPTSTR pExt = StrChrI( pszName, _T('.') );
                if ( pExt )
                {
                    ++pExt;
                    ListView_SetItemText( m_hWnd, nItem, 2, pExt );
                }
                else
                {
                    ListView_SetItemText( m_hWnd, nItem, 2, _T("未知文件") );
                }
            }
        }
    }
    pEnumIDList->Release();
    TRACE_RETURN S_OK;
}

HRESULT CSEShellView::SaveViewState(void)
{
    ATLASSERT(m_pShellBrowser); 
    CComPtr<IStream> spStream;

    if( FAILED( m_pShellBrowser->GetViewStateStream(STGM_WRITE, &spStream) ) ) 
    {
        TRACE_RETURN S_OK;
    }
    long lVersion = 1;
    DWORD dwWritten = 0;
    spStream->Write(&lVersion, sizeof(lVersion), &dwWritten);
    spStream->Write(&m_folderSettings.ViewMode, sizeof(m_folderSettings.ViewMode), &dwWritten);
    TRACE_RETURN S_OK;
}

HRESULT CSEShellView::SelectItem(LPCITEMIDLIST /*pidlItem*/, UINT /*uFlags*/)
{
    TRACE_FUNCTION();
    TRACE_RETURN S_OK;
}


STDMETHODIMP CSEShellView::GetItemObject(UINT uItem, REFIID riid, LPVOID* ppRetVal)
{
    TRACE_RETURN E_NOINTERFACE;
}

//TranslateAccelerator():Translates accelerator key strokes 
//                        when a namespace extension's view has the focus. 
STDMETHODIMP CSEShellView::TranslateAccelerator(LPMSG pmsg)
{
    TRACE_RETURN S_FALSE;
}

HRESULT CSEShellView::UIActivate(UINT uState)
{
    TRACE_FUNCTION();
    if ( SVUIA_DEACTIVATE == uState )
    {
        UninitMenu();
    }
    else
    {
        InitMenu();
    }
    TRACE_RETURN S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// Implementation

HRESULT CSEShellView::_Init(CSEShellFolder* pFolder,LPCITEMIDLIST pidlRoot)
{
    TRACE_FUNCTION();
    assert(pFolder);
    if( pFolder==NULL ) 
    {
        TRACE_RETURN E_FAIL;
    }
    m_pFolder = pFolder;
    m_pFolder->AddRef();
    m_pidlRoot=m_pidlManager.Copy(pidlRoot);
    LPENUMIDLIST pEnum;
    m_pFolder->EnumObjects( NULL, -1, &pEnum );
    pEnum->Release();
    TRACE_RETURN S_OK;
}

LRESULT CALLBACK CSEShellView::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    static bool bLBDown = false;
    CSEShellView *pThis = (CSEShellView *)(LONG_PTR)::GetWindowLong(hWnd, GWLP_USERDATA);
    LRESULT lret = 0;
    if ( orgProc )
    {
        lret = CallWindowProc( orgProc, hWnd, uMessage, wParam, lParam );
    }
    else
    {
        lret = DefWindowProc( hWnd, uMessage, wParam, lParam );
    }
    if ( WM_COMMAND == uMessage
	          && pThis )
    {
        pThis->OnCommand(GET_WM_COMMAND_ID(wParam, lParam),
            GET_WM_COMMAND_CMD(wParam, lParam),
            GET_WM_COMMAND_HWND(wParam, lParam));
    }
    else if ( WM_LBUTTONDBLCLK == uMessage
        && pThis )
    {
        NMHDR nmhdr = {0};
        nmhdr.code = NM_DBLCLK;
        pThis->OnNotify( &nmhdr );
    }  
    else if ( WM_RBUTTONDOWN == uMessage
        && pThis )
    {
        NMHDR nmhdr = {0};
        nmhdr.code = NM_RCLICK;
        pThis->OnNotify( &nmhdr );
    }
    else if ( WM_SIZE == uMessage 
        && pThis )
    {
        InvalidateRect( pThis->m_hWnd, NULL, true );
        UpdateWindow(pThis->m_hWnd);
    }
    return lret;
}

LRESULT CSEShellView::OnCommand( DWORD dwCmdID, DWORD dwCmd, HWND hwndCmd )
{
    if ( dwCmdID == 1000 )
    {
        const TCHAR* pszRoot = DialogBoxReRoot( m_hWnd, g_szRealRoot );
        if ( pszRoot )
        {
            CopyMemory( g_szRealRoot, pszRoot, (_tcslen(pszRoot)+1)*sizeof(TCHAR) );
            m_pShellBrowser->BrowseObject(NULL, SBSP_PARENT );
            //IShellFolder* pFolder = NULL;
            //m_pFolder->BindToObject( NULL, NULL, IID_IShellFolder, (void**)&pFolder );
            //if ( pFolder )
            //{
            //    pFolder->Release();
            //}
        }
    }
    return 0;
}

void CSEShellView::InitTooBar()
{
    TRACE_FUNCTION();
    assert(m_pShellBrowser);
    if ( m_pShellBrowser )
    {
        TBBUTTON btnArray[2] = {0};
        btnArray[0].dwData = 0;
        btnArray[0].fsStyle = BTNS_BUTTON|BTNS_SHOWTEXT;
        btnArray[0].idCommand = 1000;
        btnArray[0].fsState = TBSTATE_ENABLED;
        btnArray[0].iString = (INT_PTR)_T("设置");
        btnArray[1] = btnArray[0];
        btnArray[1].idCommand = 1001;
        btnArray[1].iString = (INT_PTR)_T("Button2");
        m_pShellBrowser->SetToolbarItems(btnArray, 2,  FCT_MERGE );
    }
    TRACE_RETURN;
}

// MergeFileMenu
void MergeFileMenu( HMENU hSubMenu )
{
    if( hSubMenu )
    {
        MENUITEMINFO mii;
        ZeroMemory( &mii, sizeof(MENUITEMINFO) );
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
        mii.fType = MFT_STRING;
        mii.fState = MFS_ENABLED;
        mii.dwTypeData = _T("File 菜单项1");
        mii.wID = 1007;
        InsertMenuItem( hSubMenu, 0, TRUE, &mii );

        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
        mii.fType = MFT_STRING;
        mii.fState = MFS_ENABLED;
        mii.dwTypeData = _T("File 菜单项2");
        mii.wID = 1008;
        InsertMenuItem( hSubMenu, 0, TRUE, &mii );

        mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
        mii.fType = MFT_SEPARATOR;
        mii.fState = MFS_ENABLED;
        InsertMenuItem( hSubMenu, 2, TRUE, &mii );
    }
}

void CSEShellView::InitMenu()
{
    TRACE_FUNCTION();
    UninitMenu();
    m_hMenu = CreateMenu();
    if(m_hMenu)
    {
        OLEMENUGROUPWIDTHS omw = {0, 0, 0, 0, 0, 0};
        if( FAILED(m_pShellBrowser->InsertMenusSB(m_hMenu, &omw)) )
        {
            LOG( _T("Menu Failed") );
        }

        MENUITEMINFO mii;
        ZeroMemory(&mii, sizeof(MENUITEMINFO));
        mii.cbSize = sizeof(mii);
        mii.fMask = MIIM_SUBMENU | MIIM_TYPE | MIIM_STATE;
        mii.fType = MFT_STRING;
        mii.fState = MFS_ENABLED;
        mii.dwTypeData = _T("菜单1");
        {
            HMENU hSubMenu = CreatePopupMenu();
            if(hSubMenu)
            {
                TCHAR szText[100] = {0};
                MENUITEMINFO mii;

                ZeroMemory(&mii, sizeof(MENUITEMINFO));
                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
                mii.fType = MFT_STRING;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("子菜单1");
                mii.wID = 1003;
                InsertMenuItem(hSubMenu, static_cast<UINT>(-1), TRUE, &mii);

                ZeroMemory(&mii, sizeof(MENUITEMINFO));
                mii.cbSize = sizeof(MENUITEMINFO);
                mii.fMask = MIIM_TYPE | MIIM_ID | MIIM_STATE;
                mii.fType = MFT_STRING;
                mii.fState = MFS_ENABLED;
                mii.dwTypeData = _T("子菜单2");;
                mii.wID = 1004;
                InsertMenuItem(hSubMenu, static_cast<UINT>(-1), TRUE, &mii);
            }
            mii.hSubMenu = hSubMenu;
        }
        if(mii.hSubMenu)
        {
            InsertMenuItem(m_hMenu, FCIDM_MENU_HELP, FALSE, &mii);
        }
        DeleteMenu(m_hMenu, FCIDM_MENU_EDIT, MF_BYCOMMAND);
        LOG( _T("deleting Edit menu") );
        ZeroMemory(&mii, sizeof(MENUITEMINFO));
        mii.cbSize = sizeof(MENUITEMINFO);
        mii.fMask = MIIM_SUBMENU;
        if(GetMenuItemInfo(m_hMenu, FCIDM_MENU_FILE, FALSE, &mii))
        {
            MergeFileMenu(mii.hSubMenu);
        }
        m_pShellBrowser->SetMenuSB(m_hMenu, NULL, m_hWnd);
    }
    TRACE_RETURN;
}

void CSEShellView::UninitMenu()
{
    TRACE_FUNCTION();
    if(m_hMenu)
    {
        m_pShellBrowser->SetMenuSB(NULL, NULL, NULL);
        m_pShellBrowser->RemoveMenusSB(m_hMenu);
        DestroyMenu(m_hMenu);
        m_hMenu = NULL;
    }
    TRACE_RETURN;
}

void CSEShellView::Clear()
{
    if ( m_hWnd )
    {
        LV_ITEM item = {0};
        for ( int _i = 0; _i < ListView_GetItemCount(m_hWnd); ++_i )
        {
            item.iItem = _i;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            ListView_GetItem( m_hWnd, &item );
            LPITEMIDLIST pItem = (LPITEMIDLIST)(LONG_PTR)item.lParam;
            if ( pItem )
            {
                m_pidlManager.Delete(pItem);
            }
        }
        ListView_DeleteAllItems(m_hWnd);
    }
}
#define ITEMLISTPTR( x ) ((LPITEMIDLIST)(LONG_PTR)x)
LRESULT CSEShellView::OnNotify( LPNMHDR pNmhdr )
{
    TRACE_FUNCTION();
    if( pNmhdr )
    {
        if( pNmhdr->code == NM_DBLCLK )
        {
            int iIndex = ListView_GetNextItem( m_hWnd, -1, LVNI_FOCUSED );
            if (iIndex == -1)
            {
                return S_OK;
            }
            LV_ITEM item = {0};
            item.iItem = iIndex;
            item.iSubItem = 0;
            item.mask = LVIF_PARAM;
            ListView_GetItem( m_hWnd, &item );
            if ( item.lParam )
            {               
                LPITEMIDLIST pItem = m_pidlManager.Copy( ITEMLISTPTR(item.lParam) );
                if( m_pidlManager.GetItemType( pItem ) == CPidlManager::FOLDER )
                {
                    LOG(_T("%s 打开文件夹 %s"), __TFUNCTION__, m_pidlManager.GetItemName( pItem ) );
                    //打开文件夹
                    m_pShellBrowser->BrowseObject( pItem , SBSP_DEFBROWSER | SBSP_RELATIVE );
                }
                else
                {   
                    TCHAR szPath[MAX_PATH] = {0};
                    DWORD dwLen = MAX_PATH;
                    LPITEMIDLIST pFullItem = m_pidlManager.Concatenate(m_pFolder->GetRootPath(), pItem );
                    m_pidlManager.GetFullName( pFullItem, szPath, &dwLen );
                    m_pidlManager.ReplaceRoot( szPath );
                    m_pidlManager.Delete( pFullItem );
                    //打开文件
                    LONG ret = (LONG)(LONG_PTR)ShellExecute(NULL, _T("open"),  szPath, NULL, NULL, SW_SHOWNORMAL);
                    if ( ret > 32 )
                    {
                        return TRUE;
                    }
                    switch ( ret )
                    {
                    case ERROR_BAD_FORMAT:
                        {
                            ::MessageBox( NULL, _T("文件已损坏"), _T("提示"), MB_OK );
                        }
                        break;
                    case SE_ERR_ASSOCINCOMPLETE:
                    case SE_ERR_NOASSOC:
                        {
                            TCHAR szRunParam[MAX_PATH<<1] = {0};
                            _sntprintf( szRunParam, MAX_PATH<<1, _T("shell32.dll,OpenAs_RunDLL \"%s\""), szPath );
                            ShellExecute(NULL, NULL, _T("rundll32.exe "), szRunParam, NULL, SW_SHOW); 
                        }
                        break;
                    default:
                        {
                        }
                    }
                }
                m_pidlManager.Delete( pItem );
                pItem = NULL;
            }
        }
        else if( pNmhdr->code == NM_RCLICK )
        {
            LPITEMIDLIST* ppIdl = NULL;
            int nSelCount = ListView_GetSelectedCount(m_hWnd);
            if ( nSelCount > 0 )
            {
                ppIdl = (LPITEMIDLIST*)_Module.m_Allocator.Alloc( sizeof(LPITEMIDLIST)*nSelCount );
                int iIndex = ListView_GetNextItem( m_hWnd, -1, LVIS_SELECTED );
                LV_ITEM item = {0};
                item.iSubItem = 0;
                item.mask = LVIF_PARAM;
                int _i = 0;
                while( iIndex >= 0 
                    && _i < nSelCount )
                {    
                    item.iItem = iIndex;
                    ListView_GetItem( m_hWnd, &item );
                    ppIdl[_i] = m_pidlManager.Copy( ITEMLISTPTR(item.lParam) );
                    ++_i;
                    iIndex = ListView_GetNextItem( m_hWnd, -1, LVIS_SELECTED );
                }
            }
            LPCONTEXTMENU pMenu = NULL;
            HRESULT hr = E_FAIL;
            hr = m_pFolder->GetUIObjectOf( m_hWnd, nSelCount, \
                (LPCITEMIDLIST*)ppIdl, IID_IContextMenu, NULL, (void**)&pMenu );
            if ( FAILED(hr)
                || !pMenu)
            {
                LOG(_T("IID_IContextMenu Failed"));
                if ( nSelCount > 0 )
                {
                    for ( int _i = 0; _i < nSelCount; ++_i )
                    {
                        m_pidlManager.Delete( ppIdl[_i] );
                    }
                    _Module.m_Allocator.Free( ppIdl );
                }
                return S_OK;
            }
            HMENU hMenu = ::CreatePopupMenu();
            if ( !hMenu )
            {
                if ( nSelCount > 0 )
                {
                    for ( int _i = 0; _i < nSelCount; ++_i )
                    {
                        m_pidlManager.Delete( ppIdl[_i] );
                    }
                    _Module.m_Allocator.Free( ppIdl );
                }
                return S_OK;
            }
            hr = pMenu->QueryContextMenu( hMenu, 0, 10, 32767, CMF_NORMAL | CMF_EXPLORE );
            if ( FAILED(hr) )
            {
                if ( nSelCount > 0 )
                {
                    for ( int _i = 0; _i < nSelCount; ++_i )
                    {
                        m_pidlManager.Delete( ppIdl[_i] );
                    }
                    _Module.m_Allocator.Free( ppIdl );
                }
                DestroyMenu( hMenu );
                return hr;
            }
            POINT point = {0};
            GetCursorPos( &point );
            UINT command = (UINT)TrackPopupMenu ( hMenu, TPM_RETURNCMD | TPM_LEFTALIGN, point.x, point.y, 0, m_hWnd, NULL );
            CMINVOKECOMMANDINFO cmi = {0};
            cmi.cbSize = sizeof (CMINVOKECOMMANDINFO);
            cmi.lpVerb = (LPSTR) MAKEINTRESOURCE (command-10);
            cmi.nShow = SW_SHOWNORMAL;
            pMenu->InvokeCommand( &cmi );
            pMenu->Release();
            if ( nSelCount > 0 )
            {
                for ( int _i = 0; _i < nSelCount; ++_i )
                {
                    m_pidlManager.Delete( ppIdl[_i] );
                }
                _Module.m_Allocator.Free( ppIdl );
            }
            DestroyMenu( hMenu );
        }
    }
    TRACE_RETURN S_OK;
}