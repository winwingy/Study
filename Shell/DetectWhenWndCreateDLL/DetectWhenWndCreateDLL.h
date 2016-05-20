#ifndef _DETECT_WHEN_WND_CREATE_DLL_H_
#define _DETECT_WHEN_WND_CREATE_DLL_H_

#ifdef IN_DLL
#define DLL_FUN __declspec(dllexport)
#else
#define DLL_FUN __declspec(dllimport)
#endif

void DLL_FUN Hook();
void DLL_FUN UnHook();

#endif