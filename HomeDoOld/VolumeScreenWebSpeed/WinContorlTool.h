#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;
namespace wingy_ui
{
class CTrayIcon;
class WWindow;
}

class CMyVolumeCtrl;
class CSetDialog;

class WinControlTool
{
public:	
	~WinControlTool(void);

	struct ProgressToIDHotKey
	{
		string path;
		UINT ID;
		UINT vkCtrl;
		UINT vkKey;

		UINT killID;
		UINT vkKillCtrl;
		UINT vkKillKey;
		ProgressToIDHotKey()
			: ID(0)
			, vkCtrl(0)
			, vkKey(0)
			, killID(0)
			, vkKillCtrl(0)
			, vkKillKey(0)
		{

		}
	};
	
	static WinControlTool* GetInstance();
    void Release();
    void Init(const std::string& iconPathconst, HWND MainHwnd);

	void OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void OnCommand(HWND hWnd, int wmID, int wmEvent);
	void OnHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void ShowTrayIcon(HINSTANCE hInst, HWND hWnd, UINT uCallbackMessage, 
        const TCHAR* szIconPath, const TCHAR* szTips);
    void OnTrayProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static VOID CALLBACK OnTimer( HWND hwnd, UINT uMsg, 
        UINT_PTR idEvent, DWORD dwTime );
    void OnDelayCreate(HWND hWnd);

private:	
    WinControlTool(void);
	CMyVolumeCtrl* GetMyVolumeCtrl();
	static string toupperString(const string& strLower);
    void OPenCongfigIni();    
	void SplitStringBySign(vector<string>& result, 
        const string& stringIn, const string& sign);
	void ReplaceString(string& orc, const string& findWhat, const string& replaceTo);
	void TranslateStringToVKKey(const string& stringIn, UINT* vkCtrl, UINT* vkKey);
	void OnInitHotKey(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void InitProgressHotKey(HWND hWnd);
	void OnInitPowerOnStartProgress(HWND hWnd);	
	
    void OnMenuTrac(HWND hWnd, int wmID, int wmEvent);
	string GetKillNameBuff();
	void OnKillProcess(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static VOID CALLBACK PowerOnStartProgressTimeProc( HWND hwnd, UINT uMsg,
        UINT_PTR idEvent, DWORD dwTime );
	static VOID CALLBACK GetWebTimeTimerProc( HWND hwnd, UINT uMsg, 
        UINT_PTR idEvent, DWORD dwTime );
	
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
	void KillProgressByNames(const vector<string>& namelist, bool tryExistFirst);
	void TerminateNameExe(string& strNameExe);

	void RaiseToken();

	string W2A(wstring strWide);
	bool ForcegroundWindowFullScreen(HWND forcegroundWindow);
	void OnHotKeyNotScreenSave(HWND hWnd, UINT message,
        WPARAM wParam, LPARAM lParam);
	void TipsSound();

    void OnTimerNotScreenSave(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void OnInitStartSet(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void OnSaveEnent(wingy_ui::WWindow* sender, HWND hWnd);

    CMyVolumeCtrl* myVolumeCtrl_;
	static WinControlTool* winControlTool_;
    wingy_ui::CTrayIcon* trayIcon_;
	vector<ProgressToIDHotKey> progressToIDHotkeyList_;
	HWND forcegroundWindowNotScreenSaveHwnd_;
    int notScreenSaveCanTryCntLeave_;	   
    CSetDialog* setDlg_;
};

