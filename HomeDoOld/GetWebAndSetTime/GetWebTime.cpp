#include <Windows.h>
#include <WinInet.h>
#include <stdio.h>
#include <tchar.h>
#pragma comment(lib, "winInet.lib")
#include <string>
using namespace std;




// HTTP/1.1 200 OK
// Content-Length: 14
// Date: Mon, 16 Dec 2013 06:23:34 GMT
// Connection: keep-alive
// Content-Type: text/plain
// Cache-Control: max-age=30, must-revalidate
//http://120.132.134.130:19999/richlifeApp
int FindHostPortPath(const wstring& strURLPortPathIn, wstring& strHost, DWORD& dwPort, wstring& strPath)
{
#define HTTP_BEG _T("://")
	dwPort = 0;
	size_t stPos = strURLPortPathIn.find(HTTP_BEG);
	if ( stPos!= -1)
	{
		stPos+= _tcslen(HTTP_BEG);
		size_t stPosEnd = strURLPortPathIn.find(_T(":"), stPos);
		if ( stPosEnd!=-1 )
		{
			strHost.assign(strURLPortPathIn, stPos, stPosEnd-stPos);			
		}
		else //没有port
		{
			if ( strURLPortPathIn.find(_T("HTTPS://")) != -1 || strURLPortPathIn.find(_T("https://")) != -1 )
			{
				dwPort = INTERNET_DEFAULT_HTTPS_PORT;
			}
			else
				dwPort = INTERNET_DEFAULT_HTTP_PORT;
		}

		size_t stPosEndPath = strURLPortPathIn.find(_T("/"), stPos);

		if ( dwPort == 0 )//有port
		{
			wstring strPort;
			if( stPosEndPath!=-1 )
			{
				strPort.assign(strURLPortPathIn, ++stPosEnd, stPosEndPath-stPosEnd);
			}
			else
			{
				strPort.assign(strURLPortPathIn, ++stPosEnd, strURLPortPathIn.size()-stPosEnd);
			}
			dwPort = _tstoi(strPort.c_str());			
		}

		//http://120.132.134.130:19999/richlifeApp
		if ( stPosEndPath!=-1 )
		{
			strPath.assign(strURLPortPathIn, ++stPosEndPath, strURLPortPathIn.size()-stPosEndPath);
			if ( strHost.empty() )//没有Port 有path
			{
				strHost.assign(strURLPortPathIn, stPos, stPosEndPath-1-stPos);
			}
		}
		else
		{
			if ( strHost.empty() )
			{
				strHost.assign(strURLPortPathIn, stPos, strURLPortPathIn.size()-stPos);//没有Port 没有path时
			}
		}

		return 0;
	}
	return -1;
}
int InternetOpenConnectRequest(const wstring& strURLIn, const wstring& strHeaderIn,
								wstring& strHeader,  string& strBody, SYSTEMTIME& stTime, bool bOnlyHeader)
{
	HINTERNET hOpen = NULL, hConnect=NULL, hRequest = NULL;
	BOOL bRet = TRUE;
	DWORD dwErr = 0;
	try
	{
		hOpen = InternetOpen(NULL, NULL, NULL, NULL, 0);

		wstring strHost, strPath;
		DWORD dwPort = 0;
		FindHostPortPath(strURLIn, strHost, dwPort, strPath);

		hConnect = InternetConnect(hOpen, strHost.c_str(), (INTERNET_PORT)dwPort, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);
		wstring strMethod = _T("GET");
// 		if ( bOnlyHeader == true )
// 		{
// 			strMethod = _T("KILL");
// 		}
		hRequest = HttpOpenRequest(hConnect, strMethod.c_str(), strPath.c_str(), NULL, NULL, NULL,
			INTERNET_FLAG_DONT_CACHE, NULL );
		bRet = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
		if ( bRet )
		{
			//Get Header
			TCHAR szHeader[1024] = {0};
			DWORD dwHeaderSize = _countof(szHeader);
			HttpQueryInfo(hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, szHeader, &dwHeaderSize, NULL);
			OutputDebugString(szHeader);
			OutputDebugString(_T("\n"));
			strHeader = szHeader;

			//获取GMT时间 不一定能得到GMT时间的, 有时会返回Expires: Mon, 16 Dec 2013 06:23:54 GMT说超时了, 不返回时间TND
			SYSTEMTIME stSysTime = {0};
			DWORD dwSysTimeSize = sizeof(stSysTime);
			bRet = HttpQueryInfo(hRequest, HTTP_QUERY_FLAG_SYSTEMTIME|HTTP_QUERY_DATE, &stSysTime, &dwSysTimeSize, NULL);
			dwErr = GetLastError();
			TCHAR szShowGMT[1024] = {0}; 
			swprintf_s(szShowGMT, _countof(szShowGMT), _T("%d-%d-%d %d:%d:%d"), stSysTime.wYear, stSysTime.wMonth
				, stSysTime.wDay, stSysTime.wHour,stSysTime.wMinute, stSysTime.wSecond);	
			OutputDebugString(szShowGMT);
			OutputDebugString(_T("\n"));
			stTime = stSysTime;

			//Get Body
			if(bOnlyHeader == false)
			{
				char szBuf[1024] = {0};
				DWORD dwSzBufSize = _countof(szBuf);
				DWORD dwReaded = 0;
				do 
				{
					bRet = InternetReadFile(hRequest, szBuf, dwSzBufSize, &dwReaded );
					if ( bRet && dwReaded)
					{
						strBody.append(szBuf, 0, dwReaded);
					}

				} while (bRet && dwReaded);
				OutputDebugStringA(strBody.c_str());
				OutputDebugString(_T("\n"));
			}					
		}
		else
		{
			dwErr = GetLastError();
		}

	}
	catch(...)
	{

	}
	if(hOpen)
		InternetCloseHandle(hOpen);
	if(hConnect)
		InternetCloseHandle(hConnect);
	if(hRequest)
		InternetCloseHandle(hRequest);
	return 0;
}

