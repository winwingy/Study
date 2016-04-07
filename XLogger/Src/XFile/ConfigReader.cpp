#include "StdAfx.h"
#include "ConfigReader.h"
#include "Parameter.h"
#include <shlwapi.h>
#include "..\Logger\XLogUtil.h"

CConfigReader::CConfigReader(void)
{
}

CConfigReader::~CConfigReader(void)
{
}

BOOL CConfigReader::Read(CString strAppPath, 
						 CString strFileName, 
						 CString strSection, 
						 LONG FileType,
						 CParameter &Parameter)
{
	if (m_strSection.IsEmpty())
		GetDefSection(strFileName, strSection);

	m_strAppPath  = strAppPath;
	m_strFileName = strFileName;
	m_strSection  = strSection;

	LOGGER_WRITE(_T("AppPath:") << m_strAppPath.GetBuffer());
	LOGGER_WRITE(_T("IniFile:") << m_strFileName.GetBuffer());
	LOGGER_WRITE(_T("Section:") << m_strSection.GetBuffer());

	CString strValue;
	if (ReadConfig(_T("Encrypt"), strValue))
		Parameter.m_bEncrypt = (strValue == _T("1"));
	LOGGER_WRITE(_T("Encrypt:") << Parameter.m_bEncrypt);

	if (ReadConfig(_T("Compress"), strValue))
		Parameter.m_bCompress = (strValue == _T("1"));
	LOGGER_WRITE(_T("Compress:") << Parameter.m_bCompress);

	if (ReadConfig(_T("BackupDays"), strValue))
	{
		INT iValue = _wtoi(strValue);
		if (iValue > 0)
			Parameter.m_iBackupDays = iValue;
	}
	LOGGER_WRITE(_T("BackupDays:") << Parameter.m_iBackupDays);

	if (ReadConfig(_T("FileSize"), strValue))
	{
		INT iValue = _wtoi(strValue);
		if (iValue > 0 && iValue <= 100)
			Parameter.m_iFileSize = iValue;
	}
	LOGGER_WRITE(_T("FileSize:") << Parameter.m_iFileSize);

	if (ReadConfig(_T("Level"), strValue) && strValue.GetLength() > 0)
	{
		strValue.MakeUpper();
		Parameter.m_bLogLevel[0] = strValue.Find(_T("DEBUG")) >= 0;
		Parameter.m_bLogLevel[1] = strValue.Find(_T("INFO")) >= 0;
		Parameter.m_bLogLevel[2] = strValue.Find(_T("WARNING")) >= 0;
		Parameter.m_bLogLevel[3] = strValue.Find(_T("ERROR")) >= 0;
		Parameter.m_bLogLevel[4] = strValue.Find(_T("FATAL")) >= 0;
	}
	else
		strValue = _T("DEBUG INFO WARNING ERROR FATAL"); // default
	LOGGER_WRITE(_T("Level:") << strValue.GetBuffer());

	if (ReadConfig(_T("AllFilesSize"), strValue))
	{
		INT iValue = _wtoi(strValue);
		if (iValue > Parameter.m_iFileSize * 5) 
			Parameter.m_iAllFilesSize = iValue;
	}
	LOGGER_WRITE(_T("AllFilesSize:") << Parameter.m_iAllFilesSize);

	if (ReadConfig(_T("MinDiskSpace"), strValue))
	{
		INT iValue = _wtoi(strValue);
		if (iValue < 300) iValue = 300;
		Parameter.m_iMinDiskSpace = iValue;
	}
	LOGGER_WRITE(_T("MinDiskSpace:") << Parameter.m_iMinDiskSpace);

	if (ReadConfig(_T("LogFormat"), strValue))
	{
		memset(Parameter.m_cLogFormat, 0, MAX_PATH);
		WideCharToMultiByte(GetCodePage(), 0, strValue, -1, 
			Parameter.m_cLogFormat, MAX_PATH - 1, NULL, NULL);
	}
	LOGGER_WRITE(_T("LogFormat:") << Parameter.m_cLogFormat);

	if (ReadConfig(_T("FileEncoding"), strValue))
	{
		strValue.MakeUpper();
		Parameter.m_nFileEncoding = typeUNICODE;
		if (0 == strValue.Find(_T("UNICODE")))
			Parameter.m_nFileEncoding = typeUNICODE;
		else if (0 == strValue.Find(_T("UTF8")))
			Parameter.m_nFileEncoding = typeUTF8;
		else if (0 == strValue.Find(_T("ANSI")))
			Parameter.m_nFileEncoding = typeANSI;
	}
	LOGGER_WRITE(_T("Encoding:") << Parameter.m_nFileEncoding << _T(" ") << strValue.GetBuffer());

	if (!ReadConfig(_T("FileName"), strValue)
		|| strValue.GetLength() >= MAX_PATH)
		strValue = strAppPath + _T("xlog\\xlog.txt");

	if (PathGetDriveNumber(strValue) == -1)
	{
		strValue.Replace(_T("/"), _T("\\"));
		if (_T('\\') == strValue[0])
			strValue.Delete(0);
		if (strValue.Find(_T('\\')) == -1)
			strValue = m_strAppPath + _T("xlog\\") + strValue;
		else
			strValue = m_strAppPath + strValue;
	}

	SetFileName(strValue, Parameter);

	if (0 != FileType)
	{
		memset(Parameter.m_cExeName, 0, MAX_PATH);
		memcpy(Parameter.m_cExeName, ".xdb", sizeof(".xdb"));
	}

	return TRUE;
}

