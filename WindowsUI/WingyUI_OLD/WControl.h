#pragma once
#include <Windows.h>
class WControl
{
public:
    WControl(void);
    virtual ~WControl(void);

private:
    int x_;
    int y_;
    int width;
    int height;
   
};

