#include "stdafx.h"
#include "..\SaveScreenBitmap\screen_capture.h"

#pragma comment(lib, "GdiPlus.lib")
// using std::min;
// using std::max;
// using std::unique_ptr;
ULONG_PTR m_gdiplusToken;  

ScreenCapture::ScreenCapture()
{
    Gdiplus::GdiplusStartupInput StartupInput;  
    GdiplusStartup(&m_gdiplusToken,&StartupInput,NULL);  
}

ScreenCapture::~ScreenCapture()
{

}
// 
// bool ScreenCapture::Capture(HWND hWnd, Bitmap* bitmap)
// {
//     HDC desktop_dc = GetDC(nullptr);
//     bool success = false;
//     do
//     {
//         RECT window_rect = { };
//         if (!GetWindowRect(hWnd, &window_rect))
//             break;
// 
//         Graphics graphic(bitmap); 
//         HDC graphic_hdc = graphic.GetHDC();
//         success = !!BitBlt(graphic_hdc, window_rect.left, window_rect.top, 
//                            window_rect.right - window_rect.left, 
//                            window_rect.bottom - window_rect.top, desktop_dc, 0, 
//                            0, SRCCOPY);
//         graphic.ReleaseHDC(graphic_hdc);
//     } while (false);
//     ReleaseDC(nullptr, desktop_dc);
//     return success;
// }

bool ScreenCapture::Capture(HWND hWnd, Bitmap** bitmapRet)
{
    HDC desktop_dc = GetDC(nullptr);
    RECT rc = {0};
    GetWindowRect(hWnd, &rc);
    Bitmap* bitmap = new Bitmap(rc.right -rc.left, rc.bottom - rc.top);
    RECT window_rect = { };
    GetWindowRect(hWnd, &window_rect);
    bool success = false;
    do
    {
        Graphics graphic(bitmap); 
        HDC graphic_hdc = graphic.GetHDC();
        success = !!BitBlt(
            graphic_hdc, 0, 0, 
            rc.right -rc.left, rc.bottom - rc.top, 
            desktop_dc, 0, 0, SRCCOPY);
        graphic.ReleaseHDC(graphic_hdc);
    } while (false);
    ReleaseDC(nullptr, desktop_dc);
    *bitmapRet = bitmap->Clone(window_rect.left, window_rect.top, 
        window_rect.right - window_rect.left, 
        window_rect.bottom - window_rect.top, PixelFormat32bppARGB);
    delete bitmap;
    return success;
}