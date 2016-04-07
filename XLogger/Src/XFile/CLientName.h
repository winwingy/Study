#pragma once
#include <atlsync.h>

struct CLIENT {
	CString ModuleName;
	SYSTEMTIME Begin;
};

class CCLientName
{
public:
	CCLientName(void);
	~CCLientName(void);

	void Add(CString strName);
	void Delete(CString strName);
	void GetName(ATL::CAtlArray<CLIENT> &arrName);

private:

	ATL::CAtlArray<CLIENT> m_arrName;
	CCriticalSection m_oCriticalSection;
};
