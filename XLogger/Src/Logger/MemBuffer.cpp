#include <tchar.h>

#include "MemBuffer.h"
#include "SimpleLogger.h"
#include "XLogUtil.h"


CMemBuffer::CMemBuffer(void)
{
	m_hMemory      = NULL;
	m_pvMemAddress = NULL;
}

CMemBuffer::~CMemBuffer(void)
{
	Close();
}

BOOL CMemBuffer::Create(TCHAR *pcName, UINT iSize)
{
	if (NULL != m_hMemory)
	{
		return FALSE;
	}

	if ((m_hMemory = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		iSize,
		pcName)) == NULL)
	{
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 };
		GetErrorMessage(GetLastError(), Buffer);
		LOGGER_WRITE2(_T("Call CreateFileMapping FALSE! Mapping Object Name: ") 
			<< pcName << _T(". ") << Buffer);
		return FALSE;
	}

	if ((m_pvMemAddress = MapViewOfFile(m_hMemory, 
		FILE_MAP_READ | FILE_MAP_WRITE, 
		0, 
		0, 
		0)) == NULL) 
	{
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 };
		GetErrorMessage(GetLastError(), Buffer);
		LOGGER_WRITE2(_T("Call MapViewOfFile FALSE! Mapping Object Name: ") 
			<< pcName << _T(". ") << Buffer);
		m_pvMemAddress = NULL;
		CloseHandle(m_hMemory);
		m_hMemory = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CMemBuffer::Close()
{
	if (NULL != m_pvMemAddress
		&& NULL != m_hMemory)
	{
		UnmapViewOfFile(m_pvMemAddress);
		CloseHandle(m_hMemory);
		m_pvMemAddress = NULL;
		m_hMemory = NULL;
	}

	return TRUE;
}
