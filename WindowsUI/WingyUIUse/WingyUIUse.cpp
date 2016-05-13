#include "../WingyUI/WContainer.h"
#include <Windows.h>
#include <tchar.h>
#pragma comment(lib, "../Debug/WingyUI.lib")

int APIENTRY _tWinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow)
{
    wingy_UI::WContainer container;
    container.Create(NULL);

    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}