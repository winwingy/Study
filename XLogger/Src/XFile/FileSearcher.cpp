#include "StdAfx.h"
#include "FileSearcher.h"

CFileSearcher::CFileSearcher(void)
{
}

CFileSearcher::~CFileSearcher(void)
{
}

BOOL CFileSearcher::Search(const CFileBase *FileBase, 
						   SYSTEMTIME Time, 
						   CAtlArray<BACKUPFILE> &arrFileNames)
{
	CString strPath, strCondition;
	if (!FileBase->GetFileName(Time, 2, strPath, strCondition))
		return FALSE;

	return FilesSearch(strPath, strCondition, FALSE, arrFileNames);
}

BOOL CFileSearcher::Search(const CFileBase *FileBase, 
						   SYSTEMTIME Time, 
						   CString &strFileName)
{
	CString strPath, strCondition;
	if (!FileBase->GetFileName(Time, 1, strPath, strCondition))
		return FALSE;

	CAtlArray<BACKUPFILE> arrFileNames;
	if (!FilesSearch(strPath, strCondition, FALSE, arrFileNames))
		return FALSE;

	if (arrFileNames.GetCount() == 0)
		return FALSE;

	strFileName = strPath + arrFileNames[0].strFileName;	
	return TRUE;
}

BOOL CFileSearcher::FilesSearch(CString strPath,
								CString strCondition, 
								BOOL Subdirectory, 
								CAtlArray<BACKUPFILE> &arrFileNames)
{
	WIN32_FIND_DATA sFileData; 
	CString strValue = strPath + _T("*");
	HANDLE hFileFind = FindFirstFile(strValue, &sFileData); 
	if (INVALID_HANDLE_VALUE == hFileFind) 
		return FALSE; 

	do 
	{ 
		if (sFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE
			&& PathMatchSpec(sFileData.cFileName, strCondition))
		{
			BACKUPFILE sFile;
			sFile.dFileSize = (sFileData.nFileSizeHigh * ((DOUBLE)MAXDWORD + 1.0)) / 1048576.0
				+ sFileData.nFileSizeLow / 1048576.0; // 1048576 = 1024 * 1024;
			sFile.sCreateTime = sFileData.ftLastWriteTime; // 
			sFile.strFileName = sFileData.cFileName;
			sFile.strFolder = strPath;

			AddFile(sFile, arrFileNames);
		}
		else if (Subdirectory
			&& (sFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (sFileData.cFileName[0] != L'.'))
		{
			strValue = strPath + sFileData.cFileName;
			FilesSearch(strValue + _T("\\"), strCondition, Subdirectory, arrFileNames);
		}
	} 
	while (FindNextFile(hFileFind, &sFileData));
	FindClose(hFileFind);
	hFileFind = NULL;

	return TRUE;
}

__inline
BOOL CFileSearcher::AddFile(const BACKUPFILE &sFileInfo, 
							CAtlArray<BACKUPFILE> &arrFileNames)
{
	size_t iCount = arrFileNames.GetCount();
	for (size_t i = 0; i < iCount; i++)
	{
		if (CompareFileTime(&sFileInfo.sCreateTime, 
			&arrFileNames[i].sCreateTime) <= 0)
		{
			arrFileNames.InsertAt(i, sFileInfo);
			return TRUE;
		}
	}

	arrFileNames.Add(sFileInfo);
	return TRUE;
}
