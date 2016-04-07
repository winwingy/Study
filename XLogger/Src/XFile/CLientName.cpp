#include "StdAfx.h"
#include "CLientName.h"
#include "DCSection.h"

CCLientName::CCLientName(void)
{
}

CCLientName::~CCLientName(void)
{
}

void CCLientName::Add(CString strName)
{
	CDataCriticalSection Section(&m_oCriticalSection);

	CLIENT Client;
	Client.ModuleName = strName;
	GetLocalTime(&Client.Begin);
	m_arrName.Add(Client);
}

void CCLientName::Delete(CString strName)
{
	CDataCriticalSection Section(&m_oCriticalSection);

	for (UINT i = 0; i < m_arrName.GetCount(); i++)
	{
		if (m_arrName[i].ModuleName == strName)
		{
			m_arrName.RemoveAt(i);
			break;
		}
	}
}

void CCLientName::GetName(ATL::CAtlArray<CLIENT> &arrName)
{
	CDataCriticalSection Section(&m_oCriticalSection);

	arrName.RemoveAll();
	arrName.Append(m_arrName);
}
