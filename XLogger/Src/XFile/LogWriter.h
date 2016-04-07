#pragma once
#include "..\Logger\MemWriter.h"
#include "TextFile.h"

class CLogWriter : public CMemWriter
{
public:
	CLogWriter(void);
	~CLogWriter(void);

	BOOL Open(LONG FileType, CString strAppName, CString strConfigue, 
		CString strSection, CString strLogName);
	void SetMonitor(LONG Handle, CString strFileName);
	CString GetFileName() const;
	LONG64 GetItemCount() const;
	int GetThreadId() const;
	BOOL ThreadRunning();
	void Close();

private:

	void DbSave();
	void TextSave();
	void TextSave(CFormatUtil &FormatUtil, CTextFile &TextFile, BYTE *Buffer, UINT Length);
	void SendToMonitor(BYTE *Buffer, UINT Length);
	static UINT WINAPI WriteThread(void* lpParam);
	void InitMonitor();

	CFileBase *m_pFileBase;

	HWND m_hMonitor;
	uintptr_t m_hThread;

	BOOL m_bSaveLog;
	INT m_iThreadId;
	INT m_iActiveSign;
	INT m_iLastSign;
	LONG m_lFileType;
	LONG64 m_iItemCount;
	CString m_strAppName;
	CString m_strConfigue;
	CString m_strSection;
	CString m_strLogName;
};
