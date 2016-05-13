#pragma once
#include <Windows.h>
#include <ShellAPI.h>
#include "exportUI.h"

namespace wingy_ui
{

class UI_API CTrayIcon
{
public:
    CTrayIcon(void);
    ~CTrayIcon(void);

    void InitTrayIcon(HINSTANCE hInst, HWND hWnd, UINT uCallbackMessage, 
        const TCHAR* szIconPath, const TCHAR* szTips);
    void CTrayIcon::ShowTrayIcon();
    void CTrayIcon::DeleteTrayIcon();
    void CTrayIcon::ShowBollon(LPCTSTR infoTitle, LPCTSTR info, UINT infoType);


private:
    NOTIFYICONDATA iconData_;
};

}