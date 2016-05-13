#include "StdAfx.h"
#include "SetDialog.h"
#include "GlobalResource.h"
#include "WStatic.h"
#include "WHotKeyEdit.h"
#include "WinDefine.h"
#include "WEdit.h"
#include "WStaticEx.h"
#include "WButton.h"
#include "WTask.h"


using namespace wingy_ui;

CSetDialog::CSetDialog(HWND mainWnd)    
    : mainWnd_(mainWnd)
    , showTray_(new WStatic)
    , showTrayHotKey_(new WHotKeyEdit)
    , volumeUp_(new WStatic)
    , volumeUpHotKey_(new WHotKeyEdit)
    , volumeDown_(new WStatic)
    , volumeDownHotKey_(new WHotKeyEdit)
    , closeScreen_(new WStatic)
    , closeScreenHotKey_(new WHotKeyEdit)
    , killProcess_(new WStatic)
    , killProcessHotKey_(new WHotKeyEdit)
    , notScreenSave_(new WStatic)
    , seprateLine_(0)
    , notScreenSaveHotKey_(new WHotKeyEdit)
    , addProgressButton_(new WButton)
    , save_(new wingy_ui::WButton)
{

}


CSetDialog::~CSetDialog(void)
{
    
}

// void WSetDialog::CreateHotKeyControls(const std::string& configSection, const std::string& configKey, )
// {
//     CreateWindow("STATIC", configSection.c_str(), "WS_VISIBLE|WS_CHILD", 10, 10, 50, 30, hWnd_, )
//         SetWindowPos(HWND_TOP)
// 
// }

bool CSetDialog::HasSameHotKey(wingy_ui::WHotKeyEdit* control)
{
    if (showTrayHotKey_.get() != control &&
        showTrayHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }
    if (volumeUpHotKey_.get() != control &&
        volumeUpHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }
    if (volumeDownHotKey_.get() != control &&
        volumeDownHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }
    if (closeScreenHotKey_.get() != control &&
        closeScreenHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }   
    if (killProcessHotKey_.get() != control &&
        killProcessHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }
    if (notScreenSaveHotKey_.get() != control &&
        notScreenSaveHotKey_->GetHotKeyCode() == control->GetHotKeyCode())
    {
        return true;
    }

    for (auto iter = progressHotKeyList_.begin(); 
        iter != progressHotKeyList_.end(); ++iter)
    {
        if (control != iter->StartKey.get() &&
            iter->StartKey->GetHotKeyCode() == control->GetHotKeyCode())
        {
            return true;
        }
        if (control != iter->KillKey.get() &&
            iter->KillKey->GetHotKeyCode() == control->GetHotKeyCode())
        {
            return true;
        }
    }    
   return false;
}



void CSetDialog::OnHotKeyKillFocusEvent(wingy_ui::WWindow* control)
{
    wingy_ui::WHotKeyEdit* hotKey =
        reinterpret_cast<wingy_ui::WHotKeyEdit*>(control);
    if (HasSameHotKey(hotKey))
    {
        hotKey->ShowOldKey();
    }
}

void CSetDialog::test(int id)
{

}

void CSetDialog::testB()
{

}

void CSetDialog::DelayDeleteControl(wingy_ui::WWindow* control)
{
    if (progressHotKeyList_.size() > 1)
    {
        for (auto iter = progressHotKeyList_.begin();
            iter != progressHotKeyList_.end(); ++iter)
        {
            if (iter->DeleteHotKey.get() == control)
            {
                progressHotKeyList_.erase(iter);
                break;
            }
        }
    }
}

void CSetDialog::OnProgressHotKeyDeleteClick(wingy_ui::WWindow* control)
{
    WTask::GetInstance()->PostTaskB(std::bind(&CSetDialog::DelayDeleteControl,
        this, control));
}

