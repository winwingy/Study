#pragma once
#include "filebase.h"

class CTextFile :
	public CFileBase
{
public:
	CTextFile(void);
	virtual ~CTextFile(void);

	BOOL Open(CString strAppPath, CString strFileName, CString strSection);
	BOOL Write(SYSTEMTIME &Time, char *TimeBuff,  char *LogBuff);
	void Close();
};
