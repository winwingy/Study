#pragma once
#include <string>
#include <Windows.h>
#include "UIDefine.h"

namespace GlobalResource_const
{
const TCHAR g_szIconPath[] = "icon_show.ico";
const TCHAR g_szTrayTips[] = "Wingy Quick Tool";
}


class GlobalResource
{
public:
    static GlobalResource* GetInstance();
    ~GlobalResource(void);

    tstring iconPath_;
    HWND MainHwnd_;

private:
    GlobalResource(void);
    static GlobalResource* global_;
};

