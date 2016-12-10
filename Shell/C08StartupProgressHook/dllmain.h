// dllmain.h : 模块类的声明。

class CC08StartupProgressHookModule : public ATL::CAtlDllModuleT< CC08StartupProgressHookModule >
{
public :
	DECLARE_LIBID(LIBID_C08StartupProgressHookLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_C08STARTUPPROGRESSHOOK, "{795D2BDC-2884-4C3D-A5B9-36135F2CC6FC}")
};

extern class CC08StartupProgressHookModule _AtlModule;
