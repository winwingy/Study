#pragma once
#ifndef __wtypes_h__
#include <wtypes.h>
#endif

class CHexFormat
{
public:
	CHexFormat(void);
	~CHexFormat(void);

	bool HexFormat(const BYTE *Value, UINT VSize, CHAR *Buffer, UINT BSize);

private:

	void HextoAsc(const BYTE Hex, CHAR Asc[2]) const;
};
