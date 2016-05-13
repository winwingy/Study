// RandomNum6Bits.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include <ctime>
using namespace std;

//产生6位数字的随机数
void general6BitRan()
{
	srand( time( 0 ) );
	int iShow(0);

	for (int iCnt(0); iCnt < 1000; ++iCnt)
	{
		for (int i(0); i < 6; )
		{
			int iRd = rand();
			printf("%d\t", iRd);
			if ( iRd < 10 && iRd > -10 )
			{
				continue;
			}
			iShow += iRd % 10;
			iShow *= 10;
			++i;
		}		
		printf("%d: %06d\t", iCnt, iShow /= 10);
		iShow = 0;
	}
}

//产生随机6位数字和字母 0 纯数字, 1 数字和字母(大写) 
void generalRandom(char szRet[], int iBits, int iType = 0)
{
	srand( time( 0 ) );
	szRet[iBits -1] = 0;

	//iType = 0
	if ( 0 == iType )
	{
		for (int i(0); i < iBits; ++i)
		{
			int iRd = rand();
			int iTemp = iRd % 10;
			szRet[i] = '0' + iTemp;		
		}
		return;
	}

	//iType == 1
	//0-9 A-Z  10 + 26 = 36	
	for (int i(0); i < iBits; ++i)
	{
		int iRd = rand();
		int iTemp = iRd % 36;
		if ( iTemp <= 9)//数字
		{
			szRet[i] = '0' + iTemp;
		}
		else//字母
		{
			szRet[i] = 'A' + iTemp -10;
		}		
	}		
}


int main()
{
	char szRanNum[7] = {0};
	generalRandom(szRanNum, 7-1, 0);
	printf("%s\n", szRanNum);

	generalRandom(szRanNum, 7-1, 1);
	printf("%s\n", szRanNum);

	
	system("pause");
	return 0; 
} 