#include "StdAfx.h"
#include "Parameter.h"

CParameter::CParameter(void)
{
	UINT iMillion   = 1/*1024 * 1024*/;
	m_bEncrypt      = FALSE;
	m_bCompress     = FALSE;
	m_iAllFilesSize = 300;
	m_iBackupDays   = 90;
	m_iFileSize     = iMillion * 5; 
	m_iMinDiskSpace = iMillion * 300;
	m_nFileEncoding = typeUTF8;

	memset(m_cFixFolder,   0, MAX_PATH);
	memset(m_cCycleFolder, 0, MAX_PATH);
	memset(m_cCycleName,   0, MAX_PATH);
	memset(m_cCreateTime,  0, MAX_PATH);
	memset(m_cSaveTime,    0, MAX_PATH);
	memset(m_cExeName,     0, MAX_PATH);
	memset(m_cLogFormat,   0, MAX_PATH);

#pragma warning(push)
#pragma warning(disable: 4996)
	strcat(m_cLogFormat, "%Y/%m/%d %H:%M:%S %03N ");
#pragma warning(pop)

	for (int i = 0; i < 5; i++)
		m_bLogLevel[i] = TRUE;
	m_bDelete = FALSE;
}

CParameter::~CParameter(void)
{
}

void CParameter::operator=(const CParameter& Parameter) 
{ 
	m_bEncrypt      = Parameter.m_bEncrypt;
	m_bCompress     = Parameter.m_bCompress;
	m_iAllFilesSize = Parameter.m_iAllFilesSize;
	m_iBackupDays   = Parameter.m_iBackupDays;
	m_iFileSize     = Parameter.m_iFileSize; 
	m_iMinDiskSpace = Parameter.m_iMinDiskSpace;
	m_nFileEncoding = Parameter.m_nFileEncoding;
	m_bDelete       = Parameter.m_bDelete;

	memcpy(m_cFixFolder,   Parameter.m_cFixFolder,   MAX_PATH);
	memcpy(m_cCycleFolder, Parameter.m_cCycleFolder, MAX_PATH);
	memcpy(m_cCycleName,   Parameter.m_cCycleName,   MAX_PATH);
	memcpy(m_cCreateTime,  Parameter.m_cCreateTime,  MAX_PATH);
	memcpy(m_cSaveTime,    Parameter.m_cSaveTime,    MAX_PATH);
	memcpy(m_cExeName,     Parameter.m_cExeName,     MAX_PATH);
	memcpy(m_cLogFormat,   Parameter.m_cLogFormat,   MAX_PATH);
	memcpy(m_bLogLevel,    Parameter.m_bLogLevel,    sizeof(m_bLogLevel));
}

BOOL CParameter::operator==(const CParameter& Parameter) const
{
	if (m_bEncrypt != Parameter.m_bEncrypt)
		return FALSE;

	if (m_bCompress != Parameter.m_bCompress)
		return FALSE;

	if (m_iAllFilesSize != Parameter.m_iAllFilesSize)
		return FALSE;

	if (m_iBackupDays != Parameter.m_iBackupDays)
		return FALSE;

	if (m_iFileSize != Parameter.m_iFileSize)
		return FALSE;

	if (m_iMinDiskSpace != Parameter.m_iMinDiskSpace)
		return FALSE;

	if (m_nFileEncoding != Parameter.m_nFileEncoding)
		return FALSE;

	if (0 != memcmp(m_cFixFolder, Parameter.m_cFixFolder, MAX_PATH)) 
		return FALSE;

	if (0 != memcmp(m_cCycleFolder, Parameter.m_cCycleFolder, MAX_PATH)) 
		return FALSE;

	if (0 != memcmp(m_cCycleName, Parameter.m_cCycleName, MAX_PATH))
		return FALSE;

	if (0 != memcmp(m_cCreateTime, Parameter.m_cCreateTime, MAX_PATH)) 
		return FALSE;

	if (0 != memcmp(m_cSaveTime, Parameter.m_cSaveTime, MAX_PATH)) 
		return FALSE;

	if (0 != memcmp(m_cExeName, Parameter.m_cExeName, MAX_PATH))
		return FALSE;

	if (0 != memcmp(m_cLogFormat, Parameter.m_cLogFormat, MAX_PATH)) 
		return FALSE;

	if (0 != memcmp(m_bLogLevel, Parameter.m_bLogLevel, sizeof(m_bLogLevel))) 
		return FALSE;

	return TRUE;
}
