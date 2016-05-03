// WLogger.cpp : WinMain µÄÊµÏÖ


#include "stdafx.h"
#include "resource.h"
#include "WLogger_i.h"
#include "xdlldata.h"
#include "Logger.h"


using namespace ATL;


class CWLoggerModule : public ATL::CAtlExeModuleT< CWLoggerModule >
{
public :
	DECLARE_LIBID(LIBID_WLoggerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WLOGGER, "{9E45D334-E31B-4C17-9623-4054FB22BD8F}")
	};

CWLoggerModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    MessageBox(nullptr, L"ss", L"ss", MB_OK);
	return _AtlModule.WinMain(nShowCmd);
}

