
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

UINT GetCodePage(UINT LangID = 0);

#ifdef UNICODE
#else
#endif

#if (_MSC_VER == 1200)
#define x_memcpy()
#define x_strlen(string, maxsize) strlen(string)
#define x_sprintf(buffer, size, format, param, param2) sprintf(buffer, format, param, param2)
#else
#define x_strlen(string, maxsize) strnlen_s(string, maxsize)
#define x_sprintf(buffer, size, format, param, param2) sprintf_s(buffer, size, format, param, param2)
#endif

#define ERROR_BUFFER_SIZE 1024
void GetErrorMessage(DWORD Error, TCHAR Buffer[ERROR_BUFFER_SIZE]);

UINT GetStrSize(const TCHAR *Value, int MaxSize = -1);
void MemoryCopy(TCHAR *Buffer, int BSize, const TCHAR *Value, int VSize);
void StringCopy(TCHAR *Buffer, UINT Length, const TCHAR *Value);
BOOL CreateTempConfig(const TCHAR *FileName, const TCHAR *Section, const TCHAR *Config, TCHAR TempFile[MAX_PATH]);
BOOL GetAppVersion(VS_FIXEDFILEINFO &FileInfo);
LONG GetProcessidFromName(LPCTSTR name);
