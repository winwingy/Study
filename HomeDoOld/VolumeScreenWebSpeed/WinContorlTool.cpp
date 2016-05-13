#include "StdAfx.h"
#include "WinContorlTool.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <TlHelp32.h>
#include <shellapi.h>
#include <assert.h>
#include "TrayIcon.h"
#pragma comment(lib, "Kernel32.lib")

#include "WHotKeyEdit.h"
#include "MyVolumeCtrl.h"

#include "WinDefine.h"
#include "WMMsg.h"
#include "SetDialog.h"
#include "GlobalResource.h"
#include "WStatic.h"


using namespace std;
using namespace wingy_ui;

WinControlTool* WinControlTool::winControlTool_ = NULL;

#ifdef _DEBUG
#define MYTRACE(text) \
{\
    char szBuf[1024];\
    sprintf_s(szBuf, 1024, "%s : %d   %s\n", __FUNCTION__, __LINE__, (text));\
    OutputDebugString(szBuf);\
}
#else
#define MYTRACE 
#endif

WinControlTool::WinControlTool(void)
    : myVolumeCtrl_(new CMyVolumeCtrl)
    , forcegroundWindowNotScreenSaveHwnd_(NULL)
    , notScreenSaveCanTryCntLeave_(0)
    , trayIcon_(NULL)
    , setDlg_(NULL)
{
}

WinControlTool* WinControlTool::GetInstance()
{
    if(winControlTool_ == NULL)
    {
        winControlTool_ = new WinControlTool;
    }
    return winControlTool_;
}

void WinControlTool::Release()
{
    if (winControlTool_)
    {
        delete winControlTool_;
        winControlTool_ = NULL;
    }
}

void WinControlTool::Init(const tstring& iconPath, HWND MainHwnd)
{
    GlobalResource::GetInstance()->iconPath_ = iconPath;
    GlobalResource::GetInstance()->MainHwnd_ = MainHwnd;
}


CMyVolumeCtrl* WinControlTool::GetMyVolumeCtrl()
{
    return myVolumeCtrl_;
}


WinControlTool::~WinControlTool(void)
{
    if (myVolumeCtrl_)
    {
        delete myVolumeCtrl_;
        myVolumeCtrl_ = NULL;
    }	
    if (trayIcon_)
    {
        delete trayIcon_;
        trayIcon_ = NULL;
    }
}


tstring WinControlTool::toupperString(const tstring& strLower)
{
    tstring strRet;
    for ( tstring::const_iterator iter = strLower.begin(); iter != strLower.end(); ++iter )
    {
        strRet.push_back(toupper(*iter));
    }
    return strRet;
}


void WinControlTool::ReplaceString(tstring& orc, const tstring& findWhat, const tstring& replaceTo)
{
    tstring::size_type posBeg = 0;
    tstring::size_type posEnd = -1;
    while( (posEnd = orc.find(findWhat, posBeg)) != tstring::npos)
    {
        orc.replace(posEnd, findWhat.length(), replaceTo);
        posBeg = posEnd + findWhat.length();
    }
}

void WinControlTool::SplitStringBySign(vector<tstring>& result, const tstring& stringIn, const tstring& sign)
{
    tstring::size_type posBeg = 0;
    tstring::size_type posEnd = -1;
    while( (posEnd = stringIn.find(sign, posBeg)) != -1)//""  a++b++c a++b++ ++b++C++ a++++b
    {
        tstring child(stringIn, posBeg, posEnd - posBeg);		
        if (!child.empty())
        {
            result.push_back(child);
        }
        posBeg = posEnd + sign.length();
    }

    tstring childOut(stringIn, posBeg, stringIn.length() - posBeg);		
    if (!childOut.empty())
    {
        result.push_back(childOut);
    }
}

