#include "StdAfx.h"
#include "DCSection.h"

CDataCriticalSection::CDataCriticalSection(CCriticalSection *pSection /*= NULL*/)
{
	m_oCriticalSection = pSection;
	if (NULL == pSection)
	{
		static CCriticalSection l_oCriticalSection;
		m_oCriticalSection = &l_oCriticalSection;
	}

	m_oCriticalSection->Enter(); 
}

CDataCriticalSection::~CDataCriticalSection(void)
{
	m_oCriticalSection->Leave();
}