void CSetDialog::OnAddProgressButtonClick(wingy_ui::WWindow* control)
{
    if (progressHotKeyList_.size() > 0)
    {
        ProgressHotKeyList hotkey = 
            progressHotKeyList_[progressHotKeyList_.size() - 1];

        int sepTemp = hotkey.Path->GetPos().bottom + 40;

        ProgressHotKeyList ProgressHotkey;
        ProgressHotkey.Path.reset(new wingy_ui::WEdit);
        ProgressHotkey.StartKey.reset(new wingy_ui::WHotKeyEdit);
        ProgressHotkey.KillKey.reset(new wingy_ui::WHotKeyEdit);
        ProgressHotkey.DeleteHotKey.reset(new wingy_ui::WButton);
        ProgressHotkey.Path->Create("", 30, sepTemp + 10,
            GetPos().right - 80, 20, hWnd_);            
        ProgressHotkey.StartKey->Create(100, sepTemp+ 40, 100, 20, hWnd_);
        ProgressHotkey.KillKey->Create(300, sepTemp+ 40, 100, 20, hWnd_);
        ProgressHotkey.DeleteHotKey->Create(_T("Del"), 450, sepTemp+ 40,
            100, 20, hWnd_);
        ProgressHotkey.DeleteHotKey->SetLBtnDownEvent(
            std::bind(&CSetDialog::OnProgressHotKeyDeleteClick, 
            this, std::placeholders::_1));

        ProgressHotkey.StartKey->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));

        ProgressHotkey.KillKey->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));

        progressHotKeyList_.push_back(ProgressHotkey);

        WRect rect = addProgressButton_->GetPos();
        rect.Move(rect.left,  ProgressHotkey.KillKey->GetPos().bottom + 40);
        addProgressButton_->SetPos(rect);
    }
}


LRESULT CSetDialog::OnInitHotKey(WPARAM wParam, LPARAM lParam, BOOL& handle)
{
    {
        showTray_->Create("", 400, 10, 150, 20, hWnd_);
        showTray_->SetTextColor(RGB(0, 255, 0));
        showTrayHotKey_->Create(550, 10, 150, 20, hWnd_);
        showTray_->SetWindowText(HOTKEY_CONFIG_TEXT::ShowTray);
        int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::ShowTray, 0, CONFIG_INF_FILENAME); 	
        showTrayHotKey_->InitHotKeyText(globalCode);
        showTrayHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
    }

	{
		volumeUp_->Create("", 50, 10, 150, 20, hWnd_);
        volumeUp_->SetTextColor(RGB(0, 255, 0));
		volumeUpHotKey_->Create(200, 10, 150, 20, hWnd_);
		volumeUp_->SetWindowText(HOTKEY_CONFIG_TEXT::VolumeUp);
		int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::VolumeUp, 0, CONFIG_INF_FILENAME); 	
		volumeUpHotKey_->InitHotKeyText(globalCode);
        volumeUpHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
	}

	{
		volumeDown_->Create("", 50, 50, 150, 20, hWnd_);
        volumeDown_->SetTextColor(RGB(0, 255, 0));
		volumeDownHotKey_->Create(200, 50, 150, 20, hWnd_);
		volumeDown_->SetWindowText(HOTKEY_CONFIG_TEXT::VolumeDown);
		int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::VolumeDown, 0, CONFIG_INF_FILENAME); 	
		volumeDownHotKey_->InitHotKeyText(globalCode);
        volumeDownHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
	}

	{
		closeScreen_->Create("", 50, 90, 150, 20, hWnd_);
        closeScreen_->SetTextColor(RGB(0, 255, 0));
		closeScreenHotKey_->Create(200, 90, 150, 20, hWnd_);
		closeScreen_->SetWindowText(HOTKEY_CONFIG_TEXT::CloseScreen);
		int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY,
            HOTKEY_CONFIG_TEXT::CloseScreen, 0, CONFIG_INF_FILENAME); 	
		closeScreenHotKey_->InitHotKeyText(globalCode);
        closeScreenHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
	}

	{
		killProcess_->Create("", 50, 130, 150, 20, hWnd_);
        killProcess_->SetTextColor(RGB(0, 255, 0));
		killProcessHotKey_->Create(200, 130, 150, 20, hWnd_);
		killProcess_->SetWindowText(HOTKEY_CONFIG_TEXT::KillProcess);
		int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::KillProcess, 0, CONFIG_INF_FILENAME); 	
		killProcessHotKey_->InitHotKeyText(globalCode);
        killProcessHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
	}

	{
		notScreenSave_->Create("", 50, 170, 150, 20, hWnd_);
        notScreenSave_->SetTextColor(RGB(0, 255, 0));
		notScreenSaveHotKey_->Create(200, 170, 150, 20, hWnd_);
		notScreenSave_->SetWindowText(HOTKEY_CONFIG_TEXT::NotScreenSave);
		int globalCode = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::NotScreenSave, 0, CONFIG_INF_FILENAME); 	
		notScreenSaveHotKey_->InitHotKeyText(globalCode);
        notScreenSaveHotKey_->SetKillFocusEvent(
            std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
            this, std::placeholders::_1));
	}

    {
        seprateLine_ = notScreenSaveHotKey_->GetPos().bottom + 30;
        int sepTemp = seprateLine_ + 30;
        for (int i = 1; ; ++i)
        {
 
            ProgressHotKeyList ProgressHotkey;
            ProgressHotkey.Path.reset(new wingy_ui::WEdit);
            ProgressHotkey.StartKey.reset(new wingy_ui::WHotKeyEdit);
            ProgressHotkey.KillKey.reset(new wingy_ui::WHotKeyEdit);
            ProgressHotkey.DeleteHotKey.reset(new wingy_ui::WButton);
            ProgressHotkey.Path->Create("", 30, sepTemp + 10,
                GetPos().right - 80, 20, hWnd_);            
            ProgressHotkey.StartKey->Create(100, sepTemp+ 40, 100, 20, hWnd_);
            ProgressHotkey.KillKey->Create(300, sepTemp+ 40, 100, 20, hWnd_);
            ProgressHotkey.DeleteHotKey->Create(_T("Del"), 450, sepTemp+ 40,
                100, 20, hWnd_);
            ProgressHotkey.DeleteHotKey->SetLBtnDownEvent(
                std::bind(&CSetDialog::OnProgressHotKeyDeleteClick, 
                this, std::placeholders::_1));
            sepTemp += 80;

            tstring progressName = "Progress";
            char szbuf[10];
            _itoa_s(i, szbuf, 10);
            progressName += szbuf;
            tstring path = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY,
                progressName.c_str(), "", CONFIG_INF_FILENAME); 
            string hotKeyName = "ProgressHotKey" + string(szbuf);
            int hotkey = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY,
                hotKeyName.c_str(), 0, CONFIG_INF_FILENAME); //ctrl+num3
            ProgressHotkey.Path->SetWindowText(path);
            ProgressHotkey.StartKey->InitHotKeyText(hotkey);
            ProgressHotkey.StartKey->SetKillFocusEvent(
                std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
                this, std::placeholders::_1));

            string killhotKeyName = "ProgressKillHotKey" + string(szbuf);
            int killhotkey = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY, 
                killhotKeyName.c_str(), 0, CONFIG_INF_FILENAME); //ctrl+num3
            ProgressHotkey.KillKey->InitHotKeyText(killhotkey);
            ProgressHotkey.KillKey->SetKillFocusEvent(
                std::bind(&CSetDialog::OnHotKeyKillFocusEvent,
                this, std::placeholders::_1));

            progressHotKeyList_.push_back(ProgressHotkey);

            if (path.empty())
            {
                break;
            } 
        }
        addProgressButton_->Create("Add", 200, sepTemp, 50, 30, hWnd_);
        addProgressButton_->SetLBtnDownEvent(
            std::bind(&CSetDialog::OnAddProgressButtonClick,
            this, std::placeholders::_1));
    }
    
    return handle;
}

