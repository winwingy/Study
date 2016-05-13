#pragma once
#include <Windows.h>
class WRect : public RECT
{
public:
    WRect(void);
    WRect(int l, int t, int r, int b);
    WRect(const RECT& rhs);
    ~WRect(void);
    int Width() const {return right - left;}
    int Height() const {return bottom - top;}
    operator LPRECT ();
    POINT LeftTop() const;
    POINT RightBottom() const;
    SIZE GetSIZE() const;
};

