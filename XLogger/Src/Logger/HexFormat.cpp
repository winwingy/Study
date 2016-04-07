#include <stdio.h>

#include "HexFormat.h"

CHexFormat::CHexFormat(void)
{
}

CHexFormat::~CHexFormat(void)
{
}

__inline
void CHexFormat::HextoAsc(const BYTE Hex, CHAR Asc[2]) const
{	
	BYTE Value = (Hex >> 4) & 0x0f;
	if (Value < 0x0a) Asc[0] = Value + 0x30;
	else Asc[0] = Value + 0x37;		
	Value = Hex & 0x0f;
	if (Value < 0x0a) Asc[1] = Value + 0x30;
	else Asc[1] = Value + 0x37;
}

bool CHexFormat::HexFormat(const BYTE *Value, 
						   UINT VSize, 
						   CHAR *Buffer, 
						   UINT BSize)
{
/*
0x0000  69 66 20 28 21 41 66 78  57 69 6E 49 6E 69 74 28  if (!AfxWinInit(
0x0010  3A 3A 47 65 74 4D 6F 64  75 6C 65 48 61 6E 64 6C  ::GetModuleHandl
*/
	const UINT HexNumber  = 8;
	const UINT LineLength = (12 +  HexNumber * 8);
	const UINT CharIndex  = (LineLength - HexNumber * 2);
	const UINT TempLength = (LineLength * 2);

	UINT ScrIndex = 0;
	UINT DstIndex = 0;
	BOOL Format   = TRUE;

	while (Format && ScrIndex < VSize)
	{
		UINT Index = 2;
		CHAR Temp[TempLength] = { 0 };
		memset(Temp, ' ', TempLength);
		memcpy(Temp, "\r\n", Index);

#pragma warning(push)
#pragma warning(disable: 4996)
		Index += sprintf((char*)Temp + Index, "0x%04X  ", ScrIndex);
#pragma warning(pop)

		UINT Begin = ScrIndex;
		UINT Count = ScrIndex + HexNumber;
		if (Count > VSize) 
			Count = VSize;

		for (ScrIndex; ScrIndex < Count; ScrIndex++)
		{
			HextoAsc(Value[ScrIndex], Temp + Index);
			Index += 3;
		}

		Count = ScrIndex + HexNumber;
		if (Count > VSize) 
			Count = VSize;

		Index += 1;
		for (ScrIndex; ScrIndex < Count; ScrIndex++)
		{
			HextoAsc(Value[ScrIndex], Temp + Index);
			Index += 3;
		}

		Count = Begin + HexNumber * 2;
		if (Count > VSize) 
			Count = VSize;

		Index = CharIndex;
		for (Begin; Begin < Count; Begin++)
		{
			BYTE Data = Value[Begin];
			if (Data >= 32 && Data <= 126)
				Temp[Index++] = Data;
			else
				Temp[Index++] = '.';
		}

		Begin = DstIndex * LineLength; 
		UINT End = ++DstIndex * LineLength; 
		if (End > BSize)
		{
			End = BSize;
			Format = FALSE;
		}

		memcpy(Buffer + Begin, Temp, End - Begin);
	}

	return true;
}
