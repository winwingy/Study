#include "StdAfx.h"
#include "TextFile.h"

CTextFile::CTextFile(void)
{
}

CTextFile::~CTextFile(void)
{
	Close();
}

BOOL CTextFile::Open(CString strAppPath, 
					 CString strFileName,
					 CString strSection)
{
	CFileBase::InitParameter(strAppPath, 
		strFileName, strSection, 0);
	return TRUE;
}

BOOL CTextFile::Write(SYSTEMTIME &Time,
					  char *TimeBuff,  
					  char *LogBuff)
{
	USING_TIME_INFO;
	FILE* Handle = NULL;
	if (GetHandle(Time, &Handle))
	{
		USING_TIME_INFO;
		m_iFileSize += fprintf(Handle, "%s %s\r\n", TimeBuff, LogBuff);
		return TRUE;
	}

	return FALSE;
}

void CTextFile::Close()
{
	CFileBase::CloseFile();
}