TCHAR* MONTH_REF[] = {_T("Jan") , _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul")
									 , _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")};

//取时间 Expires: Mon, 16 Dec 2013 06:23:54 GMT
int GetSystemTime(const wstring& strHeader, const wstring& strFlag, SYSTEMTIME& stTime)
{
	int iRet = -1;
	do 
	{
		size_t stPos = strHeader.find(strFlag);
		if ( stPos!=-1 )
		{
			stPos = strHeader.find(_T(","));
			if ( stPos!=-1 )
			{
				while ( strHeader[++stPos] == _T(' ') && strHeader[stPos] != 0)
				{
					;
				}
				//day
				size_t stPosDay = strHeader.find(_T(" "), stPos);
				if ( stPosDay!= -1)
				{
					wstring strDay(strHeader, stPos, stPosDay-stPos);
					stTime.wDay = _ttoi(strDay.c_str());
				}
				else
				{
					break;
				}
				//Month
				size_t stPosMonth = strHeader.find(_T(" "), ++stPosDay);
				if ( stPosMonth!= -1)
				{
					wstring strMonth(strHeader, stPosDay, stPosMonth-stPosDay);
					for ( int i = 0; i < _countof(MONTH_REF); ++i )
					{
						if ( _tcscmp(MONTH_REF[i], strMonth.c_str()) == 0 )
						{
							stTime.wMonth = ++i;
						}
					}					
				}
				else
				{
					break;
				}
				//Year
				size_t stPosYear = strHeader.find(_T(" "), ++stPosMonth);
				if ( stPosYear!= -1)
				{
					wstring strValue(strHeader, stPosMonth, stPosYear-stPosMonth);
					stTime.wYear = _ttoi(strValue.c_str());
				}
				else
				{
					break;
				}
				//取时间 Expires: Mon, 16 Dec 2013 06:23:54 GMT
				//Hour
				size_t stPosHour = strHeader.find(_T(":"), ++stPosYear);
				if ( stPosYear!= -1)
				{
					wstring strValue(strHeader, stPosYear, stPosHour-stPosYear);
					stTime.wHour = _ttoi(strValue.c_str());
				}
				else
				{
					break;
				}
				size_t stPosBeg = stPosHour;
				//Min
				size_t stPosEnd = strHeader.find(_T(":"), ++stPosBeg);
				if ( stPosEnd!= -1)
				{
					wstring strValue(strHeader, stPosBeg, stPosEnd-stPosBeg);
					stTime.wMinute = _ttoi(strValue.c_str());
				}
				else
				{
					break;
				}
				//Sec
				stPosBeg = ++stPosEnd;
				stPosEnd = strHeader.find(_T(" "), stPosBeg);
				if ( stPosEnd!= -1)
				{
					wstring strValue(strHeader, stPosBeg, stPosEnd-stPosBeg);
					stTime.wSecond = _ttoi(strValue.c_str());
				}
				else
				{
					break;
				}
				iRet = 0;
			}

		}
	} while (0);

	return iRet;
}

void RaiseToken()
{
	HANDLE hToken;   
	TOKEN_PRIVILEGES tkp;  
	BOOL bRet(FALSE);

	//OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
	//GetCurrentProcess（）函数的作用是得到本进程的句柄  
	bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   	
	bRet = LookupPrivilegeValue(NULL,SE_SYSTEMTIME_NAME,&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  	
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED ; 
	//AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
	bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);  
}


void RaiseTokenTest()
{
	HANDLE   hToken;   
	TOKEN_PRIVILEGES   tkp;  
	BOOL bRet(FALSE);
	static bool bPrivi(false);
	if ( !bPrivi )
	{
		//定义变量   
		bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);  
		bRet = LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid);  	
		tkp.PrivilegeCount =  1; 
		tkp.Privileges[0].Attributes   =   SE_PRIVILEGE_ENABLED;   
		bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);  
		bPrivi = true;
	}
}



