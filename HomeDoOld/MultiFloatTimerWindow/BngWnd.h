#pragma once
#include "BaseLayerWnd.h"
class BngWnd : public BaseLayerWnd
{
public:
    BngWnd(void);
    ~BngWnd(void);

protected:
    virtual bool ProcessMessage(UINT msg, WPARAM wParam, 
        LPARAM lParam, LRESULT* result) override;

private:


};

