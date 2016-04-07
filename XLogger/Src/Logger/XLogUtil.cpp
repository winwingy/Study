#include <ASSERT.H>
#ifndef _INC_TCHAR
#include <tchar.h>
#endif
#ifndef __ATLDEF_H__
#include <atldef.h>
#endif

#include <wtypes.h>
#include <TlHelp32.h>

#include "XLogUtil.h"
#include "SimpleLogger.h"

#pragma comment(lib, "Version.lib")

UINT GetCodePage(UINT LangID /*= 0*/)
{
	if (0 == LangID)
		LangID = GetSystemDefaultLangID();

	WCHAR CodePage[128] = { 0 };
	if (GetLocaleInfoW(LangID, LOCALE_IDEFAULTANSICODEPAGE, CodePage, 127) && 0 != CodePage[0])
		return _wtoi(CodePage);

	return CP_ACP;
}

UINT GetStrSize(const TCHAR *Value, int MaxSize /*= -1*/)
{
	if (NULL == Value)
		return 0;

	return (UINT)_tcscnlen(Value, MaxSize);
}

void MemoryCopy(TCHAR *Buffer, int BSize, const TCHAR *Value, int VSize)
{
#ifdef UNICODE
	wmemcpy_s(Buffer, BSize, Value, VSize);
#else
	memcpy(Buffer, Value, VSize);
#endif
}

void StringCopy(TCHAR *Buffer, UINT Length, const TCHAR *Value)
{
#ifdef UNICODE
	wmemset(Buffer, 0, Length);
	size_t iSize = GetStrSize(Value, Length - 1);
	wmemcpy(Buffer, Value, iSize);
#else
	memset(Buffer, 0, Length);
	size_t iSize = GetStrSize(Value, Length - 1);
	memcpy(Buffer, Value, iSize);
#endif
}

void GetErrorMessage(DWORD Error, TCHAR Buffer[ERROR_BUFFER_SIZE])
{
	LPTSTR Message = NULL;
	if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, Error, 0, (LPTSTR) &Message, 0, NULL) == 0)
	{
		_stprintf_s(Buffer, ERROR_BUFFER_SIZE - 1, _T("HRESULT:%ld"), Error);
		return;
	}

	UINT Length = GetStrSize(Message, 512);
	while (Length > 0 && (Message[Length - 1] == _T('\r') 
		|| Message[Length - 1] == _T('\n')))
	{
		Message[Length - 1] = 0;
		Length--;
	}

	if (ERROR_BUFFER_SIZE <= Length)
		Length = ERROR_BUFFER_SIZE - 1;

	MemoryCopy(Buffer, Length, Message, Length);
	Buffer[Length] = 0;

	LocalFree(Message);
	Message = NULL;
}

BOOL GetDefaultSection(const TCHAR *FileName, TCHAR Section[MAX_PATH])
{
	TCHAR Buffer[ERROR_BUFFER_SIZE] = {0};
	GetPrivateProfileSectionNames(Buffer, 1023, FileName);

	if (0 == Buffer[0])
		return FALSE;

	UINT Length = GetStrSize(Buffer, MAX_PATH);
	if (MAX_PATH <= Length)
		return FALSE;	

	MemoryCopy(Section, MAX_PATH, Buffer, Length);
	Section[Length] = 0;

	return TRUE;
}

