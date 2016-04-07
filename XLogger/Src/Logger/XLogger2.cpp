#include <wtypes.h>
#include <tchar.h>
#include <atldef.h>
#include <atlsync.h>
#include <atlcoll.h>
#include <shlobj.h>

#include "XLogger.h"
#include "XLogger2.h"
#include "Connection.h"
#include "XLogUtil.h"
#include "SimpleLogger.h"

namespace XLOGGER {

#define BUFFER_SIZE     1025

	extern UINT SetSystemTime(CHAR *Buffer, XLogLevel eLevel);
	extern BOOL XLogWriteTo(XCONNECTDATA *Logger, const BYTE *Buffer, UINT Length);

	struct HANDLEDATA {
		char FileName[MAX_PATH];
		HANDLE Handle;
		int Line;
	};
	struct CRTSECTION {
		char FileName[MAX_PATH];
		LPVOID Section;
		DWORD Begin;
		DWORD End;
		int Line;
	};

	HANDLE       ProcessHandle = NULL;
	static long  ObjectNumber = 0;
	SYSTEM_INFO  SystemInfo;

	ATL::CCriticalSection     *pSection = NULL;
	ATL::CAtlArray<HANDLEDATA> arrHandles;
	ATL::CAtlArray<CRTSECTION> arrCrtSection;

	void GetConnectInfo2(XCONNECTDATA **Connect)
	{
		static XCONNECTDATA ConnectInfo2;
		*Connect = &ConnectInfo2;
	}

	class CSection {
		ATL::CCriticalSection *m_pSection;
	public:
		CSection(ATL::CCriticalSection *pSection) : m_pSection(NULL) {
			ATLASSERT(NULL != pSection);
			m_pSection = pSection;
			m_pSection->Enter();
		}
		~CSection() {
			ATLASSERT(NULL != m_pSection);
			m_pSection->Leave();
		}
	};

	BOOL GetLogPath(TCHAR Path[MAX_PATH])
	{
		memset(Path, 0, sizeof(TCHAR) * MAX_PATH);
		if (!SHGetSpecialFolderPath(NULL, Path, CSIDL_MYDOCUMENTS, FALSE))
		{
			TCHAR Error[ERROR_BUFFER_SIZE] = { 0 }; 
			GetErrorMessage(GetLastError(), Error); 
			LOGGER_WRITE2(_T("查询日志存储路径失败:") << Error);
			return FALSE;
		}

#pragma warning(push)
#pragma warning(disable:4996)	
		_tcscat(Path, _T("\\CSD\\log\\xlog\\"));
#pragma warning(pop)

		return TRUE;
	}

