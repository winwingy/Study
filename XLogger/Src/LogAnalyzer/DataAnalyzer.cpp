#include "StdAfx.h"
#include "DataAnalyzer.h"

#include "Data.h"

CDataAnalyzer::CDataAnalyzer(CData *pData)
{
	ASSERT(NULL != pData);
	m_pData = pData;
}

CDataAnalyzer::~CDataAnalyzer(void)
{
}

void CDataAnalyzer::RelationAnalyzer(CStdioFile &Write)
{
	CData *pData = (CData*)m_pData;
	if (NULL == pData)
		return;
	
	CString strResult;
	UINT Count   = pData->m_arrLogData2.GetCount();

	for (UINT i = 0; i < Count; i++)
	{
		LOGITEM2 &Item = pData->m_arrLogData2[i];
		Item.Sign = FALSE;
	}

	int Space = 2;
	for (UINT i = 0; i < Count; i++)
	{
		LOGITEM2 &Item = pData->m_arrLogData2[i];
		if (Item.Ender && !Item.Sign && -1 != Item.Index)
		{
			SaveRelation(Write, i, Space);
			Write.WriteString(_T("\r\n"));
		}
	}
}

int CDataAnalyzer::SaveRelation(CStdioFile &Write, int Index, int Space)
{
	CData *pData    = (CData*)m_pData;
	int Count       = pData->m_arrLogData2.GetCount();
	LOGITEM2 &Item2 = pData->m_arrLogData2[Index];
	LOGITEM  &Item  = pData->m_arrLogData[Item2.Index];
	Item2.Sign      = TRUE;

	CString strValue;
	if (Item2.Ender)
		SaveRelation(Write, Item, Space);

	for (int i = Index + 1; i < Count; i++)
	{
		LOGITEM2 &Temp = m_pData->m_arrLogData2[i];
		if (Temp.Sign || -1 == Temp.Index)
			continue;

		if (Temp.Index == Item2.Index
			&& !Temp.Ender)
			return i;

		LOGITEM  &Temp2  = m_pData->m_arrLogData[Temp.Index];
		if (Temp2.ProcessId == Item.ProcessId
			&& Temp2.ThreadId == Item.ThreadId)
			i = SaveRelation(Write, i, Space + 4);
	}

	return Count;
}

void CDataAnalyzer::SaveRelation(CStdioFile &Write, 
	struct LOGITEM &Item, int Space)
{
	CString strValue;
	CData *pData = (CData*)m_pData;

	if (Item.Return)
	{
		LONG64 Using = GetTime(Item.Enter, Item.Leave);
		strValue.Format(_T("%d/%02d/%02d %02d:%02d:%02d %04ld %04ld [%4lld] "), 
			Item.Enter.wYear, Item.Enter.wMonth, Item.Enter.wDay,
			Item.Enter.wHour, Item.Enter.wMinute, Item.Enter.wSecond,
			Item.Enter.wMilliseconds,
			Item.ThreadId,
			Using);
	}
	else
	{
		strValue.Format(_T("%d/%02d/%02d %02d:%02d:%02d %04ld %04ld [?Err] "), 
			Item.Enter.wYear, Item.Enter.wMonth, Item.Enter.wDay,
			Item.Enter.wHour, Item.Enter.wMinute, Item.Enter.wSecond,
			Item.Enter.wMilliseconds,
			Item.ThreadId);
	}

	Write.WriteString(strValue);
	Write.WriteString(CString(_T(' '), Space));
	Write.WriteString(pData->m_arrFunction[Item.Function].Function);
	Write.WriteString(_T("\n"));
}

LONG64 CDataAnalyzer::GetTime(SYSTEMTIME Begin, SYSTEMTIME End)
{
	LONG64 Value0 = 0;
	LONG64 Value1 = 0;

	if (Begin.wDay != End.wDay)
	{
		Value0 = Begin.wDay * 24 * 60 * 60 * 1000;
		Value1 = End.wDay * 24 * 60 * 60 * 1000;
	}

	if (Begin.wHour != End.wHour)
	{
		Value0 += Begin.wHour * 60 * 60 * 1000;
		Value1 += End.wHour * 60 * 60 * 1000;
	}

	if (Begin.wMinute != End.wMinute)
	{
		Value0 += Begin.wMinute * 60 * 1000;
		Value1 += End.wMinute * 60 * 1000;
	}

	if (Begin.wSecond != End.wSecond)
	{
		Value0 += Begin.wSecond * 1000;
		Value1 += End.wSecond * 1000;
	}

	Value0 += Begin.wMilliseconds;
	Value1 += End.wMilliseconds;

	return (Value1 - Value0);
}

