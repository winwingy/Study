// TestBB.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <iostream>
#include <assert.h>
using namespace std;



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
            int len = ::SendMessage(list_view, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)(remote_addr));
            ::SendMessage(list_view, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)(remote_addr + offsetof(DESKTOP_ICON_INFO, rc)));
            ReadProcessMemory(process_handle, remote_addr, &icon_info, sizeof(icon_info), NULL);

            if (icon_info.item_text[0] != 0)
            {
                //assert(0 && L"找到了！！！");
            }

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



void GetRect(HWND listview, HANDLE process, int i)
{
    DWORD err = GetLastError();
    RECT* rcOT=(RECT*)VirtualAllocEx(process, NULL, sizeof(RECT), MEM_COMMIT,
        PAGE_READWRITE);
    err = GetLastError();
    
    RECT rc = {0};
    rc.left = LVIR_BOUNDS;
    SIZE_T hasWrited = 0;
    BOOL ret = WriteProcessMemory(process, rcOT, &rc, sizeof(rc), &hasWrited);
    err = GetLastError();
    ret = SendMessage(listview, LVM_GETITEMRECT, (WPARAM)i, (LPARAM)rcOT);
    err = GetLastError();
    ret = ReadProcessMemory(process, rcOT, &rc, sizeof(rc), &hasWrited);
    err = GetLastError();
    VirtualFreeEx(process, rcOT, 0, MEM_RELEASE);
}

//hImageList=(HIMAGELIST)SHGetFileInfo((LPCTSTR)lpidl,0,&fi, 
    //sizeof(SHFILEINFO), SHGFI_PIDL|SHGFI_SYSICONINDEX|SHGFI_ICON);

void GetItemText(HWND listview, HANDLE process, int i)
{
    LVITEM lvi = {0};
    LVITEM *_lvi = nullptr;
    char item[512] = {0};
    char *_item = nullptr;
    unsigned long pid = 0;   
   
    int count=(int)SendMessage(listview, LVM_GETITEMCOUNT, 0, 0);
    _lvi=(LVITEM*)VirtualAllocEx(process, NULL, sizeof(LVITEM),
        MEM_COMMIT, PAGE_READWRITE);
    _item=(char*)VirtualAllocEx(process, NULL, 512, MEM_COMMIT,
        PAGE_READWRITE);

    lvi.mask = LVIF_STATE|LVIF_IMAGE|LVIF_TEXT;
    lvi.cchTextMax=512;
    lvi.pszText=_item;
    lvi.iSubItem=1;

    lvi.iItem = i;
    BOOL result =  WriteProcessMemory(process, _lvi, &lvi,
        sizeof(LVITEM), NULL);
    auto ret = SendMessage(listview, LVM_GETITEM, (WPARAM)0,
        (LPARAM)_lvi);
    result = ReadProcessMemory(process, _item, item, 512, NULL);
    result = ReadProcessMemory(process, _lvi, &lvi, sizeof(LVITEM), NULL);

    VirtualFreeEx(process, _lvi, 0, MEM_RELEASE);
    VirtualFreeEx(process, _item, 0, MEM_RELEASE);
}


void WebGetDesktopIcon()
{
    HWND listview = FindWindow(_T("progman"), NULL); 
    listview = FindWindowEx(listview, 0, _T("shelldll_defview"), NULL); 
    listview = FindWindowEx(listview, 0, _T("syslistview32"), NULL); 

    int count=(int)SendMessage(listview, LVM_GETITEMCOUNT, 0, 0);


    LVITEM lvi = {0};
    LVITEM *_lvi = nullptr;
    char item[512] = {0};
    char *_item = nullptr;
    unsigned long pid = 0;
    HANDLE process = nullptr;

    GetWindowThreadProcessId(listview, &pid);
    process=OpenProcess(PROCESS_VM_OPERATION|PROCESS_VM_READ|
        PROCESS_VM_WRITE|PROCESS_QUERY_INFORMATION, FALSE, pid);

    _lvi=(LVITEM*)VirtualAllocEx(process, NULL, sizeof(LVITEM),
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    _item=(char*)VirtualAllocEx(process, NULL, 1024, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);

    //lvi.mask = LVIF_TEXT;
    lvi.cchTextMax=512;
    lvi.pszText=_item;
    lvi.iSubItem=0;

    for(int i=0; i<count; i++) 
    {
        BOOL result =  WriteProcessMemory(process, _lvi, &lvi, sizeof(LVITEM), NULL);
        auto ret = SendMessage(listview, LVM_GETITEMTEXT, (WPARAM)i, (LPARAM)_lvi);

        GetRect(listview, process, i);
        GetItemText(listview, process, i);

        {
            TCHAR szZB[1024];
            GetWindowText(listview, szZB, 1024);
            int a  = 10;
        }
        result = ReadProcessMemory(process, _item, item, 512, NULL);
        cout << item;
        cout << endl;
    }

    VirtualFreeEx(process, _lvi, 0, MEM_RELEASE);
    VirtualFreeEx(process, _item, 0, MEM_RELEASE);

}
int main() {
    LPCWSTR pattern = L"ABD";
    RECT rc = {0};
    HWND desktop = nullptr;
    GetDesktopIconInfo( pattern, rc, desktop);

   

}

