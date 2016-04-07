#include <wtypes.h>
#include <tchar.h>
#include <atldef.h>

#include "XLogger.h"
#include "HexFormat.h"
#include "Connection.h"
#include "SimpleLogger.h"
#include "XLogUtil.h"

namespace XLOGGER {

	BOOL RestartXFile(XCONNECTDATA *Logger)
	{
		const TCHAR *XFile = _tcsrchr(Logger->XFile, _T('\\'));
		LONG Processid = GetProcessidFromName(XFile + 1);
		if (-1 == Processid)
		{
			LOGGER_WRITE2(_T("判断日志是否正在运行失败."));
			return FALSE;
		}
		
		if (0 == Processid)
		{
			LOGGER_WRITE2(_T("日志组件意外退出, 正在重新启动..."));

			XCONNECTDATA Temp;
			memcpy(&Temp, Logger, sizeof(Temp));

			Logger->MemWriter.Close();
			Logger->bLogOpened = FALSE;
			BOOL Result = Open2(Temp.cConfigFile, Temp.cSection, Temp.iFileType, Temp.cConfigValue, Logger);
			memset(&Temp, 0, sizeof(Temp)); 
			if (!Result)
			{
				LOGGER_WRITE2(_T("日志组件重新启动失败."));
				return FALSE;
			}

			return TRUE;
		}

		LOGGER_WRITE2(_T("日志存储失败, 但是日志组件正在运行. ?????????"));
		return FALSE;
	}

	void SleepTime(INT iFree)
	{
#if (MAX_ITEM_COUNT < 500)
#error "*** buffer size error!!!! ***"
#endif
		static int iWaitTime[3] = { 1, 15, 80 };
		const static int iLimit[3] = { 400, 200, 100 };
		if (iLimit[2] > iFree)
		{
			Sleep(iWaitTime[2]);
		} 
		else if (iLimit[1] > iFree)
		{
			Sleep(iWaitTime[1]);
		}
		else if (iLimit[0] > iFree)
		{
			Sleep(iWaitTime[0]);
		}
	}

	BOOL XLogWriteTo(XCONNECTDATA *Logger, const BYTE *Buffer, UINT Length)
	{
		static BOOL Error = FALSE;
		if (Error)
			return FALSE;

		INT iFree = 0;
		INT iResult = Logger->MemWriter.Write(Buffer, Length, iFree);
		if (-1 == iResult)
			Error = TRUE;
		
		SleepTime(iFree);
		return TRUE;
	}

	UINT SetSystemTime(CHAR *Buffer, XLogLevel eLevel)
	{
		if (logDebug > eLevel  || logFatal < eLevel)
			eLevel = logInfo;

		const short iIdLen      = sizeof(DWORD);
		const short iTimeLen    = sizeof(SYSTEMTIME);
		const DWORD dwLevel     = (DWORD)eLevel;
		const DWORD dwProcessId = GetCurrentProcessId();
		const DWORD dwThreadId  = GetCurrentThreadId();

		SYSTEMTIME sTime;
		GetLocalTime(&sTime);	

		UINT iIndex = 0;
		memcpy(Buffer + iIndex, &sTime, iTimeLen);
		iIndex += iTimeLen;

		memcpy(Buffer + iIndex, &dwProcessId, iIdLen);
		iIndex += iIdLen;

		memcpy(Buffer + iIndex, &dwThreadId, iIdLen);	
		iIndex += iIdLen;

		memcpy(Buffer + iIndex, &dwLevel, iIdLen);
		iIndex += iIdLen;

		return iIndex;
	}

	BOOL CopyValue(CHAR *Buffer, UINT Size, UINT &Copyed, const TCHAR *Value, UINT CodePage)
	{
#ifdef UNICODE
		INT RetVal = WideCharToMultiByte(CodePage, 0, Value, -1, Buffer, Size, NULL, NULL);
		if (0 == RetVal)
		{
			DWORD LastError = GetLastError();
			if (ERROR_INSUFFICIENT_BUFFER != LastError)
			{
				TCHAR Error[ERROR_BUFFER_SIZE] = { 0 }; 
				GetErrorMessage(LastError, Error); 
				LOGGER_WRITE2(_T("字符编码转换失败(UNICOUE=>CHAR):") << Error);
				return FALSE;
			}
			RetVal = Size + 1;
		}
		Copyed = (RetVal - 1);
#else
		UINT iStrLen = strnlen(Value, Size);
		Copyed = (Size < iStrLen) ? Size : iStrLen;
		memcpy(Buffer, Value, Copyed);
#endif

		return TRUE;
	}

	BOOL CopyFile(CHAR *Buffer, UINT End, UINT MaxSize, UINT &Copyed, const CHAR* File, INT Line)
	{
		Copyed = End;

		if (NULL != File)
		{
			UINT iFileLen = (UINT)x_strlen(File, MAX_PATH);
			if (MAX_PATH < iFileLen)
			{
				LOGGER_WRITE_A("代码文件名错误:" << File << ". " << Buffer);
				return FALSE;
			}

			const int MaxFileSize = MAX_PATH * 3;
			CHAR FileBuf[MaxFileSize] = { 0 };
			iFileLen = x_sprintf(FileBuf, (MaxFileSize - 1), " [%s %d]", File, Line);

			UINT iNext = End;
			if ((iFileLen + End) >= MaxSize)
			{
				UINT iIgnore = 3;
				End = MaxSize - iFileLen - iIgnore;
				memcpy(Buffer + End, "....", iIgnore);
				iNext = End + iIgnore;
			}

			memcpy(Buffer + iNext, FileBuf, iFileLen);
			Copyed = (iNext + iFileLen);  
		}

		return TRUE;
	}

