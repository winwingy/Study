#include "StdAfx.h"
#include "Data.h"

#define ENTER_SIGN_POSITION        36
#define POSITION_INDICATE         _T('|')  

CData::CData(void)
{
}

CData::~CData(void)
{
	RemoveAll();
}

void CData::RemoveAll()
{
	m_arrFileName.RemoveAll();
	m_arrFunction.RemoveAll();
	m_arrLogData.RemoveAll();
	m_arrLogData2.RemoveAll();
}

BOOL CData::InitData(CAtlArray<CString> &arrLog, CString &strError)
{
	RemoveAll();
	strError = _T("");

	const UINT Count = arrLog.GetCount();
	for (UINT i = 0; i < Count; i++)
	{
		if (!InitData(arrLog[i], strError))
			return FALSE;
	}

	if (m_arrLogData.GetCount() == 0)
	{
		strError = _T("没有读到日志数据。");
		return FALSE;
	}

	return TRUE;
}
/*
2013/10/11 18:09:49 426 03660 06056 0|CIVDShellFolder::FinalConstruct|.\IVDShellFolder.cpp|170|187 */
BOOL CData::InitData(CString &Log, CString &strError)
{
	int Index = Log.Find(POSITION_INDICATE, ENTER_SIGN_POSITION - 1) - 1;
	if (0 >= Index)
	{
		strError.Format(_T("错误日志数据。缺少分隔标志：%s"), Log);
		return FALSE;
	}

	LOGITEM2 Item;
	BOOL RetVal = FALSE;
	Item.Index = -1;
	if (_T('1') == Log[Index])
	{
		RetVal = InitData_Enter(Log, Item.Index, strError);
		Item.Ender = TRUE;
	}
	else if (_T('0') == Log[Index])
	{
		RetVal = InitData_Leave(Log, Item.Index, strError);
		Item.Ender = FALSE;
	}
	else
	{
		//strError.Format(_T("数据行没有函数进入或者返回标志：%s"), Log);
		// 文件中包含其它类型的记录，需要允许不符合要求的数据存在。
		RetVal = TRUE;
	}

	if (RetVal)
	{
		Item.Sign = FALSE;
		m_arrLogData2.Add(Item);
	}

	return RetVal;
}

BOOL CData::InitData_Enter(CString &Log, 
						   int &Index, 
						   CString &strError)
{
	LOGITEM Item;
	Item.Return   = FALSE;
	Item.Function = -1;
	Item.File     = -1;
	Item.CPU      = 0.0;

	GetTimeInfo(Log, Item.Enter, Item.ProcessId, Item.ThreadId);
	if (!InitData_File(Log, Item.File, Item.Function, Item.Line, Item.ObjectID, strError))
		return FALSE;

	Index = m_arrLogData.GetCount();
	m_arrLogData.Add(Item);

	return TRUE;
}

BOOL CData::GetTimeInfo(CString &Log, 
						SYSTEMTIME &Time, 
						long &ProcessId, 
						long &ThreadId)
{
	int Value[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

	swscanf(Log, _T("%d/%d/%d %d:%d:%d %d %d %d"),
		&Value[0], &Value[1], &Value[2], 
		&Value[3], &Value[4], &Value[5],
		&Value[6], &Value[7], &Value[8]);

	Time.wYear         = Value[0]; 
	Time.wMonth        = Value[1];
	Time.wDay          = Value[2];
	Time.wHour         = Value[3];
	Time.wMinute       = Value[4];
	Time.wSecond       = Value[5];
	Time.wMilliseconds = Value[6];
	ProcessId          = Value[7];
	ThreadId           = Value[8];

	return TRUE;
}

BOOL CData::InitData_Leave(CString &Log, 
						   int &Index, 
						   CString &strError)
{
	int File     = -1;
	int Function = -1;
	int Line     = -1;
	LONG64 ObjId = -1;

	if (!InitData_File(Log, File, Function, Line, ObjId, strError))
		return FALSE;

	LOGITEM Temp;
	GetTimeInfo(Log, Temp.Enter, Temp.ProcessId, Temp.ThreadId);

	int Find = Log.ReverseFind(_T('|'));
	CString strCPU = Log.Mid(Find + 1);

	UINT Count = m_arrLogData.GetCount();
	for (LONG i = Count - 1; i >= 0; i--)
	{
		LOGITEM &Item = m_arrLogData[i];
		if (Item.ObjectID == ObjId)
		{
			Index       = i;
			Item.Return = TRUE;
			Item.Leave  = Temp.Enter; 
			Item.CPU    = _wtof(strCPU);
			break;
		}
	}

	return TRUE;
}

BOOL CData::InitData_File(CString &Log, 
						  int &File, 
						  int &Function,
						  int &Line,
						  LONG64 &ObjId,  
						  CString &strError)
{
	int Begin = Log.Find(POSITION_INDICATE, ENTER_SIGN_POSITION - 1) + 1;
	int End = Log.Find(POSITION_INDICATE, Begin);
	if (-1 == End)
	{
		strError.Format(_T("数据行没有数据分隔标志：%s"), Log);
		return FALSE;	
	}

	CString strFunction = Log.Mid(Begin, End - Begin);
	AddFunction(strFunction, Function);

	do {
		int Length = Log.GetLength();
		Begin = End + 2;
		if (Begin >= Length)
			break;

		End = Log.Find(POSITION_INDICATE, Begin);
		if (End <= Begin)
			break;

		CString strFile = Log.Mid(Begin, End - Begin);
		AddFileName(strFile, File);

		Begin = End + 1;
		End = Log.Find(POSITION_INDICATE, Begin);
		if (End <= Begin)
			break;

		CString strLine = Log.Mid(Begin, End - Begin);
		Line = _wtoi(strLine);

		End++;
		CString strObjId = Log.Mid(End, Log.GetLength() - End);
		if (strObjId.IsEmpty())
			break;

		ObjId = _wtoi64(strObjId);
		return TRUE;

	} while (FALSE);

	strError.Format(_T("数据不完整：%s"), Log);
	return FALSE;
}

BOOL CData::AddFileName(CString &Value, 
						int &Index)
{
	Index = -1;

	UINT Count = m_arrFileName.GetCount();
	for (UINT i = 0; i < Count; i++)
	{
		if (Value == m_arrFileName[i])
		{
			Index = i;
			return TRUE;
		}
	}

	m_arrFileName.Add(Value);
	Index = Count;

	return TRUE;
}

BOOL CData::AddFunction(CString &Value, 
						int &Index)
{
	Index = -1;

	UINT Count = m_arrFunction.GetCount();
	for (UINT i = 0; i < Count; i++)
	{
		FUNCTION &Data = m_arrFunction[i];
		if (Value == Data.Function)
		{
			Index = i;
			Data.Count++;
			return TRUE;
		}
	}

	FUNCTION Data;
	Data.Count = 1;
	Data.Function = Value;

	m_arrFunction.Add(Data);
	Index = Count;

	return TRUE;
}

