#include "filebase.h"
#include "LogWriter.h"

#pragma once

class CDbFile : public CFileBase
{
public:
	CDbFile(void);
	virtual ~CDbFile(void);

	BOOL Open(LONG FileType, CString strAppPath, CString strFileName, CString strSection);
	BOOL Write(SYSTEMTIME &Time, BYTE *Buffer, UINT iLength);
	void Close();

private:

	LONG m_lFileType;
	VS_FIXEDFILEINFO m_sFileInfo;

	BOOL WriteHeader(FILE* Handle);
};
