#include "StdAfx.h"
#include "DbFile.h"
#include "..\Logger\XLogUtil.h"

CDbFile::CDbFile(void)
{	
	m_lFileType = 0;

	GetAppVersion(m_sFileInfo);
}

CDbFile::~CDbFile(void)
{
	Close();
}

BOOL CDbFile::Open(LONG FileType,
				   CString strAppPath, 
				   CString strFileName,
				   CString strSection)
{
	m_lFileType = FileType;
	m_oParameter.m_iFileSize = 50;

	CFileBase::InitParameter(strAppPath, strFileName, strSection, FileType);

	return TRUE;
}

BOOL CDbFile::Write(SYSTEMTIME &Time,
					BYTE *Buffer, 
					UINT iLength)
{
	// 两个内容不加密：表示数据的长度和日期时间数据。

	const static size_t iSize   = sizeof(UINT);
	const static size_t iHeader = sizeof(SYSTEMTIME);

	FILE* Handle = NULL;
	if (GetHandle(Time, &Handle))
	{
		if (0 == m_iFileSize)
			WriteHeader(Handle);

		if (m_oParameter.m_bEncrypt)
			NULL;

		fwrite(&iLength, 1, iSize, Handle);
		fwrite(Buffer, 1, iLength, Handle);

		m_iFileSize += (iLength + iSize);
	}

	return TRUE;
}

__inline
BOOL CDbFile::WriteHeader(FILE* Handle)
{
	ATLASSERT(NULL != Handle);

	static UINT iCodePage = GetCodePage();
	static UINT iLength   = sizeof(FILEHEADER);
	
	FILEHEADER Header;
	memset(&Header, 0, iLength);

	strcat_s(Header.cSign, 20, FILESIGN);
	Header.iCodePage = iCodePage;
	Header.bEncrypt  = m_oParameter.m_bEncrypt;
	Header.dwType    = m_lFileType;
	Header.dwVerMS   = m_sFileInfo.dwProductVersionMS;
	Header.dwVerLS   = m_sFileInfo.dwProductVersionLS;

	fwrite(&Header, sizeof(char), iLength, Handle);
	m_iFileSize = iLength;

	return TRUE;
}

void CDbFile::Close()
{
	CFileBase::CloseFile();
}
