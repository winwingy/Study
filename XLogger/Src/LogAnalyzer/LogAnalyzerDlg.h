// LogAnalyzerDlg.h : 头文件
//

#pragma once

#include "Data.h"
#include "afxwin.h"

// CLogAnalyzerDlg 对话框
class CLogAnalyzerDlg : public CDialog
{
// 构造
public:
	CLogAnalyzerDlg(CWnd* pParent = NULL);	// 标准构造函数

	CData m_oLogData;	
	CRect m_rcDlgRect;
	CRect m_rcEditRect;

	void EnableAnalyzer(BOOL bEnable);
	void SetDlgTitle(CString FileName = _T(""));

// 对话框数据
	enum { IDD = IDD_LOGANALYZER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenfile();
	afx_msg void OnBnClickedButtonNotreturn();
	afx_msg void OnBnClickedButtonLongtime();
	CEdit m_edtResult;
	afx_msg void OnBnClickedButtonRelation();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonCpu();
	afx_msg void OnBnClickedButtonTimes();
};
