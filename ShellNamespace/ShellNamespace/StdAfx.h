// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(__STD_AFX_H__)
#define __STD_AFX_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
#include <shlobj.h>
#include "SEModule.h"
#include "resource.h"

extern CSEModule _Module;
#define MAX_STRLEN 2048


#include <atlcom.h>
#include <assert.h>
#include <Windows.h>
extern const CLSID CLSID_SEShellFolder;
extern const CLSID CLSID_SEShellView;
extern const CLSID CLSID_SEOverlayIcon;
extern const TCHAR g_szRoot[MAX_PATH];
extern TCHAR g_szRealRoot[MAX_PATH];

#endif // !defined(__STD_AFX_H__)
