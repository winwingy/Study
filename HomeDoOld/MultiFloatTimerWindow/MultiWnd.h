#pragma once
class BngWnd;
class WTextLayerWnd;

class MultiWnd
{
public:
    MultiWnd(void);
    ~MultiWnd(void);
    void Show();


private:
    BngWnd* bngWnd_;
    WTextLayerWnd* dateWnd_;
    WTextLayerWnd* timeWnd_;
};

