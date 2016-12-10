// IShellFolderUse.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <Shobjidl.h>
#include <ShlObj.h>


namespace enum_folder_item
{
    void StrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, LPSTR pszBuf)
    {
        lstrcpyA(pszBuf, "");
        switch(pStr->uType)
        {
        case STRRET_WSTR:
            {
                WideCharToMultiByte(CP_ACP, 0, pStr->pOleStr, -1, pszBuf,
                    MAX_PATH, nullptr, nullptr);
                break;
            }
        case STRRET_OFFSET:
            {
                lstrcpyA(pszBuf, reinterpret_cast<char*>(pidl) + pStr->uOffset);
                break;
            }
        case STRRET_CSTR:
            {
                lstrcpyA(pszBuf, pStr->cStr);
                break;
            }

        }


    }

    typedef BOOL (CALLBACK*FOLDERCONTENTPROC)(LPCSTR, HICON, DWORD);

    int SHEnumFolderContent(LPSHELLFOLDER pFolder, FOLDERCONTENTPROC pfn,
        DWORD dwData, LPITEMIDLIST* ppidl)
    {
        int iNumberOfItems = 0;
        // 枚举内容
        LPENUMIDLIST pEnumIDList = nullptr;
        pFolder->EnumObjects(nullptr, SHCONTF_FOLDERS|SHCONTF_NONFOLDERS, 
            &pEnumIDList);
        ULONG ulFetched = 0;
        LPITEMIDLIST pItem = nullptr;
        while (NOERROR == pEnumIDList->Next(1, &pItem, &ulFetched))
        {
            do
            {
                STRRET sName;
                TCHAR szBuf[MAX_PATH] = {0};
                HRESULT hr = pFolder->GetDisplayNameOf(pItem, 0, &sName);
                if (!SUCCEEDED(hr))
                {
                    break;
                }
                StrretToString(pItem, &sName, szBuf);
                // 唤醒回调函数
                if (pfn)
                {
                    // 获取图标
                    UINT u = 0;
                    int iIconIndex = 0;
                    HICON hIcon = nullptr;
                    HICON hIconSm = nullptr;
                    TCHAR szIconFile[MAX_PATH] = {0};
                    LPEXTRACTICON pExtractIcon = nullptr;
                    pFolder->GetUIObjectOf(nullptr, 1,
                        const_cast<LPCITEMIDLIST*>(&pItem), IID_IExtractIcon, 
                        nullptr, reinterpret_cast<void**>(&pExtractIcon));
                    if (!pExtractIcon)
                    {
                        break;
                    }
                    pExtractIcon->GetIconLocation(0, szIconFile, MAX_PATH, 
                        &iIconIndex, &u);
                    pExtractIcon->Extract(szIconFile, iIconIndex, &hIcon,
                        &hIconSm, MAKELONG(32, 16));
                    pExtractIcon->Release();
                    if (hIcon == nullptr)
                    {
                        ExtractIconEx(szIconFile, iIconIndex, &hIcon, nullptr, 1);
                    }
                    if (!pfn(szBuf, hIcon, dwData))
                    {
                        // 返回当前的PIDL
                        if (ppidl != nullptr)
                        {
                            *ppidl = pItem;
                            break;
                        }
                    }
                }
            }while(0);

            if (ppidl && *ppidl)
            {
                break;
            }
            ++iNumberOfItems;
        }
        return iNumberOfItems;
    }

    HRESULT SHPathToPidlEx(
        LPCTSTR szPath, LPITEMIDLIST* ppidl, LPSHELLFOLDER pFolder)
    {
        OLECHAR wszPath[MAX_PATH] = {0};
        ULONG nCharsParsed = 0;
        LPSHELLFOLDER pShellFolder = NULL;
        BOOL bFreeOnExit = FALSE;
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szPath, -1, wszPath, MAX_PATH);
        // 默认使用桌面的IShellFolder
        if(pFolder == NULL)
        {
            SHGetDesktopFolder(&pShellFolder);
            bFreeOnExit = TRUE;
        }
        else
            pShellFolder = pFolder;
        HRESULT hr = pShellFolder->ParseDisplayName(
            NULL, NULL, wszPath, &nCharsParsed, ppidl, NULL);
        if(bFreeOnExit)
            pShellFolder->Release();
        return hr;
    }

    BOOL CALLBACK SearchText(LPCSTR pszItem, HICON hIcon, DWORD dwData)
    {
        return static_cast<BOOL>((lstrcmpi(pszItem, 
            reinterpret_cast<char*>(dwData))));
    }

    void FindInDesktop(IShellFolder** shellFolder, const TCHAR* findName,
        LPITEMIDLIST* pidl)
    {
        LPMALLOC malloc = nullptr;
        SHGetMalloc(&malloc);
        SHGetDesktopFolder(shellFolder);
        int iNumOfItems = SHEnumFolderContent(*shellFolder, nullptr, 0, nullptr);
        int rc = SHEnumFolderContent(*shellFolder, SearchText,
            reinterpret_cast<DWORD>(findName), pidl);
    }

    void FindInComp(IShellFolder** shellFolder, const TCHAR* findName,
        LPITEMIDLIST* pidl)
    {
        LPITEMIDLIST pidlMyComp = nullptr;
        SHGetSpecialFolderLocation(nullptr, CSIDL_DRIVES, &pidlMyComp);
        IShellFolder* shellFolderSub = nullptr;
        *shellFolder = nullptr;
        SHGetDesktopFolder(shellFolder);
        (*shellFolder)->BindToObject(pidlMyComp, nullptr,
            IID_IShellFolder, reinterpret_cast<void**>(&shellFolderSub));
        (*shellFolder)->Release();
        LPMALLOC malloc = nullptr;
        SHGetMalloc(&malloc);
        malloc->Free(pidlMyComp);
        (*shellFolder) = shellFolderSub;
        int iNumOfItems = SHEnumFolderContent((*shellFolder), nullptr, 0, nullptr);
        int rc = SHEnumFolderContent((*shellFolder), SearchText, 
            reinterpret_cast<DWORD>(findName), pidl);
        if (rc == iNumOfItems)
        {
            // 做最后的努力， 他是一个路径名
            HRESULT hr = SHPathToPidlEx(findName, pidl, (*shellFolder));
        }

    }

    void EnumDesktopDir()
    {
        LPITEMIDLIST pidlMyComp = nullptr;
        SHGetSpecialFolderLocation(nullptr, CSIDL_DESKTOPDIRECTORY, &pidlMyComp);
        IShellFolder* shellFolderSub = nullptr;
        IShellFolder* shellFolder = nullptr;
        SHGetDesktopFolder(&shellFolder);
        (shellFolder)->BindToObject(pidlMyComp, nullptr,
            IID_IShellFolder, reinterpret_cast<void**>(&shellFolderSub));
        (shellFolder)->Release();
        LPMALLOC malloc = nullptr;
        SHGetMalloc(&malloc);
        malloc->Free(pidlMyComp);
        (shellFolder) = shellFolderSub;
        int iNumOfItems = SHEnumFolderContent((shellFolder), nullptr, 0, nullptr);
        const TCHAR* findName = "酷狗音乐";
        LPITEMIDLIST pidl = nullptr;
        int rc = SHEnumFolderContent((shellFolder), SearchText, 
            reinterpret_cast<DWORD>(findName), &pidl);
        shellFolderSub->Release();
    }

    void EnumFolderItem()
    {
        IShellFolder* shellFolder = nullptr;
        const TCHAR* findName = _T("酷狗音乐");
        LPITEMIDLIST pidl = nullptr;
        FindInDesktop(&shellFolder, findName, &pidl);
        FindInComp(&shellFolder, findName, &pidl);
        EnumDesktopDir();
    }

}

int _tmain(int argc, _TCHAR* argv[])
{
    enum_folder_item::EnumFolderItem();
	return 0;
}

