#include "formatutil.h"
#include "parameter.h"
#pragma once

struct BACKUPFILE {
	CString strFolder;		// 日志文件路径
	CString strFileName;	// 日志文件名称
	CString strPrimaryName;	// 去掉前缀以及后缀的日志文件名称
	FILETIME sCreateTime;	// 文件最后更改时间，或者是文件夹创建时间。
	DOUBLE dFileSize;		// 单位是兆
	BACKUPFILE() { dFileSize = 0; }
};

class CFileBase
{
public:
	CFileBase(void);
	virtual ~CFileBase(void);

	BOOL GetHandle(const SYSTEMTIME &Time, FILE** Handle);
	BOOL InitParameter(CString strAppPath, CString strFileName, CString strSection, LONG FileType);
	
	BOOL GetFileName(SYSTEMTIME Time, int iType, CString &strPath, CString &strFileName) const;
	BOOL GetFileName(SYSTEMTIME Time, int iType, char cPath[MAX_PATH], char cFileName[MAX_PATH]) const;
	
	CString GetFileName() const;
	
	void CloseFile();

	ULONG64 m_iFileSize;
	CParameter m_oParameter;

protected:

	friend class CFileSearcher;

	CFormatUtil m_oDynFolder;
	CFormatUtil m_oCycleName;
	CFormatUtil m_oCreateTime;
	CFormatUtil m_oSaveTime;
	SYSTEMTIME m_sLastTime;
	FILE* m_pFileHandle;
	ULONG64 m_iMaxFileSize;

	char m_cFileName[MAX_PATH];
	char m_cPathName[MAX_PATH];
	CString m_strFileName;
	CString m_strFolderName;
	LONG m_iBkCount;

	BOOL GetFileName(const SYSTEMTIME &Time, CString &strFileName);
	void FileBackup(const SYSTEMTIME &Time);
	void AdjustFileName(const SYSTEMTIME &Time, BOOL bAllFile = TRUE);
	BOOL CreateFolder(CString strFolder);
	void SetFileName(CString strFileName);
};
