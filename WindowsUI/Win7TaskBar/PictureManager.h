#pragma once
#include <GdiPlus.h>
using namespace Gdiplus;
class PictureManager
{
public:
    PictureManager(void);
    ~PictureManager(void);
    Gdiplus::Bitmap* LoadBitmapFromResource(
        HMODULE module, UINT resID, LPCTSTR resType);

    // 有BUG得到的 bitmap大小为0
    Gdiplus::Bitmap* ResizeBitmap(Bitmap* bitmapSrc, int destWidth,
                                  int destHeight, bool keepAspect);
    
    Gdiplus::Bitmap* LoadBitmapFromRc(UINT rcID, int width, int height);
private:
    ULONG_PTR gdiToken_;

};

