#pragma once
#include <Windows.h>
#include "WWindow.h"
#include <memory>
#include <vector>
#include <functional>

#include <string>

namespace wingy_ui
{
    class WStatic;
    class WHotKeyEdit;
    class WButton;
    class WEdit;
}

class CSetDialog : public wingy_ui::WWindow
{
public:
    CSetDialog(HWND mainWnd);
    ~CSetDialog(void);
    struct ProgressHotKeyList
    {
         std::shared_ptr<wingy_ui::WEdit> Path;
         std::shared_ptr<wingy_ui::WHotKeyEdit> StartKey;
         std::shared_ptr<wingy_ui::WHotKeyEdit> KillKey;
         std::shared_ptr<wingy_ui::WButton> DeleteHotKey;
    };
    virtual HWND Create(const tstring& windowText, int x, int y, int nWidth,
        int nHeight, HWND hWndParent) override;

    void SetSaveEvent(std::function<void (wingy_ui::WWindow*)> handle)
    { saveEvent_ = handle;}

protected:
    virtual LRESULT CSetDialog::OnCreate(WPARAM wParam, LPARAM lParam,
                                         BOOL& handle) override;
    virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    virtual LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam, BOOL& handle) override;
    LRESULT OnInitHotKey(WPARAM wParam, LPARAM lParam, BOOL& handle);
    virtual LRESULT OnPaint(WPARAM wParam, LPARAM lParam, BOOL& handle) override;


private:  
    void OnSaveLBtndown(WWindow* sender);
    void OnHotKeyKillFocusEvent(wingy_ui::WWindow* control);
    bool HasSameHotKey(wingy_ui::WHotKeyEdit* control);
    void OnProgressHotKeyDeleteClick(wingy_ui::WWindow* control);
    void test(int id);
    void testB();
    void DelayDeleteControl(wingy_ui::WWindow* control);
    void OnAddProgressButtonClick(wingy_ui::WWindow* control);


    std::shared_ptr<wingy_ui::WStatic> showTray_;
    std::shared_ptr<wingy_ui::WHotKeyEdit> showTrayHotKey_;

    std::shared_ptr<wingy_ui::WStatic> volumeUp_;
    std::shared_ptr<wingy_ui::WHotKeyEdit> volumeUpHotKey_;

	std::shared_ptr<wingy_ui::WStatic> volumeDown_;
	std::shared_ptr<wingy_ui::WHotKeyEdit> volumeDownHotKey_;

	std::shared_ptr<wingy_ui::WStatic> closeScreen_;
	std::shared_ptr<wingy_ui::WHotKeyEdit> closeScreenHotKey_;

	std::shared_ptr<wingy_ui::WStatic> killProcess_;
	std::shared_ptr<wingy_ui::WHotKeyEdit> killProcessHotKey_;

	std::shared_ptr<wingy_ui::WStatic> notScreenSave_;
	std::shared_ptr<wingy_ui::WHotKeyEdit> notScreenSaveHotKey_;

    int seprateLine_;
    std::vector<ProgressHotKeyList> progressHotKeyList_;
    std::shared_ptr<wingy_ui::WButton> addProgressButton_;

    std::shared_ptr<wingy_ui::WButton> save_;
    std::function<void (wingy_ui::WWindow*)> saveEvent_;

    HWND mainWnd_;
};

