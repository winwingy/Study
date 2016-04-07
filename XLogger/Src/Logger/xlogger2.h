#pragma once

#ifndef __wtypes_h__
#include <wtypes.h>
#endif

#ifdef ENABLE_XLOGGER
#define XLOGGER2_OPEN(FileName)                            XLOGGER::OpenX(FileName);
#define XLOGGER2_CLOSE                                     XLOGGER::CloseX();
#define XLOGGER2_FUNCT_INFO                                XLOGGER::XLogger2 object##__LINE__##(__FUNCTION__, __FILE__, __LINE__);
#define XLOGGER2_CreateEvent(VAL0, VAL1, VAL2, VAL3)       XLOGGER::CreateEventX(VAL0, VAL1, VAL2, VAL3, __FILE__, __LINE__)
#define XLOGGER2_CreateThread(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5) XLOGGER::CreateThreadX(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5, __FILE__, __LINE__)
#define XLOGGER2__beginthread(VAL0, VAL1, VAL2)            XLOGGER::_beginthreadX(VAL0, VAL1, VAL2, __FILE__, __LINE__)
#define XLOGGER2__beginthreadex(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5) XLOGGER::_beginthreadexX(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5, __FILE__, __LINE__)  
#define XLOGGER2_CloseHandle(Handle)                       XLOGGER::CloseHandleX(Handle, __FILE__, __LINE__)
#define XLOGGER2_WaitForSingleObject(Handle, Milliseconds) XLOGGER::WaitForSingleObjectX(Handle, Milliseconds, __FILE__, __LINE__)
#define XLOGGER2_EnterCriticalSection(Section)             XLOGGER::EnterCriticalSectionX(Section, __FILE__, __LINE__)
#define XLOGGER2_LeaveCriticalSection(Section)             XLOGGER::LeaveCriticalSectionX(Section, __FILE__, __LINE__)
#else
#define XLOGGER2_OPEN(FileName)                            NULL;
#define XLOGGER2_CLOSE                                     NULL;
#define XLOGGER2_FUNCT_INFO                                NULL;
#define XLOGGER2_CreateEvent(VAL0, VAL1, VAL2, VAL3)       ::CreateEvent(VAL0, VAL1, VAL2, VAL3)
#define XLOGGER2_CreateThread(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5) ::CreateThread(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5)
#define XLOGGER2__beginthread(VAL0, VAL1, VAL2)            ::_beginthread(VAL0, VAL1, VAL2)
#define XLOGGER2__beginthreadex(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5) ::_beginthreadex(VAL0, VAL1, VAL2, VAL3, VAL4, VAL5)  
#define XLOGGER2_CloseHandle(Handle)                       ::CloseHandle(Handle)
#define XLOGGER2_WaitForSingleObject(Handle, Milliseconds) ::WaitForSingleObject(Handle, Milliseconds)
#define XLOGGER2_EnterCriticalSection(Section)             ::EnterCriticalSection(Section)
#define XLOGGER2_LeaveCriticalSection(Section)             ::LeaveCriticalSection(Section)
#endif

namespace XLOGGER {

	BOOL WINAPI OpenX(const TCHAR *FileName = NULL);
	void WINAPI CloseX();
	HANDLE WINAPI CreateEventX(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset,
		BOOL bInitialState, LPCTSTR lpName, CHAR* File, INT Line);
	HANDLE WINAPI CreateThreadX(LPSECURITY_ATTRIBUTES Attributes, SIZE_T Size, 
		LPTHREAD_START_ROUTINE Address, LPVOID Parameter, DWORD Flags,LPDWORD Id, CHAR* File, INT Line);
	uintptr_t _beginthreadX(void( *start_address )( void * ), unsigned stack_size, void *arglist, CHAR* File, INT Line);
	uintptr_t _beginthreadexX(void * _Security, unsigned _StackSize, unsigned (__stdcall * _StartAddress) (void *), 
		void * _ArgList, unsigned _InitFlag, unsigned * _ThrdAddr, CHAR* File, INT Line);
	BOOL WINAPI CloseHandleX(HANDLE Handle, CHAR* File, INT Line);
	DWORD WINAPI WaitForSingleObjectX(HANDLE Handle, DWORD Milliseconds, CHAR* File, INT Line);
	void WINAPI EnterCriticalSectionX(LPCRITICAL_SECTION Section, CHAR* File, INT Line);
	void WINAPI LeaveCriticalSectionX(LPCRITICAL_SECTION Section, CHAR* File, INT Line);

	class XLogger2
	{
		DWORD Time;
		FILETIME Begin[4];
		int  FileLine;
		char FileName[MAX_PATH];
		char Function[MAX_PATH * 2];
		long ObjectID;

	public:
		XLogger2(const char *funct, const char *file, int line);
		~XLogger2();
	};
};
