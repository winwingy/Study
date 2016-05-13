#pragma once
#include <MMSystem.h>
#pragma comment(lib, "Winmm.lib")
class CMyVolume
{
public:
	CMyVolume(void);
	~CMyVolume(void);
	static int GetVolume();
	static int GetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc);
	static BOOL GetVolumeControl(HMIXER   hmixer   ,long   componentType,long   ctrlType,MIXERCONTROL*   mxc) ;
	
	static BOOL SetValue(int nVol);
	static BOOL SetVolumeValue(HMIXER hmixer, MIXERCONTROL *mxc, long volume);
	//…Ë÷√/ø™∆Ù æ≤“Ù
	static BOOL SetMute(BOOL bMute);
	static BOOL SetMuteValue(HMIXER hmixer, MIXERCONTROL *mxc, BOOL mute);

};
