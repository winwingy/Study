#pragma once

#define SECOND_COUNT       60
#define MINUTE_COUNT       60
#define HOUR_COUNT         24

class CStateMonitor
{
public:
	CStateMonitor(void);
	~CStateMonitor(void);

	void InitMonitor();

private:

	CString GetProcessUserName(HANDLE hProcess);

	void SetHour(BOOL bNext = TRUE);
	void SetMinute();
	void SetSecond();
	void SaveToFile();
	void SaveToFile(FILE* hFileHandle);

	static UINT WINAPI MonitorThread(void* lpParam);

	char m_pcProcessUserName[MAX_PATH];

	BOOL m_bThreadRuning;
	uintptr_t m_hThreadHandle;

	int m_iSecondIndex;
	int m_iMinuteIndex;
	int m_iHourIndex;

	SYSTEMTIME m_sBeginTime;
	CTime m_sCreateTime;

	BOOL m_bSaveError[MAX_SERVER_COUNT];

	BOOL m_bUsable[MAX_SERVER_COUNT];
	LONG64 m_iBaseCount[MAX_SERVER_COUNT];
	double m_iSecondCount[MAX_SERVER_COUNT][SECOND_COUNT];
	double m_iMinuteCount[MAX_SERVER_COUNT][MINUTE_COUNT];
	double m_iHourCount[MAX_SERVER_COUNT][HOUR_COUNT];

	CString m_strFileName[MAX_SERVER_COUNT];

	WCHAR m_pcFileName[MAX_PATH];
	WCHAR m_pcBkFileName[MAX_PATH];
};
