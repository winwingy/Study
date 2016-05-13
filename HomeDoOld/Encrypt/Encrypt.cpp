#include <atlstr.h>

char* m_Key = "abc";

CString Encrypt_Key(CString Source,CString Key)
{
	int iSourceLen,iKeyLen;
	int iCount;
	div_t div_Result;
	char strTarget[255];
	char pKey[255],pSource[255],pMid[255];
	CString strTmp;
	int i,j,n;
	BYTE PWD_key[255];

	j=0;
	for (i=32;i<=126;i++)
	{
		PWD_key[i]=j;
		j++;
	}//取ASCII值在32~126之间的可视字符

	iSourceLen=Source.GetLength();
	iKeyLen=Key.GetLength();
	div_Result=div(iSourceLen,iKeyLen);
	if (div_Result.rem)
		iCount=div_Result.quot+1;
	else
		iCount=div_Result.quot;
	wsprintf(pKey,"%s",m_Key);

	n=0;
	for (i=1;i<=iCount;i++)
	{
		strTmp=Source.Mid((i-1)*iKeyLen,iKeyLen);
		if (strTmp.GetLength()<iKeyLen)
		{
			for (j=1;j<=iKeyLen-strTmp.GetLength();j++)
				strTmp+=" ";
		}
		wsprintf(pSource,"%s",strTmp);

		for (j=1;j<=iKeyLen;j++)
		{
			int k1;
			int k2;
			k1=PWD_key[pKey[j-1]];
			k2=PWD_key[pSource[j-1]];
			int k=k1+k2;
			div_Result=div(k,ENCRYPT_KEY);
			k=div_Result.rem;
			pMid[j-1]=k;
			strTarget[n++]=k;
		}//对源文进行替换加密处理

	}
	for (j=0;j<n;j++) 
	{
		strTarget[j]+=32;
		if (strTarget[j]==32) strTarget[j]=''*'';  //用“*”替代密文中的空格
	}
	strTarget[n]=''\0'';

	CString strResult;
	strResult.Format("%s",strTarget);
	return(strResult);
}



int main()
{

	Encrypt_Key

	


	return 0;
}