	BOOL WINAPI OpenX(const TCHAR *FileName /*= NULL*/)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);

		if (1 < InterlockedIncrement(&Connect->lOpenCount))
			return TRUE;

		TRACE_FUNCTION_INFO;
		TCHAR Buffer[BUFFER_SIZE] = { 0 };
		const TCHAR *Config = _T("FileName=Log%m%d.txt,LogFormat=%Y/%m/%d %H:%M:%S %03N %05P %05T");

		if (NULL != FileName)
		{
			TCHAR Path[MAX_PATH] = { 0 };
			if (!GetLogPath(Path))
				return FALSE;

			SYSTEMTIME Time;
			GetLocalTime(&Time);

			TCHAR File[MAX_PATH] = { 0 };
			_stprintf_s(File, MAX_PATH - 1, _T("FileName=%s%s %d%d%d%d.txt"), 
				Path, FileName, Time.wMonth, Time.wDay, Time.wHour, Time.wMinute);
			_stprintf_s(Buffer, BUFFER_SIZE - 1, _T("%s%s"), File,
				_T(",LogFormat=%Y/%m/%d %H:%M:%S %03N %05P %05T,BackupDays=10,FileSize=40"));

			Config = Buffer;
		}
			
		ProcessHandle = GetCurrentProcess();
		memset(&SystemInfo, 0, sizeof(SystemInfo));
		GetSystemInfo(&SystemInfo);
		if (0 == SystemInfo.dwNumberOfProcessors)
			SystemInfo.dwNumberOfProcessors = 1;

		Connect->Init();
		if (!Open2(NULL, NULL, 0, Config, Connect))
			return FALSE;

		Connect->lOpenCount = 1;
		pSection = new ATL::CCriticalSection();
		return TRUE;
	}

	void SaveHandle();
	void WINAPI CloseX()
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);

		if (0 == InterlockedDecrement(&Connect->lOpenCount))
		{
			TRACE_FUNCTION_INFO;
			SaveHandle();
			if (NULL != pSection)
			{
				delete pSection;
				pSection = NULL;
			}
			Close2(Connect);			
		}
	}

	LONG64 TimeToInt64(FILETIME Time)
	{
		LONG64 Value = Time.dwHighDateTime;
		return ((Value << 32) + Time.dwLowDateTime);
	}

	double GetCPUUsage(DWORD TickCount, FILETIME Begin[4])
	{
		double RetVal = 0;

		if (100 < TickCount)
		{
			FILETIME Current[4];
			if (GetThreadTimes(GetCurrentThread(), &Current[0], 
				&Current[1], &Current[2], &Current[3]))
			{
				// 说明，GetThreadTimes 函数返回时间单位是100纳秒，转换为毫秒应该除10000。
				// 由于输出值是百分比，所以要减小100，最后是除100。另外，再除以CPU核心个数。
				// 参考：1秒=1000毫秒 1毫秒=1000微秒 1微秒=1000纳秒 
				LONG64 KernelTime = TimeToInt64(Current[2]) - TimeToInt64(Begin[2]);
				LONG64 UserTime   = TimeToInt64(Current[3]) - TimeToInt64(Begin[3]);
				LONG64 AllUsing   = (KernelTime + UserTime) / 100 / SystemInfo.dwNumberOfProcessors; 
				RetVal = AllUsing / (double)TickCount;
			}
		}

		return RetVal;
	}

	void SetHandleData(HANDLE Handle, const CHAR* File, INT Line, HANDLEDATA &Data)
	{
		memset(&Data, 0, sizeof(Data));
		strcpy_s(Data.FileName, MAX_PATH - 1, File);
		Data.Line   = Line;
		Data.Handle = Handle;
	}

	void SetSectionData(LPCRITICAL_SECTION Section, const CHAR* File, INT Line, CRTSECTION &Data)
	{
		memset(&Data, 0, sizeof(Data));
		strcpy_s(Data.FileName, MAX_PATH - 1, File);
		Data.Line    = Line;
		Data.Section = Section;
		Data.Begin   = 0;
		Data.End     = 0;
	}

	void XLogWriteTo2(const char *Format, ...)
	{
		CHAR Buffer[BUFFER_SIZE] = { 0 };
		const UINT iHeadSize = SetSystemTime(Buffer, logInfo);
		const UINT iBuffSize = BUFFER_SIZE - iHeadSize;

		va_list argList;
		va_start(argList, Format);
#pragma warning(push)
#pragma warning(disable:4996)
		UINT Length = vsprintf(Buffer + iHeadSize, Format, argList);
#pragma warning(pop)
		va_end(argList);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		XLogWriteTo(Connect, (const BYTE*)Buffer, Length + iHeadSize);
	}

	void SaveHandle() 
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);

		if (!Connect->bLogOpened 
			|| NULL == pSection)
			return;

		CSection Lock(pSection);
		int Count = (int)arrHandles.GetCount();
		for (int i = 0; i < Count; i++)
		{
			HANDLEDATA &Data = arrHandles[i];
			XLogWriteTo2("*!!! Handle: %ld [.%s %d]", Data.Handle, Data.FileName, Data.Line);
		}
		arrHandles.RemoveAll();

		Count = (int)arrCrtSection.GetCount();
		for (int i = 0; i < Count; i++)
		{
			CRTSECTION &Data = arrCrtSection[i];
			XLogWriteTo2("CriticalSection !!! [.%s %d]", Data.FileName, Data.Line);
		}
		arrCrtSection.RemoveAll();
	}

	XLogger2::XLogger2(const char *funct, const char *file, int line)
	{
		ATLASSERT(NULL != file);
		ATLASSERT(NULL != funct);

		UINT Length = (UINT)strnlen(file, MAX_PATH - 1);
		memcpy(FileName, file, Length);
		FileName[Length] = 0;

		Length = (UINT)strnlen(funct, MAX_PATH * 2 - 1);
		memcpy(Function, funct, Length);
		Function[Length] = 0;

		FileLine = line;
		ObjectID = InterlockedIncrement(&ObjectNumber);
		Time     = GetTickCount();

		HANDLE Handle = GetCurrentThread();
		GetThreadTimes(Handle, &Begin[0], &Begin[1], &Begin[2], &Begin[3]);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
			XLogWriteTo2("1|%s|%s|%d|%ld", Function, FileName, FileLine, ObjectID);
	}

	XLogger2::~XLogger2(void)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
		{		
			DWORD TickCount = GetTickCount() - Time;
			double CPUUsage = GetCPUUsage(TickCount, Begin);
			XLogWriteTo2("0|%s|%s|%d|%ld|%ld|%.1lf", Function, FileName, FileLine, ObjectID, TickCount, CPUUsage);
		}
	}

	HANDLE WINAPI CreateEventX(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
		BOOL bInitialState, LPCTSTR lpName, CHAR* File, INT Line)
	{
		HANDLE Handle = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
		{
			HANDLEDATA Data;
			SetHandleData(Handle, File, Line, Data);
			XLogWriteTo2("*Add Handle: %ld [.%s %d]", Data.Handle, File, Line);

			CSection Lock(pSection);
			arrHandles.Add(Data);
		}
		return Handle; 
	}

	HANDLE WINAPI CreateThreadX(LPSECURITY_ATTRIBUTES Attributes, SIZE_T Size, LPTHREAD_START_ROUTINE Address, 
		LPVOID Parameter, DWORD Flags,LPDWORD Id, CHAR* File, INT Line)
	{
		HANDLE Handle = ::CreateThread(Attributes, Size, Address, Parameter, Flags, Id);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
		{
			HANDLEDATA Data;
			SetHandleData(Handle, File, Line, Data);
			XLogWriteTo2("*Add Handle: %ld [.%s %d]", Data.Handle, File, Line);

			CSection Lock(pSection);
			arrHandles.Add(Data);
		}
		return Handle; 
	}

	uintptr_t _beginthreadX(void( *start_address )( void * ), unsigned stack_size, void *arglist, CHAR* File, INT Line)
	{
		uintptr_t Handle = _beginthread(start_address, stack_size, arglist);
		
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
		{
			HANDLEDATA Data;
			SetHandleData((HANDLE)Handle, File, Line, Data);
			XLogWriteTo2("*Add Handle: %ld [.%s %d]", Data.Handle, File, Line);

			CSection Lock(pSection);
			arrHandles.Add(Data);
		}

		return Handle; 
	}

	uintptr_t _beginthreadexX(void * _Security, unsigned _StackSize, unsigned (__stdcall * _StartAddress) (void *), 
		void * _ArgList, unsigned _InitFlag, unsigned * _ThrdAddr, CHAR* File, INT Line)
	{
		uintptr_t Handle = _beginthreadex(_Security, _StackSize, _StartAddress, _ArgList, _InitFlag, _ThrdAddr);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (Connect->bLogOpened && NULL != pSection)
		{
			HANDLEDATA Data;
			SetHandleData((HANDLE)Handle, File, Line, Data);
			XLogWriteTo2("*Add Handle: %ld [.%s %d]", Data.Handle, File, Line);

			CSection Lock(pSection);
			arrHandles.Add(Data);
		}

		return Handle; 
	}

	BOOL WINAPI CloseHandleX(HANDLE Handle, CHAR* File, INT Line)
	{
		BOOL RetVal = CloseHandle(Handle);

		XCONNECTDATA *Connect = NULL;
		GetConnectInfo2(&Connect);
		if (!Connect->bLogOpened)
			return RetVal;
		
		CSection Lock(pSection);
		int Count = (int)arrHandles.GetCount();
		for (int i = 0; i < Count; i++)
		{
			if (arrHandles[i].Handle == Handle)
			{
				arrHandles.RemoveAt(i);					
				break;
			}
		}

		XLogWriteTo2("*Delete Handle: %ld [.%s %d]", Handle, File, Line);
		return RetVal;
	}

	class CLock0 {
		HANDLEDATA m_sData;
		DWORD m_dwBegin;

	public:
		CLock0(HANDLE Handle, CHAR* File, INT Line) {
			XCONNECTDATA *Connect = NULL;
			GetConnectInfo2(&Connect);
			if (Connect->bLogOpened && NULL != pSection) {
				m_dwBegin = GetTickCount();
				SetHandleData(Handle, File, Line, m_sData);
				XLogWriteTo2("WaitForSingleObject -> Handle: %ld [.%s %d]",
					Handle, File, Line);
			}
		}
		~CLock0() {
			XCONNECTDATA *Connect = NULL;
			GetConnectInfo2(&Connect);
			if (Connect->bLogOpened && NULL != pSection) {
				XLogWriteTo2("WaitForSingleObject <- Handle: %ld Time: %ld [.%s %d]",
					m_sData.Handle, GetTickCount() - m_dwBegin, m_sData.FileName, m_sData.Line);
			}
		}
	};

	DWORD WINAPI WaitForSingleObjectX(HANDLE Handle, DWORD Milliseconds, CHAR* File, INT Line)
	{
		CLock0 Lock(Handle, File, Line);
		return WaitForSingleObject(Handle, Milliseconds);
	}

	class CLock1 {
		CRTSECTION m_sData;
	public:
		CLock1(LPCRITICAL_SECTION Section, CHAR* File, INT Line) {
			SetSectionData(Section, File, Line, m_sData);
			m_sData.Begin = GetTickCount();

			XCONNECTDATA *Connect = NULL;
			GetConnectInfo2(&Connect);
			if (Connect->bLogOpened && NULL != pSection) {
				CSection Lock(pSection);
				arrCrtSection.Add(m_sData);
				XLogWriteTo2("CriticalSection -> Object: %ld [.%s %d]",
					(LONG)Section, File, Line);
			}
		}
		~CLock1() {
			XCONNECTDATA *Connect = NULL;
			GetConnectInfo2(&Connect);
			if (Connect->bLogOpened && NULL != pSection) {
				CSection Lock(pSection);
				int Count = (int)arrCrtSection.GetCount();
				for (int i = 0; i < Count; i++) {
					CRTSECTION &sData = arrCrtSection[i];
					if (sData.Section == m_sData.Section) {
						sData.End = GetTickCount();
						break;
					}
				}
			} 
		}
	};

	void WINAPI EnterCriticalSectionX(LPCRITICAL_SECTION Section, CHAR* File, INT Line)
	{
		//CLock1 Lock(Section, File, Line);
		::EnterCriticalSection(Section);
	}

	void WINAPI LeaveCriticalSectionX(LPCRITICAL_SECTION Section, CHAR* File, INT Line)
	{
		::LeaveCriticalSection(Section);
		/*if (TRUE == ConnectInfo2.bLogOpened && NULL != pSection) {
			CSection Lock(pSection);
			int Count = arrCrtSection.GetCount();
			for (int i = 0; i < Count; i++)
			{
				CRTSECTION &sData = arrCrtSection[i];
				if (sData.Section == Section)
				{
					XLogWriteTo2("CriticalSection <- Object: %ld Wait: %ld Leave: %ld [.%s %d]",
						(LONG)Section, sData.End - sData.Begin, GetTickCount() - sData.Begin,
						File, Line);
					arrCrtSection.RemoveAt(i);
					break;
				}
			}
		} //*/
	}

};
