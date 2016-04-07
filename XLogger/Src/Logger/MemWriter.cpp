#include <tchar.h>
#include <assert.h>

#include "MemWriter.h"
#include "SimpleLogger.h"
#include "XLogUtil.h"

const UINT CMemWriter::m_iHeaderSize = 50; 
const UINT CMemWriter::m_iReadWait   = 25;
const UINT CMemWriter::m_iWriteWait  = 1500;
const UINT CMemWriter::m_iUintSize   = sizeof(UINT);

CMemWriter::CMemWriter(void) 
{
	m_hRead       = NULL;
	m_hWrite      = NULL;
	m_iReadIndex  = NULL;
	m_iWriteIndex = NULL;
	m_iBuffFlush  = NULL;
}

CMemWriter::~CMemWriter(void)
{
	Close();
}

void CMemWriter::CreateEventName(TCHAR *pcBuffer, const TCHAR *pcName,
								 const TCHAR *pcSign) const
{
#pragma warning(push)
#pragma warning(disable:4996)
#ifdef UNICODE
	wmemset(pcBuffer, 0, MAX_PATH);
	wcscat(pcBuffer, pcName);
	wcscat(pcBuffer, pcSign);
#else
	memset(pcBuffer, 0, MAX_PATH);
	strcat(pcBuffer, pcName);
	strcat(pcBuffer, pcSign);
#endif
#pragma warning(pop)
}

BOOL CMemWriter::Create(TCHAR *pcName)
{
	UINT iBuffSize = m_iHeaderSize + (MAX_ITEM_BUFFER + m_iUintSize) * MAX_ITEM_COUNT;
	if (!CMemBuffer::Create(pcName, iBuffSize))
		return FALSE;

	TCHAR pcBuffer[MAX_PATH * 2] = { 0 };
	CreateEventName(pcBuffer, pcName, _T("_XLOGGER_WRITE_EVENT"));
	m_hWrite = CreateMutex(NULL, FALSE, pcBuffer);
	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		ResetEvent(m_hWrite);
		SetEvent(m_hWrite);
	}

	CreateEventName(pcBuffer, pcName, _T("_XLOGGER_READ_EVENT"));
	if ((m_hRead = CreateEvent(NULL, FALSE, TRUE, pcBuffer)) == NULL)
	{
		Close();
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 };
		GetErrorMessage(GetLastError(), Buffer);
		LOGGER_WRITE2(_T("Call CreateEvent FALSE! Object Name: ") 
			<< pcName << _T(". ") << Buffer);
		return FALSE;
	}

	m_iReadIndex  = (short*)m_pvMemAddress + 0;
	m_iWriteIndex = (short*)m_pvMemAddress + 1;
	m_iBuffFlush  = (LONG*)m_pvMemAddress  + 2;
	BYTE *pcBegin = (BYTE*)m_pvMemAddress + m_iHeaderSize;

	for (int i = 0; i < MAX_ITEM_COUNT; i++)
		m_pcBuffer[i] = pcBegin + (MAX_ITEM_BUFFER + m_iUintSize) * i;

	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		*m_iReadIndex  = -1;
		*m_iWriteIndex = -1;
		*m_iBuffFlush  = 0;
	}
	ResetEvent(m_hRead);

	return TRUE;
}

BOOL CMemWriter::Close()
{
	m_iReadIndex  = NULL;
	m_iWriteIndex = NULL;
	m_iBuffFlush  = NULL;

	if (NULL != m_hRead)
	{
		CloseHandle(m_hRead); 
		m_hRead = NULL;
	}

	if (NULL != m_hWrite)
	{
		CloseHandle(m_hWrite); 
		m_hWrite = NULL;
	}

	return CMemBuffer::Close();
}

int CMemWriter::Write(const BYTE *pcBuffer, 
					  UINT iLength, 
					  int &iFree)
{
	assert(NULL != m_hRead);
	assert(NULL != m_pvMemAddress);

	iFree = 0;
	int TryTimes = 0;
	DWORD Current = GetTickCount();

TRYSAVE:
	if (WAIT_TIMEOUT != WaitForSingleObject(m_hWrite, m_iWriteWait))
	{
		BOOL bRetVal = WriteInternal(pcBuffer, iLength, iFree);
		if (bRetVal)
			SetEvent(m_hRead);
		else
			LOGGER_WRITE2(_T("Buffer is full!!!"));

		ReleaseMutex(m_hWrite);
		return bRetVal ? 0 : 1;
	}

	LOGGER_WRITE2(_T("Write: WAIT_TIMEOUT!!! Wait:") << ULONG(GetTickCount() - Current)
		<< _T(" Time:") << ULONG(GetTickCount())
		<< _T(" ThreadId:") << GetCurrentThreadId());

	// 为什么强制解锁？防止其它客户端由于故障没有解锁。
	ReleaseMutex(m_hWrite);

	if (++TryTimes < 2)
		goto TRYSAVE; 

	LOGGER_WRITE2(_T("写数据遇到致命错误!!!"));
	return -1;
}

__inline
BOOL CMemWriter::WriteInternal(const BYTE *pcBuffer,
							   UINT iLength, 
							   int &iFree)
{
	short iIndex = *m_iWriteIndex + 1;
	if (MAX_ITEM_COUNT <= iIndex)
	{
		iIndex = 0;
		if (-1 == *m_iReadIndex)
			return FALSE;
	}

	UINT Count = 0;
	const UINT MaxCount = UINT(m_iWriteWait * 0.5);
	while (iIndex == *m_iReadIndex 
		&& MaxCount > ++Count)
		Sleep(1);

	if (MaxCount == Count)
		return FALSE; 

	BYTE* pcMemory = m_pcBuffer[iIndex];
	memcpy(pcMemory, &iLength, m_iUintSize); 
	memcpy(pcMemory + m_iUintSize, pcBuffer, iLength);	
	*m_iWriteIndex = iIndex;

	if (-1 != *m_iReadIndex)
	{
		iFree = *m_iReadIndex - iIndex;	
		if (0 > iFree)
			iFree = MAX_ITEM_COUNT + iFree;
		else if (0 == iFree)
			iFree = MAX_ITEM_COUNT;
		//else if (0 < iFree)
		//	iFree = iFree;
	}
	else
		iFree = MAX_ITEM_COUNT - iIndex - 1;

	return TRUE;
}

BOOL CMemWriter::Read(BYTE pcBuffer[MAX_ITEM_BUFFER], 
					  UINT &iLength) const
{
	if (*m_iWriteIndex == *m_iReadIndex || -1 == *m_iWriteIndex)
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_hRead, m_iReadWait))
			return FALSE;

		if (*m_iWriteIndex == *m_iReadIndex || -1 == *m_iWriteIndex)
			return FALSE;
	}

	*m_iReadIndex += 1;
	if (MAX_ITEM_COUNT <= *m_iReadIndex)
		*m_iReadIndex = 0;

	const BYTE* pcMemory = m_pcBuffer[*m_iReadIndex];
	memcpy(&iLength, pcMemory, m_iUintSize);
	memcpy(pcBuffer, pcMemory + m_iUintSize, iLength);

	return TRUE;
}
