#include "StdAfx.h"
#include ".\pidlmanager.h"
#include "Log.h"
bool CPidlManager::ReplaceRoot( TCHAR* pszDir )
{
    if ( pszDir )
    {
        TCHAR szDir[MAX_PATH] = {0};
        memcpy( szDir, g_szRealRoot, MAX_PATH*sizeof(TCHAR) );
        LPTSTR pszTmp = StrChrI( pszDir, _T('\\') );
        if ( pszTmp )
        {
            ++pszTmp;
            LPTSTR pRootEnd = &szDir[_tcslen(szDir)];
            memcpy( pRootEnd, pszTmp, (_tcslen(pszTmp)+1)*sizeof(TCHAR) );
            memcpy( pszDir, szDir, MAX_PATH*sizeof(TCHAR) );
            *pRootEnd = 0;
            return true;
        }
        else
        {
            memcpy( pszDir, g_szRealRoot, MAX_PATH*sizeof(TCHAR) );
        }
    }
    return false; 
}
LPITEMIDLIST CPidlManager::Create( ITEM_TYPE type, const TCHAR* pszName\
                                  , const TCHAR* pszRealName, ITEM_STATE state /*= ITEM_STATE::LOCATE */ )
{
    DWORD dwNameLen = (DWORD)_tcslen(pszName)+1;
    DWORD dwRealNameLen = 0;
    USHORT usAllocsize = (USHORT)(sizeof(ITEMIDLIST) + sizeof( PIDLDATA ) + (dwNameLen-1)*sizeof(TCHAR)-1);
    if ( pszRealName )
    {
        dwRealNameLen = (DWORD)_tcslen(pszName)+1;
        usAllocsize += (USHORT)dwRealNameLen*sizeof(TCHAR);
    }

    // 分配一个PIDL, 以空ITEMIDLIST结束.
    LPITEMIDLIST pidlNew = NULL;
    pidlNew = (LPITEMIDLIST) _Module.m_Allocator.Alloc(usAllocsize + sizeof(ITEMIDLIST) );
    if (pidlNew)
    {
        ::ZeroMemory(pidlNew,usAllocsize + sizeof(ITEMIDLIST));

        //向ITEMID填充数据
        LPITEMIDLIST pidlTemp = pidlNew;
        pidlTemp->mkid.cb = (USHORT)usAllocsize;

        CPidlManager::LPPIDLDATA     pData = NULL;
        pData = GetDataPointer(pidlTemp);
        pData->type = type;
        ::CopyMemory(pData->szName, pszName, dwNameLen * sizeof(TCHAR));
        if ( pszRealName )
        {
            ::CopyMemory(((TCHAR*)pData->szName)+dwNameLen+1, \
                pszRealName, dwRealNameLen * sizeof(TCHAR));
        }
        pData->state = state;
        pData->dwLength = dwRealNameLen+dwNameLen;

        //设置0结束符
        pidlTemp = GetNextItem(pidlTemp);
        pidlTemp->mkid.cb = 0;
        pidlTemp->mkid.abID[0] = 0;
    }
    return pidlNew;
}

LPITEMIDLIST CPidlManager::GetNextItem( LPITEMIDLIST lpIDL )
{
    if ( !lpIDL )
    {
        return NULL;
    }
    return (LPITEMIDLIST)(LPBYTE)(((LPBYTE)lpIDL)+lpIDL->mkid.cb);
}

CPidlManager::LPPIDLDATA CPidlManager::GetDataPointer( const LPCITEMIDLIST lpIDL )
{
    if ( !lpIDL )
    {
        return NULL;
    }
    return (LPPIDLDATA)lpIDL->mkid.abID;
}


//==================================================================
LPITEMIDLIST CPidlManager::Concatenate(LPCITEMIDLIST pidlHead, LPCITEMIDLIST pidlTail)
{
    LPITEMIDLIST   pidlNew;
    UINT    cb1 = 0;
    UINT    cb2 = 0;

    //都是空的直接返回
    if(!pidlHead && !pidlTail)
    {
        return NULL;
    }
    //pidlHead为空,返回pidlTail的拷贝
    if(!pidlHead)
    {
        pidlNew = Copy(pidlTail);
        return pidlNew;
    }
    //pidlTail为空,返回pidlHead的拷贝
    if(!pidlTail)
    {
        pidlNew = Copy(pidlHead);
        return pidlNew;
    }
    cb1 = GetByteSize(pidlHead) - sizeof(ITEMIDLIST);//删除一个空节点
    cb2 = GetByteSize(pidlTail);
    //为新链表分配空间
    pidlNew = (LPITEMIDLIST)_Module.m_Allocator.Alloc(cb1 + cb2);
    if(pidlNew)
    {
        ::ZeroMemory(pidlNew,cb1+cb2);
        //拷贝pidlHead
        ::CopyMemory(pidlNew, pidlHead, cb1);
        //拷贝pidlTail
        ::CopyMemory(((LPBYTE)pidlNew) + cb1, pidlTail, cb2);
    }
    return pidlNew;
}

