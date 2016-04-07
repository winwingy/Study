#include "StdAfx.h"
#include "LogWriter.h"

#include "DbFile.h"
#include "TextFile.h"
#include "LogFactory.h"
#include "..\Logger\XLogUtil.h"

// 5秒没有任何数据，就关闭文件句柄
#define CLOSE_FILE_TIME       5000

void AddHistryInfo(const CParameter &Parameter);
void DeleteHistryInfo(const CParameter &Parameter);

CLogWriter::CLogWriter(void)
{
	m_bSaveLog    = FALSE;
	m_hThread     = NULL;
	m_pFileBase   = NULL;
	m_hMonitor    = 0;
	m_iItemCount  = 0;
	m_iActiveSign = 0;
	m_iLastSign   = 0;
	m_iThreadId   = -1;
}

CLogWriter::~CLogWriter(void)
{
	Close();
}

BOOL CLogWriter::Open(LONG FileType, 
					  CString strAppName, 
					  CString strConfigue, 
					  CString strSection, 
					  CString strLogName)
{
	if (NULL != m_hThread)
		return FALSE;

	m_lFileType   = FileType;
	m_strAppName  = strAppName;
	m_strConfigue = strConfigue;
	m_strSection  = strSection;
	m_strLogName  = strLogName;

	TCHAR Buffer[MAX_PATH] = { 0 };
#ifdef UNICODE
	wmemcpy(Buffer, strLogName, strLogName.GetLength());
#else
	memcpy(Buffer, strLogName, strLogName.GetLength());
#endif
	if (!CMemWriter::Create(Buffer))
		return FALSE;

	m_iItemCount  = 0;
	m_iActiveSign = 0;
	m_bSaveLog    = TRUE;

	unsigned int iThreadID = 0;
	m_hThread = _beginthreadex(NULL, 0, &CLogWriter::WriteThread, (void*)this, 0, &iThreadID);
	if (0 != m_hThread)
	{
		LOGGER_WRITE2(_T("创建存储线程成功:") << (LONG)m_hThread);
		return TRUE;
	}
	
	TCHAR ErrorMessage[ERROR_BUFFER_SIZE] = { 0 };
	GetErrorMessage(GetLastError(), ErrorMessage);
	LOGGER_WRITE2(_T("创建存储线程失败:") << ErrorMessage);

	return FALSE;
}

void CLogWriter::SetMonitor(LONG Handle, CString strFileName)
{
	ATLASSERT(NULL != m_pFileBase);
	if (NULL == m_pFileBase)
		return;

	if (m_pFileBase->GetFileName() == strFileName)
		m_hMonitor = (HWND)Handle;
}

void CLogWriter::Close()
{
	if (NULL == m_hThread)
		return;

	m_bSaveLog = FALSE;
	if (WAIT_TIMEOUT == WaitForSingleObject(
		(HANDLE)m_hThread, DWORD(1500)))
	{
		DWORD ExitCode = 0;
		TerminateThread((HANDLE)m_hThread, ExitCode);
		LOGGER_WRITE(_T("强制终止存储线程:") << (LONG)m_hThread);
	}
	CloseHandle((HANDLE)m_hThread);
	LOGGER_WRITE(_T("存储线程关闭:") << (LONG)m_hThread);
	m_hThread = NULL;

	CMemWriter::Close();
}

UINT WINAPI CLogWriter::WriteThread(void* lpParam)
{
	CLogWriter *Server = (CLogWriter*)lpParam;
	Server->m_iThreadId = GetCurrentThreadId();
	ATLASSERT(NULL != Server);
	LOGGER_WRITE2(_T("存储线程启动:") << Server->m_iThreadId);

	Server->m_iItemCount = 0;
	if (0 == Server->m_lFileType)
		Server->TextSave();
	else
		Server->DbSave();

	Server->m_hMonitor = 0;
	Server->m_pFileBase = NULL;

	LOGGER_WRITE2(_T("存储线程退出:") << Server->m_iThreadId 
		<< _T(". 存储记录:") << Server->m_iItemCount); 
	_endthreadex(0);

	return 0;
}

void CLogWriter::DbSave()
{
	BYTE DataBuffer[MAX_ITEM_BUFFER];
	SYSTEMTIME *sTime = (SYSTEMTIME*)DataBuffer; 
	DWORD dwTryTime = 0;
	UINT iLength; 

	int iIndex = m_strAppName.ReverseFind(_T('\\'));
	ATLASSERT(-1 != iIndex);
	CString strAppPath = m_strAppName.Left(iIndex + 1);

	CDbFile DbFile;
	m_pFileBase = &DbFile;
	DbFile.Open(m_lFileType, strAppPath, m_strConfigue, m_strSection);
	AddHistryInfo(DbFile.m_oParameter);
	InitMonitor();

	while (m_bSaveLog)
	{
		m_iActiveSign++;

		if (CMemWriter::Read(DataBuffer, iLength))
		{
			m_iItemCount++;
			DbFile.Write(*sTime, DataBuffer, iLength);
			SendToMonitor(DataBuffer, iLength);
			dwTryTime = 0;
			continue;
		}

		dwTryTime += m_iReadWait;
		if (dwTryTime > CLOSE_FILE_TIME) 
			DbFile.Close();
	}

	DbFile.Close();
	DeleteHistryInfo(DbFile.m_oParameter);
}

