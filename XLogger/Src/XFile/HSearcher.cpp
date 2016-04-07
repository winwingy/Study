#include "StdAfx.h"
#include "HSearcher.h"
#include "..\Logger\XLogUtil.h"


CHistorySearcher::CHistorySearcher(void)
{
	m_dFilesSize = 0;
}

CHistorySearcher::~CHistorySearcher(void)
{
}

DOUBLE CHistorySearcher::GetAllFileSize() const
{
	return m_dFilesSize;
}

BOOL CHistorySearcher::Conversion(const CHAR *cFormat, 
								  CHAR Buffer[MAX_PATH]) const
{
	const CHAR cType[] = "aAbBcdHIjmMpSUwWxXyYzZ";
	const WORD iLength = sizeof(cType);
	WORD iCount = (WORD)strnlen(cFormat, MAX_PATH);
	memcpy(Buffer, cFormat, iCount);

	for (WORD i = 0; i < iCount - 1; i++)
	{
		if ('%' != Buffer[i])
			continue;

		WORD iBegin = i++;
		if ('%' == Buffer[i])
		{
			memmove(Buffer + i - 1, Buffer + i, iCount - i);
			iCount--; i--;
			continue;
		}

		for (i; i < iCount; i++)
		{
			if (NULL != memchr(cType, Buffer[i], iLength))
			{
				if (0 < iBegin && Buffer[iBegin - 1] == '*') iBegin--;
				Buffer[iBegin] = '*';
				WORD iLength = i - iBegin++;
				memmove(Buffer + iBegin, Buffer + iBegin + iLength, iCount - i);
				iCount -= iLength;	
				i--;
				break;
			}
			if (isdigit(Buffer[i]) == 0) break;
		}
	}

	memset(Buffer + iCount, 0, MAX_PATH - iCount);
	return TRUE;
}

BOOL CHistorySearcher::Search(const CParameter &Parameter, 
							  CAtlArray<BACKUPFILE> &arrFileNames)
{
	CHAR FileName[MAX_PATH] = { 0 };

#pragma warning(push)
#pragma warning(disable: 4996)
	strcat(FileName, "*");
	strcat(FileName, Parameter.m_cCycleName);
	strcat(FileName, Parameter.m_cCreateTime);
	strcat(FileName, Parameter.m_cSaveTime);
	strcat(FileName, Parameter.m_cExeName);
#pragma warning(pop)

	CHAR MatchName[MAX_PATH] = { 0 };
	Conversion(FileName, MatchName);

	m_dFilesSize = 0;
	if (!FilesSearch(CString(Parameter.m_cFixFolder), 
		CString(MatchName), TRUE, arrFileNames))
	{
		CString Error = CString(Parameter.m_cFixFolder) + CString(MatchName);
		TCHAR ErrorMessage[ERROR_BUFFER_SIZE] = { 0 };
		GetErrorMessage(GetLastError(), ErrorMessage);
		LOGGER_WRITE(_T("查找备份文件失败:") << ErrorMessage << _T(" ") << Error.GetBuffer());
		return FALSE;
	}

	return TRUE;
}

BOOL CHistorySearcher::AddFile(const BACKUPFILE &sFileInfo, 
							   CAtlArray<BACKUPFILE> &arrFileNames)
{
	m_dFilesSize += sFileInfo.dFileSize;
	const size_t iMaxCount = 100;
	if (arrFileNames.GetCount() > iMaxCount)
		arrFileNames.RemoveAt(iMaxCount, 
		arrFileNames.GetCount() - iMaxCount);

	Sleep(5);
	return CFileSearcher::AddFile(sFileInfo, arrFileNames);
}