void CDataAnalyzer::NotreturnAnalyzer(CString &strResult)
{
	CData *pData = (CData*)m_pData;
	if (NULL == pData)
		return;

	CAtlArray<LOGITEM> &arrLogData = pData->m_arrLogData;
	UINT Count = arrLogData.GetCount();

	strResult = _T("");
	for (UINT i = 0; i < Count; i++)
	{
		LOGITEM &Item = arrLogData[i];
		if (Item.Return)
			continue;

		ASSERT(-1 != Item.Function);
		CString strTemp;
		strTemp.Format(_T("%d/%02d/%02d %02d:%02d:%02d %03d %ld %ld %s\r\n"), 
			Item.Enter.wYear, Item.Enter.wMonth, Item.Enter.wDay,
			Item.Enter.wMonth, Item.Enter.wMinute, Item.Enter.wSecond,
			Item.Enter.wMilliseconds,
			Item.ProcessId, Item.ThreadId,
			pData->m_arrFunction[Item.Function].Function);
		strResult += strTemp;
	}
}

void CDataAnalyzer::TimeAnalyzer(int iMinTime, CString &strResult)
{
	CData *pData = (CData*)m_pData;
	if (NULL == pData)
		return;

	CAtlArray<LOGITEM> &arrLogData = pData->m_arrLogData;
	UINT Count = arrLogData.GetCount();

	strResult = _T("");
	for (UINT i = 0; i < Count; i++)
	{
		LOGITEM &Item = arrLogData[i];
		if (!Item.Return)
			continue;

		LONG64 Using = GetTime(Item.Enter, Item.Leave);
		if (Using >= iMinTime)
		{
			CString strTemp;
			strTemp.Format(_T("%d/%02d/%02d %02d:%02d:%02d %03d %ld %ld %s "), 
				Item.Enter.wYear, Item.Enter.wMonth, Item.Enter.wDay,
				Item.Enter.wHour, Item.Enter.wMinute, Item.Enter.wSecond,
				Item.Enter.wMilliseconds,
				Item.ProcessId, Item.ThreadId,
				pData->m_arrFunction[Item.Function].Function);

			int Space = 80 - strTemp.GetLength();
			if (0 < Space)
				strTemp += CString(_T(' '), Space);

			strTemp.AppendFormat(_T(" 耗时: %3lld  CPU: %%%.1lf\r\n"), Using, Item.CPU);
			strResult += strTemp;
		}
	}
}

void CDataAnalyzer::CPUUsageAnalyzer(CString &strResult)
{
	strResult = _T("");
	CData *pData = (CData*)m_pData;
	if (NULL == pData)
		return;

	CAtlArray<LOGITEM> &arrLogData = pData->m_arrLogData;
	UINT Count = arrLogData.GetCount();

	for (UINT i = 0; i < Count; i++)
	{
		LOGITEM &Item = arrLogData[i];
		if (Item.CPU <= 0.0001)
			continue;

		CString strTemp;
		strTemp.Format(_T("%d/%02d/%02d %02d:%02d:%02d %03d %ld %ld %s "), 
			Item.Enter.wYear, Item.Enter.wMonth, Item.Enter.wDay,
			Item.Enter.wHour, Item.Enter.wMinute, Item.Enter.wSecond,
			Item.Enter.wMilliseconds,
			Item.ProcessId, Item.ThreadId,
			pData->m_arrFunction[Item.Function].Function);

		int Space = 80 - strTemp.GetLength();
		if (0 < Space)
			strTemp += CString(_T(' '), Space);

		strTemp.AppendFormat(_T(" CPU：%%%.1lf\r\n"), Item.CPU);
		strResult += strTemp;
	}
}

void CDataAnalyzer::FunctionUsing(CString &strResult)
{
	strResult = _T("");
	CData *pData = (CData*)m_pData;
	if (NULL == pData)
		return;

	CAtlArray<FUNCTION> &arrFunction = pData->m_arrFunction;
	UINT Count = arrFunction.GetCount();

	CAtlArray<int> arrIndex;
	for (UINT i = 0; i < Count; i++)
	{
		FUNCTION &Data = arrFunction[i];
		if (0 == arrIndex.GetCount())
		{
			arrIndex.Add(i);
		}
		else
		{
			BOOL Insert = FALSE;
			for (int j = 0; j < arrIndex.GetCount(); j++)
			{
				if (Data.Count >= arrFunction[arrIndex[j]].Count)
				{
					arrIndex.InsertAt(j, i);
					Insert = TRUE;
					break;
				}
			}

			if (!Insert)
				arrIndex.Add(i);
		}
	}

	strResult = _T("函数名称                                           执行次数\r\n----------------------------------------------------------------------------------------\r\n");

	Count = arrIndex.GetCount();
	for (UINT i = 0; i < Count; i++)
	{
		FUNCTION &Data = arrFunction[arrIndex[i]];

		CString strTemp;
		strTemp.Format(_T("%s"), Data.Function);

		int Space = 50 - strTemp.GetLength();
		if (0 < Space)
			strTemp += CString(_T(' '), Space);

		strTemp.AppendFormat(_T(" %d\r\n"), Data.Count);
		strResult += strTemp;
	}
}
