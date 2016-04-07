#include "StdAfx.h"
#include "FormatUtil.h"
#include <time.h>

CFormatUtil::CFormatUtil(char *pType /*= NULL*/)
{
	m_pcFormat = NULL;
	Initialization(pType);
}

CFormatUtil::~CFormatUtil(void)
{
	if (NULL != m_pcFormat)
	{
		delete m_pcFormat;
		m_pcFormat = NULL;
	}
}

void CFormatUtil::Initialization(char *pType)
{
	if (NULL != m_pcFormat)
	{
		delete m_pcFormat;
		m_pcFormat = NULL;
	}

	m_iLength    = 0;
	m_iItemCount = 0;
	m_pcFormat   = NULL;

	size_t iSize = sizeof(FORAMTITEM);
	for (int i = 0; i < MAX_FORMAT_ITEM; i++)
		memset(&m_sFormat[i], 0, iSize);

	char *pDefault = "aAbBcdHIjmMpSUwWxXyYzZNPTG"; 
	size_t iLength = strnlen_s(pDefault, MAX_FORMAT_ITEM - 1);
	memset(m_cType, 0, MAX_FORMAT_ITEM);
	memcpy(m_cType, pDefault, iLength);

	size_t iLen2 = strnlen_s(pType, MAX_FORMAT_ITEM - 1);
	if (0 == iLen2)
		return;

	for (size_t i = 0; i < iLength; i++)
	{
		size_t j = 0;
		char &cData = m_cType[i];
		for (j; j < iLen2; j++)
		{
			if (pType[j] == cData)
				break;
		}
		if (j == iLen2)
			cData = '\0';
	}
}

BOOL CFormatUtil::SetFormat(char *pcFormat)
{
	if (NULL == pcFormat)
		return FALSE;

	m_iLength = strnlen_s(pcFormat, MAX_PATH);
	m_pcFormat = new char[m_iLength + 1];
	memcpy(m_pcFormat, pcFormat, m_iLength);
	m_pcFormat[m_iLength] = '\0';

	FormatPaser();

	return TRUE;
}

BOOL CFormatUtil::SetFormat(char *pType, 
							char *pcFormat)
{
	Initialization(pType);
	return SetFormat(pcFormat);
}

BOOL CFormatUtil::FormatPaser()
{
	WORD iBegin = 0;
	WORD iCount = (WORD)m_iLength;

	for (WORD i = 0; i < iCount - 1; i++)
	{
		if ('%' != m_pcFormat[i])
			continue;

		FORAMTITEM sItem;
		sItem.iType = MAX_FORMAT_ITEM;
		sItem.iBegin = iBegin;
		sItem.iFormat = i;
		sItem.iEnd = i;
		i++;

		if ('%' == m_pcFormat[i])
		{
			for (WORD j = i; j < iCount; j++)
				m_pcFormat[j - 1] = m_pcFormat[j];
			m_iLength--; 
			iCount--;
			i--;
			continue;
		}

		for (WORD j = i; j < iCount; j++)
		{
			char cData = m_pcFormat[j];
			if (GetType(cData, sItem.iType))
			{
				i = j;
				sItem.iEnd = j;
				break;
			}
			if (isdigit(cData) == 0)
				break;
		}

		int iVal = sItem.iEnd - sItem.iFormat;
		if (0 == iVal || 4 < iVal)
			continue;

		if (sItem.iType == m_iMillisecond
			|| sItem.iType == m_iProcess
			|| sItem.iType == m_iThread)
			m_pcFormat[sItem.iEnd] = 'd';

		AddItem(sItem);
		iBegin = sItem.iEnd + 1; 
	}

	return TRUE;
}

BOOL CFormatUtil::GetType(char cData, WORD &iType) const
{
	for (WORD i = 0; i < MAX_FORMAT_ITEM; i++)
	{
		if (m_cType[i] == cData)
		{
			iType = i;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CFormatUtil::AddItem(FORAMTITEM &sItem)
{
	if (MAX_FORMAT_ITEM > m_iItemCount)
	{
		sItem.iLength = sItem.iEnd - sItem.iFormat + 1;
		m_sFormat[m_iItemCount++] = sItem;
		return TRUE;
	}

	return FALSE;
}

UINT CFormatUtil::FormatString(char Buffer[MAX_PATH], 
							   const SYSTEMTIME &Time) const
{
	char Temp[FORMAT_BUFFER_LENGTH] = { 0 };
	UINT iLength = FormatString(Temp, Time, 0, 0);
	memcpy(Buffer, Temp, MAX_PATH);
	return iLength;
}

UINT CFormatUtil::FormatString(char Buffer[FORMAT_BUFFER_LENGTH], 
							   const SYSTEMTIME &Time,
							   DWORD dwProcess,
							   DWORD dwThread) const
{
	if (0 == m_iItemCount)
	{
		memcpy(Buffer, m_pcFormat, m_iLength);
		Buffer[m_iLength] = '\0';
		return 0;
	}

	struct tm gm = {Time.wSecond, Time.wMinute, Time.wHour, Time.wDay, 
		Time.wMonth - 1, Time.wYear-1900, Time.wDayOfWeek, 0, 0};

	int iIndex = 0;
	int iLength = 0;
	const FORAMTITEM *sItem = NULL;
	char Format[100] = { 0 };

	DWORD dwValue[3];
	dwValue[0] = Time.wMilliseconds;
	dwValue[1] = dwProcess;
	dwValue[2] = dwThread;

	for (size_t i = 0; i < m_iItemCount; i++)
	{
		sItem = &m_sFormat[i];
		iLength = sItem->iFormat - sItem->iBegin; 
		memcpy(Buffer + iIndex, m_pcFormat + sItem->iBegin, iLength);
		iIndex += iLength;
		iLength = sItem->iEnd - sItem->iFormat + 1;
		memcpy(Format, m_pcFormat + sItem->iFormat, iLength);
		Format[iLength] = '\0';

		iLength = FORMAT_BUFFER_LENGTH - iIndex;
		if (sItem->iType <= m_iTime) // date
		{
			if (128 > iLength) return FALSE;
			iIndex += (int)strftime(Buffer + iIndex, iLength, Format, &gm);
			continue;
		}
		else // if (sItem->iType == m_iMillisecond)
		{
			if (20 > iLength) return FALSE;
			iIndex += sprintf_s(Buffer + iIndex, iLength, Format, 
				dwValue[sItem->iType - m_iMillisecond]);
			continue;
		}
	}

	int iBufLen = FORMAT_BUFFER_LENGTH - iIndex;
	if (0 == iBufLen)
	{
		Buffer[iIndex-1] = '\0';
		return iIndex;
	}

	iLength = int(m_iLength - sItem->iEnd - 1);
	if (iLength >= iBufLen)
		iLength = iBufLen - 1;

	if (0 < iLength)
	{
		memcpy(Buffer + iIndex, m_pcFormat + sItem->iEnd + 1, iLength);
		iIndex += iLength;
	}
	Buffer[iIndex] = '\0';

	return iIndex;
}
