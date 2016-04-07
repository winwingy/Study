#pragma once

class CData;
class CDataAnalyzer
{
public:
	CDataAnalyzer(CData *pData);
	~CDataAnalyzer(void);

	void RelationAnalyzer(CStdioFile &Write);
	void NotreturnAnalyzer(CString &strResult);
	void CPUUsageAnalyzer(CString &strResult);
	void FunctionUsing(CString &strResult);
	void TimeAnalyzer(int iMinTime, CString &strResult);

private:

	LONG64 GetTime(SYSTEMTIME Begin, SYSTEMTIME End);
	int SaveRelation(CStdioFile &Write, int Index, int Space);
	void SaveRelation(CStdioFile &Write, struct LOGITEM &Item, int Space);

	class CData *m_pData;
};
