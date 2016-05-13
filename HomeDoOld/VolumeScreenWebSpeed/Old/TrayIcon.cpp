#include "StdAfx.h"
#include "TrayIcon.h"


CTrayIcon::CTrayIcon(void)
{
    memset(&iconData_, 0, sizeof(iconData_));
}

void CTrayIcon::InitTrayIcon(HINSTANCE hInst, HWND hWnd, UINT uCallbackMessage, const TCHAR* szIconPath, const TCHAR* szTips)
{
    iconData_.cbSize = sizeof(iconData_);
    iconData_.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP;
    iconData_.hIcon = (HICON)::LoadImage(hInst, szIconPath, IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    iconData_.hWnd = hWnd;
    iconData_.uCallbackMessage = uCallbackMessage;
    strcpy_s(iconData_.szTip, _countof(iconData_.szTip), szTips);
}

void CTrayIcon::ShowTrayIcon()
{
    Shell_NotifyIcon(NIM_ADD, &iconData_);
}

void CTrayIcon::DeleteTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &iconData_);
}

void CTrayIcon::ShowBollon(LPCTSTR infoTitle, LPCTSTR info, UINT infoType)
{
    iconData_.uFlags = NIF_INFO;
    iconData_.uTimeout = 1500;
    iconData_.dwInfoFlags = NIF_INFO;
    strcpy_s(iconData_.szInfo, sizeof(iconData_.szInfo), info);
    strcpy_s(iconData_.szInfoTitle, _countof(iconData_.szInfoTitle), infoTitle);

    Shell_NotifyIcon(NIM_MODIFY, &iconData_);
}


CTrayIcon::~CTrayIcon(void)
{
    Shell_NotifyIcon(NIM_DELETE, &iconData_);
}
