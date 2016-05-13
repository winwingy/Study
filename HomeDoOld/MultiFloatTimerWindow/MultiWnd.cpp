#include "StdAfx.h"
#include "MultiWnd.h"
#include "BngWnd.h"
#include "WTextLayerWnd.h"
#include "WDateWnd.h"
#include "WTimeWnd.h"


MultiWnd::MultiWnd(void)
    : bngWnd_(new BngWnd)
    , dateWnd_(new WDateWnd)
    , timeWnd_(new WTimeWnd)
{
}


MultiWnd::~MultiWnd(void)
{
}



void MultiWnd::Show()
{
    int xScreen = GetSystemMetrics(SM_CXSCREEN);
    int yScreen = GetSystemMetrics(SM_CYSCREEN);

    int bngW = 300;
    int bngH = 100;
    int bngHOri = 5;
    bngWnd_->Create(nullptr, (xScreen-bngW)/2, bngHOri, bngW, bngH);
    bngWnd_->SetAlpha(100);
    bngWnd_->ShowWindow(SW_SHOW); 
    bngWnd_->SetWindowLongPtr(GWL_EXSTYLE, 
        WS_EX_TOPMOST|bngWnd_->GetWindowLongPtr(GWL_EXSTYLE));
    bngWnd_->InvalidateRect(); 
   
    
    int timeW = 200;
    int timeH = 50;
    int timeHOri = 30;
    timeWnd_->Create(bngWnd_->GetHWND(), (xScreen - timeW)/2, timeHOri, 
        timeW, timeH);
    timeWnd_->SetWindowText(L"");
    timeWnd_->ShowWindow(SW_SHOW);
    timeWnd_->SetAlpha(255);
    timeWnd_->SetTextColor(RGB(0, 0, 255));
    timeWnd_->SetBkColor(RGB(100, 100, 0));
    timeWnd_->InvalidateRect();
   
//     int dateW = 200;
//     int dateH = 50;
//     int dateHOri = 60;
//     dateWnd_->Create(bngWnd_->GetHWND(), (xScreen - dateW)/2, dateHOri, 
//         dateW, dateH);
//     dateWnd_->SetWindowText(L"");
//     dateWnd_->ShowWindow(SW_SHOW);
//     dateWnd_->SetAlpha(255);
//     dateWnd_->SetTextColor(RGB(0, 0, 255));
//     dateWnd_->SetBkColor(RGB(0, 50, 50));
//     dateWnd_->InvalidateRect();



//    bngWnd_->AddDeferWindowPos(dateWnd_->GetHWND());
 //   bngWnd_->AddDeferWindowPos(timeWnd_->GetHWND());

//     dateWnd_->AddDeferWindowPos(bngWnd_->GetHWND());
//     dateWnd_->AddDeferWindowPos(timeWnd_->GetHWND());

    timeWnd_->AddDeferWindowPos(bngWnd_->GetHWND());
 //   timeWnd_->AddDeferWindowPos(dateWnd_->GetHWND());
}
