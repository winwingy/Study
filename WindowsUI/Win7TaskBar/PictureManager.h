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


};