void WinControlTool::TranslateStringToVKKey(const tstring& stringIn, UINT* vkCtrl, UINT* vkKey)
{
    vector<tstring> vklist;
    SplitStringBySign(vklist, stringIn, "+");
    for (vector<tstring>::iterator it = vklist.begin(); it != vklist.end(); ++it)
    {
        if (*it == "ctrl" || *it == "CTRL")
        {
            *vkCtrl |= MOD_CONTROL;
        }
        else if (*it == "shift" || *it == "SHIFT")
        {
            *vkCtrl |= MOD_SHIFT;
        }
        else if (*it == "alt" || *it == "ALT")
        {
            *vkCtrl |= MOD_ALT;
        }
        else if (it->length() == 4) 
        {
            if (it->find("num") != -1)
            {
                *vkKey = (*it)[3] - '0' + VK_NUMPAD0;				
            }
            else
            {
                assert(0 && "no support key!!!");
            }				
        }		
        else if(it->length() == 1) 
        {
            *vkKey = toupperString(*it)[0];			
        }
        else
        {
            assert(0 && "no support key!!!");
        }
    }
}

void WinControlTool::InitProgressHotKey(HWND hWnd)
{
    BOOL bRet = FALSE;
    progressToIDHotkeyList_.clear();
    for (int i = 1; ; ++i)
    {
        tstring progressName = "Progress";
        char szbuf[10];
        _itoa_s(i, szbuf, 10);
        progressName += szbuf;
        tstring path = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, progressName.c_str(), "", CONFIG_INF_FILENAME); 
        if (path.empty())
        {
            break;
        }
        string hotKeyName = "ProgressHotKey" + string(szbuf);
        int hotkey = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, hotKeyName.c_str(), 0, CONFIG_INF_FILENAME); //ctrl+num3
        string killhotKeyName = "ProgressKillHotKey" + string(szbuf);
        int killhotkey = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, killhotKeyName.c_str(), 0, CONFIG_INF_FILENAME); //ctrl+num3
        
        ProgressToIDHotKey idHotKey;
        idHotKey.path = path;		
        {
            idHotKey.ID = HOTKEY_PROGRESS_BEGIN + i;
            idHotKey.vkCtrl = wingy_ui::WHotKeyEdit::GetModifiers(hotkey);
            idHotKey.vkKey = wingy_ui::WHotKeyEdit::GetVitualKey(hotkey);
            bRet = RegisterHotKey(hWnd, idHotKey.ID, idHotKey.vkCtrl, idHotKey.vkKey);
        }

        if (killhotkey)
        {
            idHotKey.killID = HOTKEY_PROGRESS_KILL_BEGIN + i;
            idHotKey.vkKillCtrl = wingy_ui::WHotKeyEdit::GetModifiers(killhotkey);
            idHotKey.vkKillKey = wingy_ui::WHotKeyEdit::GetVitualKey(killhotkey);
            bRet = RegisterHotKey(hWnd, idHotKey.killID, idHotKey.vkKillCtrl, idHotKey.vkKillKey);
        }		
        progressToIDHotkeyList_.push_back(idHotKey);		
    }
}


void WinControlTool::OnInitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //ShowTray
    int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::ShowTray, 0, CONFIG_INF_FILENAME); 	
    BOOL bRet = RegisterHotKey(hWnd, HOTKEY_SHOW_TRAY, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));

    globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::VolumeUp, 0, CONFIG_INF_FILENAME); 	
    bRet = RegisterHotKey(hWnd, HOTKEY_VOLUME_UP, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));

    globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::VolumeDown, 0, CONFIG_INF_FILENAME); 	
    bRet = RegisterHotKey(hWnd, HOTKEY_VOLUME_DOWN, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));

    globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::CloseScreen, 0, CONFIG_INF_FILENAME); 	
    bRet = RegisterHotKey(hWnd, HOTKEY_CLOSE_SCREEN, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));	

    globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::KillProcess, 0, CONFIG_INF_FILENAME); 	
    bRet = RegisterHotKey(hWnd, HOTKEY_KILL_PROCESS, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));

    globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, HOTKEY_CONFIG_TEXT::NotScreenSave, 0, CONFIG_INF_FILENAME); 	
    bRet = RegisterHotKey(hWnd, HOTKEY_NOT_SCREEN_SAVE, WHotKeyEdit::GetModifiers(globalCode), WHotKeyEdit::GetVitualKey(globalCode));

    //程序Hotkey
    InitProgressHotKey(hWnd);
}


