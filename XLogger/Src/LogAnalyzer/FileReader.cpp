#include "StdAfx.h"
#include "FileReader.h"

CFileReader::CFileReader(void)
{
}

CFileReader::~CFileReader(void)
{
}

BOOL CFileReader::ReadFile(CString strFileName, CString &strError)
{
	CStdioFile File;
	CFileException ex;

	if (!File.Open(strFileName, CFile::modeRead, &ex))
	{
		TCHAR szError[1024] = { 0 };
		ex.GetErrorMessage(szError, 1023);
		strError.Format(_T("无法打开文件：%s。\r\n错误：%s。"), strFileName, szError);
		return FALSE;
	}

	CString strValue;
	while (File.ReadString(strValue))
	{
		if (strValue.GetLength() > 36)
			m_arrLogData.Add(strValue);
	}

	File.Close();
	return TRUE;
}