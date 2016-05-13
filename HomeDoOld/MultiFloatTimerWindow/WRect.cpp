#include "StdAfx.h"
#include "WRect.h"


WRect::WRect(void)
{
}

WRect::WRect( int l, int t, int r, int b )
{
    left = l;
    top = t;
    right = r;
    bottom = b;
}

WRect::WRect( const RECT& rhs )
{
    left = rhs.left;
    top = rhs.top;
    right =rhs.right;
    bottom = rhs.bottom;
}


WRect::~WRect(void)
{
}

WRect::operator LPRECT ()
{
    return this;
}

POINT WRect::LeftTop() const
{
    POINT pt = {left, top};
    return pt;
}

POINT WRect::RightBottom() const
{
    POINT pt = {right, bottom};
    return pt;
}

SIZE WRect::GetSIZE() const
{
    SIZE sz = {Width(), Height()};
    return sz;
}
