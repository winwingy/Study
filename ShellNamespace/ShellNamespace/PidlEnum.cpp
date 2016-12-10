#include "StdAfx.h"
#include ".\pidlenum.h"
#include "Log.h"

HRESULT CPidlEnum::FinalConstruct()
{
    TRACE_FUNCTION_EX();
    m_nHideItemCount = m_nFileItemCount = m_nHideItemCount = 0;
    m_pFirst = m_pLast = m_pCurrent = NULL;
    TRACE_RETURN S_OK;
}

void CPidlEnum::FinalRelease()
{
    TRACE_FUNCTION_EX();
    Delete();
    m_pFirst = m_pLast = m_pCurrent = NULL;
    m_nHideItemCount = m_nFileItemCount = m_nHideItemCount = 0;
    TRACE_RETURN; 
}

BOOL CPidlEnum::Delete(void)
{
    TRACE_FUNCTION_EX();
    //释放idls
    LPENUMLIST  pDelete;
    while(m_pFirst)
    {
        pDelete = m_pFirst;
        m_pFirst = pDelete->pNext;
        m_pidlManager.Delete(pDelete->pidl);
        _Module.m_Allocator.Free(pDelete);
    }
    m_pFirst = m_pLast = m_pCurrent = NULL;
    TRACE_RETURN TRUE;
}

BOOL CPidlEnum::AddPidl(LPITEMIDLIST pidl)
{
    if(pidl)
    {
        //TRACE_FUNCTION_EX();
        LPENUMLIST  pNew;
        pNew = (LPENUMLIST)_Module.m_Allocator.Alloc(sizeof(ENUMLIST));
        if ( pNew )
        {
            //set the next pointer
            pNew->pNext = NULL;
            pNew->pidl = pidl;
            //is this the first item in the list?
            if(!m_pFirst)
            {
                m_pFirst = pNew;
                m_pCurrent = m_pFirst;
            }
            if(m_pLast)
            {
                //add the new item to the end of the list
                m_pLast->pNext = pNew;
            }
            //update the last item pointer
            m_pLast = pNew;
            return TRUE;
        }
    }
    return FALSE;
}


HRESULT CPidlEnum::_Init(LPCITEMIDLIST pidlRoot,DWORD dwFlags)
{
    LOG( _T("%s pidlRoot = %p"), __TFUNCTION__, pidlRoot );
    TRACE_FUNCTION_EX();
    Delete();
    //生成枚举链表
    TCHAR szPath[MAX_PATH] = {0};
    DWORD dwLen = MAX_PATH;
    m_pidlManager.GetFullName( pidlRoot, szPath, &dwLen );
    LOG( _T("%s pidlPath = %s"), __TFUNCTION__, szPath );
    m_pidlManager.ReplaceRoot( szPath );
    LOG( szPath );
    m_bIncludeHide = (0 != (dwFlags & SHCONTF_INCLUDEHIDDEN));
    m_nFolderItemCount = m_nFileItemCount = m_nHideItemCount = 0;
    EnumDir( szPath, dwFlags );
    Reset();
    TRACE_RETURN S_OK;
}

HRESULT CPidlEnum::Next(DWORD dwElements, LPITEMIDLIST apidl[], LPDWORD pdwFetched)
{ 
    TRACE_FUNCTION_EX();
    DWORD    dwIndex;
    HRESULT  hr = S_OK;
    if(dwElements > 1 && !pdwFetched)
    {
        LOG(_T("%s E_INVALIDARG %d, %p"), __TFUNCTION__, dwElements, pdwFetched );
        TRACE_RETURN E_INVALIDARG;
    }

    for(dwIndex = 0; dwIndex < dwElements; dwIndex++)
    {
        //is this the last item in the list?
        if(!m_pCurrent)
        {
            LOG(_T("%s Error m_pCurrent is NULL"), __TFUNCTION__ );
            hr = S_FALSE;
            break;
        }
        apidl[dwIndex] = m_pidlManager.Copy(m_pCurrent->pidl);
        m_pCurrent = m_pCurrent->pNext;
    }
    if(pdwFetched)
    {
        *pdwFetched = dwIndex;
    }
    TRACE_RETURN hr;
}    