void WinControlTool::OnInitPowerOnStartProgress(HWND hWnd)
{
    int value = WinDefine::GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressCount", 10, CONFIG_INF_FILENAME); 	
    int valueTime = WinDefine::GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, "PowerOnStartProgressTime", 5000, CONFIG_INF_FILENAME); 	
    WinDefine::GetInstance()->powerOnStartProgressTime_ = valueTime;
    BOOL bRet = FALSE;
    for (int i = 1; i <= value; ++i)
    {
        string progressName = "StartProgress";
        char szbuf[10];
        _itoa_s(i, szbuf, 10);
        progressName += szbuf;
        string path = WinDefine::GetValueFromConfig(CONFIG_POWER_ON_START_PROGRESS, progressName.c_str(), "", CONFIG_INF_FILENAME); 	
        if (!path.empty())
        {
            WinDefine::GetInstance()->powerOnStartProgress_.push_back(path);
        }
    }
    SetTimer(hWnd, TIMER_POWER_ON_START_PROGRESS, WinDefine::GetInstance()->powerOnStartProgressTime_, PowerOnStartProgressTimeProc);
}


void WinControlTool::OnInitStartSet(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //音量控制
    WinDefine* winDefine = WinDefine::GetInstance();	
    winDefine->g_iIsInitVolume = WinDefine::GetValueFromConfig(CONFIG_SET, "IsInitVolume", 1, CONFIG_INF_FILENAME); 
    if (winDefine->g_iIsInitVolume)
    {
        winDefine->g_initVolumeConst = winDefine->g_initVolume = WinDefine::GetValueFromConfig(CONFIG_SET, "InitVolume", 30, CONFIG_INF_FILENAME); 
        winDefine->g_iInitVolumeTime = WinDefine::GetValueFromConfig(CONFIG_SET, "InitVolumeTime", 5000, CONFIG_INF_FILENAME);
        SetTimer(hWnd, TIMER_INIT_VOLUME, winDefine->g_iInitVolumeTime, OnTimer);	
    }
    winDefine->g_perVoulumeGap = WinDefine::GetValueFromConfig(CONFIG_SET, "PerVolumeGap", 3, CONFIG_INF_FILENAME);

    //不屏保
    winDefine->notScreenSaveTime_ = WinDefine::GetValueFromConfig(CONFIG_SET, "notScreenSaveTime", 3, CONFIG_INF_FILENAME); 
    winDefine->notScreenSaveCanTryCnt_ = WinDefine::GetValueFromConfig(CONFIG_SET, "notScreenSaveCanTryCnt", 3, CONFIG_INF_FILENAME); 

    //获取网络时间
    int isGetWebTime = WinDefine::GetValueFromConfig(CONFIG_SET, "IsGetWebTime", 0, CONFIG_INF_FILENAME); 
    int valueIni = WinDefine::GetValueFromConfig(CONFIG_SET, "GetWebTimeSpace", 20000, CONFIG_INF_FILENAME); 
    if (isGetWebTime)
    {
        SetTimer(hWnd, TIMER_GET_WEB_TIME, valueIni, OnTimer);	
    }
}

void WinControlTool::OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    RaiseToken();

    if (WinDefine::GetCongfigPath(CONFIG_INF_FILENAME).empty())
    {
        string Text = "目录下没有" + string(CONFIG_INF_FILENAME) + "文件";
        MessageBox(hWnd, Text.c_str(), "警告", MB_OK);
    }

    OnInitPowerOnStartProgress(hWnd);

    OnInitHotKey(hWnd, message, wParam, lParam);

    OnInitStartSet(hWnd, message, wParam, lParam);
}

void WinControlTool::OnDelayCreate(HWND hWnd)
{
     //test
}

string WinControlTool::W2A(wstring strWide)
{
    int iSize = (strWide.size()+1)*2;
    char* temp = new char[iSize];
    WideCharToMultiByte(CP_ACP, 0, strWide.c_str(), -1, (LPSTR)temp, iSize, NULL, NULL);
    return temp; 
}

