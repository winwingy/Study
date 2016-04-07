#pragma once
#include "membuffer.h"

#if (MAX_ITEM_COUNT > SHRT_MAX)
#error "*** buffer size error!!!! ***"
#endif

class CMemWriter :
	public CMemBuffer
{
public:
	CMemWriter(void);
	virtual ~CMemWriter(void);

	BOOL Create(TCHAR *pcName);
	BOOL Close();

	int Write(const BYTE *pcBuffer, UINT iLength, int &iFree);
	BOOL Read(BYTE pcBuffer[MAX_ITEM_BUFFER], UINT &iLength) const;

	// 共享内存头部预留50个字节数。已经使用12个字节存储数据索引信息。
	const static UINT m_iHeaderSize /*= 50*/; 

protected:
	const static UINT m_iReadWait /*= 25*/;
	const static UINT m_iWriteWait /*= 1000*/;
	
private:

	HANDLE m_hRead;
	HANDLE m_hWrite;
	short *m_iReadIndex;
	short *m_iWriteIndex;
	LONG *m_iBuffFlush;
	BYTE *m_pcBuffer[MAX_ITEM_COUNT];

	const static UINT m_iUintSize /*= sizeof(UINT)*/;

	BOOL WriteInternal(const BYTE *pcBuffer, UINT iLength, int &iFree);
	void CreateEventName(TCHAR *pcBuffer, const TCHAR *pcName, const TCHAR *pcSign) const;
};
