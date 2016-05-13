#ifndef _WCONTAINER_H_
#define _WCONTAINER_H_
#include <Windows.h>
#include "WUIDefine.h"

namespace wingy_UI
{

class WContainer
{
public:
    WContainer(void);
    virtual ~WContainer(void);
    bool Create(HWND hwnd);


private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, 
        LPARAM lParam);

     HWND hwnd_;


};

}

#endif
