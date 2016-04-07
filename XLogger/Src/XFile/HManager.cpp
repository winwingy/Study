#include "StdAfx.h"
#include "HManager.h"
#include "DCSection.h"
#include "HSearcher.h"
#include "..\Logger\XLogUtil.h"

CHistoryManager::CHistoryManager(void)
{
	m_dAllFileSize  = 0;
	m_bThreadRuning = FALSE;
	m_hThreadHandle = NULL;
}

CHistoryManager::~CHistoryManager(void)
{
	CloseThread();
}

BOOL CHistoryManager::Add(const CParameter &Parameter)
{
	CDataCriticalSection Section;

	size_t iCount = m_arrParameter.GetCount();
	for (size_t i = 0; i < iCount; i++)
	{
		if (m_arrParameter[i] == Parameter)
			return TRUE;
	}

	m_arrParameter.Add(Parameter);

	return TRUE;
}

BOOL CHistoryManager::Delete(const CParameter &Parameter)
{
	CDataCriticalSection Section;

	size_t iCount = m_arrParameter.GetCount();
	for (size_t i = 0; i < iCount; i++)
	{
		if (m_arrParameter[i] == Parameter)
		{
			m_arrParameter[i].m_bDelete = TRUE;
			break;
		}
	}

	return TRUE;
}

BOOL CHistoryManager::Delete2(const CParameter &Parameter)
{
	CDataCriticalSection Section;

	size_t iCount = m_arrParameter.GetCount();
	for (size_t i = 0; i < iCount; i++)
	{
		if (m_arrParameter[i] == Parameter)
		{
			m_arrParameter.RemoveAt(i);
			break;
		}
	}

	return TRUE;
}

BOOL CHistoryManager::GetParam(UINT iIndex, 
							   CParameter &Parameter) const
{
	CDataCriticalSection Section;

	UINT iCount = (UINT)m_arrParameter.GetCount();
	if (iIndex >= iCount)
		return FALSE;

	Parameter = m_arrParameter[iIndex];

	return TRUE;
}

UINT CHistoryManager::GetParamCount() const
{
	CDataCriticalSection Section;
	return (UINT)m_arrParameter.GetCount();
}

BOOL CHistoryManager::CreateThread()
{
	if (m_bThreadRuning || NULL != m_hThreadHandle)
		return FALSE;

	m_bThreadRuning = TRUE;
	unsigned int uiThreadID = 0;
	m_hThreadHandle = _beginthreadex(NULL, 0, DeleteFilesThread,
		this, 0, &uiThreadID);
	if (0 != m_hThreadHandle)
	{
		LOGGER_WRITE2(_T("创建备份文件处理线程:") << m_hThreadHandle);
		return TRUE;
	}

	TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 };
	GetErrorMessage(GetLastError(), Buffer);
	LOGGER_WRITE2(_T("创建备份文件处理线程失败:") << Buffer);

	return FALSE;
}

BOOL CHistoryManager::CloseThread()
{
	if (NULL == m_hThreadHandle)
		return TRUE;

	m_bThreadRuning = FALSE;
	if (WAIT_TIMEOUT == WaitForSingleObject(
		HANDLE(m_hThreadHandle), DWORD(1000 * 60))) // 设60秒的原因是，希望一轮删除动作执行完毕。
	{
		DWORD ExitCode = 0;
		TerminateThread(HANDLE(m_hThreadHandle), ExitCode);
		LOGGER_WRITE(_T("强制终止备份文件处理线程."));
	}

	LOGGER_WRITE2(_T("关闭备份文件处理线程:") << m_hThreadHandle);
	CloseHandle((HANDLE)m_hThreadHandle);
	m_hThreadHandle = NULL;

	return TRUE;
}

UINT WINAPI CHistoryManager::DeleteFilesThread(void* lpParam)
{
	CHistoryManager *pManager = (CHistoryManager*)lpParam;
	DWORD dwLastTime = 0;

	while (pManager->m_bThreadRuning)
	{
		if ((GetTickCount() - dwLastTime) > 5000) 
		{
			pManager->DeleteFiles();
			dwLastTime = GetTickCount();
		}
		pManager->XSleep();
	}

	_endthreadex(0);
	return 0;
}

void CHistoryManager::DeleteFiles()
{
	CParameter Parameter;
	LONG iCount = GetParamCount();
	for (LONG i = (iCount - 1); i >= 0; i--)
	{
		XSleep();
		if (!GetParam(i, Parameter))
			return;

		DeleteFiles(Parameter);

		if (Parameter.m_bDelete)
			Delete2(Parameter);
	}
}

