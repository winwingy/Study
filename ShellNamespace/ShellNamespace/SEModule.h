#pragma once
#ifndef __SEMODULE_H__
#define __SEMODULE_H__
#include <commctrl.h>
#include <assert.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "comctl32.lib")

class CMalloc
{
public:
    LPVOID Alloc(ULONG cb)
    {
        return CoTaskMemAlloc(cb);
    }
    void Free(LPVOID p)
    {
        CoTaskMemFree( p );
    }
};

class CSEImageList
{
public:

    HIMAGELIST m_hImageListSmall;
    HIMAGELIST m_hImageListLarge;

    CSEImageList() : m_hImageListSmall(NULL), m_hImageListLarge(NULL)
    {

    }
    bool Init(  )
    {
        // 创建小图标列表
        int nSmallCx = ::GetSystemMetrics(SM_CXSMICON);
        int nSmallCy = ::GetSystemMetrics(SM_CYSMICON);
        m_hImageListSmall = ImageList_Create(nSmallCx, nSmallCy, ILC_COLOR32 | ILC_MASK, 4, 0);

        // 创建大图标列表
        int nLargeCx = ::GetSystemMetrics(SM_CXICON);
        int nLargeCy = ::GetSystemMetrics(SM_CYICON);
        m_hImageListLarge = ImageList_Create(nLargeCx, nLargeCy, ILC_COLOR32 | ILC_MASK, 4, 0);
        return m_hImageListLarge&&m_hImageListSmall;
    }

    int AddIcon( HICON hSmall, HICON hLarge )
    {
        if( !hSmall 
            || !hLarge )
        {
            return -1;
        }
        int nIndexSmall = ImageList_AddIcon( m_hImageListSmall, hSmall );
        int nIndexLarge = ImageList_AddIcon( m_hImageListSmall, hLarge );
        assert( nIndexLarge == nIndexSmall );
        return nIndexSmall;
    }

    virtual ~CSEImageList()
    {
        if( m_hImageListSmall ) 
            ImageList_Destroy(m_hImageListSmall);
        if( m_hImageListLarge ) 
            ImageList_Destroy(m_hImageListLarge);
    }
};

class CSEModule: public CComModule
{
public:
    HRESULT Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE	h, const GUID* plibid =	NULL);

public:
    //内存分配
    CMalloc      m_Allocator;
    //图标列表
    CSEImageList m_ImageLists;
    LPITEMIDLIST m_pidlROOT;
    TCHAR m_szInstallPath[MAX_PATH];
    WORD	m_wWinMajor;
    WORD	m_wWinMinor;

    CLIPFORMAT   m_CFSTR_DRAGDROP;
#ifndef	_NO_CLIPFORMATS
    // Clipboard	formats
    CLIPFORMAT m_CFSTR_SHELLIDLIST;
    CLIPFORMAT m_CFSTR_FILECONTENTS;
    CLIPFORMAT m_CFSTR_PASTESUCCEEDED;
    CLIPFORMAT m_CFSTR_FILEDESCRIPTOR;
    CLIPFORMAT m_CFSTR_PERFORMEDDROPEFFECT;
    CLIPFORMAT m_CFSTR_PREFERREDDROPEFFECT;
    CLIPFORMAT m_CFSTR_LOGICALPERFORMEDDROPEFFECT;
    CLIPFORMAT m_CFSTR_OLECLIPBOARDPERSISTONFLUSH;
#if	(_WIN32_WINNT >= 0x0500)
    CLIPFORMAT m_CFSTR_TARGETCLSID;
#endif // _WIN32_WINNT
#endif // _NO_CLIPFORMATS
};

#endif