void CSetDialog::OnSaveLBtndown(WWindow* sender)
{
    {
        int globalCode = showTrayHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::ShowTray, hotkeyStr, CONFIG_INF_FILENAME); 
    }

    {
        int globalCode = volumeUpHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::VolumeUp, hotkeyStr, CONFIG_INF_FILENAME); 
    }

    {
        int globalCode = volumeDownHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::VolumeDown, hotkeyStr, CONFIG_INF_FILENAME); 
    }

    {
        int globalCode = closeScreenHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::CloseScreen, hotkeyStr, CONFIG_INF_FILENAME); 
    }

    {
        int globalCode = killProcessHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::KillProcess, hotkeyStr, CONFIG_INF_FILENAME); 
    }

    {
        int globalCode = notScreenSaveHotKey_->GetHotKeyCode();
        TCHAR hotkeyStr[20];
        _itot_s(globalCode, hotkeyStr, 10);
        if (globalCode == -1)
        {
            hotkeyStr[0] = 0;
        }
        WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
            HOTKEY_CONFIG_TEXT::NotScreenSave, hotkeyStr, CONFIG_INF_FILENAME); 
    }


    {
        for (int i = 1; ; ++i)
        {
            tstring progressName = "Progress";
            char szbuf[10];
            _itoa_s(i, szbuf, 10);
            progressName += szbuf;

            {
                tstring path = WinDefine::GetValueFromConfig(CONFIG_SET_HOTKEY,
                    progressName.c_str(), "", CONFIG_INF_FILENAME); 
                if (path.empty())
                {
                    break;
                } 
            }

            WinDefine::SetConfigValue(CONFIG_SET_HOTKEY,
                progressName.c_str(), _T(""), CONFIG_INF_FILENAME); 
            string hotKeyName = "ProgressHotKey" + string(szbuf);
            WinDefine::SetConfigValue(CONFIG_SET_HOTKEY,
                hotKeyName.c_str(), "", CONFIG_INF_FILENAME);  
            string killhotKeyName = "ProgressKillHotKey" + string(szbuf);
            WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
                killhotKeyName.c_str(), "", CONFIG_INF_FILENAME); 
        }

        for (auto iter = progressHotKeyList_.begin(); 
            iter != progressHotKeyList_.end(); ++iter)
        {
            tstring progressName = "Progress";
            char szbuf[10];
            _itoa_s(iter - progressHotKeyList_.begin() + 1, szbuf, 10);
            progressName += szbuf;

            WinDefine::SetConfigValue(CONFIG_SET_HOTKEY,
                progressName, iter->Path->GetWindowText(), CONFIG_INF_FILENAME);            
            {
                string hotKeyName = "ProgressHotKey" + string(szbuf);
                TCHAR hotkeyStr[20];
                _itot_s(iter->StartKey->GetHotKeyCode(), hotkeyStr, 10);
                if (iter->StartKey->GetHotKeyCode() == -1)
                {
                    hotkeyStr[0] = 0;
                }
                WinDefine::SetConfigValue(CONFIG_SET_HOTKEY,
                    hotKeyName, hotkeyStr, CONFIG_INF_FILENAME);  
            }
            {
                string killhotKeyName = "ProgressKillHotKey" + string(szbuf);
                TCHAR hotkeyStr[20];
                _itot_s(iter->KillKey->GetHotKeyCode(), hotkeyStr, 10);
                if (iter->KillKey->GetHotKeyCode() == -1)
                {
                    hotkeyStr[0] = 0;
                }
                WinDefine::SetConfigValue(CONFIG_SET_HOTKEY, 
                    killhotKeyName, hotkeyStr, CONFIG_INF_FILENAME); 
            }
        }
    }

    if (saveEvent_)
    {
        saveEvent_(this);
    }
}