int GetWebTimeAndSet(const wstring& strURL)
{
	int iRet = -1;
	do 
	{
		wstring strHeader;
		string strBody; 
		SYSTEMTIME stTime = {0};
		int iCode = InternetOpenConnectRequest(strURL, _T(""), strHeader, strBody, stTime, true);
		if ( stTime.wYear == 0 && stTime.wDay == 0 )//取数据失败 返回Expires: Mon, 16 Dec 2013 06:23:54 GMT
		{
			//取得SYSTEMTIME
			GetSystemTime(strHeader, _T("Expires:"), stTime);
			if ( stTime.wDay != 0 && stTime.wHour != 0 )
			{
				if( SetSystemTime(&stTime) )
				{
					memset((void*)(&stTime), 0, sizeof(stTime));
					iCode = InternetOpenConnectRequest(strURL, _T(""), strHeader, strBody, stTime, true);
					if ( stTime.wYear != 0 )
					{
						iRet = 0;//拿正确的时间回来成功了						
					}
				}
			}
		}
		else
		{
			iRet = 0;//拿正确的时间回来成功
		}
		


			HANDLE hToken;   
			TOKEN_PRIVILEGES tkp;  
			BOOL bRet(FALSE);

			//OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
			//GetCurrentProcess（）函数的作用是得到本进程的句柄  
			bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   	
			bRet = LookupPrivilegeValue(NULL,SE_SYSTEMTIME_NAME,&tkp.Privileges[0].Luid); 

			tkp.PrivilegeCount = 1;  	
			tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED ; 
			//AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
			bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
			GetSystemTime(&stTime);
			bRet = SetSystemTime(&stTime);
			DWORD dwErr = GetLastError();
			bRet = SetLocalTime(&stTime);
			dwErr = GetLastError();

		if( 0== iRet && SetSystemTime(&stTime) )
		{
		 	//转为本地时间
		 	TIME_ZONE_INFORMATION TimeZoneInformation = {0};
		 	GetTimeZoneInformation(&TimeZoneInformation);
		 	SYSTEMTIME LocalTime = {0};
		 	SystemTimeToTzSpecificLocalTime(&TimeZoneInformation, &stTime, &LocalTime);	
		 	{
		 		TCHAR szShowGMT[1024] = {0};
		 		swprintf_s(szShowGMT, _countof(szShowGMT), _T("%d-%d-%d %d:%d:%d\n"), LocalTime.wYear, LocalTime.wMonth
		 			, LocalTime.wDay, LocalTime.wHour,LocalTime.wMinute, LocalTime.wSecond);	
		 		OutputDebugString(szShowGMT);
		 	}			
		}
		else
		{
			DWORD dwErr = GetLastError();
			iRet = -1;
		}


	} while (0);
	return iRet;
}

void Test()
{
	HANDLE hToken;   
	TOKEN_PRIVILEGES tkp;  
	BOOL bRet(FALSE);
	SYSTEMTIME stTime= {0};

	//OpenProcessToken（）这个函数的作用是打开一个进程的访问令牌   
	//GetCurrentProcess（）函数的作用是得到本进程的句柄  
	bRet = OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken);   	
	bRet = LookupPrivilegeValue(NULL,SE_SYSTEMTIME_NAME,&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  	
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED ; 
	//AdjustTokenPrivileges（）的作用是通知Windows   NT修改本进程的权利  
	bRet = AdjustTokenPrivileges(hToken,FALSE,&tkp,0,(PTOKEN_PRIVILEGES)NULL,0);
	GetSystemTime(&stTime);
	bRet = SetSystemTime(&stTime);
	DWORD dwErr = GetLastError();
	bRet = SetLocalTime(&stTime);
	dwErr = GetLastError();


}


int main()
{
	Test();
	return 0;

	RaiseToken();
	int iRetA = -1, iRetB =-1;
	//iRetA = GetWebTimeAndSet( _T("http://www.msftncsi.com/ncsi.txt"));//这个URL拿的结果差4分钟, 也不一定， 在家实测没时间差

	OutputDebugString(_T("Now BaiduTime********:   "));
	iRetB = GetWebTimeAndSet( _T("http://open.baidu.com/special/time/"));//这个URL拿的时间准 了吧!, 
	if ( iRetA == 0 || iRetB == 0 )
	{
		return 0;
	}	
	return  -1;
}