#pragma once
#include "WTextLayerWnd.h"
class WDateWnd : public WTextLayerWnd
{
public:
    WDateWnd(void);
    ~WDateWnd(void);
    virtual bool ProcessMessage( UINT msg, WPARAM wParam, 
        LPARAM lParam, LRESULT* result ) override;

protected:

};

