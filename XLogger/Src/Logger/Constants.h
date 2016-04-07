#ifndef __wtypes_h__
#include <wtypes.h>
#endif

enum LogLevel
{
	logDebug = 0,
	logInfo = 1,
	logWarning = 2,
	logError = 3,
	logFatal = 4
};

#pragma pack(push,1)
struct FILEHEADER {
	char cSign[20];
	DWORD dwVerMS;
	DWORD dwVerLS;
	DWORD dwType;
	UINT iCodePage;
	BOOL bEncrypt;
	char cRemark[20];
};
#pragma pack(pop)

#define MAX_ITEM_COUNT          1500
#define MAX_ITEM_BUFFER         (1024 * 5)  // ∑÷≈‰ƒ⁄¥Ê‘º7’◊◊Û”“£®7.3£©°£
#define FILESIGN                "XLogger File"
