#pragma once
#include "Parameter.h"
#include "FileBase.h"

class CHistoryManager
{
public:
	CHistoryManager(void);
	~CHistoryManager(void);

	BOOL CreateThread(); 
	BOOL CloseThread(); 

	BOOL Add(const CParameter &Parameter);
	BOOL Delete(const CParameter &Parameter);

private:

	BOOL m_bThreadRuning;
	DOUBLE m_dAllFileSize;
	uintptr_t m_hThreadHandle;
	CAtlArray<CParameter> m_arrParameter;

	static UINT WINAPI DeleteFilesThread(void* lpParam);

	BOOL Delete2(const CParameter &Parameter);
	BOOL GetParam(UINT iIndex, CParameter &Parameter) const;
	UINT GetParamCount() const;
	void DeleteFiles();
	void DeleteFiles(CParameter &Parameter);
	void DeleteFilesDays(const CParameter &Parameter, CAtlArray<BACKUPFILE> &arrFiles);
	void DeleteFilesSpace(const CParameter &Parameter, CAtlArray<BACKUPFILE> &arrFiles);
	void DeleteFilesSize(const CParameter &Parameter, CAtlArray<BACKUPFILE> &arrFiles);
	void DeleteFiles(const CParameter &Parameter, const BACKUPFILE &FileInfo) const;
	BOOL GetDiskFreeSpace(CString strPathName, LONGLONG &lFreeSpace) const;
	void XSleep(DWORD Milliseconds = 20) const { Sleep(Milliseconds); }
};
