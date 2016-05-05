
// TestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test.h"
#include "TestDlg.h"
#include "afxdialogex.h"
#include <assert.h>
#include <string>
#include "../LogLib/LogLib.h"
#pragma comment(lib, "../Debug/Loglib.lib")

#import "../Debug/WLogger.exe" no_namespace

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestDlg 对话框



CTestDlg::CTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CTestDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CTestDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CTestDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTestDlg 消息处理程序

BOOL CTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTestDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestDlg::OnBnClickedButton1()
{
    // TODO:  在此添加控件通知处理程序代码
    ILogger* logger = nullptr;
    HRESULT hr = ::CoCreateInstance(__uuidof(Logger), 0, CLSCTX_LOCAL_SERVER,
                                    __uuidof(ILogger), (void**)&logger);
    assert(SUCCEEDED(hr));
    DWORD err = GetLastError();
    //logger->InitLog();

    std::wstring text = L"hello World";
    BSTR textStr = ::SysAllocString(text.c_str());
    LONG writedLen = logger->Write(&textStr, ::SysStringLen(textStr));
    ::SysFreeString(textStr);

    BSTR bstr(::SysAllocStringByteLen(nullptr, 100));
    LONG readed = 0;
    readed = logger->Read(&bstr, 100);
    ::SysFreeString(bstr);
    logger->Exit();
    logger->Release();

}


void CTestDlg::OnBnClickedButton2()
{
    // TODO:  在此添加控件通知处理程序代码
    FILE* fp = nullptr;
    errno_t err = _tfopen_s(&fp, L"D:\\test\\Unicode.txt", L"w,ccs=UNICODE");
    if (0 != err)
    {
        assert(FALSE);
        return;
    }
    wchar_t buf[5] = L"1234";
    int writted = fwrite(buf, 2, 4, fp);
    fflush(fp);
    fclose(fp);
}


void CTestDlg::OnBnClickedButton3()
{
    // TODO:  在此添加控件通知处理程序代码
    LogLib log;
    log.Init(L"D:\\test\\atl.log", LOG_INFO_LEVEL_INFO, 100, 1);
    LOG_INFO((L"Hello Log\n"));
    LOG_INFO(L"Hello%d\n", 5);

}
