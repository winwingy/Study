#pragma once
#include <Windows.h>
#include "exportUI.h"
namespace wingy_ui
{
class UI_API WRect : public RECT
{
public:
    WRect(void);
    WRect(int l, int t, int r, int b);
    WRect(const RECT& rect);
    ~WRect(void);
    int Width() const;
    int Height() const;
    void Move(int x, int y);
    operator RECT ()const;
    RECT GetRECT() const;
};
}


