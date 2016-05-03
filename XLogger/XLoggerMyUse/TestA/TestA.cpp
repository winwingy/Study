// TestA.cpp : WinMain µÄÊµÏÖ


#include "stdafx.h"
#include "resource.h"
#include "TestA_i.h"
#include "xdlldata.h"


using namespace ATL;


class CTestAModule : public ATL::CAtlExeModuleT< CTestAModule >
{
public :
	DECLARE_LIBID(LIBID_TestALib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TESTA, "{EEC90053-A0F4-40CE-B63D-1602450F86FA}")
	};

CTestAModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
								LPTSTR /*lpCmdLine*/, int nShowCmd)
{
	return _AtlModule.WinMain(nShowCmd);
}

