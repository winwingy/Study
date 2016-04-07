#pragma once

#ifdef __ATLCOLL_H__
#include <atlcoll.h>
#endif

#include "DCSection.h"

template< class T >
class CDataArray
{
public:
	CDataArray(void) {}
	~CDataArray(void) {}

	UINT GetCount() const;
	BOOL GetAt(UINT Index, T &data) const;
	void Add(const T &data);
	void Delete(const T &data);

private:
	ATL::CAtlArray<T> m_arrDataArray;
};

template< class T >
inline UINT CDataArray<T>::GetCount() const
{
	CDataCriticalSection Section;

	return (UINT)m_arrDataArray.GetCount();
}

template< class T >
inline BOOL CDataArray<T>::GetAt(UINT Index, T &data) const
{
	CDataCriticalSection Section;

	if (m_arrDataArray.GetCount() <= Index)
		return FALSE;

	data = m_arrDataArray[Index];
	return TRUE;
}

template< class T >
inline void CDataArray<T>::Add(const T &data)
{
	CDataCriticalSection Section;

	m_arrDataArray.Add(data);
}

template< class T >
inline void CDataArray<T>::Delete(const T &data)
{
	CDataCriticalSection Section;

	for (UINT i = 0; i < m_arrDataArray.GetCount(); i++)
	{
		if (data == m_arrDataArray[i])
		{
			m_arrDataArray.RemoveAt(i);
			break;
		}
	}
}
