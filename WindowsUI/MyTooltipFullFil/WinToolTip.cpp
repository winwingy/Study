#include "StdAfx.h"
#include "WinToolTip.h"
#include <comdef.h>
#include <GdiPlus.h>
#include <WindowsX.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")


int ConvertWinFontHeightToNeg(int nHeight)
{
    if (nHeight > 0)
    {
        HDC dc = ::GetDC(nullptr);
        int nCaps = ::GetDeviceCaps(dc, LOGPIXELSY);
        if (nCaps == 0)
            nCaps = 72;
        ::ReleaseDC(nullptr, dc);

        return -::MulDiv(nHeight, nCaps, 72);
    }

    return nHeight;
}


int GetTextWidth(const std::wstring& text, SIZE* sz)
{
    if (text.empty())
    {
        return 0;
    }

    LOGFONTW lf = {0};
    lf.lfHeight = ConvertWinFontHeightToNeg(12);
    lf.lfCharSet = GB2312_CHARSET;
    _tcscpy_s(lf.lfFaceName, L"ËÎÌו");
    HFONT font = CreateFontIndirect(&lf);
    HDC cpDC = ::CreateCompatibleDC(nullptr);
    HGDIOBJ oldFont = SelectObject(cpDC, font);
    GetTextExtentPoint(cpDC, text.c_str(), text.size(), sz);

    SelectObject(cpDC, oldFont);
    DeleteObject(font);
    DeleteDC(cpDC);
    return sz->cx;
}

void CalWindowsRect(const std::wstring& text, SIZE* wndSz)
{
    int marginX = 3;
    int marginY = 3;
    SIZE sz = {0};
    GetTextWidth(text, &sz);
    wndSz->cx = marginX * 2 + sz.cx;
    wndSz->cy = marginY * 2 + sz.cy;

}

WinToolTip* WinToolTip::winToolTip_= nullptr;
WinToolTip::WinToolTip(void)
    : hWnd_(nullptr)
    , ptrID_(nullptr)
{
}


WinToolTip::~WinToolTip(void)
{
}

WinToolTip* WinToolTip::GetInstance()
{
    if (winToolTip_ == nullptr)
    {
        winToolTip_ = new WinToolTip;
    }
    return winToolTip_;
}

HBITMAP CreateDIBBitmap(HDC hdc, int width, int height)
{
    BITMAPINFOHEADER bih = {0};
    bih.biSize = sizeof(bih);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;
    BYTE* pBits;   
    HBITMAP bitmap = static_cast<HBITMAP>(CreateDIBSection(hdc, 
        reinterpret_cast<BITMAPINFO*>(&bih), 
        DIB_RGB_COLORS, reinterpret_cast<void**>(&pBits), nullptr, 0));
    return bitmap;
}




bool WinToolTip::WndProc(UINT message, WPARAM wParam, 
    LPARAM lParam, LRESULT* res)
{
    switch(message)
    {
    case WM_NCHITTEST:
        {
            *res = HTTRANSPARENT;
            return TRUE;
            break;
        }
    case WM_PAINT:
        {
            PAINTSTRUCT ps = {0};
            HDC hdc = BeginPaint(hWnd_, &ps);
            HDC memDC = CreateCompatibleDC(hdc);

            RECT rect = {0};
            GetWindowRect(hWnd_, &rect);
            HBITMAP bitmap = CreateDIBBitmap(hdc, rect.right - rect.left,
                rect.bottom - rect.top);
            SelectBitmap(memDC, bitmap);

            Graphics graphics(memDC);
            SolidBrush brush(Color::YellowGreen);
            graphics.FillRectangle(&brush, 0, 0, rect.right - rect.left
                , rect.bottom - rect.top);
            Gdiplus::Font font(L"ËÎÌו", 12);
            graphics.DrawString(info_.c_str(), info_.length(), &font, 
                PointF(0.0, 0.0), &brush);

            POINT pointDst = {rect.left, rect.top};
            SIZE szDst = {rect.right - rect.left, rect.bottom - rect.top};
            POINT pointSrc = {0, 0};
            BLENDFUNCTION bfc = {0};
            bfc.AlphaFormat = 1;
            bfc.SourceConstantAlpha = 125;
            UpdateLayeredWindow(hWnd_, hdc, &pointDst, &szDst, memDC, &pointSrc,
                0, &bfc, ULW_ALPHA);

            DeleteBitmap(bitmap);
            DeleteDC(memDC);
            EndPaint(hWnd_, &ps);
            break;
        }
    }

    return false;
}

LRESULT CALLBACK WinToolTip::StaticWndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    WinToolTip* winToolTip = nullptr;
    bool handle = false; 
    LRESULT res = FALSE;
    if (message == WM_NCCREATE)
    {
        LPCREATESTRUCT lcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        winToolTip = reinterpret_cast<WinToolTip*>(lcs->lpCreateParams);
        SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG)winToolTip);
    }
    else 
    {
        winToolTip = 
            reinterpret_cast<WinToolTip*>(GetWindowLongPtr(hWnd, GWL_USERDATA));
        if (winToolTip)
        {
            handle = winToolTip->WndProc(message, wParam, lParam, &res);
        }
    }

    if (handle == false)
    {
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return res;
}

void InitGdiPlus()
{
    static bool init = false;
    if (init == false)
    {
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);
    }
}

void WinToolTip::Create()
{
    WNDCLASSEX wcs = {0};
    wcs.cbSize = sizeof(wcs);
    wcs.hInstance = (HINSTANCE)GetModuleHandle(nullptr);
    wcs.lpfnWndProc = (WNDPROC)StaticWndProc;
    wcs.lpszClassName = L"WinToolTip";
    ATOM at = RegisterClassEx(&wcs);

    hWnd_ = CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_LAYERED|WS_EX_TOPMOST, 
        wcs.lpszClassName, L"", WS_POPUP, -10, -10, -1, -1, nullptr, nullptr, 
        wcs.hInstance, this);    
}

void WinToolTip::ShowTips( const std::wstring& info, void* ptrID )
{
    if (hWnd_ == nullptr)
    {
        Create();
        InitGdiPlus();
    }

    if(info.empty())
    {
        info_ = info;
        ptrID_ = ptrID; 
        ShowWindow(hWnd_, SW_HIDE);
    }
    else if (info != info_ || ptrID_ != ptrID)
    { 
        info_ = info;
        ptrID_ = ptrID;             
        POINT pt;
        GetCursorPos(&pt);
        SIZE wndSz = {0};
        CalWindowsRect(info, &wndSz);
        MoveWindow(hWnd_, pt.x, pt.y, wndSz.cx, wndSz.cy, TRUE);
        ShowWindow(hWnd_, SW_SHOWMINNOACTIVE);
        ::SendMessage(hWnd_, WM_PAINT, 0, 0);
    }
    else if (info.size()>0)
    {
        POINT pt;
        GetCursorPos(&pt);
        SIZE wndSz = {0};
        CalWindowsRect(info, &wndSz);
        MoveWindow(hWnd_, pt.x, pt.y, wndSz.cx, wndSz.cy, TRUE);
    }

  
}

void WinToolTip::ClientToScreen( HWND hwnd, RECT* rect )
{
    POINT lt = {rect->left, rect->top};
    POINT rb = {rect->right, rect->bottom};
    ::ClientToScreen(hwnd, &lt);
    ::ClientToScreen(hwnd, &rb);
    RECT rectTmp = {lt.x, lt.y, rb.x, rb.y};
    *rect = rectTmp;
}
