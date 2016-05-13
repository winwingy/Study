#include "StdAfx.h"
#include "BaseLayerWnd.h"
#include <assert.h>


BaseLayerWnd::BaseLayerWnd(void)
    : hWnd_(nullptr)
    , deferPosList_()
    , oldWndRect_()
    , oldWndPoint_ ()
    , alpha_(255)
    , textColor_(RGB(1, 1, 1))
    , bkColor_(RGB(255, 255, 255))
{

}


BaseLayerWnd::~BaseLayerWnd(void)
{
}

HWND BaseLayerWnd::Create( HWND hWnd, int x, int y, int cx, int cy )
{
    WNDCLASSEX wcsx = {0};
    wcsx.cbSize = sizeof(wcsx);
    wcsx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcsx.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
    wcsx.lpfnWndProc = WNDPROC(staticWndProc);
    wcsx.lpszClassName = L"BaseLayerWnd";
    wcsx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcsx.style = CS_VREDRAW|CS_HREDRAW;

    {
        WNDCLASS wcsInfo = {0};
        BOOL reg = ::GetClassInfo(wcsx.hInstance, wcsx.lpszClassName, &wcsInfo);
        if (reg && wcsInfo.lpfnWndProc != wcsx.lpfnWndProc)
        {
            ::UnregisterClass(wcsx.lpszClassName, wcsx.hInstance);
        }
    }
    RegisterClassEx(&wcsx);

    HWND hWnd_ = CreateWindowEx(WS_EX_LAYERED, wcsx.lpszClassName, L"", 
        WS_POPUP|WS_CLIPSIBLINGS|WS_CLIPCHILDREN, x, y, cx, cy, hWnd, nullptr,
        wcsx.hInstance, this);
    assert(hWnd_);
    return hWnd_;    
}

