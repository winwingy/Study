#pragma once
#include "filesearcher.h"

class CHistorySearcher :
	public CFileSearcher
{
public:
	CHistorySearcher(void);
	~CHistorySearcher(void);
	
	BOOL Search(const CParameter &Parameter, CAtlArray<BACKUPFILE> &arrFileNames);
	DOUBLE GetAllFileSize() const;

private:

	DOUBLE m_dFilesSize;

	BOOL Conversion(const CHAR *cFormat, CHAR Buffer[MAX_PATH]) const;
	virtual BOOL AddFile(const BACKUPFILE &sFileInfo, CAtlArray<BACKUPFILE> &arrFileNames);
};
