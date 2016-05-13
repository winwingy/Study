#pragma once
#include "BaseLayerWnd.h"

class WTextLayerWnd : public BaseLayerWnd
{
public:
    WTextLayerWnd(void);
    ~WTextLayerWnd(void);

protected:
    virtual bool ProcessMessage(UINT msg, WPARAM wParam, 
        LPARAM lParam, LRESULT* result) override;

};

