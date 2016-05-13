#pragma once
#include <Windows.h>
namespace wingy_ui
{
class WRect : public RECT
{
public:
    WRect(void);
    WRect(int l, int t, int r, int b);
    WRect(const RECT& rect);
    ~WRect(void);
    int Width() const;
    int Height() const;
    operator RECT ()const;
    RECT GetRECT() const;
};
}


