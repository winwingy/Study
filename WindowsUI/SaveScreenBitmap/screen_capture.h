#ifndef _SCREEN_CAPTURE_H_
#define _SCREEN_CAPTURE_H_
#include <windows.h>
#include <GdiPlus.h>
#include <memory>

using Gdiplus::Bitmap;
using Gdiplus::Graphics;

class ScreenCapture
{
public:
    ScreenCapture();
    ~ScreenCapture();

    bool Capture(HWND hWnd, Bitmap** bitmap);

};

#endif