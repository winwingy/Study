#include "StdAfx.h"
#include "FileBase.h"
#include "ConfigReader.h"
#include "filesearcher.h"
#include "DCSection.h"
#include <direct.h>

#ifndef __ATLUTIL_H__
#include <atlutil.h>
#endif

void __cdecl LogEvent(LPCTSTR pszFormat, ...) throw();

CFileBase::CFileBase(void)
: m_oDynFolder("aAbBdjmUwWyY"),
m_oCycleName("aAbBdjmUwWyY"),
m_pFileHandle(NULL),
m_iMaxFileSize(1024),
m_iBkCount(-1)
{
	GetLocalTime(&m_sLastTime);
	memset(m_cFileName, 0, MAX_PATH);
	memset(m_cPathName, 0, MAX_PATH);
}

CFileBase::~CFileBase(void)
{
	CloseFile();
	m_iBkCount = -1;
	AdjustFileName(m_sLastTime, FALSE);
}

BOOL CFileBase::InitParameter(CString strAppPath,
							  CString strFileName,
							  CString strSection, 
							  LONG FileType)
{
	CConfigReader Reader;
	Reader.Read(strAppPath, strFileName, strSection, FileType, m_oParameter);

	m_iMaxFileSize = m_oParameter.m_iFileSize * 1024 * 1024;

	m_oDynFolder.SetFormat(m_oParameter.m_cCycleFolder);
	m_oCycleName.SetFormat(m_oParameter.m_cCycleName);
	m_oCreateTime.SetFormat(m_oParameter.m_cCreateTime);
	m_oSaveTime.SetFormat(m_oParameter.m_cSaveTime);

	return TRUE;
}

BOOL CFileBase::GetHandle(const SYSTEMTIME &Time, 
						  FILE** Handle)
{
	USING_TIME_INFO;
	FileBackup(Time);
	m_sLastTime = Time;

	if (NULL != m_pFileHandle)
	{
		*Handle = m_pFileHandle;
		return TRUE;
	}

	CString strFileName;
	if (!GetFileName(Time, strFileName))
		return FALSE;

	SetFileName(strFileName);
	m_pFileHandle = _wfsopen(strFileName, _T("ab"), _SH_DENYNO);
	if (NULL == m_pFileHandle)
	{
		CString strError;
		strError.Format(_T("Create file[%s] error:%s!!!"), 
			strFileName, AtlGetErrorDescription(GetLastError()));
		LOGGER_WRITE(strError.GetBuffer());
		LogEvent(strError);
		return FALSE;
	}

	fseek(m_pFileHandle, 0, SEEK_END);
	m_iFileSize = ftell(m_pFileHandle);

	*Handle = m_pFileHandle;

	return TRUE;
}

__inline
void CFileBase::FileBackup(const SYSTEMTIME &Time)
{
	if (NULL == m_pFileHandle)
		return;

	if (m_sLastTime.wDay != Time.wDay)
	{
		CString strValue[2]; 
		if (!GetFileName(Time, 1, strValue[0], strValue[1]))
			return;

		CString strFileName = GetFileName();
		ATLASSERT(strFileName.GetLength() != 0);
		CString strMatchName = strValue[0] + strValue[1];
		if (!PathMatchSpec(strFileName, strMatchName))
		{
			m_iBkCount = -1;
			CloseFile();
		}
	}
	else if (m_iFileSize > m_iMaxFileSize)
	{
		CloseFile();
		AdjustFileName(Time);
	}
}

__inline
void CFileBase::AdjustFileName(const SYSTEMTIME &Time,
							   BOOL bAllFile /*= TRUE*/)
{
	if (-1 != m_iBkCount)
	{
		ATLASSERT(m_strFileName.GetLength() != 0);
		CString strValue;
		strValue.Format(_T("%ld_"), m_iBkCount++);
		CString strFileName = m_strFileName;
		int Index = strFileName.ReverseFind(_T('\\'));
		strFileName.Insert(Index + 1, strValue);
		_wrename(m_strFileName, strFileName);
		return;
	}

	static CFileSearcher Searcher;
	CAtlArray<BACKUPFILE> arrFileNames;
	Searcher.Search(this, Time, arrFileNames);
	CString strNewName, strValue, strPath;
	m_iBkCount = (LONG)arrFileNames.GetCount();

	for (long i = 0; i < m_iBkCount; i++)
	{
		strValue = arrFileNames[i].strFileName;
		int iIndex = strValue.Find(_T('_'));
		if (-1 != iIndex)
			strValue.Delete(0, iIndex + 1);
		else if (!bAllFile)
			continue;

		strPath = arrFileNames[i].strFolder;
		strNewName.Format(_T("%d_%s"), i, strValue);
		if (strNewName != arrFileNames[i].strFileName)
			_wrename(strPath + arrFileNames[i].strFileName, 
			strPath + strNewName);
	}
}

