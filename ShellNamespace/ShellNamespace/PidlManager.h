#ifndef __PIDL_MANAGER_H__
#define __PIDL_MANAGER_H__
class CPidlManager
{
public:
    typedef enum _enum_item_type
    {
        FOLDER,
        FILE,
    }ITEM_TYPE;

    typedef enum _enum_item_state
    {
        LOCATE,         //本地正常文件(非操作中状态)
        CONFLICT,       //文件有冲突
        REMOTE,         //远程
        COMITING,       //上传中
        DOWNLOADING,    //下载中
        PAUSEDOWNLOAD,  //暂停下载
        PAUSECOMIT,     //暂停上传
    }ITEM_STATE;

    typedef struct _tag_pidl_data
    {
        ITEM_TYPE  type;
        ITEM_STATE state;
        DWORD dwLength;
        TCHAR szName[1]; //Name\0RealPath\0
    }PIDLDATA, FAR* LPPIDLDATA;

public:
    LPITEMIDLIST Create( ITEM_TYPE type, const TCHAR* pszName\
        , const TCHAR* pszRealName, ITEM_STATE state = LOCATE  );
    LPITEMIDLIST GetNextItem( LPITEMIDLIST lpIDL );
    UINT         CountItem( LPITEMIDLIST lpIDL );
    void         Delete(LPITEMIDLIST pidl);
    LPITEMIDLIST GetLastItem(LPCITEMIDLIST pidl);
    UINT         GetByteSize(LPCITEMIDLIST pidl);
    bool         IsSingle(LPCITEMIDLIST pidl);
    LPITEMIDLIST Concatenate(LPCITEMIDLIST, LPCITEMIDLIST);
    LPITEMIDLIST Copy(LPCITEMIDLIST pidlSrc);
    const TCHAR* GetItemName(LPCITEMIDLIST);
    ITEM_TYPE    GetItemType(LPCITEMIDLIST pidl);
    ITEM_STATE   GetItemState( LPCITEMIDLIST pidl );
    HRESULT      GetFullName(LPCITEMIDLIST pidl,LPTSTR szFullName,DWORD *pdwLen); 
    const TCHAR* GetRealPath( LPCITEMIDLIST pidl ); 
    bool         HasSubFolder(LPCITEMIDLIST pidl); 
    bool         ReplaceRoot( TCHAR* pszDir );
    const TCHAR* GetRootDir();
    bool         IsValidDir(LPCTSTR lpszDir);

private:
    LPPIDLDATA   GetDataPointer( const LPCITEMIDLIST lpIDL );
    //取idl的真实长度
    DWORD        GetPidlDataLen( const LPPIDLDATA lpPidl );
    //取节点名字
    const TCHAR* GetPidlDataName( const LPPIDLDATA lpPidl );
    //取真实节点路径
    const TCHAR* GetPidlDataRealPath( const LPPIDLDATA lpPidl );

};
#endif