#include "StdAfx.h"
#include <WindowsX.h>
#include ".\seshellview.h"
#include "PidlEnum.h"
#include "Log.h"
HRESULT CSEShellView::FinalConstruct()
{
    TRACE_FUNCTION();

    m_pShellBrowser=NULL;
    m_hWnd=NULL;   

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
    if ( m_pShellBrowser )
    {
        //m_pShellBrowser->Release();
        m_pShellBrowser=NULL;
    }   
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
    *phWnd = ::CreateWindowEx(WS_EX_CONTROLPARENT,
        _T("SE_VIEW"),
        NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP,
        prcView->left,
        prcView->top,
        prcView->right - prcView->left,
        prcView->bottom - prcView->top,
        hwndShell,
        NULL,
        _Module.GetModuleInstance(),
        (LPVOID) this);
    if( *phWnd == NULL ) 
    {
        TRACE_RETURN HRESULT_FROM_WIN32(::GetLastError());
    }
    m_pShellBrowser->AddRef();
    
    InitTooBar();
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

        int nPos = _sntprintf( pszInfo, 128, _T("%d 个对象"), pEnumInstance->GetAllItemCount() );
        if ( pEnumInstance->GetHideItemCount() > 0
            && !pEnumInstance->IsIncludeHide() )
        {
            _sntprintf( pszInfo+nPos, 128-nPos, _T("(加 %d 个隐藏对象)"), pEnumInstance->GetHideItemCount() );   
        }
        m_pShellBrowser->SendControlMsg(FCW_STATUS, SB_SETTEXT, 0, (LPARAM)szInfo, &lResult);
    }
    pEnumIDList->Release();
    TRACE_RETURN S_OK;
}

HRESULT CSEShellView::SaveViewState(void)
{
    ATLASSERT(m_pShellBrowser); 
    CComPtr<IStream> spStream;

    if( FAILED( m_pShellBrowser->GetViewStateStream(STGM_WRITE, &spStream) ) ) 
        TRACE_RETURN S_OK;
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
    Refresh();
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
    CSEShellView *pThis = (CSEShellView *)(LONG_PTR)::GetWindowLong(hWnd, GWL_USERDATA);
    if ( pThis )
    {
        if( uMessage == WM_NCCREATE ) 
        {
            LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
            pThis = (CSEShellView *) lpcs->lpCreateParams;
            ::SetWindowLong(hWnd, GWL_USERDATA, (LONG)(LONG_PTR)pThis);
            pThis->m_hWnd = hWnd;
            return 1;
        }
        else if ( WM_COMMAND == uMessage )
        {
            return pThis->OnCommand(GET_WM_COMMAND_ID(wParam, lParam),
                GET_WM_COMMAND_CMD(wParam, lParam),
                GET_WM_COMMAND_HWND(wParam, lParam));
        }
    }
    return DefWindowProc( hWnd, uMessage, wParam, lParam );
}

LRESULT CSEShellView::OnCommand( DWORD dwCmdID, DWORD dwCmd, HWND hwndCmd )
{
    return 0;
}

void CSEShellView::InitTooBar()
{
    return;
    assert(m_pShellBrowser);
    if ( m_pShellBrowser )
    {
        TBBUTTON btnArray[2] = {0};
        btnArray[0].dwData = 0;
        btnArray[0].fsStyle = BTNS_BUTTON;
        btnArray[0].idCommand = 1000;
        btnArray[0].fsState = TBSTATE_ENABLED;
        btnArray[0].iString = (INT_PTR)_T("Button1");
        btnArray[1] = btnArray[0];
        btnArray[1].idCommand = 1001;
        btnArray[1].iString = (INT_PTR)_T("Button2");
        m_pShellBrowser->SetToolbarItems(btnArray, 2,  FCT_MERGE );
    }
}