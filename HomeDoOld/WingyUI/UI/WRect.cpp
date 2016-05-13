#include "StdAfx.h"
#include "WRect.h"


namespace wingy_ui
{

    WRect::WRect(void)
    {
    }

    WRect::WRect(int l, int t, int r, int b)       
    {
        left = l;
        top = t;
        right = r;
        bottom = b;
    }

    WRect::operator  RECT ()const
    {
        RECT rect = {left, top, right, bottom};
        return rect;
    }

    WRect::WRect(const RECT& rect)
    {
        left = rect.left;
        top = rect.top;
        right = rect.right;
        bottom = rect.bottom;
    }

    int WRect::Width() const
    {
        return right - left;
    }

    int WRect::Height() const
    {
        return bottom - top;
    }


    WRect::~WRect(void)
    {
    }

    RECT WRect::GetRECT() const
    {
        return (RECT)(*this);
    }

    void WRect::Move( int x, int y )
    {
        int width = Width();
        int height = Height();
        left = x;
        top = y;
        right = x + width;
        bottom = y + height;
    }
}