BOOL CConfigReader::GetDefSection(CString strFileName, 
								  CString &strSection)
{
	TCHAR Buffer[MAX_PATH] = {0};
	GetPrivateProfileSectionNames(Buffer, MAX_PATH - 1, strFileName);

	strSection = _T("");
	if (0 == Buffer[0])
		return FALSE;

	strSection = CString(Buffer);
	return TRUE;
}

BOOL CConfigReader::SetFileName(CString strValue, 
								CParameter &Parameter)
{
	CString strFixFolder, strDynFolder, strCycle, 
		strCreate, strSave, strExeName;

	if (!FileNamePaser(strValue, strFixFolder, strDynFolder,
		strCycle, strCreate, strSave, strExeName))
	{
		LOGGER_WRITE2(_T("!!! File name error:") << strValue.GetBuffer());
		return FALSE;
	}

	UINT iCodePage = GetCodePage();
#define WIDECHARTOMULTIBYTE(WideChar, MultiByte) \
	WideCharToMultiByte(iCodePage, 0, WideChar, -1, MultiByte, MAX_PATH - 1, NULL, NULL);

	WIDECHARTOMULTIBYTE(strFixFolder, Parameter.m_cFixFolder);
	WIDECHARTOMULTIBYTE(strDynFolder, Parameter.m_cCycleFolder);
	WIDECHARTOMULTIBYTE(strCycle,     Parameter.m_cCycleName);
	WIDECHARTOMULTIBYTE(strCreate,    Parameter.m_cCreateTime);
	WIDECHARTOMULTIBYTE(strSave,      Parameter.m_cSaveTime);
	WIDECHARTOMULTIBYTE(strExeName,   Parameter.m_cExeName);

	LOGGER_WRITE_A("FixFolder:" << Parameter.m_cFixFolder
		<< ". CycleFolder:"     << Parameter.m_cCycleFolder
		<< ". CycleName:"       << Parameter.m_cCycleName
		<< ". CreateTime:"      << Parameter.m_cCreateTime
		<< ". SaveTime:"        << Parameter.m_cSaveTime
		<< ". ExeName:"         << Parameter.m_cExeName);

	return TRUE;
}

BOOL CConfigReader::FileNamePaser(CString strValue,
								  CString &strFixFolder, 
								  CString &strDynFolder,
								  CString &strCycle,
								  CString &strCreate, 
								  CString &strSave, 
								  CString &strExeName)
{
	int iIndex = strValue.ReverseFind(_T('.'));
	if (-1 == iIndex)
		return FALSE;

	strExeName = strValue.Mid(iIndex);
	strValue = strValue.Left(iIndex);
	iIndex = strValue.ReverseFind(_T('\\'));
	if (-1 == iIndex)
		return FALSE;

	iIndex += 1;
	FileNamePaser(strValue.Left(iIndex), strFixFolder, strDynFolder);
	strValue.Delete(0, iIndex);

	iIndex = strValue.Find(_T('-'));
	if (-1 != iIndex)
	{
		strSave = strValue.Mid(iIndex);
		strValue = strValue.Left(iIndex);
	}
	iIndex = strValue.Find(_T('['));
	if (-1 != iIndex)
	{
		strCycle = strValue.Left(iIndex);
		strCreate = strValue.Mid(iIndex + 1);
		strCreate.Replace(_T("]"), _T(""));
	}
	else
		strCycle = strValue;

	return TRUE;
}

BOOL CConfigReader::FileNamePaser(CString strValue, 
								  CString &strFixFolder, 
								  CString &strDynFolder)
{
	int iIndex = strValue.Find(_T('%'));
	if (-1 == iIndex)
	{
		strFixFolder = strValue;
		return TRUE;
	}

	for (iIndex; iIndex >= 0; iIndex--)
	{
		if (_T('\\') == strValue[iIndex])
			break;
	}
	ATLASSERT(-1 != iIndex);

	iIndex++;
	strFixFolder = strValue.Left(iIndex);
	strDynFolder = strValue.Mid(iIndex);

	return TRUE;
}

BOOL CConfigReader::ReadConfig(CString strKeyName, 
							   CString &strValue)
{
	strValue = _T("");

#define __GetPrivateProfileString(key, default, value) \
	GetPrivateProfileString(m_strSection, \
	key, default, value.GetBuffer(MAX_PATH), MAX_PATH, m_strFileName);\
	value.ReleaseBuffer(); 

	CString strTemp[2];
	__GetPrivateProfileString(strKeyName, _T(""), strTemp[0]);
	__GetPrivateProfileString(strKeyName, _T("Test"), strTemp[1]);

	if (strTemp[1].GetLength() > MAX_PATH
		|| strTemp[0] != strTemp[1])
		return FALSE;

	strValue = strTemp[0];
	strValue.Replace(_T("_"), _T(""));

	return TRUE;
}