BOOL WriteConfig(const TCHAR *FileName, 
				 const TCHAR *Section, 
				 const TCHAR *Config)
{
	const int MaxCount = 1024;
	const int ItemCount = 512;

	int Index = 0;
	int Begin = 0;

	int Length = GetStrSize(Config, MaxCount);

	while (Index < Length)
	{
		while (Index < Length && Config[Index] != _T(',')) Index++;
		if (Index > Begin)
		{
			int Length2 = GetStrSize(Config, MaxCount);
			if (Length2 >= ItemCount)
				return FALSE;

			TCHAR Data[ItemCount] = { 0 };
			MemoryCopy(Data, ItemCount, Config + Begin, Index - Begin);

			TCHAR Key[ItemCount] = { 0 };
			TCHAR Value[ItemCount] = { 0 };

			int Index2 = 0;
			while (Index2 < Length2 && Data[Index2] != _T('=')) { Key[Index2] = Data[Index2]; Index2++; }

			int Index3 = ++Index2;
			while (Index2 < Length2) { Value[Index2 - Index3] = Data[Index2]; Index2++; }

			if (GetStrSize(Key, ItemCount) && GetStrSize(Value, ItemCount))
				WritePrivateProfileString(Section, Key, Value, FileName);

			Begin = ++Index;
		}
	}

	return TRUE;
}

BOOL CreateTempConfig(const TCHAR *FileName, 
					  const TCHAR *Section, 
					  const TCHAR *Config, 
					  TCHAR TempFile[MAX_PATH])
{
	TCHAR Path[MAX_PATH] = { 0 };
	if (0 == GetTempPath(MAX_PATH, Path))
	{
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 }; 
		GetErrorMessage(GetLastError(), Buffer); 
		LOGGER_WRITE2(_T("临时路径获取失败:") << Buffer);
		return FALSE;
	}

	if (0 == GetTempFileName(Path, _T("XLg"), 0, TempFile))
	{
		TCHAR Buffer[ERROR_BUFFER_SIZE] = { 0 }; 
		GetErrorMessage(GetLastError(), Buffer); 
		LOGGER_WRITE2(_T("临时路径获取失败:") << Buffer);
		return FALSE;
	}

	if (NULL != FileName)
	{
		CopyFile(FileName, TempFile, FALSE);
		if (NULL == Section)
		{
			TCHAR TempSection[MAX_PATH] = { 0 };
			if (!GetDefaultSection(TempFile, TempSection))
			{
				LOGGER_WRITE2(_T("查询文件默认节名称失败."));
				return FALSE;
			}

			WriteConfig(TempFile, TempSection, Config);
		}
		else
		{
			WriteConfig(TempFile, Section, Config);
		}
	}
	else
	{
		WriteConfig(TempFile, _T("Default"), Config);
	}

	return TRUE;
}

BOOL GetAppVersion(VS_FIXEDFILEINFO &FileInfo)
{
	BOOL RetVal = FALSE;
	BYTE *VerBuffer = NULL;

	do 
	{
		TCHAR AppName[MAX_PATH] = { 0 };
		GetModuleFileName(GetModuleHandle(NULL), AppName, MAX_PATH);
		memset(&FileInfo, 0, sizeof(VS_FIXEDFILEINFO));

		DWORD VerSize = GetFileVersionInfoSize(AppName, 0); 
		if (0 == VerSize)
			break;

		VerBuffer = new BYTE[VerSize + 1];
		if (!GetFileVersionInfo(AppName, 0, VerSize, (LPVOID)VerBuffer))
			break;

		UINT ViodSize = 0;
		LPVOID lpVoid = NULL;
		if (!VerQueryValue(VerBuffer, _T ("\\"), &lpVoid, &ViodSize))
			break;

		memcpy(&FileInfo, lpVoid, ViodSize);
		RetVal = TRUE;

#pragma warning(push)
#pragma warning(disable:4127)
	} while (FALSE);
#pragma warning(pop)

	if (NULL != VerBuffer)
	{
		delete []VerBuffer;
		VerBuffer = NULL;
	}

	return RetVal;
}

LONG GetProcessidFromName(LPCTSTR name)
{
	PROCESSENTRY32 pe;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
		return -1;

	LONG id = 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnapshot, &pe))
	{
		while(1)
		{
			pe.dwSize = sizeof(PROCESSENTRY32);
			if (!Process32Next(hSnapshot,&pe))
				break;

			if (_tcscmp(pe.szExeFile, name) == 0)
			{
				id = pe.th32ProcessID;
				break;
			}
		}
	}

	CloseHandle(hSnapshot);
	return id;
}
