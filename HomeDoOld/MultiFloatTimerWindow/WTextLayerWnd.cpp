#include "StdAfx.h"
#include "WTextLayerWnd.h"
#include "WTool.h"
#include <WindowsX.h>


WTextLayerWnd::WTextLayerWnd(void)
{
}


WTextLayerWnd::~WTextLayerWnd(void)
{
}

bool WTextLayerWnd::ProcessMessage( UINT msg, WPARAM wParam,
    LPARAM lParam, LRESULT* result )
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
            OutputDebugString(L" WTextLayerWnd WM_PAINT\n");
            PAINTSTRUCT ps = {0};
            HDC hdc = BeginPaint(hWnd_, &ps);
 
            WRect clientRect = GetClientRect();
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP comBitmap = WTool().CreateDIBCompatibleBitmap(memDC, 
                clientRect.Width(), clientRect.Height());
            SelectBitmap(memDC, comBitmap);

            {
                HBRUSH brush = CreateSolidBrush(bkColor_);
                FillRect(memDC, clientRect, brush);
                DeleteBrush(brush);
            }
 
            std::wstring text = GetWindowText();
            SetBkMode(memDC, TRANSPARENT);
            ::SetTextColor(memDC, textColor_);
            ::DrawText(memDC, text.c_str(), text.length(),
                clientRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

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
