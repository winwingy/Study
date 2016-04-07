// LogAnalyzerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LogAnalyzer.h"
#include "LogAnalyzerDlg.h"

#include "FileReader.h"
#include "DataAnalyzer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CLogAnalyzerDlg 对话框




CLogAnalyzerDlg::CLogAnalyzerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogAnalyzerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_edtResult);
}

BEGIN_MESSAGE_MAP(CLogAnalyzerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &CLogAnalyzerDlg::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_NOTRETURN, &CLogAnalyzerDlg::OnBnClickedButtonNotreturn)
	ON_BN_CLICKED(IDC_BUTTON_LONGTIME, &CLogAnalyzerDlg::OnBnClickedButtonLongtime)
	ON_BN_CLICKED(IDC_BUTTON_RELATION, &CLogAnalyzerDlg::OnBnClickedButtonRelation)
	ON_BN_CLICKED(IDC_BUTTON_CPU, &CLogAnalyzerDlg::OnBnClickedButtonCpu)
	ON_BN_CLICKED(IDC_BUTTON_TIMES, &CLogAnalyzerDlg::OnBnClickedButtonTimes)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CLogAnalyzerDlg 消息处理程序

BOOL CLogAnalyzerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetDlgItemInt(IDC_EDIT_TIME, 10);
	m_edtResult.SetLimitText(1024 * 1024 * 1024);
	SetDlgTitle();

	GetClientRect(&m_rcDlgRect);
	ClientToScreen(&m_rcDlgRect);
	m_edtResult.GetWindowRect(&m_rcEditRect);
	EnableAnalyzer(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLogAnalyzerDlg::EnableAnalyzer(BOOL bEnable)
{
	GetDlgItem(IDC_BUTTON_NOTRETURN)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_LONGTIME)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_RELATION)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CPU)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_TIMES)->EnableWindow(bEnable);
}

void CLogAnalyzerDlg::SetDlgTitle(CString FileName /*= _T("")*/)
{
	CString strValue = AfxGetApp()->m_pszAppName;
	if (FileName.GetLength() != 0)
		strValue.AppendFormat(_T(" - %s"), FileName);
	SetWindowText(strValue);
}

void CLogAnalyzerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLogAnalyzerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLogAnalyzerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLogAnalyzerDlg::OnBnClickedButtonOpenfile()
{
	SetDlgTitle();
	m_edtResult.SetWindowText(_T(""));

	CFileDialog Open(TRUE);
	if (IDOK != Open.DoModal())
	{
		EnableAnalyzer(FALSE);
		return;
	}

	CWaitCursor Wait;
	CString strFileName = Open.GetPathName();

	CString strError;
	CFileReader Reader;
	DWORD Begin = GetTickCount();

	if (!Reader.ReadFile(strFileName, strError))
	{	
		m_edtResult.SetWindowText(strError);
		EnableAnalyzer(FALSE);
		return;
	}

	DWORD End = GetTickCount();
	if (m_oLogData.InitData(Reader.m_arrLogData, strError))
	{
		strError.Format(_T("读文件成功。用时：%ld毫秒。"), End - Begin);
		m_edtResult.SetWindowText(strError);
		SetDlgTitle(strFileName);
		EnableAnalyzer(TRUE);
	}
	else
	{
		m_edtResult.SetWindowText(strError);
		EnableAnalyzer(FALSE);
	}
}

void CLogAnalyzerDlg::OnBnClickedButtonNotreturn()
{
	CDataAnalyzer Analyzer(&m_oLogData);

	CString strResult;
	Analyzer.NotreturnAnalyzer(strResult);
	if (strResult.GetLength() == 0)
		strResult = _T("没有找到。");

	m_edtResult.SetWindowText(strResult);
}

void CLogAnalyzerDlg::OnBnClickedButtonLongtime()
{
	CDataAnalyzer Analyzer(&m_oLogData);

	CString strResult;
	Analyzer.TimeAnalyzer(GetDlgItemInt(IDC_EDIT_TIME), strResult);
	if (strResult.GetLength() == 0)
		strResult = _T("没有找到。");

	m_edtResult.SetWindowText(strResult);
}

void CLogAnalyzerDlg::OnBnClickedButtonCpu()
{
	CDataAnalyzer Analyzer(&m_oLogData);

	CString strResult;
	Analyzer.CPUUsageAnalyzer(strResult);
	if (strResult.GetLength() == 0)
		strResult = _T("没有找到。");

	m_edtResult.SetWindowText(strResult);
}

void CLogAnalyzerDlg::OnBnClickedButtonTimes()
{
	CDataAnalyzer Analyzer(&m_oLogData);

	CString strResult;
	Analyzer.FunctionUsing(strResult);
	if (strResult.GetLength() == 0)
		strResult = _T("没有找到。");

	m_edtResult.SetWindowText(strResult);
}

void CLogAnalyzerDlg::OnBnClickedButtonRelation()
{
	CFileDialog Open(FALSE);
	if (IDOK != Open.DoModal())
		return;

	CString strFileName = Open.GetPathName();

	CStdioFile Write;
	if (!Write.Open(strFileName, CFile::modeWrite | CFile::modeCreate))
	{
		AfxMessageBox(_T("创建文件失败。"));
		return;
	}

	CDataAnalyzer Analyzer(&m_oLogData);
	Analyzer.RelationAnalyzer(Write);
	Write.Close();

	CString strCommand;
	strCommand.Format(_T("notepad.exe %s"), strFileName);

	char Buffer[512] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, strCommand, -1, Buffer, 510, NULL, NULL);
	if (WinExec(Buffer, SW_SHOW) > 31)
		return;
	
	//TCHAR Error[ERROR_BUFFER_SIZE] = { 0 };
	//GetErrorMessage(GetLastError(), Error);
	//AfxMessageBox(Error);
}

void CLogAnalyzerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (NULL != m_edtResult.m_hWnd)
	{
		int Left = m_rcEditRect.left - m_rcDlgRect.left;
		int Top = m_rcEditRect.top - m_rcDlgRect.top;
		int Width = cx - Left - (m_rcDlgRect.right - m_rcEditRect.right);
		int Height = cy - Top - (m_rcDlgRect.bottom - m_rcEditRect.bottom);
		if (100 > Width) Width  = 100;
		if (50 > Height) Height = 50;
		m_edtResult.MoveWindow(Left, Top, Width, Height);
	}
}
