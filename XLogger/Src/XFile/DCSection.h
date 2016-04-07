#pragma once

#ifndef __ATLSYNC_H__
#include <atlsync.h>
#endif

class CDataCriticalSection
{
public:
	CDataCriticalSection(CCriticalSection *pSection = NULL);
	~CDataCriticalSection(void);

private:
	CCriticalSection *m_oCriticalSection;
};