LRESULT CSetDialog::OnCreate(WPARAM wParam, LPARAM lParam, BOOL& handle)
{
    OnInitHotKey(wParam, lParam, handle);

    wingy_ui::WRect rect = GetClientRect();
    save_->Create("±£´æ", rect.Width() - 100, rect.Height() - 80, 80, 25, hWnd_);
    save_->SetLBtnDownEvent(std::bind(&CSetDialog::OnSaveLBtndown,
        this, std::placeholders::_1));
    return FALSE;
}

HWND CSetDialog::Create(const tstring& windowText, int x, int y, int nWidth,
    int nHeight, HWND hWndParent)
{
    WNDCLASS wcs = {0};
    wcs.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcs.hInstance = GetModuleHandle(NULL);
    wcs.lpfnWndProc = StaticWndProc;
    wcs.lpszClassName = _T("WWingySetDialog");
    wcs.style = CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wcs.hIcon = (HICON)LoadImage(NULL,
        GlobalResource::GetInstance()->iconPath_.c_str(),
        IMAGE_ICON,0,0,LR_LOADFROMFILE);
    wcs.hCursor = LoadCursor(NULL, IDC_ARROW);
    ATOM at = RegisterClass(&wcs);

    hWnd_ = ::CreateWindowEx(0, wcs.lpszClassName, windowText.c_str(), 
        WS_VISIBLE|WS_OVERLAPPEDWINDOW|
        WS_DLGFRAME|WS_SYSMENU|WS_CLIPCHILDREN/*|WS_VSCROLL|WS_HSCROLL*/,
        x, y, nWidth, nHeight, hWndParent, 
        NULL, wcs.hInstance, this);
    DWORD err = GetLastError();

    return hWnd_;
}

LRESULT CSetDialog::OnPaint( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    PAINTSTRUCT ps = {0};
    HDC dc = BeginPaint(hWnd_, &ps);
    MoveToEx(dc, 0, seprateLine_, nullptr);
    LineTo(dc, GetPos().right, seprateLine_);
    WRect rect(0, seprateLine_-10, GetPos().right, seprateLine_ + 10);
    ::DrawText(dc, _T("Progress Accelertor"),
        _tcslen(_T("Progress Accelertor")), &rect, DT_CENTER|DT_VCENTER);
    EndPaint(hWnd_, &ps);
    return FALSE;
}

LRESULT CSetDialog::OnSize( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    WRect rect = save_->GetPos();
    WRect setDlg = GetPos();
    rect.Move(setDlg.right - 100, setDlg.bottom - 40);
    save_->SetPos(rect);
    return FALSE;
}

LRESULT CSetDialog::OnDropFiles( WPARAM wParam, LPARAM lParam, BOOL& handle )
{
    return FALSE;
}
