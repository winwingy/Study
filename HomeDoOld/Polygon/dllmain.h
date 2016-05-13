// dllmain.h : 模块类的声明。

class CPolygonModule : public CAtlDllModuleT< CPolygonModule >
{
public :
	DECLARE_LIBID(LIBID_PolygonLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_POLYGON, "{FF31C14C-2300-4BA9-9BD4-96737832A9EA}")
};

extern class CPolygonModule _AtlModule;
