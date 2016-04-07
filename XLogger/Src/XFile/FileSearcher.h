#pragma once

#include "FileBase.h"

class CFileSearcher
{
public:
	CFileSearcher(void);
	virtual ~CFileSearcher(void);

	// 查找部分文件，用于备份改名。
	virtual BOOL Search(const CFileBase *FileBase, SYSTEMTIME Time, CAtlArray<BACKUPFILE> &arrFileNames);

	// 查找当前要写入的文件，该文件已经存在磁盘
	virtual BOOL Search(const CFileBase *FileBase, SYSTEMTIME Time, CString &strFileName);

protected:
	BOOL FilesSearch(CString strPath, CString strCondition, 
		BOOL Subdirectory, CAtlArray<BACKUPFILE> &arrFileNames);
	virtual BOOL AddFile(const BACKUPFILE &sFileInfo, CAtlArray<BACKUPFILE> &arrFileNames);
};