	BOOL XLogWrite(XCONNECTDATA *Logger, 
		const TCHAR *Value, 
		XLogLevel Level, 
		const CHAR* File, 
		INT Line)
	{
		if (Level < Logger->iLevel)
			return TRUE;

		CHAR Buffer[(MAX_ITEM_BUFFER + MAX_PATH) * 2] = { 0 };
		const UINT iHeadSize = SetSystemTime(Buffer, Level);
		const UINT iBuffSize = MAX_ITEM_BUFFER - iHeadSize;

		UINT Copyed = 0;
		if (!CopyValue(Buffer + iHeadSize, iBuffSize, Copyed, Value, Logger->iCodePage))
			return FALSE;

		if (!CopyFile(Buffer + iHeadSize, Copyed, iBuffSize, Copyed, File, Line))
			return FALSE;

		return XLogWriteTo(Logger, (const BYTE *)Buffer, (iHeadSize + Copyed));
	}

	BOOL XLogWrite(XCONNECTDATA *Logger, 
		const LPVOID Value, 
		UINT Length, 
		XLogLevel Level, 
		const CHAR* File, 
		INT Line)
	{
		if (Level < Logger->iLevel)
			return TRUE;

		CHAR Buffer[(MAX_ITEM_BUFFER + MAX_PATH) * 2] = { 0 };
		const UINT iHeadSize = SetSystemTime(Buffer, Level);
		const UINT iBuffSize = MAX_ITEM_BUFFER - iHeadSize;

		if (0 == Logger->iFileType)
		{
			static CHexFormat Format;
			if (!Format.HexFormat((const BYTE*)Value, Length, Buffer + iHeadSize, iBuffSize))
				return FALSE;

			UINT HexSize = (UINT)x_strlen(Buffer + iHeadSize, iBuffSize);
			if (!CopyFile(Buffer + iHeadSize, HexSize, iBuffSize, Length, File, Line))
				return FALSE;
		}
		else
		{
			Length = Length > iBuffSize ? iBuffSize : Length;
			memcpy(Buffer + iHeadSize, Value, Length);
		}

		return XLogWriteTo(Logger, (const BYTE *)Buffer, Length + iHeadSize);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 
	//  C  部分定义
	//
	//////////////////////////////////////////////////////////////////////////////////////////////

	void GetConnectInfo(XCONNECTDATA **Connect)
	{
		static XCONNECTDATA ConnectInfo;
		*Connect = &ConnectInfo;
	}

	BOOL WINAPI Open(const TCHAR *FileName, 
		const TCHAR *Section,
		INT FileType, 
		const TCHAR *Value)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo(&Connect);
		return Open2(FileName, Section, FileType, Value, Connect);
	}

	BOOL WINAPI Write(const TCHAR *Value, 
		XLogLevel Level, 
		const CHAR* File, 
		INT Line)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo(&Connect);
		if (NULL == Value || !Connect->bLogOpened)
			return FALSE;

		return XLogWrite(Connect, Value, Level, File, Line);
	}	

	BOOL WINAPI Write2(const LPVOID Value, 
		UINT Length, 
		XLogLevel Level, 
		const CHAR* File, 
		INT Line)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo(&Connect);
		if (NULL == Value || !Connect->bLogOpened)
			return FALSE;

		return XLogWrite(Connect, Value, Length, Level, File, Line);
	}

	void WINAPI SetLevel(XLogLevel Level)
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo(&Connect);
		Connect->iLevel = Level;
	}

	void WINAPI Close()
	{
		XCONNECTDATA *Connect = NULL;
		GetConnectInfo(&Connect);
		Close2(Connect);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	// 
	//  C++  部分定义
	//
	//////////////////////////////////////////////////////////////////////////////////////////////

	XLogger::XLogger(void)
	{
		m_pWriter = NULL;
	}

	XLogger::~XLogger(void)
	{
		Close();
	}

	BOOL XLogger::Open(const TCHAR *FileName,
		const TCHAR *Section, 
		INT FileType, 
		const TCHAR *Value)
	{
		if (NULL != m_pWriter)
			return FALSE;

		XCONNECTDATA *pLogger = new XCONNECTDATA;

		pLogger->Init();
		if (!Open2(FileName, Section, FileType, Value, pLogger))
		{
			delete pLogger;
			pLogger = NULL;
			return FALSE;
		}

		m_pWriter = pLogger;
		return TRUE;
	}

	BOOL XLogger::Write(const TCHAR *Value,
		XLogLevel Level,
		const CHAR* File, 
		INT Line)
	{
		if (NULL == Value || NULL == m_pWriter)
			return FALSE;

		return XLogWrite((XCONNECTDATA*)m_pWriter, Value, Level, File, Line);
	}

	BOOL XLogger::Write2(const LPVOID Value, 
		UINT Length, 
		XLogLevel Level,
		const CHAR* File, 
		INT Line)
	{
		if (NULL == Value || NULL == m_pWriter)
			return FALSE;

		return XLogWrite((XCONNECTDATA*)m_pWriter, Value, Length, Level, File, Line);
	}

	void XLogger::SetLevel(XLogLevel Level)
	{
		if (NULL != m_pWriter)
			((XCONNECTDATA*)m_pWriter)->iLevel = Level;
	}

	void XLogger::Close()
	{
		if (NULL == m_pWriter)
			return;

		Close2((XCONNECTDATA*)m_pWriter);

		delete m_pWriter;
		m_pWriter = NULL;
	} 
};
