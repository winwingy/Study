#pragma once

class CFileReader
{
public:
	CFileReader(void);
	~CFileReader(void);

	CAtlArray<CString> m_arrLogData;

	BOOL ReadFile(CString strFileName, CString &strError);
};