BOOL CALLBACK WinControlTool::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    DWORD dwThisID = 0;
    GetWindowThreadProcessId(hwnd, &dwThisID);
    if (dwThisID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }	
    return TRUE;
}	

void WinControlTool::KillProgressByNames(const vector<string>& nameList, bool tryExistFirst)
{
    vector<string> vecUpName;
    std::transform(nameList.begin(), nameList.end(), std::back_inserter(vecUpName), toupperString);

    PROCESSENTRY32 pe = {0};
    pe.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    BOOL bRet = Process32First(hSnapshot, &pe);
    TRACE_ZZ("%d\n", bRet);
    DWORD dwErr = GetLastError();
    TRACE_ZZ("%d\n", dwErr);
    do 
    {
        TRACE_WW(pe.szExeFile);
        TRACE_WW("\n");		
        _strupr_s(pe.szExeFile);		
        TRACE_WW(pe.szExeFile);
        TRACE_WW("\nGo TO Kill\n");
        for ( vector<string>::const_iterator it = vecUpName.begin(); it < vecUpName.end(); ++it )
        {
            if ( *it == pe.szExeFile )
            {				
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
                TRACE_WW(pe.szExeFile);
                TRACE_WW("\t\t");
#ifdef TRACE_WW
                char szProcess[50] = {0};
                sprintf_s(szProcess, _countof(szProcess), "%p", hProcess);
                TRACE_WW(szProcess);
                TRACE_WW("\n");
#endif
                if(tryExistFirst)
                {
                    EnumWindows(EnumWindowsProc, pe.th32ProcessID);
                    Sleep(3000);
                }				
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }	
    } while (Process32Next(hSnapshot, &pe));
    CloseHandle(hSnapshot);
}



void WinControlTool::TerminateNameExe(string& strNameExe)
{
    if (strNameExe.empty())
    {
        return;
    }	
    ReplaceString(strNameExe, "\r\n", "\n");
    ReplaceString(strNameExe, "\r", "\n");
    vector<string> vecName;
    SplitStringBySign(vecName, strNameExe, "\n");
    KillProgressByNames(vecName, false);
}


VOID CALLBACK WinControlTool::PowerOnStartProgressTimeProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
    KillTimer(hwnd, idEvent);
    for (vector<string>::iterator it = WinDefine::GetInstance()->powerOnStartProgress_.begin();
        it !=  WinDefine::GetInstance()->powerOnStartProgress_.end(); ++it)
    {
        SHELLEXECUTEINFOA stExecInfo = {0};
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_FLAG_NO_UI ;
        stExecInfo.lpFile = it->c_str();
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteExA(&stExecInfo);
    }
}

void WinControlTool::OnTimerNotScreenSave(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    WinControlTool* thisClass =  WinControlTool::GetInstance(); 
    bool notScreenContinue = false;

    if( thisClass->forcegroundWindowNotScreenSaveHwnd_ && 
        IsWindow(thisClass->forcegroundWindowNotScreenSaveHwnd_))
    {
        HWND fore = GetForegroundWindow();
        if (fore == thisClass->forcegroundWindowNotScreenSaveHwnd_)
        {
            MYTRACE("fore == thisClass->forcegroundWindowNotScreenSaveHwnd");
            INPUT input = {0};
            input.type = INPUT_KEYBOARD; 
            input.ki.wVk = VK_F24; 			
            SendInput(1, &input, sizeof(INPUT));               
            notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;
            notScreenContinue = true;
        }
    }

    if (!notScreenContinue)
    {
        MYTRACE(" if (!notScreenContinue)");
        if(--notScreenSaveCanTryCntLeave_ <= 0)
        {
            KillTimer(hwnd, idEvent);            
        }           
    }
}