void CPidlManager::Delete(LPITEMIDLIST pidl)
{
    if (pidl)
    {
        _Module.m_Allocator.Free(pidl);
        pidl = NULL;
    }
}

LPITEMIDLIST CPidlManager::GetLastItem(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST pidlLast = NULL;
    LPITEMIDLIST pidIter = (LPITEMIDLIST)pidl;
    if(pidIter)
    {
        while ( pidIter->mkid.cb)
        {
            pidlLast = pidIter;
            pidIter = GetNextItem(pidIter);
        }
        return pidlLast;
    }
    return NULL;
}

LPITEMIDLIST CPidlManager::Copy(LPCITEMIDLIST pidlSrc)
{
    LPITEMIDLIST pidlTarget = NULL;
    UINT usize = 0;
    if (!pidlSrc)
    {
        return NULL;
    }
    usize = GetByteSize(pidlSrc);
    pidlTarget = (LPITEMIDLIST) _Module.m_Allocator.Alloc(usize);
    if (pidlTarget == NULL)
    {
        return NULL;
    }
    ::ZeroMemory(pidlTarget,usize);
    ::CopyMemory(pidlTarget, pidlSrc, usize);

    return pidlTarget;
}

UINT CPidlManager::GetByteSize(LPCITEMIDLIST pidl)
{
    UINT usize = 0;
    LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
    if (!pidl)
    {
        return 0;
    }
    while (pidlTemp->mkid.cb != 0)
    {
        usize += pidlTemp->mkid.cb;
        pidlTemp = GetNextItem(pidlTemp);
    }  
    usize += sizeof(ITEMIDLIST);
    return usize;
}

bool CPidlManager::IsSingle(LPCITEMIDLIST pidl)
{
    return GetNextItem((LPITEMIDLIST)pidl)->mkid.cb == 0;
}

bool CPidlManager::HasSubFolder(LPCITEMIDLIST pidl)
{
    TRACE_FUNCTION();
    TCHAR szPath[MAX_PATH]={0};
     DWORD dwLen=MAX_PATH;
     GetFullName(pidl,szPath,&dwLen);
     ReplaceRoot( szPath );
     LOG( _T("%s %s"), __TFUNCTION__, szPath );
     TCHAR szFind[MAX_PATH] = {0};  
     _sntprintf( szFind, MAX_PATH, _T("%s\\*.*"), szPath );
     LOG( _T("%s %s"), __TFUNCTION__, szFind );
     WIN32_FIND_DATA FindFileData;  
     HANDLE hFind=::FindFirstFile(szFind,&FindFileData);  
     if(INVALID_HANDLE_VALUE == hFind)
     {
         LOG( _T("%s  %s not dir"), __TFUNCTION__, szFind );
         return false;  
     }
     LPITEMIDLIST pItem = NULL;
     while(TRUE)  
     {  
         if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
             && FindFileData.cFileName[0] != _T('.'))  
         {  
             LOG( _T("%s  %s has sub dir"), __TFUNCTION__, szFind );
             FindClose(hFind); 
             TRACE_RETURN true;
         }   
         if(!FindNextFile(hFind,&FindFileData))
         {
             break;  
         }
     }  
     FindClose(hFind);  
     LOG( _T("%s  %s no sub dir"), __TFUNCTION__, szFind );
     TRACE_RETURN false;
}

//get the pathname releative to NSF's root folder of folder object use its complex PIDL
HRESULT CPidlManager::GetFullName(LPCITEMIDLIST pidl,LPTSTR szFullName,DWORD *pdwLen)
{
    if(!pdwLen)
    {
        return E_FAIL;
    }
    *pdwLen=0;
    
    if(!pidl) 
    {
        *pdwLen = (DWORD)_tcslen(g_szRoot)+1;
        memcpy( szFullName, g_szRoot, *pdwLen*sizeof(TCHAR) );
        //根目录
        return S_OK;
    }
    memcpy( szFullName, g_szRoot, (_tcslen(g_szRoot)+1)*sizeof(TCHAR) );
    *pdwLen = (DWORD)_tcslen(g_szRoot);
    LPITEMIDLIST pidlTemp = (LPITEMIDLIST) pidl;
    while (pidlTemp->mkid.cb != 0)
    {
        const TCHAR* szTemp = GetItemName( pidlTemp );
        if( szFullName)
        {
            if(0 == *pdwLen)
            {
                _tcscpy(szFullName,szTemp);
            }
            else
            {
                _tcscat(szFullName,_TEXT("\\"));
                _tcscat(szFullName,szTemp);
            }
            *pdwLen =(DWORD)_tcslen(szFullName);
        }
        else
        {
            *pdwLen+=(DWORD)_tcslen(szTemp);
        }
        pidlTemp = GetNextItem(pidlTemp);
    }  
    *pdwLen += 1;
    return S_OK;
}