LRESULT CALLBACK BaseLayerWnd::staticWndProc( HWND hWnd, UINT msg, 
    WPARAM wParam, LPARAM lParam )
{
    BaseLayerWnd* pThis = nullptr;
    LRESULT result = S_OK;
    if (msg == WM_NCCREATE)
    {
        LPCREATESTRUCT ps = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pThis = reinterpret_cast<BaseLayerWnd*>(ps->lpCreateParams);
        pThis->hWnd_ = hWnd;
        ::SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        result = TRUE;
    }
    else
    {
        pThis = reinterpret_cast<BaseLayerWnd*>(
            ::GetWindowLongPtr(hWnd, GWL_USERDATA));
    }

    bool handle = false;
    if (pThis)
    {
        handle = pThis->ProcessMessage(msg, wParam, lParam, &result);
    }

    if (!handle)
    {
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return result;
}
 

bool BaseLayerWnd::ProcessMessage( UINT msg, WPARAM wParam, 
    LPARAM lParam, LRESULT* result )
{
    bool handle = false;
    switch (msg)
    {
    case WM_NCHITTEST:
        {
            *result = HTCAPTION;
            handle = true;
            break;
        }
    case WM_CLOSE:
        {
            PostQuitMessage(0);
            break;
        }
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
        {   
            OutputDebugString(L"WM_LBUTTONDOWN\n");
            GetCursorPos(&oldWndPoint_);
            break;
        }
    case WM_LBUTTONUP:
    case WM_NCLBUTTONUP:
        {
            OutputDebugString(L"WM_LBUTTONUP\n");
            break;
        }
        //case WM_ACTIVATE
    case WM_MOVE:
        {
            OutputDebugString(L"WM_MOVE");
            if (GetCapture() == hWnd_)
            {
                POINT cursor = {0};
                GetCursorPos(&cursor);
                int xDelta = cursor.x - oldWndPoint_.x;
                int yDelta = cursor.y - oldWndPoint_.y;
                for (auto& wnd = deferPosList_.begin(); wnd != deferPosList_.end()
                    ; ++ wnd)
                {
                    wnd->second.left += xDelta;
                    wnd->second.right += xDelta;
                    wnd->second.top += yDelta;
                    wnd->second.bottom += yDelta;
                    SetWindowPos(wnd->first, HWND_NOTOPMOST, 
                        wnd->second.left + xDelta, 
                         wnd->second.top + yDelta,
                         0, 0,           
                        SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
                }
                oldWndPoint_ = cursor;
            }
            else
            {
                WRect newRect = GetWindowRect();
                int xDelta = newRect.left - oldWndRect_.left;
                int yDelta = newRect.top - oldWndRect_.top;
                for (auto& wnd = deferPosList_.begin(); wnd != deferPosList_.end()
                    ; ++ wnd)
                {
                    wnd->second.left += xDelta;
                    wnd->second.right += xDelta;
                    wnd->second.top += yDelta;
                    wnd->second.bottom += yDelta;
                }
            }
            oldWndRect_ = GetWindowRect();
            break;
        }
    }
    return handle;
}

BOOL BaseLayerWnd::ShowWindow(__in int nCmdShow )
{
    return ::ShowWindow(hWnd_, nCmdShow);
}

WRect BaseLayerWnd::GetClientRect()
{
    WRect rect;
    ::GetClientRect(hWnd_, rect);
    return rect;
}

WRect BaseLayerWnd::GetWindowRect()
{
    WRect rect;
    ::GetWindowRect(hWnd_, rect);
    return rect;
}

WRect BaseLayerWnd::ClientToScreen( const WRect& rect )
{
    POINT lt = rect.LeftTop();
    ::ClientToScreen(hWnd_, &lt);
    POINT rb = rect.RightBottom();
    ::ClientToScreen(hWnd_, &rb);
    return WRect(lt.x, lt.y, rb.x, rb.y);
}

void BaseLayerWnd::SetWindowText( std::wstring text )
{
    ::SetWindowText(hWnd_, text.c_str());
}

std::wstring BaseLayerWnd::GetWindowText()
{
    TCHAR buf[1004];
    ::GetWindowText(hWnd_, buf, sizeof(buf));
    return buf;
}

void BaseLayerWnd::InvalidateRect()
{
    PostMessage(hWnd_, WM_PAINT, 0, 0);
}

void BaseLayerWnd::AddDeferWindowPos( HWND hWnd )
{
    oldWndRect_ = GetWindowRect();
    if (!::IsWindow(hWnd))
    {
        assert(L"!::IsWindow(hWnd)");
        return;
    } 
    RECT rect = {0};
    ::GetWindowRect(hWnd, &rect);
    if (deferPosList_.find(hWnd) == deferPosList_.end())
    {      
        deferPosList_.insert(std::make_pair(hWnd, WRect(rect)));
    }
    else
    {
        deferPosList_[hWnd] = WRect(rect);
    }
}

void BaseLayerWnd::BeforeDeferWindowPos()
{
    oldWndRect_ = GetWindowRect();
}

void BaseLayerWnd::AfterDeferWindowPos()
{
    RECT rect = GetWindowRect();
    int xDelta = rect.left - oldWndRect_.left;
    int yDelta = rect.top - oldWndRect_.top;
    for (auto& wnd = deferPosList_.begin(); wnd != deferPosList_.end(); )
    {
        if (!::IsWindow(wnd->first))
        {
            wnd = deferPosList_.erase(wnd);
        }
        else
        {
            RECT rectPer = {0};
            ::GetWindowRect(wnd->first, &rectPer);
            rectPer.left += xDelta;
            rectPer.top += yDelta;
            rectPer.right += xDelta;
            rectPer.bottom += yDelta;
            wnd->second = rectPer;
            ++wnd;
        }
    }
}

void BaseLayerWnd::SetWindowLongPtr( int nIndex, LONG_PTR dwNewLong )
{
    LONG_PTR ret = ::SetWindowLongPtr(hWnd_, nIndex, dwNewLong);
    InvalidateRect();
}

LONG_PTR BaseLayerWnd::GetWindowLongPtr( int nIndex )
{
    return ::GetWindowLongPtr(hWnd_, nIndex);
}

WRect BaseLayerWnd::ScreenToCLient( const WRect& rect )
{
    POINT lt = {rect.left, rect.top};
    ::ScreenToClient(hWnd_, &lt);
    POINT rb = {rect.right, rect.bottom};
    ::ScreenToClient(hWnd_, &rb);
    return WRect(lt.x, lt.y, rb.x, rb.y);
}


