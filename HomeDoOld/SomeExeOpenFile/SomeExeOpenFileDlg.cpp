// SomeExeOpenFileDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SomeExeOpenFile.h"
#include "SomeExeOpenFileDlg.h"


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


// CSomeExeOpenFileDlg 对话框




CSomeExeOpenFileDlg::CSomeExeOpenFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSomeExeOpenFileDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSomeExeOpenFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSomeExeOpenFileDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CSomeExeOpenFileDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BTN_EXEOPENFILE, &CSomeExeOpenFileDlg::OnBnClickedBtnExeopenfile)
END_MESSAGE_MAP()


// CSomeExeOpenFileDlg 消息处理程序

BOOL CSomeExeOpenFileDlg::OnInitDialog()
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSomeExeOpenFileDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSomeExeOpenFileDlg::OnPaint()
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
HCURSOR CSomeExeOpenFileDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL GetExeCommand(char *fullname,char* command);

void CSomeExeOpenFileDlg::OnBnClickedButton1()
{
	char command[256];
	GetExeCommand("D:\\KuGou\\By2 - 我知道.mp3", command);
	TRACE("%s%d\n", command, strlen(command));
	m_strCommand = command;
	string strCommand;
	strCommand = command;//(command, command + strlen(command));
	TRACE("%s\n", strCommand.c_str());
	strCommand.erase(0, 1);
	string::size_type szPos = strCommand.find("\"");
	if( szPos != string::npos)
	{
		m_cstrExecute = string(strCommand, 0, szPos).c_str();
	}
	//"D:\Program Files\TTPlayer\TTPlayer.exe" ""D:\KuGou\By2 - 我知道.mp3""
	//ShellExecute中 第三个参数 带空格的字符串需要用引号括起来		
	m_cstrFileName = string(strCommand, szPos + 3, strCommand.length() - (szPos+3) -1 ).c_str();	

}
//下面的函数从注册表得到各种类型的open命令行。
////
BOOL GetExeCommand(char *fullname,char* command)
{// ""%1" 1*" for .bat file
	// "xxx.exe %1"
	// ""\a b\c\yyy.exe" /p %1"
	char ext[20];
	char pathname[MAX_PATH];
	char exename[MAX_PATH];
	char *at=0;
	strcpy(pathname,fullname);
	// must ".xxx"
	if((at=strrchr(fullname,'.'))==NULL) return FALSE;
	strcpy(ext,at);

	char open_key[MAX_PATH];
	DWORD openkeylen=MAX_PATH;
	char shellname[MAX_PATH]="shell";
	// under shell may be different "open","cplopen","Open with"...
	BYTE extfile[MAX_PATH];
	DWORD cbextfile=MAX_PATH;
	DWORD dwType=REG_SZ;
	HKEY hkey;
	HKEY hsubkey;
	// from ".xxx" get "xxxfile"
	DWORD dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,ext,0,KEY_READ,&hkey);
	if (dwErrorCode != ERROR_SUCCESS)
	{ // error not found
		command[0]=0;
		return FALSE;
	}
	else
	{ // ".xxx" OK
		// get "xxxfile"
		dwErrorCode=RegQueryValueEx(hkey,NULL,0,&dwType,extfile,&cbextfile);
		RegCloseKey(hkey);
		char tmp[MAX_PATH];
		sprintf(tmp,"%s\\%s",extfile,shellname);// "cppfile\\shell"
		if (dwErrorCode == ERROR_SUCCESS)  
		{// "cppfile\\shell"
			dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,tmp,0,KEY_READ,&hsubkey);
			if (dwErrorCode == ERROR_SUCCESS)  
			{// "cppfile\\shell"
				LONG enumErr=ERROR_SUCCESS;
				DWORD j=0;
				FILETIME ftLastWriteTime;
				while (enumErr==ERROR_SUCCESS || enumErr==ERROR_MORE_DATA)
				{
					cbextfile=260;// re-init
					openkeylen=MAX_PATH;// re-init
					enumErr=RegEnumKeyEx(hsubkey,j,open_key,&openkeylen,
						NULL,NULL,NULL,&ftLastWriteTime);
					strcpy(tmp,open_key);
					_strupr(tmp);
					if(strstr(tmp,"OPEN")) // any "open"
					{
						sprintf(tmp,"%s\\%s\\%s\\command",extfile,shellname,open_key);// "cppfile\\shell"
						break;
					}
					j++;
				}
				RegCloseKey(hsubkey);
				if(enumErr!=ERROR_SUCCESS) return FALSE;
			} // end with "shell" key
			else
			{ // no "shell" key
				return FALSE;
			}
			// "shell\\...open...\\command"	
			dwErrorCode=RegOpenKeyEx(HKEY_CLASSES_ROOT,tmp,0,KEY_READ,&hsubkey);
			cbextfile=260;// re-init
			//extfile="%SystemRoot%\\system32\\NOTEPAD32.EXE %1"
			dwErrorCode=RegQueryValueEx(hsubkey,NULL,0,&dwType,extfile,&cbextfile);
			//	if fullname contains space in its pathname
			if(strchr(fullname,' ')) sprintf(exename,"\"%s\"",fullname);
			else strcpy(exename,fullname);
			// replace "%SystemRoot%" to "c:\\winnt.400" //NT
			if(strstr((char *)extfile,"%SystemRoot%"))
			{  
				char windir[MAX_PATH];
				GetWindowsDirectory(windir,MAX_PATH);
				at=strchr((char *)extfile,'\\');// after "%SystemRoot%"

				strcat(windir,at);
				strcpy((char *)extfile,windir);
			}
			// parameterse	
			strcpy(tmp,(char*)extfile);
			if(at=strstr(tmp,"%1"))
			{
				*(at+1)='s';// "%s"  
				sprintf(command,tmp,exename);
			}
			else
			{
				sprintf(command,"%s %s",tmp,exename);
			}
			RegCloseKey(hsubkey);
			return TRUE;
		}
		else
		{
			command[0]=0;
			return FALSE;
		}
	}
}
void CSomeExeOpenFileDlg::OnBnClickedBtnExeopenfile()
{
	
	TRACE("%s\n", m_cstrFileName.GetBuffer());
	//带空格的字符串需要用引号括起来
	//int iRet = (int)ShellExecute(0, "open", m_cstrExecute, "\"D:\\KuGou\\By2 - 我知道.mp3\"",NULL, SW_SHOW);
	int iRet = (int)ShellExecute(0, "open", m_cstrExecute, m_cstrFileName,NULL, SW_SHOW);
}
