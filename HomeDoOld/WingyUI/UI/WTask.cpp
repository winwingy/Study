#include "StdAfx.h"
#include "WTask.h"
#include "WWindow.h"
#include <tchar.h>

namespace wingy_ui
{
const int POST_TASK_MESSAGE_ID = 10909;
const int POST_TASK_MESSAGE_ID_B = 19999;
WTask* WTask::pThis_ = nullptr;

WTask::WTask(void)
    : id_(1)
    , hWnd_(nullptr)
    , oldProc_(0)
    , idFunction_(new std::map<int, std::function<void(int)>>)
    , idFunctionB_(new std::map<int, std::function<void()>>)
{

    
}


WTask::~WTask(void)
{
}

WTask* WTask::GetInstance()
{
    if (pThis_ == nullptr)
    {
        pThis_ = new WTask;
    }
    return pThis_;
}

bool WTask::PostTask( std::function<void (int)> taskFunction )
{
    if (hWnd_ == nullptr)
    {
        hWnd_ = Create(0, _T(""), 0, 0, 0, 0, 0, nullptr);
        assert(hWnd_);
    }
    (*idFunction_)[++id_] = taskFunction;
    return !!PostMessage(hWnd_, POST_TASK_MESSAGE_ID, id_, 0);
}

bool WTask::PostTaskB( std::function<void ()> taskFunction )
{
    if (hWnd_ == nullptr)
    {
        hWnd_ = Create(0, _T(""), 0, 0, 0, 0, 0, nullptr);
        assert(hWnd_);
    }
    (*idFunctionB_)[++id_] = taskFunction;
    return !!PostMessage(hWnd_, POST_TASK_MESSAGE_ID_B, id_, 0);
}

LRESULT WTask::ProcessMessage( UINT message, WPARAM wParam,
    LPARAM lParam, BOOL& handle )
{
    if (message == POST_TASK_MESSAGE_ID)
    {
        ((*idFunction_)[wParam])(wParam);
    }
    else if(message == POST_TASK_MESSAGE_ID_B)
    {
        ((*idFunctionB_)[wParam])();
    }
         
   return FALSE;
}

HWND WTask::Create( DWORD stleyEx, const tstring& windowText, DWORD dwStyle, 
    int x, int y, int nWidth, int nHeight, HWND hWndParent )
{
    WNDCLASSEX wcse = {0};
    wcse.cbSize = sizeof(wcse);
    wcse.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcse.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
    wcse.hInstance = GetModuleHandle(NULL);
    wcse.lpfnWndProc = StaticWndProc;
    wcse.lpszClassName = _T("WTaskClass");
    if(GetClassInfoEx(wcse.hInstance, wcse.lpszClassName, &wcse))
    {
        ::UnregisterClass(wcse.lpszClassName, wcse.hInstance);
    }
    ATOM at = RegisterClassEx(&wcse);
    assert(at);
    if (at == NULL)
    {
        return NULL;
    }

    hWnd_ = CreateWindowEx(stleyEx, wcse.lpszClassName, windowText.c_str(),
        dwStyle, x, y, nWidth, nHeight, hWndParent, NULL, 
        wcse.hInstance, (VOID*)this);
    assert(hWnd_);
    if (hWnd_ == NULL)
    {
        return hWnd_;
    }
    return hWnd_;
}

LRESULT CALLBACK WTask::StaticWndProc( HWND hWnd, UINT message, 
    WPARAM wParam, LPARAM lParam )
{
    BOOL handle = FALSE;
    WTask* thisClass = (WTask*)GetWindowLongPtr(hWnd, GWLP_USERDATA);   
    if (thisClass)
    {
        thisClass->hWnd_ = hWnd; 
    }    
    if (message == WM_NCCREATE)
    {
        CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)cs->lpCreateParams);

    }
    else
    {
        if (thisClass)
        {
            thisClass->ProcessMessage(message, wParam, lParam, handle);
        }
    }

    if (handle == FALSE)
    {
        if (thisClass && thisClass->oldProc_)
        {
            return CallWindowProc(thisClass->oldProc_, hWnd, message, 
                wParam, lParam);
        }
        else
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }        
    }
    return 0;
}

}