HRESULT CPidlEnum::Reset(void)
{ 
    m_pCurrent = m_pFirst;
    TRACE_RETURN S_OK; 
}

HRESULT CPidlEnum::Skip(DWORD dwSkip)
{
    TRACE_FUNCTION_EX();
    DWORD    dwIndex;
    HRESULT  hr = S_OK;
    for(dwIndex = 0; dwIndex < dwSkip; dwIndex++)
    {
        //is this the last item in the list?
        if(!m_pCurrent)
        {
            hr = S_FALSE;
            break;
        }
        m_pCurrent = m_pCurrent->pNext;
    }
    TRACE_RETURN hr;
}

HRESULT CPidlEnum::Clone(LPENUMIDLIST *ppEnum)
{
    TRACE_FUNCTION_EX();
    if ( !ppEnum )
    {
        return E_POINTER;
    }
    HRESULT hr = S_OK;
    *ppEnum = NULL;
    CComObject<CPidlEnum>* pEnum = NULL;
    hr = CComObject<CPidlEnum>::CreateInstance( &pEnum );
    if ( FAILED(hr) )
    {
        return hr;
    }
    if ( pEnum )
    {
        LPENUMLIST  pIter = m_pFirst;
        while(pIter)
        {
            pEnum->AddPidl( m_pidlManager.Copy( pIter->pidl ) );
            pIter = pIter->pNext;
        }
        hr = pEnum->QueryInterface( IID_IEnumIDList, (void**)ppEnum );
        pEnum->Release();
    }
    return hr;
}

bool CPidlEnum::EnumDir( const TCHAR* pszPath, DWORD dwFlags )
{
    TRACE_FUNCTION_EX();
    LOG( _T("%s path:%s, %08X"), __TFUNCTION__, pszPath, dwFlags );
    TCHAR szFind[MAX_PATH] = {0};  
    _sntprintf( szFind, MAX_PATH, _T("%s\\*.*"), pszPath );
    WIN32_FIND_DATA FindFileData;  
    HANDLE hFind=::FindFirstFile(szFind,&FindFileData);  
    if(INVALID_HANDLE_VALUE == hFind)
    {
        LOG(_T("%s Path not found"), __TFUNCTION__);
        TRACE_RETURN false;  
    }
    LPITEMIDLIST pItem = NULL;
    while(TRUE)  
    {  
        if ( FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN )
        {
            ++m_nHideItemCount;
        }
        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
        {  
            if ( FindFileData.cFileName[0] != _T('.')
                && (SHCONTF_FOLDERS&dwFlags)
                && (( dwFlags & SHCONTF_INCLUDEHIDDEN )
                || !(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN) )
                )
            {
                ++m_nFolderItemCount;
                //目录
                pItem = m_pidlManager.Create( CPidlManager::FOLDER, FindFileData.cFileName, NULL );
                AddPidl( pItem );
                LOG(m_pidlManager.GetItemName( pItem ));
            }
        }  
        else  
        {  
            if ((SHCONTF_NONFOLDERS&dwFlags)
                &&(( dwFlags & SHCONTF_INCLUDEHIDDEN )
                || !(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)) )
            {
                ++m_nFileItemCount;
                //文件
                pItem = m_pidlManager.Create( CPidlManager::FILE, FindFileData.cFileName, NULL );
                AddPidl( pItem );
            }
        }  
        if(!FindNextFile(hFind,&FindFileData))
        {
            break;  
        }
    }  
    m_nFolderItemCount -= 2;
    m_nFolderItemCount = max( 0, m_nFolderItemCount );
    FindClose(hFind);  
    TRACE_RETURN true;
}