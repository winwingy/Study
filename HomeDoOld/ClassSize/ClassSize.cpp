// ClassSize.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
class AB
{
	int m;
	float b;
	static float c;
	static const int t;
	enum opo
	{
		pss = 1,
		puu = 2,
	};
};

class KKK
{
	int m;
	enum opo	//枚举作为一个定义类型, 没有变量, 也不占大小
	{
		pss = 1,
		puu = 2,
	};
};

class KEE
{
	enum opo	//枚举作为一个定义类型, 没有变量, 也不占大小
	{
		pss = 1,
		puu = 2,
	};
	opo eOpo;	//枚举变量会给4个字节	
};

class KEEBIG
{
	enum opo	//枚举作为一个定义类型, 没有变量, 也不占大小
	{
		pss = 1,
		puu = 2,
		paa = 3,
		pbb = 4,
		pcc = 5,
		pdd = 6000,
		pee = 90000
	};
	opo eOpo;	//枚举变量会给4个字节	再大的也是4个字节, 汗!!!
};



class ZZZ
{
	int m;
	static float c;	//类的静态成员不占类的大小
	static const int t;

};




int _tmain(int argc, _TCHAR* argv[])
{
	int iSize = sizeof(AB);
	printf("%d\n", iSize);
	iSize = sizeof(KKK);
	printf("%d\n", iSize);
	iSize = sizeof(ZZZ);
	printf("%d\n", iSize);
	
	iSize = sizeof(KEE);
	printf("%d\n", iSize);

	iSize = sizeof(KEEBIG);
	printf("%d\n", iSize);

	system("pause");
	return 0;
}

