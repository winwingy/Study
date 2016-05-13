#pragma once
#include <string>
#include <assert.h>
#ifdef _UNICODE
#define tstring std::wstring
#else
#define tstring std::string
#endif


class UIDefine
{
public:
    UIDefine(void);
    ~UIDefine(void);
};

