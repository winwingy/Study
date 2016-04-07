#ifndef _INC_STDIO
#include <STDIO.H>
#endif
#ifndef _INC_SHARE
#include <SHARE.H>
#endif
#ifndef __wtypes_h__
#include <WTYPES.H>
#endif
#ifndef _INC_WCHAR
#include <WCHAR.H>
#endif

#include "SimpleLogger.h"

#define SL_MAX_LOG_LENGTH           512
#define SL_MAX_FILE_SIZE            (1024 * 1024 * 3)
#define SL_MAX_FILE_COUNT           5

namespace SIMPLE_LOGGER {

	class CFileWriter
	{
	public:
		CFileWriter(void)
		{
#pragma warning(push)
#pragma warning(disable:4996)
			InitializeCriticalSection(&CritSection);
			FileSize = 0;
			hFileHandle = NULL;
			wmemset(FolderName, 0, MAX_PATH);
			wmemset(FullPathName, 0, MAX_PATH);
			wmemset(FileName, 0, MAX_PATH);
			WCHAR ModuleFile[MAX_PATH] = { 0 };
			::GetModuleFileNameW(GetModuleHandle(NULL), ModuleFile, MAX_PATH);
			WCHAR *Temp = wcsrchr(ModuleFile, L'.');
			wmemcpy(Temp, L"Log.txt", 7);
			//swprintf(Temp, (MAX_PATH - wcslen(ModuleFile) + wcslen(Temp)), L"%XLog.txt", GetCurrentProcessId());
			wmemcpy(FullPathName, ModuleFile, MAX_PATH);
			Temp = wcsrchr(ModuleFile, L'\\');
			wcscpy(FileName, Temp + 1); Temp[1] = 0;
			wcscpy(FolderName, ModuleFile);
			WCHAR BkFileName[MAX_PATH];
			BkFileCount = SL_MAX_FILE_COUNT;
			do {
				wprintf(BkFileName, L"%s%d_%s", FolderName, BkFileCount - 1, FileName);
			} while(!FileExisted(BkFileName) && --BkFileCount > 0);
			Open();
#pragma warning(pop)
		}

		~CFileWriter(void) 
		{
			Close();
			DeleteCriticalSection(&CritSection);
		}

		bool FileExisted(WCHAR *FileName) 
		{
			DWORD dwAttricutes = ::GetFileAttributesW(FileName);
			return dwAttricutes != DWORD(-1) && ((dwAttricutes & FILE_ATTRIBUTE_DIRECTORY) == 0) ;
		}

		void Open()
		{
			if (NULL != hFileHandle)
				return;

			hFileHandle = _wfsopen(FullPathName, L"ab", _SH_DENYNO);
			if (NULL == hFileHandle)
				return;

			fseek(hFileHandle, 0, SEEK_END);
			FileSize = ftell(hFileHandle);
		}

		void Close()
		{
			if (NULL != hFileHandle) {
				fclose(hFileHandle);
				hFileHandle = NULL;
				FileSize = 0;
			}			
		}

		void Write(const char *Value, const char* File, int Line, BOOL bFlush)
		{
			if (SL_MAX_FILE_SIZE < FileSize)
				FileBackup();

			if (NULL != hFileHandle) {
				SYSTEMTIME Time;
				GetLocalTime(&Time);
				FileSize += fprintf(hFileHandle,
					"%04d/%02d/%02d %02d:%02d:%02d %03d %s [%s %d]\r\n", 
					Time.wYear, Time.wMonth, Time.wDay, 
					Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds,
					Value, 
					File, Line);
				if (bFlush)
					fflush(hFileHandle);
			}
		}

		void FileBackup()
		{
			WCHAR BkFileName[MAX_PATH];
			if (SL_MAX_FILE_COUNT == BkFileCount) {
				WCHAR BkFileName2[MAX_PATH];
				wprintf(BkFileName2, L"%s%d_%s", FolderName, 0, FileName);
				DeleteFileW(BkFileName2);
				for (int i = 1; i < SL_MAX_FILE_COUNT; i++) {
					wprintf(BkFileName,  L"%s%d_%s", FolderName, i - 1, FileName);
					wprintf(BkFileName2, L"%s%d_%s", FolderName, i, FileName);
					_wrename(BkFileName2, BkFileName);
				}
				BkFileCount--;
			}

			Close();
			wprintf(BkFileName, L"%s%d_%s", FolderName, BkFileCount++, FileName);
			_wrename(FullPathName, BkFileName);
			Open();
		}

		LONG FileSize;
		UINT BkFileCount;
		FILE* hFileHandle;
		WCHAR FolderName[MAX_PATH];
		WCHAR FullPathName[MAX_PATH];
		WCHAR FileName[MAX_PATH];
		CRITICAL_SECTION CritSection;
	};

	void WriteToInternal(const CHAR *Value, const CHAR* File, int Line, BOOL bFlush)
	{
		try { 
			static CFileWriter LogWriter;
			CRITICAL_SECTION &hCritSection = LogWriter.CritSection;
			::EnterCriticalSection(&hCritSection);
			LogWriter.Write(Value, File, Line, bFlush); 
			::LeaveCriticalSection(&hCritSection);
		}
		catch (...) {}
	}

	__inline
	void Ellipsis(char Buff[3])
	{
		Buff[0] = '.'; Buff[1] = '.'; Buff[2] = '.';
	}

	void S_WriteA(const CHAR *Value, const CHAR* File, int Line, BOOL bFlush)
	{
		if (NULL != Value) {
			if (SL_MAX_LOG_LENGTH <= strlen(Value)) {
				char Buffer[SL_MAX_LOG_LENGTH] = { 0 };
				memcpy(Buffer, Value, SL_MAX_LOG_LENGTH - 4);
				Ellipsis(Buffer + (SL_MAX_LOG_LENGTH - 4));
				WriteToInternal(Buffer, File, Line, bFlush);
			}
			else
				WriteToInternal(Value, File, Line, bFlush);
		}
	}

	void S_Write(const TCHAR *Value, const CHAR* File, int Line, BOOL bFlush)
	{
		if (NULL != Value) {
#ifdef UNICODE
			char Buffer[SL_MAX_LOG_LENGTH] = { 0 };
			if (0 == WideCharToMultiByte(CP_ACP, 0, Value, -1, Buffer, SL_MAX_LOG_LENGTH - 1, NULL, NULL))
				Ellipsis(Buffer + (SL_MAX_LOG_LENGTH - 4));
			WriteToInternal(Buffer, File, Line, bFlush);
#else
			S_WriteA(Value, File, Line, bFlush);
#endif
		}
	}
};
