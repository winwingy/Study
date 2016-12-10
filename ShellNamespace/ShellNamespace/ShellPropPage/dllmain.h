// dllmain.h : 模块类的声明。

class CShellPropPageModule : public ATL::CAtlDllModuleT< CShellPropPageModule >
{
public :
	DECLARE_LIBID(LIBID_ShellPropPageLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHELLPROPPAGE, "{9F47D0EB-165A-4A71-8594-EDBC7B56D2A3}")
};

extern class CShellPropPageModule _AtlModule;
