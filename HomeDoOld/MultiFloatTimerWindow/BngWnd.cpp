#include "StdAfx.h"
#include "BngWnd.h"
#include <assert.h>
#include "WTool.h"
#include "Resource.h"
#include <WindowsX.h>


BngWnd::BngWnd(void)
{
}


BngWnd::~BngWnd(void)
{
}

bool BngWnd::ProcessMessage( UINT msg, WPARAM wParam, LPARAM lParam,
                             LRESULT* result )
{
    bool handle = false;
    switch(msg)
    {
    case WM_CREATE:
        {
            ::PostMessage(hWnd_, WM_PAINT, 0, 0);
            break;
        }        
    case WM_PAINT:
        {
            OutputDebugString(L"WM_PAINT\n");
            PAINTSTRUCT ps = {0};
            HDC hdc = BeginPaint(hWnd_, &ps);

            WRect clientRect = GetClientRect();
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP comBitmap = WTool().CreateDIBCompatibleBitmap(memDC, 
                clientRect.Width(), clientRect.Height());
            SelectBitmap(memDC, comBitmap);

            {
                HBITMAP bitmapPic = reinterpret_cast<HBITMAP>(LoadImage(
                    (HINSTANCE)GetModuleHandle(nullptr), 
                    MAKEINTRESOURCE(IDB_BITMAP1), IMAGE_BITMAP, 0, 0,
                    LR_CREATEDIBSECTION));
                assert(bitmapPic);  
                HBRUSH brushPic = CreatePatternBrush(bitmapPic);
                FillRect(memDC, clientRect, brushPic);
                DeleteBitmap(bitmapPic);
                DeleteBrush(brushPic);
            }

            {
                HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
                SelectBrush(memDC, brush);
                WRect rect(860, 30,1060, 80);
                rect = ScreenToCLient(rect);
                Rectangle(memDC, rect.left, rect.top, rect.right, rect.bottom );
                DeleteBrush(brush);
            }

            {
                //±³¾°PARGB
                BITMAP bitmapAlpha = {0};
                GetObject(comBitmap, sizeof(bitmapAlpha), &bitmapAlpha);
                for (int h = 0; h < bitmapAlpha.bmHeight; ++h)
                {
                    for (int w = 0; w < bitmapAlpha.bmWidth; ++w)
                    {
                        int* pixel = (int*)((BYTE*)bitmapAlpha.bmBits +
                            bitmapAlpha.bmWidthBytes * h) + w;
                        int a = (GetRValue(*pixel) + GetGValue(*pixel) + 
                            GetBValue(*pixel))/3;
                        int r = GetRValue(*pixel)*a/255;
                        int g = GetGValue(*pixel)*a/255;
                        int b = GetBValue(*pixel)*a/255;
                        *pixel = a<<24 | RGB(r, g, b);
                    }
                }
            }

            BLENDFUNCTION bfc = {0};
            bfc.AlphaFormat = AC_SRC_ALPHA;
            bfc.SourceConstantAlpha = alpha_;
            BOOL upd = UpdateLayeredWindow(hWnd_, hdc, &GetWindowRect().LeftTop(),
                &GetWindowRect().GetSIZE(), memDC, &GetClientRect().LeftTop(), 
                0, &bfc, ULW_ALPHA);
            EndPaint(hWnd_, &ps);
            DeleteBitmap(comBitmap);
            DeleteDC(memDC);         
            handle = true;
            break;
        }
    }

    if (!handle)
    {
        return __super::ProcessMessage(msg, wParam, lParam, result);
    }
    return handle;
}
