// test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <CommCtrl.h>
#include <stddef.h>
#include <assert.h>

void Win7FindDesktop()
{

    HWND dwndparent;
    HWND dwndviem=NULL;
    HWND dwdesktopicon;
    dwndparent=FindWindowEx(0,0,L"WorkerW",L"");//获得第一个WorkerW类的窗口
    while((!dwndviem)&&dwndparent)
    {
        dwndviem=FindWindowEx(dwndparent,0,L"SHELLDLL_DefView",0);
        dwndparent=FindWindowEx(0,dwndparent,L"WorkerW",L"");
    }
    dwdesktopicon=FindWindowEx(dwndviem,0,L"SysListView32",L"FolderView");
}


#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

class sptr_base
{   
protected:
    static const uint32_t ALIGNMENT_BITS = 4;
    static const uint32_t ALIGNMENT = (1<<ALIGNMENT_BITS);
    static const uintptr_t ALIGNMENT_MASK = ALIGNMENT - 1;

protected:
    static uintptr_t     _seg_map[ALIGNMENT];
    static uintptr_t     _segs;
    static boost::mutex  _m;
    inline static uintptr_t ptr2seg(uintptr_t p)
    {
        p &= ~0xFFFFFFFFULL; // Keep only high part
        uintptr_t s = _segs;
        uintptr_t i = 0;
        for (; i < s; ++i)
            if (_seg_map[i] == p)
                return i;

        // Not found - now we do it the "right" way (mutex and all)
        boost::lock_guard<boost::mutex> lock(_m);
        for (i = 0; i < s; ++i)
            if (_seg_map[i] == p)
                return i;

        i = _segs++;
        if (_segs > ALIGNMENT)
            throw bad_segment("Segment out of range");

        _seg_map[i] = p;
        return i;
    }
};
void GetMyOwnerListView()
{

    HWND hwnd = FindWindow(nullptr, L"ListViewOwnerDraw");
    HWND listHwnd = FindWindowEx(hwnd, 0,nullptr, L"CListViewOwnerDrawDlgListView");
    assert(listHwnd);


    {
        LVITEM lvi = {0};
        LVITEM *_lvi;     
        TCHAR *_item;
        TCHAR item[1024] = {0};
        unsigned long pid;
        HANDLE process;


        GetWindowThreadProcessId(listHwnd, &pid);
        process = OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|
            PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pid);

        _lvi = (LVITEM*)VirtualAllocEx(process, NULL, sizeof(LVITEM),
            MEM_COMMIT, PAGE_READWRITE);
        _item = (TCHAR*)VirtualAllocEx(process, NULL, 1024, MEM_COMMIT,
            PAGE_READWRITE);

        int count=(int)SendMessage(listHwnd, LVM_GETITEMCOUNT, 0, 0);
        for (int i = 0; i < count; ++i)
        {
            TCHAR szBuf[1024]; 
            // lvi.mask = LVIF_TEXT;
            lvi.cchTextMax = 1024;
            lvi.pszText = _item;
            lvi.iSubItem = 0;
            SIZE_T memLen = 0;
            BOOL ret = WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEM), &memLen);

            DWORD err = GetLastError();       
            int len = (int)SendMessage(listHwnd, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);
            err = GetLastError(); 
            ret = ReadProcessMemory(process, _item, item, 1024, &memLen);
            err = GetLastError(); 
            {
                TCHAR text[1024];
                GetWindowText(listHwnd, text, 1024);
                int a = 10;
                err = GetLastError();       
            }
            err = GetLastError();       

        }

    }

}

typedef struct _DESKTOP_ICON_INFO {
    LVITEMW item;
    WCHAR item_text[MAX_PATH];
    RECT rc;
} DESKTOP_ICON_INFO, *PDESKTOP_ICON_INFO;

BOOL GetDesktopIconInfo(LPCWSTR pattern, RECT &rc, HWND &desktop)
{
    HWND progman = FindWindow(TEXT("Progman"), TEXT("Program Manager"));
    if (progman == NULL) {
        return FALSE;
    }


    HWND def_view = FindWindowEx(progman, NULL, TEXT("SHELLDLL_DefView"), NULL);
    if (def_view == NULL) {
        return FALSE;
    }

    HWND list_view = FindWindowEx(def_view, NULL, TEXT("SysListView32"), TEXT("FolderView"));
    if (list_view == NULL) {
        return FALSE;
    }
    desktop = list_view;

    ULONG process_id = 0;
    GetWindowThreadProcessId(progman, &process_id);
    if (process_id == 0) {
        return FALSE;
    }

    int count = (int)::SendMessage(list_view, LVM_GETITEMCOUNT, 0, 0);

    HANDLE process_handle = OpenProcess(
        PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_QUERY_INFORMATION, FALSE, process_id);
    if (process_handle == NULL) {
        return FALSE;
    }

    PUCHAR remote_addr = (PUCHAR)VirtualAllocEx(process_handle, NULL, 
        sizeof(DESKTOP_ICON_INFO), MEM_COMMIT, PAGE_READWRITE);

    DESKTOP_ICON_INFO icon_info;
    icon_info.item.iItem = 0;
    icon_info.item.iSubItem = 0;
    icon_info.item.mask = LVIF_TEXT;
    icon_info.item.pszText = (WCHAR *)(remote_addr + offsetof(DESKTOP_ICON_INFO, item_text));
    icon_info.item.cchTextMax = MAX_PATH;

    for (int i = 0; i < count; i++) {
        icon_info.rc.left = LVIR_BOUNDS;
        ZeroMemory(icon_info.item_text, sizeof(icon_info.item_text));
        if (WriteProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL)) {
            ::SendMessage(list_view, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, item)));
            ::SendMessage(list_view, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, rc)));
            ReadProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL);

            if (_wcsicmp(icon_info.item_text, pattern) == 0) {
                rc = icon_info.rc;
                break;
            }
        }
    }

    VirtualFreeEx(process_handle, remote_addr, 0, MEM_RELEASE);
    CloseHandle(process_handle);
    return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
//     LPCWSTR pattern = L"ABD";
//     RECT rc = {0};
//     HWND desktop = nullptr;
//     GetDesktopIconInfo( pattern, rc, desktop);


    GetMyOwnerListView();

    // Win7FindDesktop();
	return 0;
}