VOID CALLBACK WinControlTool::OnTimer( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
    if ( TIMER_CLOSE_SCREEN == idEvent || TIMER_CLOSE_SCREEN_ADD == idEvent )
    {
        KillTimer(hwnd, idEvent);
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)1);		
        ::PostMessage(hwnd, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);			
    }
    else if ( TIMER_INIT_VOLUME==idEvent )
    {	
        WinDefine* winDefine = WinDefine::GetInstance();
        if ( FALSE == winDefine->g_bFinishInitVolume && winDefine->g_iIsInitVolume == 1)
        {	
            GetInstance()->GetMyVolumeCtrl()->SetVolume(winDefine->g_initVolume);
            winDefine->g_initVolume = 0;
        }
        winDefine->g_bFinishInitVolume = TRUE;
        KillTimer(hwnd, idEvent);
    }
    else if (TIMER_GET_WEB_TIME == idEvent)
    {
        SHELLEXECUTEINFO stExecInfo = {0};
        stExecInfo.cbSize = sizeof(stExecInfo);
        stExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI ;
        stExecInfo.lpFile = "GetWebAndSetTime.exe";
        stExecInfo.nShow = SW_HIDE;
        BOOL bRet = ShellExecuteEx(&stExecInfo);
        DWORD dwErr = GetLastError();
        WaitForSingleObject(stExecInfo.hProcess, INFINITE);
        DWORD dwCode = -2;
        bRet = GetExitCodeProcess(stExecInfo.hProcess, &dwCode);
        if ( dwCode == 0 || ++WinDefine::GetInstance()->g_iGetWebTimeCnt>5)
        {
            KillTimer(hwnd, idEvent);
        }
    }
    else if (TIMER_NOT_SCREEN_SAVE == idEvent)
    {        
        GetInstance()->OnTimerNotScreenSave(hwnd, uMsg, idEvent, dwTime);
    }
}

void WinControlTool::RaiseToken()
{
    HANDLE   hToken;   
    TOKEN_PRIVILEGES   tkp;  
    BOOL bRet(FALSE);
    static bool bPrivi(false);
    if ( !bPrivi )
    {
        //定义变量   
        bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   
        char szRet[10] = {0};
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
        //GetCurrentProcess（）函数的作用是得到本进程的句柄   
        bRet = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid);  
        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        //LookupPrivilegeValue（）的作用是修改进程的权限   
        tkp.PrivilegeCount =  1;   
        //赋给本进程特权   
        tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;   
        bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);   
        //AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		{
			TRACE_WW(szRet);;
		}

        sprintf_s(szRet, _countof(szRet), "%d", bRet);
        TRACE_WW(szRet);
        TRACE_WW("\n");
        bPrivi = true;
    }
}

string WinControlTool::GetKillNameBuff()
{	
    char szPath[2048] = {0};
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    string strPath(szPath);
    int iPos = strPath.rfind("\\");
    strPath.erase(iPos);

    string strAnotherPath(strPath);

    strPath += string("\\") + CONFIG_INF_FILENAME;
    FILE* fp = NULL;
    fopen_s(&fp, strPath.c_str(), "r");
    if ( fp == NULL )
    {
        iPos = strAnotherPath.rfind("\\");
        strAnotherPath.erase(iPos);
        strAnotherPath += string("\\") + CONFIG_INF_FILENAME;
        fopen_s(&fp, strAnotherPath.c_str(), "r");
        if(fp == NULL )
        {
            return "";
        }						
    }
    char szBuffer[2048] = {0};
    fread(szBuffer, 2047, 1, fp);
    fclose(fp);

    char* beg = strstr(szBuffer, CONFIG_SET_KILLNAME_BEGIN);
    char* end = strstr(szBuffer, CONFIG_SET_KILLNAME_END);
    if (beg && end)
    {
        beg += strlen(CONFIG_SET_KILLNAME_BEGIN);
        while ('\r' == *beg || '\n' == *beg)
        {
            beg++;
        }
        return string(beg, end);
    }
    return "";
}


void WinControlTool::OnKillProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
    string killName = GetKillNameBuff();
    TerminateNameExe(killName);
    TerminateNameExe(killName);	
}

bool WinControlTool::ForcegroundWindowFullScreen(HWND forcegroundWindow)
{	
    if (forcegroundWindow)
    {
        RECT rect = {0};
        GetWindowRect(forcegroundWindow, &rect);
        int width = GetSystemMetrics(SM_CXSCREEN);
        int height = GetSystemMetrics(SM_CYSCREEN);
        if ((rect.right - rect.left >= width) && (rect.bottom - rect.top >= height))
        {
            return true;
        }
    }
    return false;
}

