#pragma once
#include <string>
#include <assert.h>
#include "exportUI.h"
#include <sstream>

#ifdef _UNICODE
#define tstring std::wstring
#define tstringstream std::wstringstream
#define __FUNCTIONT__ __FUNCTIONW__
#else
#define tstring std::string
#define tstringstream std::stringstream
#define __FUNCTIONT__ __FUNCTION__
#endif


#define TRACE_UI(x) do\
{\
    tstringstream ss;\
    ss << __FUNCTIONT__ << _T("  ") << __LINE__ << _T("  ") << x <<std::endl;\
    OutputDebugString(ss.str().c_str());\
}while(0);


namespace wingy_ui
{
class UI_API UIDefine
{
public:
    UIDefine(void);
    ~UIDefine(void);
};

}