#include "StdAfx.h"
#include ".\myvolume.h"



const int MAX_VOL = 1000;

CMyVolume::CMyVolume(void)
{
}

CMyVolume::~CMyVolume(void)
{
}

//获得音量部分, 目前只支持XP的主音量, 要完美支持看D:\workspace\DownLoadCode\VolumeCtrl
int CMyVolume::GetVolume()
{
	long device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	unsigned int rt = 0;
	MIXERCONTROL volCtrl;
	HMIXER hmixer;

	if ( mixerOpen(&hmixer, 0, 0, 0, 0) != MMSYSERR_NOERROR )
	{
		return 0;
	}
	GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl);

	int iValue = GetVolumeValue(hmixer, &volCtrl);
	iValue = iValue * MAX_VOL / volCtrl.Bounds.lMaximum;
	return iValue;
}

int CMyVolume::GetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc)
{
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue = 0;
	MIXERCONTROLDETAILS mxcd;
	mxcd.hwndOwner = 0;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if (mixerGetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER | MIXER_GETCONTROLDETAILSF_VALUE))
	{
		return -1;
	}
	return vol.dwValue;

}


BOOL  CMyVolume::GetVolumeControl(HMIXER   hmixer   ,long   componentType,long   ctrlType,MIXERCONTROL*   mxc) 
{
	MIXERLINE mxl;
	mxl.cbStruct = sizeof(mxl);
	mxl.dwComponentType = componentType;
	if (MMSYSERR_NOERROR == mixerGetLineInfo((HMIXEROBJ)hmixer, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE))
	{
		MIXERLINECONTROLS mxls;
		mxls.cbStruct = sizeof(mxls);
		mxls.dwLineID = mxl.dwLineID;
		mxls.dwControlType = ctrlType;
		mxls.cControls = 1;
		mxls.cbmxctrl = sizeof(MIXERCONTROL);
		mxls.pamxctrl = mxc;
		if (MMSYSERR_NOERROR == mixerGetLineControls((HMIXEROBJ)hmixer, &mxls, MIXER_GETLINECONTROLSF_ONEBYTYPE ))
		{
			return TRUE;
		}		
	}
	return FALSE;
}


//设置音量部分

BOOL CMyVolume::SetValue(int nVol)
{
	BOOL bSet = FALSE;
	HMIXER hmier;
	if (mixerOpen(&hmier, 0, 0, 0, 0))
	{
		return FALSE;
	}
	long device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	MIXERCONTROL volCtrl;
	BOOL bRet(FALSE);
	if ( GetVolumeControl(hmier, device, MIXERCONTROL_CONTROLTYPE_VOLUME, &volCtrl) )
	{
		nVol = nVol* volCtrl.Bounds.lMaximum/MAX_VOL;
		if ( SetVolumeValue(hmier,&volCtrl, nVol ) )
		{
			bRet = TRUE;			
		}

	}
	mixerClose(hmier);
	return bRet;
}

BOOL CMyVolume::SetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc, long volume)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_UNSIGNED vol;
	vol.dwValue = volume;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(vol);
	mxcd.paDetails = &vol;
	mxcd.cChannels = 1;
	if (mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE))
	{
		return FALSE;
	}
	return TRUE;
}





//设置/开启 静音
BOOL CMyVolume::SetMute(BOOL bMute)
{
	HMIXER hmixer;
	if ( mixerOpen(&hmixer, 0, 0, 0, 0) )
	{
		return FALSE;
	}
	
	long device = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	MIXERCONTROL volCtrl;
	GetVolumeControl(hmixer, device, MIXERCONTROL_CONTROLTYPE_MUTE, &volCtrl);
	BOOL rc(FALSE);
	if (SetMuteValue(hmixer, &volCtrl, (BOOL)bMute))
	{
		 rc = TRUE;
	}
	mixerClose(hmixer);
	return rc;

}

BOOL CMyVolume::SetMuteValue(HMIXER hmixer, MIXERCONTROL *mxc, BOOL mute)
{
	MIXERCONTROLDETAILS mxcd;
	MIXERCONTROLDETAILS_BOOLEAN mxcdMute;
	mxcdMute.fValue = mute;
	mxcd.hwndOwner = 0;
	mxcd.dwControlID = mxc->dwControlID;
	mxcd.cbStruct = sizeof(mxcd);
	mxcd.cbDetails = sizeof(mxcdMute);
	mxcd.paDetails = &mxcdMute;
	mxcd.cChannels = 1;
	mxcd.cMultipleItems = 0;
	if ( mixerSetControlDetails((HMIXEROBJ)hmixer, &mxcd, MIXER_OBJECTF_HMIXER|MIXER_SETCONTROLDETAILSF_VALUE) )
	{
		return FALSE;
	}
	return TRUE;
}




