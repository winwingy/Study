#pragma once
#include "Constants.h"

class CMemBuffer
{
public:
	CMemBuffer(void);
	virtual ~CMemBuffer(void);

	BOOL Create(TCHAR *pcName, UINT iSize);
	BOOL Close();

protected:
	void *m_pvMemAddress;

private:
	HANDLE m_hMemory;
};
