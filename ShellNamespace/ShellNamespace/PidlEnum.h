#ifndef __PIDL_ENUM_H__
#define __PIDL_ENUM_H__
#include "PidlManager.h"
typedef struct _tag_enum_list
{
    struct _tag_enum_list* pNext;
    LPITEMIDLIST           pidl;
}ENUMLIST, FAR* LPENUMLIST;

class CPidlEnum
    :public IEnumIDList
    ,public CComObjectRootEx<CComMultiThreadModel>
{
public:
    DECLARE_PROTECT_FINAL_CONSTRUCT()

    BEGIN_COM_MAP(CPidlEnum)
        COM_INTERFACE_ENTRY_IID(IID_IEnumIDList,IEnumIDList)
    END_COM_MAP()
public:
    HRESULT FinalConstruct();
    void FinalRelease();

public:
    //IEnumIDList
    STDMETHOD (Next) (DWORD, LPITEMIDLIST*, LPDWORD);
    STDMETHOD (Skip) (DWORD);
    STDMETHOD (Reset) (void);
    STDMETHOD (Clone) (LPENUMIDLIST*);

    //User Defined Funcs
    BOOL Delete(void);
    BOOL AddPidl(LPITEMIDLIST pidl);
    HRESULT _Init(LPCITEMIDLIST pidlRoot,DWORD dwFlags);
    int GetAllItemCount(){return m_nFileItemCount+m_nFolderItemCount;}
    int GetFileCount(){return m_nFileItemCount;}
    int GetFolderCount(){return m_nFolderItemCount;}
    int GetHideItemCount( ){return m_nHideItemCount; }
    bool IsIncludeHide(  ){return m_bIncludeHide;}
protected:
    bool EnumDir( const TCHAR* pszPath, DWORD dwFlags );

private:
    LPENUMLIST m_pFirst;
    LPENUMLIST m_pLast;
    LPENUMLIST m_pCurrent; 

    CPidlManager m_pidlManager;
    int m_nHideItemCount;
    int m_nFileItemCount;
    int m_nFolderItemCount;
    bool m_bIncludeHide;
};
#endif