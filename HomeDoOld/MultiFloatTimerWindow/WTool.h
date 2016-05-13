#pragma once
class WTool
{
public:
    WTool(void);
    ~WTool(void);
    HBITMAP CreateDIBCompatibleBitmap(HDC hdc, int nWidth, int nHeight );
};

