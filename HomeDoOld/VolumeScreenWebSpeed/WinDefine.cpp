#include "StdAfx.h"
#include "WinDefine.h"



WinDefine* WinDefine::windefine_ = NULL;

WinDefine::WinDefine(void)
{
	g_initVolume = 0;
	g_initVolumeConst = 0;
	g_perVoulumeGap = 10;
	g_iInitVolumeTime = 1000;
	g_iIsInitVolume = 1;
	g_bFinishInitVolume = FALSE;
	g_iGetWebTimeCnt = 0;
	powerOnStartProgressTime_ = 5000;
    notScreenSaveTime_ = 0;
    notScreenSaveCanTryCnt_ = 0;
}


WinDefine::~WinDefine(void)
{
}

WinDefine* WinDefine::GetInstance()
{
	if (windefine_ == NULL)
	{
		windefine_ = new WinDefine;
	}
	return windefine_;
}


tstring WinDefine::GetCongfigPath(const tstring& strFileName)
{
    char szPath[2048] = {0};
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    char* pDe = strrchr(szPath, '\\');
    tstring strPath;
    if ( pDe )
    {
        *pDe = 0;
        strPath.assign(szPath);
    }
    tstring strConfigPath = strPath + "\\" + strFileName;			
    FILE* fp = NULL;
    fopen_s(&fp, strConfigPath.c_str(), "r");
    if ( fp==NULL )
    {				  
        return "";
        //         UINT iPos = strPath.rfind("\\");
        //         strPath.erase(iPos);
        //         strConfigPath = strPath + "\\" + strFileName;	
    }
    else 
    {
        fclose(fp);
    }

    return strConfigPath;
}




tstring WinDefine::GetValueFromConfig(const tstring& strAppName, 
    const tstring& strKeyName, const tstring& strDefault, const tstring& strFileName)
{
    tstring strConfigPath = GetCongfigPath(strFileName);

    char szVolume[2048] = {0};
    GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(),
        strDefault.c_str(), szVolume, sizeof(szVolume)-1, strConfigPath.c_str() );	
    return szVolume;
}

int WinDefine::GetValueFromConfig(const tstring& strAppName, const 
    tstring& strKeyName, int strDefault, const tstring& strFileName)
{
    tstring strConfigPath = GetCongfigPath(strFileName);    
    return GetPrivateProfileInt(strAppName.c_str(), strKeyName.c_str(), 
        strDefault, strConfigPath.c_str());
}

void WinDefine::SetConfigValue(const tstring& strAppName, 
    const tstring& strKeyName, const tstring& text,
    const tstring& strFileName)
{
    tstring strConfigPath = GetCongfigPath(strFileName);
    WritePrivateProfileString(strAppName.c_str(), strKeyName.c_str(),
         text.c_str(), strConfigPath.c_str() );
}
