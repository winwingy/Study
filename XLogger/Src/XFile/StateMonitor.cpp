#include "StdAfx.h"
#include "StateMonitor.h"

#include "LogFactory.h"
#include "..\Logger\SimpleLogger.h"
#include "..\Logger\XLogUtil.h"
#include <atlcoll.h>

LONG GetLockCount();

CStateMonitor::CStateMonitor(void)
{
	m_iSecondIndex = 0;
	m_iMinuteIndex = 0;
	m_iHourIndex = 0;

	m_sCreateTime = CTime::GetCurrentTime();

	for (int i = 0; i < MAX_SERVER_COUNT; i++)
		m_bSaveError[i] = m_bUsable[i] = FALSE;

	int iValSize = sizeof(double);
	memset(m_iBaseCount,   0, sizeof(LONG64) * MAX_SERVER_COUNT);
	memset(m_iSecondCount, 0, iValSize * MAX_SERVER_COUNT * SECOND_COUNT);
	memset(m_iMinuteCount, 0, iValSize * MAX_SERVER_COUNT * MINUTE_COUNT);
	memset(m_iHourCount,   0, iValSize * MAX_SERVER_COUNT * HOUR_COUNT);

	wmemset(m_pcFileName, 0, MAX_PATH);
	wmemset(m_pcBkFileName, 0, MAX_PATH);

	WCHAR FileName[MAX_PATH] = { 0 };
	DWORD iLength = GetModuleFileNameW(GetModuleHandle(NULL), FileName, MAX_PATH);
	if ((MAX_PATH - iLength) > 10)
	{
		WCHAR *Temp = wcsrchr(FileName, L'.');
		wmemcpy(Temp, L"Sta.Txt.", 7);
		wmemcpy(m_pcFileName, FileName, MAX_PATH);
		wmemcpy(Temp, L"Sta2.Txt.", 8);
		wmemcpy(m_pcBkFileName, FileName, MAX_PATH);
	}

	UINT CodePage = GetCodePage();
	CString strValue = GetProcessUserName(GetCurrentProcess());

	memset(m_pcProcessUserName, 0, MAX_PATH);
	WideCharToMultiByte(CodePage, 0, strValue, -1, m_pcProcessUserName, MAX_PATH - 1, NULL, NULL);

	if (0 == m_pcProcessUserName[0])
	{
		m_pcProcessUserName[0] = '!';
		m_pcProcessUserName[1] = '!';
		m_pcProcessUserName[2] = '!';
	}
}

CStateMonitor::~CStateMonitor(void)
{
	m_bThreadRuning = FALSE;
	if (WAIT_TIMEOUT == WaitForSingleObject(
		HANDLE(m_hThreadHandle), DWORD(1000 * 60)))
	{
		DWORD ExitCode = 0;
		TerminateThread(HANDLE(m_hThreadHandle), ExitCode);
	}

	CloseHandle((HANDLE)m_hThreadHandle);
	m_hThreadHandle = NULL;
}

