#include "Parameter.h"
#pragma once

class CConfigReader
{
public:
	CConfigReader(void);
	~CConfigReader(void);

	BOOL Read(CString strAppPath, CString strFileName, CString strSection, LONG FileType, CParameter &Parameter);

private:
	BOOL FileNamePaser(CString strValue, CString &strFixFolder, CString &strDynFolder, 
		CString &strCycle, CString &strCreate, CString &strSave, CString &strExeName);
	BOOL FileNamePaser(CString strValue, CString &strFixFolder, CString &strDynFolder);
	BOOL SetFileName(CString strValue, CParameter &Parameter);
	BOOL ReadConfig(CString strKeyName, CString &strValue);
	BOOL GetDefSection(CString strFileName, CString &strSection);
	CString m_strFileName;
	CString m_strSection;
	CString m_strAppPath;
};