void PlaySoundHappy()
{
    unsigned FREQUENCY[] = {392,392,440,392,523,494,  
        392,392,440,392,587,523,  
        392,392,784,659,523,494,440,  
        689,689,523,587,523};  

    unsigned DELAY[] = {375,125,500,500,500,1000,  
        375,125,500,500,500,1000,  
        375,125,500,500,500,500,1000,  
        375,125,500,500,500,1000};  
    int i;  
    for (i = 0; i < 6; i++)  //先填10短些
    {  
        Beep(FREQUENCY[i], DELAY[i]);  
    }  
}

void WinControlTool::OnHotKeyNotScreenSave(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (forcegroundWindowNotScreenSaveHwnd_)
    {
        KillTimer(hWnd, TIMER_NOT_SCREEN_SAVE);
        forcegroundWindowNotScreenSaveHwnd_ = NULL;
    }
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow)
    {
        forcegroundWindowNotScreenSaveHwnd_ = foregroundWindow;				
        SetTimer(hWnd, TIMER_NOT_SCREEN_SAVE, WinDefine::GetInstance()->notScreenSaveTime_*1000, NULL);
        notScreenSaveCanTryCntLeave_ = WinDefine::GetInstance()->notScreenSaveCanTryCnt_;
        PlaySoundHappy();
    }
}

void WinControlTool::TipsSound()
{
    int beepArr[] = {MB_OK, MB_ICONQUESTION};
    for (int b = 0; b < sizeof(beepArr)/sizeof(beepArr[0]); ++b)
    {
        for (int i = 0; i < 5; ++i)
        {
            ::MessageBeep(beepArr[b]);
            Sleep(500);
        }
    }
}

void WinControlTool::ShowTrayIcon(HINSTANCE hInst, HWND hWnd, UINT uCallbackMessage, const TCHAR* szIconPath, const TCHAR* szTips)
{
    if (trayIcon_ == NULL)
    {
        trayIcon_ = new wingy_ui::CTrayIcon;
    }
    trayIcon_->InitTrayIcon(hInst, hWnd, uCallbackMessage, szIconPath, szTips);
    trayIcon_->ShowTrayIcon();
}

void WinControlTool::OPenCongfigIni()
{
    ShellExecute(NULL, NULL, WinDefine::GetCongfigPath(CONFIG_INF_FILENAME).c_str(), NULL, NULL, SW_SHOW);
}

void WinControlTool::OnMenuTrac(HWND hWnd, int wmID, int wmEvent)
{
    switch(wmID)
    {
    case CONTROL_ID::CONTROL_ID_SET:
        {
            OPenCongfigIni();       

            break;
        }
    case CONTROL_ID::CONTROL_ID_HOTKEY_SET:
        {
            if (setDlg_ == NULL)
            {
                setDlg_ = new CSetDialog(hWnd);
            }
            setDlg_->Create("设置",  100, 100, 800, 800, nullptr);
            setDlg_->SetSaveEvent(std::bind(&WinControlTool::OnSaveEnent, 
                this, std::placeholders::_1, hWnd));
            break;
        }
    case CONTROL_ID::CONTROL_ID_EXIT:
        {
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
    }
}

void WinControlTool::OnSaveEnent(wingy_ui::WWindow* sender, 
    HWND hWnd)
{
    OnInitHotKey(hWnd, 0, 0, 0);
}

void WinControlTool::OnCommand(HWND hWnd, int wmID, int wmEvent)
{
    if (wmID > CONTROL_ID::CONTROL_ID_BEGIN && wmID < CONTROL_ID::CONTROL_ID_END)
    {
        OnMenuTrac(hWnd, wmID, wmEvent);
    }
}


void WinControlTool::OnTrayProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(lParam)
    {
    case WM_LBUTTONDOWN:
        {
            OutputDebugString("asdfasdf");
            break;
        }
    case WM_RBUTTONDOWN:
        {
            HMENU menu = CreatePopupMenu();
            AppendMenu(menu, MF_STRING, CONTROL_ID::CONTROL_ID_SET, "打开ini");
            AppendMenu(menu, MF_STRING, CONTROL_ID::CONTROL_ID_HOTKEY_SET, "设置快捷键");
            AppendMenu(menu, MF_STRING, CONTROL_ID::CONTROL_ID_EXIT, "退出");    
            POINT pt = {0};
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);
            BOOL ret = TrackPopupMenu(menu, TPM_RIGHTALIGN|TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);            
            PostMessage(hWnd, WM_NULL, 0, 0);
            DestroyMenu(menu);
            break;
        }
    }
}