CString CStateMonitor::GetProcessUserName(HANDLE hProcess)
{
	HANDLE hToken = NULL;
	BOOL bFuncReturn = FALSE;
	CString strUserName = _T("");
	PTOKEN_USER pToken_User = NULL;
	DWORD dwTokenUser = 0;
	TCHAR szAccName[MAX_PATH] = {0};
	TCHAR szDomainName[MAX_PATH] = {0};

	if (NULL == hProcess)
		return strUserName;

	bFuncReturn = ::OpenProcessToken(hProcess,TOKEN_QUERY, &hToken);
	if (0 == bFuncReturn)
		return strUserName;

	if (NULL != hToken)
	{
		GetTokenInformation(hToken, TokenUser, NULL, 0L, &dwTokenUser);
		if (0 < dwTokenUser)
		{
			pToken_User = (PTOKEN_USER)::GlobalAlloc(GPTR, dwTokenUser);
		}

		if (NULL != pToken_User)
		{
			bFuncReturn = GetTokenInformation(hToken, TokenUser, pToken_User, dwTokenUser, &dwTokenUser);
		}

		if (bFuncReturn && NULL != pToken_User)
		{
			SID_NAME_USE eUse  = SidTypeUnknown;
			DWORD dwAccName    = 0L; 
			DWORD dwDomainName = 0L;
			PSID  pSid = pToken_User->User.Sid;
			bFuncReturn = LookupAccountSid(NULL, pSid, NULL, &dwAccName,NULL,&dwDomainName,&eUse );
			if (0 < dwAccName && MAX_PATH > dwAccName && 0 < dwDomainName && MAX_PATH >= dwDomainName)
			{
				bFuncReturn = LookupAccountSid(NULL, pSid, szAccName, &dwAccName, szDomainName, &dwDomainName, &eUse);
			}

			if (0 != bFuncReturn)
				strUserName = szAccName;
		}
	}

	if (NULL != pToken_User)
	{
		::GlobalFree(pToken_User);
		pToken_User = NULL;
	}

	if (NULL != hToken)
	{
		::CloseHandle(hToken);
		hToken = NULL;
	}

	return strUserName;
}

void CStateMonitor::InitMonitor()
{
	m_bThreadRuning = TRUE;

	unsigned int uiThreadID = 0;
	m_hThreadHandle = _beginthreadex(NULL, 0, 
		MonitorThread, this, 0, &uiThreadID);
}

UINT WINAPI CStateMonitor::MonitorThread(void* lpParam)
{
	CStateMonitor *pMonitor = (CStateMonitor*)lpParam;

	for (int i = 0; i < MAX_SERVER_COUNT; i++)
	{
		const LOGSERVER &server = CLogFactory::m_sLogServer[i];
		pMonitor->m_iBaseCount[i] = server.oLogServer.GetItemCount();
	}

	while (pMonitor->m_bThreadRuning)
	{
		Sleep(1000);
		pMonitor->SetSecond();		
	}

	_endthreadex(0);

	return 1;
}

void CStateMonitor::SetSecond()
{
	for (int i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &server = CLogFactory::m_sLogServer[i];
		m_bUsable[i] = FALSE;
		if (0 == server.iClient)
			continue;

		if (!server.oLogServer.ThreadRunning()
			&& !m_bSaveError[i] 
			&& 0 != server.iClient)
		{
			CString strFileName = server.oLogServer.GetFileName();
			LOGGER_WRITE2(_T("!!! 存储线程(") 
				<< server.oLogServer.GetThreadId() << _T(")遇到错误终止运行, 文件无法继续存储:") 
				<< strFileName.GetBuffer());
			m_bSaveError[i] = TRUE;
		}

		LONG64 iCount = server.oLogServer.GetItemCount();
		m_iSecondCount[i][m_iSecondIndex] = double(iCount - m_iBaseCount[i]);
		m_iBaseCount[i] = iCount;
		m_bSaveError[i] = FALSE;
		m_bUsable[i] = TRUE;
	}

	if (0 == m_iSecondIndex)
		GetLocalTime(&m_sBeginTime);

	if (SECOND_COUNT == ++m_iSecondIndex)
	{
		SetMinute();
		SaveToFile();
		m_iSecondIndex = 0;
	}
}

void CStateMonitor::SetMinute()
{
	if (MINUTE_COUNT == m_iMinuteIndex)
	{
		for (int i = 0; i < MAX_SERVER_COUNT; i++)
		{
			for (int j = 0; j < MINUTE_COUNT - 1; j++)
				m_iMinuteCount[i][j] = m_iMinuteCount[i][j + 1];
		}
		m_iMinuteIndex--;
	}

	for (int i = 0; i < MAX_SERVER_COUNT; i++)
	{
		if (!m_bUsable[i])
			continue;
		
		int iIndex = 0;
		while (iIndex < SECOND_COUNT)
		{
			m_iMinuteCount[i][m_iMinuteIndex] = 0;
			for (int j = 0; j < 60; j++)
				m_iMinuteCount[i][m_iMinuteIndex] += m_iSecondCount[i][iIndex++];
		}		
	}	

	m_iMinuteIndex++;

	static int MinuteCount = 0;
	if (60 == ++MinuteCount)
	{
		SetHour();
		MinuteCount = 0;
	}
	else
	{
		SetHour(FALSE);
	}
}

