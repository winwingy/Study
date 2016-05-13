#ifndef _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#define _VOLUME_SCREEN_WEB_SPEED_WIN_DEFINE_H_H
#include <windows.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif

#define  TRACE_WW OutputDebugString

#define TRACE_ZZ(szText, strShow) {char szBuffer[1024]={0}; \
    sprintf_s(szBuffer, _countof(szBuffer), szText, strShow); \
    OutputDebugString(szBuffer);}



enum HOTKEY
{
    HOTKEY_SHOW_TRAY = 1000,
	HOTKEY_VOLUME_UP = 1001,
	HOTKEY_VOLUME_DOWN = 1002,
	HOTKEY_CLOSE_SCREEN = 1003,
	HOTKEY_KILL_PROCESS = 1004,
	HOTKEY_KILL_TTPLAYER = 1006,
	HOTKEY_NOT_SCREEN_SAVE = 1007,

	HOTKEY_PROGRESS_BEGIN = 5000,
	HOTKEY_PROGRESS_END = 6000,

	HOTKEY_PROGRESS_KILL_BEGIN = 7000,
	HOTKEY_PROGRESS_KILL_END = 8000,
};

#define CONFIG_SET "Set"
#define CONFIG_POWER_ON_START_PROGRESS "PowerOnStartProgress"
#define CONFIG_SET_HOTKEY "HotKey"
#define CONFIG_INF_FILENAME "VolScrConfig.ini"
#define CONFIG_SET_KILLNAME_BEGIN "[KillNameBegin]"
#define CONFIG_SET_KILLNAME_END "[KillNameEnd]"



enum TIMER_ID
{
    TIMER_DELAY_INIT_PROGRESS,
	TIMER_CLOSE_SCREEN,
	TIMER_CLOSE_SCREEN_ADD,
	TIMER_INIT_VOLUME,
	TIMER_GET_WEB_TIME,
	TIMER_NOT_SCREEN_SAVE, 
	TIMER_POWER_ON_START_PROGRESS
};

namespace HOTKEY_CONFIG_TEXT
{
    const TCHAR* const ShowTray = "ShowTray"; //这样也可以
    const TCHAR VolumeUp[] = "VolumeUp";
	const TCHAR VolumeDown[] = "VolumeDown";
	const TCHAR CloseScreen[] = "CoseScreen";
	const TCHAR KillProcess[] = "KillProcess";
	const TCHAR NotScreenSave[] = "NotScreenSave";
}

namespace CONTROL_ID
{
    enum CONTROL_ID
    {
        CONTROL_ID_BEGIN = 300,
        CONTROL_ID_HOTKEY_SET,
        CONTROL_ID_SET,
        CONTROL_ID_EXIT,
        CONTROL_ID_END,
    };
}

class WinDefine
{
public:
	
	~WinDefine(void);

	static WinDefine* GetInstance();

    static tstring GetValueFromConfig(const tstring& strAppName, 
        const tstring& strKeyName, const tstring& strDefault,
        const tstring& strFileName);
    static int GetValueFromConfig(const tstring& strAppName,
        const string& strKeyName, int strDefault, const tstring& strFileName);
    static void SetConfigValue(const tstring& strAppName, 
        const tstring& strKeyName, const tstring& text,
        const tstring& strFileName);

    static tstring GetCongfigPath(const tstring& strFileName);;

	

	int g_initVolume;
	int g_initVolumeConst;
	int g_perVoulumeGap;
	int g_iInitVolumeTime;
	int g_iIsInitVolume;
	BOOL g_bFinishInitVolume;
	int g_iGetWebTimeCnt;
	vector<string> powerOnStartProgress_;
	int powerOnStartProgressTime_;
	int notScreenSaveTime_;
    int notScreenSaveCanTryCnt_;

private:
	WinDefine(void);
	static WinDefine* windefine_;
};



#endif