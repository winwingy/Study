#include "StdAfx.h"
#include "TransparentTool.h"
#include <comdef.h>   //必须要加这个， GdiPlus用到里边的宏。。
#include <GdiPlus.h>
#include <Windows.h>
#include <WTypes.h>
#include <WindowsX.h>
#include <GdiplusHeaders.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")


TransparentTool::TransparentTool(void)
{
}


TransparentTool::~TransparentTool(void)
{
}


void TransparentTool::RemoveBorder(HWND hWnd)
{
    {
        LONG oldStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
        LONG removeStyle = WS_OVERLAPPED|WS_BORDER|WS_SYSMENU|WS_CAPTION
            |WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX;
        SetWindowLongPtr(hWnd, GWL_STYLE, oldStyle & ~(removeStyle));
    }

    {
        LONG oldStyleEx = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
        LONG removeStyleEx = WS_EX_RIGHTSCROLLBAR
            |WS_EX_LEFT|WS_EX_LTRREADING;
        SetWindowLongPtr(hWnd, GWL_EXSTYLE, oldStyleEx & ~removeStyleEx);
    }

    SetWindowPos(hWnd, HWND_TOP, 0, 0, 0, 0,
        SWP_NOSIZE|SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER|
        SWP_FRAMECHANGED);
    SetMenu(hWnd, nullptr);
}

HBITMAP TransparentTool::CreateDIBBitmap(HDC hdc, int width, int height)
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

void TransparentTool::InitGDIPlus()
{
    static bool init = false;
    if (!init)
    {
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR           gdiplusToken;

        // Initialize GDI+.
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    }
}

void TransparentTool::TransparentAlphaWnd( HWND hWnd, bool drawSomething, 
    ULW_VALUE ulwValue, COLORREF transparentRGB, int alphaDegree )
{
    LONG oldStyleEx = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    SetWindowLongPtr(hWnd, GWL_EXSTYLE, oldStyleEx | WS_EX_LAYERED);

    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);
    int clientWidth = clientRect.right - clientRect.left;
    int clientHeight = clientRect.bottom - clientRect.top;

    HDC hdc = GetDC(hWnd);
    HDC comatibleDC = CreateCompatibleDC(hdc);
    HBITMAP bitmap = CreateDIBBitmap(hdc, clientWidth, clientHeight);
    // HBITMAP bitmap = CreateCompatibleBitmap(hdc, clientWidth, clientHeight); //用这个grap.FillRectangle(&brush,100, 100, 200, 200);会出现透明。 奇怪！！！

    SelectObject(comatibleDC, bitmap);

    //GDI的alpha通道默认是0，所以最好用GDI+的一套

//     HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
//     SelectObject(comatibleDC, brush);
//     Rectangle(comatibleDC, 0, 0, 500, 300);
//     DeleteBrush(brush);


    Graphics grap(comatibleDC);
   // grap.ResetTransform(); 
   // Pen pen(Color::Black);
   // grap.DrawEllipse(&pen, 200, 200, 700, 300);
   // grap.DrawRectangle(&pen, 300, 400, 800, 700);
    SolidBrush brush(Color::Yellow);
    grap.FillRectangle(&brush,400, 100, 200, 200);
    grap.FillEllipse(&brush, 100, 100, 200, 300);
    Gdiplus::Font* fontA = new Gdiplus::Font(L"宋体", 58, FontStyleBold);
    grap.DrawString(L"Hello Transparent!!!", _tcslen(L"Hello Transparent!!!"),
        fontA, PointF(200.0, 50.0), &brush);  
    



    
    RECT windowsRect;
    ::GetWindowRect(hWnd, &windowsRect);
    POINT ptDes = {windowsRect.left, windowsRect.top};
    SIZE szDes = {clientWidth, clientHeight};
    POINT ptSrc = {0, 0};
    BLENDFUNCTION bfc = {0};
    bfc.AlphaFormat = 1;
    bfc.BlendFlags = 0;
    bfc.BlendOp = 0;
    bfc.SourceConstantAlpha = alphaDegree;

    ::UpdateLayeredWindow(hWnd, hdc, &ptDes, &szDes, comatibleDC, &ptSrc, 
        transparentRGB, &bfc, ulwValue);
    DWORD err = GetLastError();

    grap.ReleaseHDC(comatibleDC);
    DeleteDC(comatibleDC);
    DeleteObject(bitmap); 
    ReleaseDC(hWnd, hdc);
}