void CStateMonitor::SetHour(BOOL bNext /*= TRUE*/)
{
	if (MINUTE_COUNT == m_iHourIndex)
	{
		for (int i = 0; i < MAX_SERVER_COUNT; i++)
		{
			for (int j = 0; j < MINUTE_COUNT - 1; j++)
				m_iHourCount[i][j] = m_iHourCount[i][j + 1];
		}
		m_iHourIndex--;
	}

	for (int i = 0; i < MAX_SERVER_COUNT; i++)
	{
		if (!m_bUsable[i])
			continue;
		
		int iIndex = 0;
		while (iIndex < MINUTE_COUNT)
		{
			m_iHourCount[i][m_iHourIndex] = 0;
			for (int j = 0; j < 60; j++)
				m_iHourCount[i][m_iHourIndex] += m_iMinuteCount[i][iIndex++];
		}
	}

	if (bNext)
		m_iHourIndex++;
}

void CStateMonitor::SaveToFile()
{
	if (0 == m_pcFileName[0] || 0 == m_pcBkFileName[0])
		return;

	FILE* hFileHandle = _wfsopen(m_pcFileName, L"ab", _SH_DENYNO);
	if (NULL == hFileHandle)
		return;

	fseek(hFileHandle, 0, SEEK_END);
	if ((1024 * 1024 * 3) < ftell(hFileHandle))
	{
		fclose(hFileHandle);
		DeleteFile(m_pcBkFileName);
		_wrename(m_pcFileName, m_pcBkFileName);
		hFileHandle = _wfsopen(m_pcFileName, L"ab", _SH_DENYNO);
		if (NULL == hFileHandle)
			return;
	}

	SaveToFile(hFileHandle);
	fclose(hFileHandle);
	hFileHandle = NULL;
}

