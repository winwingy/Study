#ifndef _STRSTREAM_
#include <sstream>
#endif
#ifndef _IOSTREAM_
#include <iostream> 
#endif
#ifndef _IOSFWD_
#include <iosfwd>
#endif
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

//#define ENABLE_XLOGGER

#define XLOGGER_OPEN            XLOGGER::Open(NULL, NULL);
#define XLOGGER_CLOSE           XLOGGER::Close();

#define XLOGGER_DEBUG(Buffer)   XLOGGER_WRITE(Buffer, XLOGGER::logDebug)
#define XLOGGER_INFO(Buffer)    XLOGGER_WRITE(Buffer, XLOGGER::logInfo)
#define XLOGGER_WARNING(Buffer) XLOGGER_WRITE(Buffer, XLOGGER::logWarning)
#define XLOGGER_ERROR(Buffer)   XLOGGER_WRITE(Buffer, XLOGGER::logError)
#define XLOGGER_FATAL(Buffer)   XLOGGER_WRITE(Buffer, XLOGGER::logFatal)

#ifdef ENABLE_XLOGGER
#define XLOGGER_WRITE(Buffer, Level) { xl_ostringstream logstream; logstream << Buffer; \
	xl_string logstring = logstream.str(); XLOGGER::Write(logstring.c_str(), \
	Level, __FILE__, __LINE__); }
#else
#define XLOGGER_WRITE(Buffer, Level) NULL;
#endif

#ifdef UNICODE
#define xl_ostringstream  std::wostringstream
#define xl_string         std::wstring
#else
#define xl_ostringstream  std::ostringstream
#define xl_string         std::string
#endif

namespace XLOGGER {

	enum XLogLevel
	{
		logDebug   = 0,
		logInfo    = 1,
		logWarning = 2,
		logError   = 3,
		logFatal   = 4
	};

	extern "C" {
		BOOL WINAPI Open(const TCHAR *FileName = NULL, const TCHAR *Section = NULL, INT FileType = 0, const TCHAR *Value = NULL);
		BOOL WINAPI Write(const TCHAR *Value, XLogLevel Level = logInfo, const CHAR* File = NULL, INT Line = -1);
		BOOL WINAPI Write2(const LPVOID Value, UINT Length, XLogLevel Level = logInfo, const CHAR* File = NULL, INT Line = -1);
		void WINAPI SetLevel(XLogLevel Level);
		void WINAPI Close();
	} 

	class __declspec(dllexport) XLogger
	{
	public:
		XLogger(void);
		~XLogger(void);

		BOOL Open(const TCHAR *FileName = NULL, const TCHAR *Section = NULL, INT FileType = 0, const TCHAR *Value = NULL);
		BOOL Write(const TCHAR *Value, XLogLevel Level = logInfo, const CHAR* File = NULL, INT Line = -1);
		BOOL Write2(const LPVOID Value, UINT Length, XLogLevel Level = logInfo, const CHAR* File = NULL, INT Line = -1);
		void SetLevel(XLogLevel Level);
		void Close();

	private:
		LPVOID m_pWriter;
	};
};
