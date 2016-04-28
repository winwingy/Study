// QuickLauchShortCut.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>
#include <string>
#include <assert.h>

typedef std::wstring AnsiString;
#define EmptyStr L""
// 以上三行放在单元文件最开始
//---------------------------------------------------------------------------
struct TShortcutCfg
{
    // 构造函数
    TShortcutCfg()
    {
        nShowCmd = SW_SHOWNORMAL;
        wHotKey = 0;
        nIconIndex = 0;
    }
    // 结构成员:
    AnsiString  strShortcutName; //
    AnsiString  strLnkDir;       //
    AnsiString  strDestFile;     //
    AnsiString  strArguments;    //
    AnsiString  strIconFile;     //
    int         nIconIndex;      //
    AnsiString  strWorkingDir;   //
    AnsiString  strDescription;  //
    WORD        wHotKey;         //
    int         nShowCmd;        //
};
//---------------------------------------------------------------------------
// 在快速启动栏创建快捷方式
bool CreateQuickLaunchShortcut(TShortcutCfg *scConfig)
{
    TCHAR szBuf[MAX_PATH];
    bool bReturn = true;
    HRESULT hr = S_FALSE;
    IShellLink *pShellLink;
    AnsiString strShortcutFile;
    LPITEMIDLIST lpItemIdList;
    SHGetSpecialFolderLocation(0, CSIDL_APPDATA, &lpItemIdList);
    SHGetPathFromIDList(lpItemIdList, szBuf);
     strShortcutFile = AnsiString(szBuf)
            + L"\\Microsoft\\Internet Explorer\\Quick Launch\\"
            + scConfig->strShortcutName + L".lnk";
     CoTaskMemFree(lpItemIdList);

    if (bReturn)
    {
        HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                   IID_IShellLink, (void **)&pShellLink);
        if (SUCCEEDED(hr))
        {
            IPersistFile *ppf;
            hr = pShellLink->QueryInterface(IID_IPersistFile, (void **)&ppf);
            if (SUCCEEDED(hr))
            {
                // 目标文件
                if (!scConfig->strDestFile.empty())
                    hr = pShellLink->SetPath(scConfig->strDestFile.c_str());
                // 参数
                if (SUCCEEDED(hr) && scConfig->strArguments != EmptyStr)
                    hr = pShellLink->SetArguments(scConfig->strArguments.c_str());
                // 显示图标
                if (SUCCEEDED(hr) && scConfig->strIconFile != EmptyStr)
                    pShellLink->SetIconLocation(scConfig->strIconFile.c_str(),
                    scConfig->nIconIndex);
                 // 起始位置
                if (SUCCEEDED(hr) && scConfig->strWorkingDir != EmptyStr)
                    pShellLink->SetWorkingDirectory(scConfig->strWorkingDir.c_str());
                // 备注
                if (SUCCEEDED(hr) && scConfig->strDescription != EmptyStr)
                    pShellLink->SetDescription(scConfig->strDescription.c_str());
                // 快捷键
                if (SUCCEEDED(hr) && scConfig->wHotKey != 0)
                    pShellLink->SetHotkey(scConfig->wHotKey);
                // 运行方式
                if (SUCCEEDED(hr) && scConfig->nShowCmd != 0)
                    pShellLink->SetShowCmd(scConfig->nShowCmd);

                hr = ppf->Save(strShortcutFile.c_str(), TRUE);
                assert(SUCCEEDED(hr));
                ppf->Release();
            }
            pShellLink->Release();
        }
    }
    return (bReturn && SUCCEEDED(hr));
}
// 调用代码举例：
//---------------------------------------------------------------------------
void Button1Click()
{
    TShortcutCfg scShortcut;
    scShortcut.strDestFile = L"D:\\Test\\test.txt";
    scShortcut.strShortcutName = L"test";
    if (CreateQuickLaunchShortcut(&scShortcut))
        OutputDebugString(L"在快速启动栏创建快捷方式成功!");
}
int _tmain(int argc, _TCHAR* argv[])
{
    ::CoInitialize(0);
    Button1Click();
    ::CoUninitialize();
	return 0;
}