void CStateMonitor::SaveToFile(FILE* hFileHandle)
{
	int iActive = 0;
	const int MaxCount = 20;
	static UINT CodePage = GetCodePage();
	CTime Current = CTime::GetCurrentTime();

	for (int i = 0; i < MAX_SERVER_COUNT; i++)
	{
		LOGSERVER &server = CLogFactory::m_sLogServer[i];
		if (!m_bUsable[i] || 0 == server.iClient)
			continue;

		char Buffer[MAX_PATH] = { 0 };
		CString strFileName = server.oLogServer.GetFileName();
		WideCharToMultiByte(CodePage, 0, strFileName, -1, Buffer, MAX_PATH - 1, NULL, NULL);

		ATL::CAtlArray<CLIENT> arrName;
		server.oClientName.GetName(arrName);

		if (server.oLogServer.ThreadRunning())
		{
			fprintf(hFileHandle, "存储文件: %s \r\n", Buffer);
		}
		else
		{
			fprintf(hFileHandle, "存储文件: %s. 存储线程停止运行!!!!!!!!!!\r\n", Buffer);
		}

		fprintf(hFileHandle, "线程ID: %d \r\n", server.oLogServer.GetThreadId());
		fprintf(hFileHandle, "存储进程: %d \r\n", arrName.GetCount());

		for (UINT j = 0; j < arrName.GetCount(); j++)
		{
			CLIENT &Client = arrName[j];
			if (Client.ModuleName.GetLength() != 0)
			{
				char Buffer[MAX_PATH] = { 0 };
				WideCharToMultiByte(CodePage, 0, Client.ModuleName, -1, Buffer, MAX_PATH - 1, NULL, NULL);

				CTimeSpan Span = Current - CTime(Client.Begin);
				fprintf(hFileHandle, 
					"  %s (%04d/%02d/%02d %02d:%02d:%02d #%02lld-%02ld-%02ld-%02ld)\r\n", 
					Buffer,
					Client.Begin.wYear, Client.Begin.wMonth, Client.Begin.wDay, 
					Client.Begin.wHour, Client.Begin.wMinute, Client.Begin.wSecond, 
					Span.GetDays(), Span.GetHours(), Span.GetMinutes(), Span.GetSeconds());
			}
		}

		fprintf(hFileHandle, "最近[60]秒开始时间: %04d/%02d/%02d %02d:%02d:%02d\r\n", 
			m_sBeginTime.wYear, m_sBeginTime.wMonth, m_sBeginTime.wDay, 
			m_sBeginTime.wHour, m_sBeginTime.wMinute, m_sBeginTime.wSecond, m_sBeginTime.wMilliseconds);

		fprintf(hFileHandle, 
			"------------------------------------------------------------------------------------\r\n最近[%d]秒内, 每秒存储数:\r\n",
			SECOND_COUNT);

		int Count = 0;
		for (int j = 0; j < SECOND_COUNT; j++)
		{
			fprintf(hFileHandle, " %6.0lf", m_iSecondCount[i][j]);
			if (MaxCount == ++Count)
			{
				Count = 0;
				fprintf(hFileHandle, "\r\n");
			}
		}

		Count = 0;
		fprintf(hFileHandle, "最近[%d]分钟内, 每分钟存储数:\r\n", MINUTE_COUNT);
		for (int j = 0; j < m_iMinuteIndex; j++)
		{
			fprintf(hFileHandle, " %6.0lf", m_iMinuteCount[i][j]);
			if (MaxCount == ++Count)
			{
				Count = 0;
				fprintf(hFileHandle, "\r\n");
			}
		}

		if (m_iMinuteIndex % MaxCount != 0)
			fprintf(hFileHandle, "\r\n");

		Count = 0;
		fprintf(hFileHandle, "最近[%d]小时内, 每小时存储数:\r\n", HOUR_COUNT);
		fprintf(hFileHandle, " %6.0lf", m_iHourCount[i][0]);
		for (int j = 1; j < m_iHourIndex; j++)
		{
			fprintf(hFileHandle, " %6.0lf", m_iHourCount[i][j]);
			if (MaxCount == ++Count)
			{
				Count = 0;
				fprintf(hFileHandle, "\r\n");
			}
		} 

		if (0 == m_iHourIndex || (m_iHourIndex % MaxCount) != 0)
			fprintf(hFileHandle, "\r\n");
		fprintf(hFileHandle, 
			"------------------------------------------------------------------------------------\r\n");

		iActive++;
	}

	if (0 < iActive)
	{	
		fprintf(hFileHandle, 
			"活动文件: %d\r\n", 
			iActive);

		static long iSaveCount = 0;
		static DWORD iProcessId = GetCurrentProcessId();
		CTimeSpan Span = Current - m_sCreateTime;

		fprintf(hFileHandle, "存储服务: %04d/%02d/%02d %02d:%02d:%02d #%02lld-%02ld-%02ld-%02ld. 用户: %s. 进程: %ld. 锁定: %ld\r\n", 
			m_sCreateTime.GetYear(), m_sCreateTime.GetMonth(), m_sCreateTime.GetDay(), 
			m_sCreateTime.GetHour(), m_sCreateTime.GetMinute(), m_sCreateTime.GetSecond(), 
			Span.GetDays(), Span.GetHours(), Span.GetMinutes(), Span.GetSeconds(),
			m_pcProcessUserName,
			iProcessId,
			GetLockCount());

		fprintf(hFileHandle, "记录时间: %04d/%02d/%02d %02d:%02d:%02d. 次数: %ld\r\n", 
			Current.GetYear(), Current.GetMonth(), Current.GetDay(), 
			Current.GetHour(), Current.GetMinute(), Current.GetSecond(),
			++iSaveCount);

		fprintf(hFileHandle, 
			"====================================================================================\r\n\r\n\r\n");
	}
}
