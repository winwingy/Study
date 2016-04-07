// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 某些 CString 构造函数将是显式的

#ifndef UNICODE
#error "*** 必须定义UNICODE字符集!!!! ***"
#endif

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
#include <atlstr.h>
#include <atlcoll.h>
#include <share.h>
#include <atltime.h>

using namespace ATL;
using namespace std;

#define WRITE_SIMPLE_LOGGER
#include "..\Logger\SimpleLogger.h"

#define MAX_SERVER_COUNT         100

class CTimeInfo
{
	DWORD Time;
	char FileName[MAX_PATH];
	char Function[MAX_PATH * 2];
	int  FileLine;
public:
	CTimeInfo(const char *funct, const char *file, int line) {
		ATLASSERT(NULL != file);
		ATLASSERT(NULL != funct);
		UINT Length = (UINT)strnlen(file, MAX_PATH - 1);
		memcpy(FileName, file, Length);
		FileName[Length] = 0;
		Length = (UINT)strnlen(funct, MAX_PATH * 2 - 1);
		memcpy(Function, funct, Length);
		Function[Length] = 0;
		FileLine = line;
		Time     = GetTickCount();
	}
	~CTimeInfo() {
		DWORD End = GetTickCount();
		if ((End - Time) > 60)
		{
			LOGGER_WRITE2_A("线程:" << GetCurrentThreadId()
				<< " 耗时:" << (End - Time) 
				<< " " << Function 
				<< " " << FileName 
				<< " " << FileLine);
		}
	}
};
#ifdef _DEBUG
#define USING_TIME_INFO CTimeInfo Time##__LINE__##(__FUNCTION__, __FILE__, __LINE__);
#else
#define USING_TIME_INFO CTimeInfo Time##__LINE__##(__FUNCTION__, __FILE__, __LINE__);
#endif