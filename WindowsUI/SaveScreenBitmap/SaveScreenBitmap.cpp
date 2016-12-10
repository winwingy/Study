#include "stdafx.h"
#include <tchar.h>
#include "screen_capture.h"
#include <GdiplusImaging.h>

INT GetEncoderClsid(const WCHAR *format, CLSID *pClsid)  
{  
    UINT  num = 0;          // number of image encoders    
    UINT  size = 0;         // size of the image encoder array in bytes    

    Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;   

    Gdiplus::GetImageEncodersSize(&num, &size);   
    if(size == 0)   
        return -1;  // Failure    

    pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));   
    if(pImageCodecInfo == NULL)   
        return -1;  // Failure    

    GetImageEncoders(num, size, pImageCodecInfo);   

    for(UINT j = 0; j < num; ++j)   
    {   
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )   
        {   
            *pClsid = pImageCodecInfo[j].Clsid;   
            free(pImageCodecInfo);   
            return j;  // Success    
        }       
    }   

    free(pImageCodecInfo);   
    return -1;  // Failure   
}  

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    ScreenCapture sc;
    Bitmap* bp = nullptr;
    HWND hWnd = FindWindow(nullptr, L"ÂÞµÏ¼Î - RTX »á»°");
    sc.Capture(hWnd, &bp);
    CLSID pngClsid;  
    GetEncoderClsid(L"image/png", &pngClsid); 
    Gdiplus::Status sta = bp->Save(L"D:\\test\\Screen.png", &pngClsid);
    delete bp;
    return 0;
}