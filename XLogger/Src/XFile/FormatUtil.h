#pragma once

#define MAX_FORMAT_ITEM            100
#define FORMAT_BUFFER_LENGTH       270 

class CFormatUtil
{
public:
	CFormatUtil(char *pType = NULL);
	~CFormatUtil(void);

	BOOL SetFormat(char *pcFormat);
	BOOL SetFormat(char *pType, char *pcFormat);
	UINT FormatString(char Buffer[MAX_PATH], const SYSTEMTIME &Time) const;
	UINT FormatString(char Buffer[FORMAT_BUFFER_LENGTH], const SYSTEMTIME &Time,
		DWORD dwProcess, DWORD dwThread) const;

private:

	char *m_pcFormat;
	size_t m_iLength;

	struct FORAMTITEM {
		WORD iBegin;	
		WORD iFormat;
		WORD iLength;
		WORD iEnd;
		WORD iType;
	};

	char m_cType[MAX_FORMAT_ITEM];
	FORAMTITEM m_sFormat[MAX_FORMAT_ITEM];
	size_t m_iItemCount;

	static const WORD m_iTime        = 21;
	static const WORD m_iMillisecond = 22;
	static const WORD m_iProcess     = 23;
	static const WORD m_iThread      = 24;
	static const WORD m_iData        = 25;

	void Initialization(char *pType);
	BOOL FormatPaser();
	BOOL AddItem(FORAMTITEM &sItem);
	BOOL GetType(char cData, WORD &iType) const;
	void Output(FORAMTITEM &sItem);
};