void WinControlTool::OnHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int iVolume;
    char szVolume[256]={0};
    int iIDHotKey = (int)wParam;

    WinDefine* winDefine = WinDefine::GetInstance();

    int iTime(0);
    BOOL bRet(FALSE);
    switch(iIDHotKey)
    {
    case HOTKEY_SHOW_TRAY:
        {
            WinControlTool::GetInstance()->ShowTrayIcon(
                GetModuleHandle(nullptr), hWnd,
                WM_USER_MSG_ID::WM_USER_MSG_ID_TRAY_PROC,
                GlobalResource_const::g_szIconPath,
                GlobalResource_const::g_szTrayTips);
            break;
        }
    case HOTKEY_VOLUME_UP:
        {
            iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
            sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
            iVolume += winDefine->g_perVoulumeGap;
            GetMyVolumeCtrl()->SetVolume(iVolume);
            break;
        }
    case HOTKEY_VOLUME_DOWN:
        {			
            if ( FALSE == winDefine->g_bFinishInitVolume )
            {
                GetMyVolumeCtrl()->SetVolume(winDefine->g_initVolume*1/3);
                winDefine->g_initVolume = 0;
                winDefine->g_bFinishInitVolume = TRUE;
            }			
            iVolume = GetMyVolumeCtrl()->GetVolume();	//得到的值偏小1						
            sprintf_s(szVolume,sizeof(szVolume), "iVolume = %d\n", iVolume);
            iVolume -= winDefine->g_perVoulumeGap;
            if ( iVolume < 0 )
            {
                iVolume = 0;
            }
            GetMyVolumeCtrl()->SetVolume(iVolume);
            break;
        }
    case HOTKEY_CLOSE_SCREEN:
        {					
            SetTimer(hWnd, TIMER_CLOSE_SCREEN, 1500, OnTimer);
            SetTimer(hWnd, TIMER_CLOSE_SCREEN_ADD, 4000, OnTimer);
            KillTimer(hWnd, TIMER_NOT_SCREEN_SAVE);  //将阻止屏幕保护的关掉
            break;
        }
    case HOTKEY_KILL_PROCESS:
        {
            OnKillProcess(hWnd, message, wParam, lParam);	
            break;
        }
    case HOTKEY_NOT_SCREEN_SAVE:
        {
            OnHotKeyNotScreenSave(hWnd, message, wParam, lParam);
            break;
        }		
    }

    if (iIDHotKey >= HOTKEY_PROGRESS_BEGIN && iIDHotKey <= HOTKEY_PROGRESS_END)
    {
        for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
            it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->ID == iIDHotKey)
            {
                SHELLEXECUTEINFOA stInfo = {0};
                stInfo.cbSize = sizeof(stInfo);			
                stInfo.lpFile = it->path.c_str();
                stInfo.nShow = SW_SHOW;
                ShellExecuteExA(&stInfo);
            }
        }
    }
    else if (iIDHotKey >= HOTKEY_PROGRESS_KILL_BEGIN && iIDHotKey <= HOTKEY_PROGRESS_KILL_END)
    {
        for (vector<ProgressToIDHotKey>::iterator it = progressToIDHotkeyList_.begin();
            it != progressToIDHotkeyList_.end(); ++it)
        {
            if (it->killID == iIDHotKey)
            {
                string::size_type index = 0;
                if ( ((index = it->path.rfind("\\")) != -1) || 
                    ((index = it->path.rfind("/")) != -1) )
                {
                    string killName(it->path, ++index, it->path.size() - index);
                    KillProgressByNames(vector<string>(1, killName), true);
                }	
            }
        }
    }
}