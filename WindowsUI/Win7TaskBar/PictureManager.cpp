#include "StdAfx.h"
#include "PictureManager.h"
#include <memory>
#include <functional>
#pragma comment(lib, "Gdiplus.lib")


PictureManager::PictureManager(void)
{
    GdiplusStartupInput input;
    GdiplusStartup(&gdiToken_, &input, nullptr);
}


PictureManager::~PictureManager(void)
{
    GdiplusShutdown(gdiToken_);
}

Gdiplus::Bitmap* PictureManager::LoadBitmapFromRc(
    UINT rcID, int width, int height)
{
    HICON icon = reinterpret_cast<HICON>(LoadImage(
        GetModuleHandle(nullptr), MAKEINTRESOURCE(rcID), IMAGE_ICON,
        width, height, LR_SHARED));
    return Bitmap::FromHICON(icon);
}

bool CopyToDestGlobal(HGLOBAL globalSrc, HGLOBAL globalDest, int sizeRsc)
{
    auto UnlockResFun = [globalSrc](void*)
    {UnlockResource(globalSrc); };
    std::unique_ptr<void, std::function<void(void*)>>
        resPointer(LockResource(globalSrc), UnlockResFun);
    if (!resPointer)
    {
        return false;
    }

    auto GlobalUnlockFun = [globalDest](void*)
    {GlobalUnlock(globalDest); };
    std::unique_ptr<void, std::function<void(void*)>>
        destPointer(GlobalLock(globalDest), GlobalUnlockFun);
    if (!destPointer)
    {
        return false;
    }

    memcpy(destPointer.get(), resPointer.get(), sizeRsc);
    return true;
}

// 有BUG得到的 bitmap大小为0
Gdiplus::Bitmap* PictureManager::LoadBitmapFromResource( 
    HMODULE module, UINT resID, LPCTSTR resType )
{
    Gdiplus::Bitmap* bitmap = nullptr;
    do 
    {
        HRSRC hRsc = FindResourceW(module, MAKEINTRESOURCE(resID), resType);
        if (!hRsc)
        {
            DWORD err = GetLastError();
            break;
        }
        int sizeRsc = SizeofResource(module, hRsc);
        auto FreeResouceFun = [](HGLOBAL h)
        {
            FreeResource(h);
        };
        std::unique_ptr < std::remove_pointer<HGLOBAL>::type,
            std::function<void(HGLOBAL p) >>
            hGlobalRes(LoadResource(module, hRsc), FreeResouceFun);
        if (!hGlobalRes)
        {
            break;
        }

        auto GlobalFreeFun = [](HGLOBAL global)
        {
            //GlobalFree(global);
        };
        std::unique_ptr < std::remove_pointer<HGLOBAL>::type,
            std::function<void(HGLOBAL) >>
            global2(GlobalAlloc(GMEM_MOVEABLE, sizeRsc), GlobalFreeFun);
        if (!global2)
        {
            break;
        }

        if (!CopyToDestGlobal(hGlobalRes.get(), global2.get(), sizeRsc))
            break; 

        LPSTREAM stream;
        HRESULT hr = CreateStreamOnHGlobal(global2.get(), TRUE, &stream);
        if (!SUCCEEDED(hr))
        {
            break;
        }
        bitmap = Gdiplus::Bitmap::FromStream(stream);
    } while (0);
    return bitmap;
}

Gdiplus::Bitmap* PictureManager::ResizeBitmap(
    Bitmap* bitmapSrc, int destWidth, int destHeight, bool keepAspect)
{
    int srcWidth = bitmapSrc->GetWidth();
    int srcHeight = bitmapSrc->GetHeight();
    float scaleWidth = static_cast<float>(destWidth) / srcWidth;
    float scaleHeight = static_cast<float>(destHeight) / srcHeight;

    if (keepAspect)
    {
        if (scaleWidth > scaleHeight)
        {
            scaleWidth = scaleHeight;
        }
        else
        {
            scaleHeight = scaleWidth;
        }
    }

    destWidth = static_cast<int>(scaleWidth * srcWidth);
    destHeight = static_cast<int>(scaleHeight * srcHeight);
    Bitmap* destBitmap = new Bitmap(destWidth, destHeight, PixelFormat24bppRGB);
    Graphics graphics(destBitmap);
    graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
    graphics.DrawImage(bitmapSrc, Rect(0, 0, destWidth, destHeight),
                       0, 0, srcWidth, srcHeight, UnitPixel);
    graphics.Flush();
    return destBitmap;
}