void CLogWriter::TextSave()
{
#pragma pack(push,1)
	struct HEADINFO {
		SYSTEMTIME sTime;
		DWORD dwProcessId;
		DWORD dwThreadId;
		DWORD dwLevel;
	};
#pragma pack(pop)

	BYTE DataBuffer[MAX_ITEM_BUFFER * 2] = { 0 };
	char TimeBuffer[MAX_ITEM_BUFFER] = { 0 };
	HEADINFO *sHeadInfo = (HEADINFO*)DataBuffer;
	char *LogData = (char*)(DataBuffer + sizeof(HEADINFO));
	DWORD dwTryTime = 0;
	UINT iLength = 0;

	CTextFile TextFile;
	m_pFileBase = &TextFile;
	int iIndex = m_strAppName.ReverseFind(_T('\\'));
	ATLASSERT(-1 != iIndex);
	TextFile.Open(m_strAppName.Left(iIndex + 1), m_strConfigue, m_strSection);
	AddHistryInfo(TextFile.m_oParameter);
	InitMonitor();

	CFormatUtil FormatUtil;
	FormatUtil.SetFormat(TextFile.m_oParameter.m_cLogFormat);
	BOOL bLogLevel[5];
	memcpy(bLogLevel, TextFile.m_oParameter.m_bLogLevel, sizeof(bLogLevel));
	while (m_bSaveLog)
	{
		USING_TIME_INFO;
		m_iActiveSign++;

		if (CMemWriter::Read(DataBuffer, iLength))
		{
			USING_TIME_INFO;
			m_iItemCount++;
			DataBuffer[iLength] = 0; // 填充结尾符。

			if (bLogLevel[sHeadInfo->dwLevel])
			{
				{USING_TIME_INFO;
				FormatUtil.FormatString(TimeBuffer, sHeadInfo->sTime, 
					sHeadInfo->dwProcessId, sHeadInfo->dwThreadId);}
				{USING_TIME_INFO;
				TextFile.Write(sHeadInfo->sTime, TimeBuffer, LogData);}
			}

			{USING_TIME_INFO;
			dwTryTime = 0;
			SendToMonitor(DataBuffer, iLength);}
		}
		else
		{
			USING_TIME_INFO;
			dwTryTime += m_iReadWait;
			if (dwTryTime > CLOSE_FILE_TIME)
				TextFile.Close();
		}		
	}
	TextFile.Close();
	DeleteHistryInfo(TextFile.m_oParameter);
} 

void CLogWriter::InitMonitor()
{
	if (NULL == m_pFileBase)
		return;

	SYSTEMTIME Time;
	GetLocalTime(&Time);

	FILE* Handle = NULL;
	m_pFileBase->GetHandle(Time, &Handle);
	CString FileName = m_pFileBase->GetFileName();

	UINT Count = CLogFactory::m_arrMonitor.GetCount();
	for (INT i = Count - 1; i >= 0; i--)
	{
		MONITOR Data;
		if (CLogFactory::m_arrMonitor.GetAt(i, Data)
			&& Data.FileName == FileName)
		{
			m_hMonitor = (HWND)Data.Handle;
			break;
		}
	}
}

__inline
void CLogWriter::SendToMonitor(BYTE *Buffer,
							   UINT Length)
{
	HWND hMonitor = m_hMonitor;
	if (0 == hMonitor)
		return;

	COPYDATASTRUCT Data;
	Data.dwData = 0;
	Data.cbData = Length;
	Data.lpData = Buffer;

	SendMessage(hMonitor, WM_COPYDATA, NULL, (LPARAM)&Data);
}

BOOL CLogWriter::ThreadRunning()
{
	if (0 != m_iLastSign && m_iLastSign != m_iActiveSign)
	{
		m_iLastSign = m_iActiveSign;
		return TRUE;
	}

	UINT WaitCount = 0;
	UINT SleepTime = 20;

	m_iLastSign = m_iActiveSign;
	while (WaitCount < (m_iReadWait * 4))
	{
		Sleep(SleepTime);
		WaitCount += SleepTime;

		if (m_iLastSign != m_iActiveSign)
		{
			m_iLastSign = m_iActiveSign;
			return TRUE;
		}
	}

	return FALSE;
}

LONG64 CLogWriter::GetItemCount() const
{
	return m_iItemCount;
}

CString CLogWriter::GetFileName() const
{
	CString strValue;

	if (NULL != m_pFileBase)
		strValue = m_pFileBase->GetFileName();

	return strValue;
}

int CLogWriter::GetThreadId() const
{
	return m_iThreadId;
}