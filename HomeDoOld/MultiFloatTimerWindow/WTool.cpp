#include "StdAfx.h"
#include "WTool.h"


WTool::WTool(void)
{
}


WTool::~WTool(void)
{
}



HBITMAP WTool::CreateDIBCompatibleBitmap(HDC hdc, int nWidth, int nHeight )
{   
    BYTE* pBits;
    BITMAPINFOHEADER          bmih;    
    ZeroMemory( &bmih, sizeof( BITMAPINFOHEADER ) );   

    bmih.biSize                 = sizeof( BITMAPINFOHEADER );    
    bmih.biWidth                = nWidth;
    bmih.biHeight               = nHeight;
    bmih.biPlanes               = 1 ;    
    bmih.biBitCount             = 32;//这里一定要是32    
    bmih.biCompression          = BI_RGB ;    
    bmih.biSizeImage            = 0 ;    
    bmih.biXPelsPerMeter        = 0 ;    
    bmih.biYPelsPerMeter        = 0 ;    
    bmih.biClrUsed              = 0 ;    
    bmih.biClrImportant         = 0 ;   
    HBITMAP hBitMap = CreateDIBSection( hdc, ( BITMAPINFO * )&bmih, 
        DIB_RGB_COLORS, ( VOID** )&pBits, NULL, 0 );   
    return hBitMap;   
}