// HRESULT CPidlManager::GetItemAttributes(LPCITEMIDLIST pidl,USHORT iAttrNum,LPTSTR pszAttrOut)
// {
//     TCHAR szTemp[MAX_PATH]=TEXT("");
// 
//     switch (iAttrNum)
//     {
//     case ATTR_TYPE :
//         {
//             LPITEMIDLIST pidlTemp;
//             pidlTemp=GetLastItem(pidl);
// 
//             if( NWS_FOLDER == GetItemType(pidlTemp))
//             {
//                 _tcscpy(pszAttrOut,_TEXT("Folder"));
//             }
//             else
//             {
//                 GetItemName(pidlTemp,szTemp);
// 
//                 SHFILEINFO sfi;  
//                 ZeroMemory(&sfi,sizeof(sfi));
// 
//                 if (SHGetFileInfo(szTemp, 0, &sfi, sizeof(sfi),  \
//                     SHGFI_USEFILEATTRIBUTES | SHGFI_TYPENAME ) ) 
//                     _tcscpy(pszAttrOut,sfi.szTypeName);
//                 else
//                 {
//                     _tcscpy(pszAttrOut,_TEXT("File"));
//                 }
//             }
//         }
//         break;
// 
//     default :
//         return E_INVALIDARG;
//     }
//     return S_OK;
// }

CPidlManager::ITEM_TYPE CPidlManager::GetItemType(LPCITEMIDLIST pidl)
{
    LPITEMIDLIST  pidlTemp = GetLastItem(pidl);
    if ( pidlTemp )
    {
        CPidlManager::LPPIDLDATA pData;
        pData = GetDataPointer(pidlTemp);
        return pData->type;
    }
    return CPidlManager::FOLDER;
}

CPidlManager::ITEM_STATE CPidlManager::GetItemState( LPCITEMIDLIST pidl )
{
    LPITEMIDLIST  pidlTemp = GetLastItem(pidl);
    if ( pidlTemp )
    {
        CPidlManager::LPPIDLDATA pData;
        pData = GetDataPointer(pidlTemp);
        return pData->state;
    }
    return CPidlManager::LOCATE;
}

const TCHAR* CPidlManager::GetItemName( LPCITEMIDLIST pidl )
{
    CPidlManager::LPPIDLDATA pData;
    pData = GetDataPointer(pidl);
    return GetPidlDataName( pData );
}

DWORD CPidlManager::GetPidlDataLen( const CPidlManager::LPPIDLDATA lpPidl )
{
    if ( !lpPidl )
    {
        return 0;
    }
    return (sizeof( PIDLDATA )+lpPidl->dwLength\
        -(lpPidl->dwLength?1:0));
}

const TCHAR* CPidlManager::GetPidlDataName( const CPidlManager::LPPIDLDATA lpPidl )
{
    if ( !lpPidl )
    {
        return NULL;
    }
    if ( lpPidl->dwLength > 0 )
    {
        return (TCHAR*)lpPidl->szName;
    }
    return NULL;
}

const TCHAR* CPidlManager::GetPidlDataRealPath( const CPidlManager::LPPIDLDATA lpPidl )
{
    if ( !lpPidl )
    {
        return NULL;
    }
    if ( lpPidl->dwLength > 0
        && lpPidl->dwLength > _tcslen((TCHAR*)lpPidl->szName)+1 )
    {
        return ((TCHAR*)lpPidl->szName)+_tcslen((TCHAR*)lpPidl->szName)+2;
    }
    return NULL;
}

const TCHAR* CPidlManager::GetRealPath( LPCITEMIDLIST pidl )
{
    CPidlManager::LPPIDLDATA pData;
    pData = GetDataPointer(pidl);
    return GetPidlDataRealPath( pData );
}

const TCHAR* CPidlManager::GetRootDir()
{
    return g_szRealRoot;
}

bool CPidlManager::IsValidDir( LPCTSTR lpszDir )
{
    if ( !_tcsnicmp(lpszDir, g_szRealRoot, _tcslen(g_szRealRoot)-1)
        || !_tcsnicmp(lpszDir, g_szRoot, _tcslen(g_szRoot) )
        )
    {
        return true;
    }
    return false;
}

UINT CPidlManager::CountItem( LPITEMIDLIST lpIDL )
{
    if ( !lpIDL )
    {
        return 0;
    }
    UINT uCount = 0;
    LPITEMIDLIST pidlTemp = lpIDL;
    while (pidlTemp->mkid.cb != 0)
    {
        ++uCount;
        pidlTemp = GetNextItem(pidlTemp);
    }  
    return uCount;
}