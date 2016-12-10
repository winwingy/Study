
// ListViewOwnerDrawDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ListViewOwnerDraw.h"
#include "ListViewOwnerDrawDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CListViewOwnerDrawDlg 对话框




CListViewOwnerDrawDlg::CListViewOwnerDrawDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CListViewOwnerDrawDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CListViewOwnerDrawDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CListViewOwnerDrawDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST1, &CListViewOwnerDrawDlg::OnLvnGetdispinfoList1)
END_MESSAGE_MAP()


// CListViewOwnerDrawDlg 消息处理程序

BOOL CListViewOwnerDrawDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    m_listCtrl.SetItemCount(200);
    m_listCtrl.SetWindowTextW(L"CListViewOwnerDrawDlgListView");
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CListViewOwnerDrawDlg::OnPaint()
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
HCURSOR CListViewOwnerDrawDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CListViewOwnerDrawDlg::OnLvnGetdispinfoList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
    LV_ITEM* pItem= &(pDispInfo)->item;

    int iItemIndx= pItem->iItem; 
    TCHAR szBuf[10] = L"A:";
    szBuf[2] = L'A' + iItemIndx;
    if (pItem->mask & LVIF_TEXT)
    {
        switch(pItem->iSubItem)
        {
        case 0:
            {
                memcpy(pItem->pszText, szBuf, 4*sizeof(TCHAR));
            }
            break;
        case 1:
            {
                memcpy(pItem->pszText, szBuf, 4*sizeof(TCHAR));
            }
            break;
        }
    }

    *pResult = 0;
}
