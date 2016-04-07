#pragma once

struct LOGITEM {
	LONG64 ObjectID;
	int File;
	int Function;
	int Line;
	long ProcessId;
	long ThreadId;
	SYSTEMTIME Enter;
	SYSTEMTIME Leave;
	double CPU;
	BOOL Return;
};

struct LOGITEM2 {
	int Index;
	BOOL Ender;
	BOOL Sign;
};

struct FUNCTION {
	int Count;
	CString Function;
};

class CData
{
public:
	CData(void);
	~CData(void);

	BOOL InitData(CAtlArray<CString> &arrLog, CString &strError);
	
private:

	CAtlArray<LOGITEM>  m_arrLogData;
	CAtlArray<LOGITEM2> m_arrLogData2;
	CAtlArray<FUNCTION>  m_arrFunction;
	CAtlArray<CString>  m_arrFileName;

	void RemoveAll();
	BOOL InitData(CString &Log, CString &strError);
	BOOL InitData_Enter(CString &Log, int &Index, CString &strError);
	BOOL InitData_Leave(CString &Log, int &Index, CString &strError);
	BOOL InitData_File(CString &Log, int &File, int &Function, int &Line, LONG64 &ObjId, CString &strError);
	BOOL GetTimeInfo(CString &Log, SYSTEMTIME &Time, long &ProcessId, long &ThreadId);
	BOOL AddFileName(CString &Value, int &Index);
	BOOL AddFunction(CString &Value, int &Index);

	friend class CDataAnalyzer;
};