__inline
BOOL CFileBase::GetFileName(const SYSTEMTIME &Time,
							CString &strFileName)
{
	CString strName[2]; 
	if (m_strFileName.GetLength() != 0)
	{
		GetFileName(Time, 1, strName[0], strName[1]);
		if (PathMatchSpec(m_strFileName, strName[0] + strName[1]))
		{
			strFileName = m_strFileName;
			return TRUE;
		}
	}

	CFileSearcher Searcher;
	if (Searcher.Search(this, Time, strFileName))
		return TRUE;
	
	GetFileName(Time, 0, strName[0], strName[1]);
	if (CreateFolder(strName[0]))
	{
		strFileName = strName[0] + strName[1];
		return TRUE;
	}

	return FALSE;
}

__inline
BOOL CFileBase::CreateFolder(CString strFolder)
{
	if (m_strFolderName == strFolder)
		return TRUE;

	CString strValue;
	int iCount = strFolder.GetLength() - 1;

	for (int i = 0; i <= iCount; i++)
	{
		if (_T('\\') != strFolder[i])
			continue;

		if (i == iCount) strValue = strFolder;
		else strValue = strFolder.Left(i + 1);

		if (PathFileExists(strValue))
			continue;

		if (!CreateDirectory(strValue, NULL))
		{
			CString strError;
			strError.Format(_T("Create folder error:%s. CFileBase::NewFileName return FALSE."), strValue);
			LOGGER_WRITE(strError.GetBuffer());
			LogEvent(strError);
			return FALSE;
		}
	}

	m_strFolderName = strFolder;
	return TRUE;
}

__inline
BOOL CFileBase::GetFileName(SYSTEMTIME Time, 
							int iType, 
							CString &strPath, 
							CString &strFileName) const
{

	char Buffer[MAX_PATH] = { 0 };
	m_oDynFolder.FormatString(Buffer, Time);

	strPath = CString(m_oParameter.m_cFixFolder);
	strPath += CString(Buffer);

	memset(Buffer, 0, MAX_PATH);
	if (0 == iType)
	{
		m_oCycleName.FormatString(Buffer, Time);
		strFileName = CString(Buffer);
		memset(Buffer, 0, MAX_PATH);
		m_oCreateTime.FormatString(Buffer, Time);
		strFileName += CString(Buffer);
		memset(Buffer, 0, MAX_PATH);
		m_oSaveTime.FormatString(Buffer, Time);
		strFileName += CString(Buffer);
		strFileName += CString(m_oParameter.m_cExeName);
		return TRUE;
	}
#pragma warning(push)
#pragma warning(disable: 4996)
	else if (1 == iType)
	{
		m_oCycleName.FormatString(Buffer, Time);
		strcat(Buffer, "*");
		strcat(Buffer, m_oParameter.m_cExeName);
		strFileName = CString(Buffer);
		return TRUE;
	}
	else if (2 == iType)
	{
		m_oCycleName.FormatString(Buffer, Time);
		strcat(Buffer, "*");
		strFileName = _T("*");
		strFileName += CString(Buffer);
		return TRUE;
	}
#pragma warning(pop)

	ATLASSERT(FALSE);

	CString strError;
	strError.Format(_T("Error Type:%d"), iType);
	LOGGER_WRITE(strError.GetBuffer());
	LogEvent(strError);

	return FALSE;
} 

__inline
void CFileBase::CloseFile()
{
	if (NULL == m_pFileHandle)
		return;

	fflush(m_pFileHandle);
	fclose(m_pFileHandle);
	m_pFileHandle = NULL;

	CString strFileName = GetFileName();
	int iIndex = strFileName.Find(_T('-'));
	if (-1 == iIndex || 0 == strlen(m_oParameter.m_cSaveTime))
		return;

#pragma warning(push)
#pragma warning(disable: 4996)
	char Buffer[MAX_PATH] = { 0 };
	m_oCreateTime.FormatString(Buffer, m_sLastTime);
	strcat(Buffer, m_oParameter.m_cExeName);
	CString strNewName = strFileName.Left(iIndex + 1) + CString(Buffer);
#pragma warning(pop)

	_wrename(strFileName, strNewName);
}

__inline
CString CFileBase::GetFileName() const
{
	CDataCriticalSection Section;
	CString strValue = m_strFileName;
	return strValue;
}

__inline
void CFileBase::SetFileName(CString strFileName)
{
	CDataCriticalSection Section;
	m_strFileName = strFileName;
}
