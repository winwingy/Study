#pragma once
#include "WTextLayerWnd.h"
class WTimeWnd : public WTextLayerWnd
{
public:
    WTimeWnd(void);
    ~WTimeWnd(void);

protected:
    virtual bool ProcessMessage(UINT msg, WPARAM wParam, 
        LPARAM lParam, LRESULT* result) override;
};