void CHistoryManager::DeleteFiles(CParameter &Parameter)
{
	CHistorySearcher Search;
	CAtlArray<BACKUPFILE> arrFileNames;
	if (!Search.Search(Parameter, arrFileNames))
		return;

	m_dAllFileSize = Search.GetAllFileSize();

	XSleep();
	DeleteFilesDays(Parameter, arrFileNames);

	XSleep();
	DeleteFilesSpace(Parameter, arrFileNames);

	XSleep();
	DeleteFilesSize(Parameter, arrFileNames);
}

void CHistoryManager::DeleteFilesDays(const CParameter &Parameter,
									  CAtlArray<BACKUPFILE> &arrFiles)
{
	if (0 == Parameter.m_iBackupDays)
		return;

	CTime timCurTime = CTime::GetCurrentTime();
	for (UINT i = 0; i < arrFiles.GetCount(); i++)
	{
		const BACKUPFILE &FileInfo = arrFiles[i];
		CTimeSpan timTimeSpan = timCurTime - CTime(FileInfo.sCreateTime);
		if (timTimeSpan.GetDays() < Parameter.m_iBackupDays) break; 
		DeleteFiles(Parameter, FileInfo);
		arrFiles.RemoveAt(i--);
	}
}

void CHistoryManager::DeleteFilesSpace(const CParameter &Parameter, 
									   CAtlArray<BACKUPFILE> &arrFiles)
{
	LONGLONG lFreeSpace = 0;
	if (!GetDiskFreeSpace(CString(Parameter.m_cFixFolder), lFreeSpace))
		return;

	double dDelete = Parameter.m_iMinDiskSpace - 
		(double)lFreeSpace + Parameter.m_iFileSize;
	for (UINT i = 0; i < arrFiles.GetCount(); i++)
	{
		if (dDelete < 0) break;
		const BACKUPFILE &FileInfo = arrFiles[i];
		dDelete -= FileInfo.dFileSize;
		DeleteFiles(Parameter, FileInfo);
		arrFiles.RemoveAt(i--);
	}
}

void CHistoryManager::DeleteFilesSize(const CParameter &Parameter,
									  CAtlArray<BACKUPFILE> &arrFiles)
{
	if (0 == Parameter.m_iAllFilesSize)
		return;

	double dDelete = m_dAllFileSize 
		- Parameter.m_iAllFilesSize 
		+ Parameter.m_iFileSize;

	for (UINT i = 0; i < arrFiles.GetCount(); i++)
	{
		if (dDelete < 0) break;
		const BACKUPFILE &FileInfo = arrFiles[i];
		dDelete -= FileInfo.dFileSize;
		DeleteFiles(Parameter, FileInfo);
		arrFiles.RemoveAt(i--);
	}
}

void CHistoryManager::DeleteFiles(const CParameter &Parameter,
								  const BACKUPFILE &FileInfo) const
{
	if (FileInfo.strFileName.GetLength() != 0)
	{
		if (!DeleteFile(FileInfo.strFolder + FileInfo.strFileName))
			return;
	}

	CString l_strFolder = FileInfo.strFolder;
	const CString strFixFolder(Parameter.m_cFixFolder);
	while (l_strFolder != strFixFolder)
	{
		if (!PathIsDirectoryEmpty(l_strFolder))
			break;

		if (!RemoveDirectory(l_strFolder))
			break;

		l_strFolder.Delete(l_strFolder.GetLength() - 1);
		int l_iIndex = l_strFolder.ReverseFind('\\');
		l_strFolder = l_strFolder.Left(l_iIndex + 1);
	}
}

BOOL CHistoryManager::GetDiskFreeSpace(CString strPathName, 
									   LONGLONG &lFreeSpace) const
{
	XSleep(2);

	strPathName.Replace(_T("/"), _T("\\"));
	const int l_iIndex = strPathName.Find(_T('\\'));
	if (0 < l_iIndex)
		strPathName = strPathName.Left(l_iIndex + 1);

	ULARGE_INTEGER  FreeAvailable, TotalNumber;
	memset(&FreeAvailable, 0, sizeof(ULARGE_INTEGER));
	memset(&TotalNumber, 0, sizeof(ULARGE_INTEGER));
	if (0 == GetDiskFreeSpaceEx(strPathName, &FreeAvailable, &TotalNumber, NULL))
	{
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 };
		GetErrorMessage(GetLastError(), Buffer);
		LOGGER_WRITE(_T("GetDiskFreeSpaceEx error:") << Buffer << _T("(") << strPathName.GetBuffer() << _T(")"));
		return FALSE;
	}

	// 1048576 = 1024 * 1024 
	lFreeSpace = (LONGLONG)(FreeAvailable.QuadPart / 1048576.0); 
	return TRUE;
}
