#pragma once

enum FileEncoding {
	typeUNICODE = 0, 
	typeUTF8 = 1, 
	typeANSI = 2
};

class CParameter
{
public:
	CParameter(void);
	~CParameter(void);

	void operator=(const CParameter& Parameter);
	BOOL operator==(const CParameter& Parameter) const;

	BOOL m_bEncrypt;
	BOOL m_bCompress;
	UINT m_iBackupDays;
	UINT m_iMinDiskSpace;
	ULONG64 m_iFileSize;
	ULONG64 m_iAllFilesSize;
	FileEncoding m_nFileEncoding;
	char m_cFixFolder[MAX_PATH];
	char m_cCycleFolder[MAX_PATH];
	char m_cCycleName[MAX_PATH];
	char m_cCreateTime[MAX_PATH];
	char m_cSaveTime[MAX_PATH];
	char m_cExeName[MAX_PATH];
	char m_cLogFormat[MAX_PATH];
	BOOL m_bLogLevel[5];
	BOOL m_bDelete;
};
