
// SubmitURLDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SubmitURL.h"
#include "SubmitURLDlg.h"
#include "SettingDialog.h"
#include "SoftwareAbout.h"
#include "curl/curl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
    void PrintLog( char *pstrLog );
#endif // _DEBUG



// 线程
DWORD WINAPI InitThreadProc(LPVOID pParam);
DWORD WINAPI SubmitThreadProc(LPVOID pParam);
DWORD WINAPI SubmitThreadProcs(LPVOID pParam);

// 提交函数 , 核心
void SearchEnginesMain( char* pURL, CURL* curl );
void SearchEnginesBaidu(char* pURL, CURL* curl );
void SearchEngines360(char* pURL, CURL* curl );
void SearchEnginesSogou(char* pURL, CURL* curl );
void SearchEnginesGoogle(char* pURL, CURL* curl );
void SearchEnginesBing(char* pURL, CURL* curl );

// 处理结果
size_t write_data_baidu(void *ptr, size_t size, size_t nmemb, void *stream);


// 构造完成的URL连接
char g_urllink[5][MAX_PATH];

// CSubmitURLDlg 对话框
CSubmitURLDlg* pDlg;

// 每个线程中对应字符串的数量
UINT g_nThreadSTRNumber = 0;


CSubmitURLDlg::CSubmitURLDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSubmitURLDlg::IDD, pParent)
	, m_nThreadNumber(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSubmitURLDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_THREADNUMBER, m_nThreadNumber);
	DDV_MinMaxUInt(pDX, m_nThreadNumber, 1, 50);
	DDX_Control(pDX, IDC_LIST, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CSubmitURLDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BEGIN, &CSubmitURLDlg::OnBnClickedBegin)
	ON_BN_CLICKED(IDC_END, &CSubmitURLDlg::OnBnClickedEnd)
	ON_BN_CLICKED(IDC_OPENFILE, &CSubmitURLDlg::OnBnClickedOpenfile)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK1,IDC_CHECK5,OnCheckButton)
	ON_MESSAGE((WM_USER+1),InitListURLNumber)
	ON_MESSAGE((WM_USER+2),ChangeListURLNumber)
	ON_MESSAGE((WM_USER+3),ChangStat)
	ON_MESSAGE((WM_USER+4),ChangeListURLNumberThreadError)
	ON_WM_TIMER()
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_KEYDOWN()
    ON_BN_CLICKED(IDC_SETTING, &CSubmitURLDlg::OnBnClickedSetting)
END_MESSAGE_MAP()


// CSubmitURLDlg 消息处理程序

BOOL CSubmitURLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// 默认是线程是停止运行
	m_bIsRun = FALSE;

	// 设置默认线程数量
	GetDlgItem(IDC_THREADNUMBER)->SetWindowText(_T("1"));

	// 设置微调按钮
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN))->SetRange(1,50);

	// 设置停止按钮禁止点击
	GetDlgItem(IDC_END)->EnableWindow(FALSE);

	// 设置风格 List 风格
	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	// 初始化List 表头 信息
	m_wndListCtrl.InsertColumn(0,_T("引擎名称"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(1,_T("成功"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(2,_T("失败"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(3,_T("剩余"),LVCFMT_LEFT,100);
	
	// 初始化List 列表 信息
	m_wndListCtrl.InsertItem(0,_T("百度搜索"));
	m_wndListCtrl.InsertItem(1,_T("360搜索"));
	m_wndListCtrl.InsertItem(2,_T("搜狗搜索"));
	m_wndListCtrl.InsertItem(3,_T("谷歌搜索"));
	m_wndListCtrl.InsertItem(4,_T("必应搜索"));

	// 初始化List 项
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0; j<3; ++j )
		{
			m_wndListCtrl.SetItemText(i,j+1,_T("0"));
		}
	}
	// 初始化数组 数字
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0 ; j<3; ++j )
			m_ArrayListNumber[i][j] = 0;
	}

	// 将对话框对象指针保存到全局变量
	pDlg = this;

   

    // 设置在线标志
    BeginSendOnLineData();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSubmitURLDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CSoftwareAbout dlgAbout;
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

void CSubmitURLDlg::OnPaint()
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
HCURSOR CSubmitURLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSubmitURLDlg::OnBnClickedBegin()
{
	// 判断是否可以点击开始按钮
	if( IsBegin() == FALSE )
		return ;// 不可以点击

	GetDlgItem(IDC_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(FALSE);

	// 禁止所有可以控控件
	ControlStateControl(FALSE);

	// 修改执行状态
	m_bIsRun =TRUE;

	// 创建线程(初始化文件信息)
	DWORD nThreadID = 0;
	CreateThread(NULL,0,InitThreadProc,this,0,&nThreadID);
}

void CSubmitURLDlg::OnBnClickedEnd()
{
	// 禁止按钮不可以点击了
	GetDlgItem(IDC_END)->EnableWindow(FALSE);

	// 修改执行状态
	m_bIsRun =FALSE;
	
}
// 控制状态
void CSubmitURLDlg::ControlStateControl( BOOL nBool )
{
	GetDlgItem(IDC_CHECK1)->EnableWindow(nBool);
	GetDlgItem(IDC_CHECK2)->EnableWindow(nBool);
	GetDlgItem(IDC_CHECK3)->EnableWindow(nBool);
	GetDlgItem(IDC_CHECK4)->EnableWindow(nBool);
	GetDlgItem(IDC_CHECK5)->EnableWindow(nBool);
	GetDlgItem(IDC_OPENFILE)->EnableWindow(nBool);
	GetDlgItem(IDC_THREADNUMBER)->EnableWindow(nBool);
	GetDlgItem(IDC_SPIN)->EnableWindow(nBool);
    GetDlgItem(IDC_SETTING)->EnableWindow(nBool);

}
void CSubmitURLDlg::OnBnClickedOpenfile()
{
	CFileDialog FileDlg(TRUE,_T("txt"),NULL,OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT,_T("文本文档 (*.txt)|*.txt||"));

	// 显示文件选择对话框
	if( FileDlg.DoModal() != IDOK )
		return; // 没有点击打开选择文件

	// 设置到EDIT
	GetDlgItem(IDC_FILEPATH)->SetWindowText(FileDlg.GetFileName());
	// 保存到成员变量中
	m_strFilePath = FileDlg.GetPathName();

}

BOOL CSubmitURLDlg::IsBegin( VOID )
{
	
	// 判断是否选中了需要Ping的搜索引擎
	if ( static_cast<CButton*>(GetDlgItem(IDC_CHECK1))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK2))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK3))->GetCheck()==1 || 
		 static_cast<CButton*>(GetDlgItem(IDC_CHECK4))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK5))->GetCheck()==1 )
	{}else{
		AfxMessageBox(_T("您未选择需要提交URL的搜索引擎"));
		return FALSE;
	}		
	
	// 判断是否选择过文件
	if ( wcscmp(m_strFilePath,_T("")) == 0)
	{ // 未选择
		AfxMessageBox(_T("您未选择需要提交的URL文件"));
		return FALSE;
	}

    CString strDomainName = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    if ( wcscmp(strDomainName,_T("")) == 0 )
    { //未设置域名
        AfxMessageBox(_T("您未配置域名, 可以点击配置按钮, 配置域名和KEY"));
        return FALSE;
    }
    
	return TRUE;
}

// 初始化线程,用于创建线程的线程
DWORD WINAPI InitThreadProc(LPVOID pParam)
{	
	CSubmitURLDlg* pDlg = (CSubmitURLDlg*)pParam;

	// 打开文件
	CFile URLFile;
	URLFile.Open(pDlg->m_strFilePath,CFile::modeRead);

	// 获取文件长度
	UINT FileLen =  0;
	FileLen = URLFile.GetLength();

	// 申请空间
	char* pFiledata = new char[FileLen+1];
	// 清空内存
	memset(pFiledata,0,FileLen+1);

	// 读取文件
	URLFile.Read(pFiledata,FileLen);

	// 关闭文件
	URLFile.Close();
	
	// 计算URL的行数
	int nURLLine = 1;
	for( int i = 0 ; i< FileLen; ++i) // waring
	{
		// 判断 \n
		if (pFiledata[i] == '\r')
			++nURLLine;
	}
	// Dlg初始化
	//////////////////////////////////////////////////////////////////////////
	// 初始化List 项
	for ( int i = 0 ; i<5; ++i )
	{ 
		for ( int j = 0; j<3; ++j )
		{
			pDlg->m_wndListCtrl.SetItemText(i,j+1,_T("0"));
		}
	}

	// 初始化数组 数字
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0 ; j<3; ++j )
			pDlg->m_ArrayListNumber[i][j] = 0;
	}

	// 发送消息,到List 做初始化操作
	pDlg->SendMessage((WM_USER+1),(WPARAM)nURLLine,0);

    // 构造请求URL
    pDlg->URLLink( );
    
	//////////////////////////////////////////////////////////////////////////

	// 再次申请空间, 给字符串分段
	char** pURL = NULL;
	pURL = new char *[nURLLine];
	for(int i=0;i<nURLLine; ++i )
		pURL[i] = new char [MAX_PATH];

	// 清空内存
	for ( int i = 0 ; i <nURLLine; ++i )
	{
		for ( int j = 0 ; j < MAX_PATH; ++j )
			pURL[i][j] = 0;
	}


	// 给字符串分段
	int nURL = 0;
	int nPos = 0;
	while( nURL < nURLLine )
	{
		for ( int i = 0 ; pFiledata[nPos]!= '\r' ; ++i )
		{
			pURL[nURL][i] = pFiledata[nPos];
			if (++nPos>FileLen)
				goto ok;
		}
		// 将\r\n去除掉
		nPos+=2;
		// 增加到下一行
		++nURL;
	}
ok:

	// 释放旧内存
	delete []pFiledata;

	// 获取线程的数量
	CString strThreadNumber;
	pDlg->GetDlgItem(IDC_THREADNUMBER)->GetWindowText(strThreadNumber);
	// 转换成数字
	UINT8 nThreadNumber = _ttoi(strThreadNumber);
	// 每个线程对应的空间
	char*** pThreadURL = NULL;
	// 每个线程对应的字符串数量
	int nThreadSTRNumber = 0;
	// 判断是否线程数量比URL数量大
	if(nThreadNumber > nURLLine )
	{
		AfxMessageBox(_T("您的URL数量不够线程分配的\r\n\r\n我们将线程数量修改为1条"));
		// 将线程数修改为1
		pDlg->GetDlgItem(IDC_THREADNUMBER)->SetWindowText(_T("1"));
		nThreadNumber = 1;
	}else
	{ 
		if( nThreadNumber == 1)
			goto BeginThread; // 1个线程,就没必要做这些工作了

		// 每个线程对应的字符串数量
		nThreadSTRNumber = ( nURLLine/nThreadNumber)+2 ;// +1防止出现不整除  ; +1用于判断空白结束

		// 申请三维空间
		pThreadURL = new char** [nThreadNumber];
		for ( int i = 0; i< nThreadNumber; ++i)
		{
			pThreadURL[i] = new char*[nThreadSTRNumber];
			for ( int j = 0; j<nThreadSTRNumber; ++j)
			{
				pThreadURL[i][j] = new char[MAX_PATH];
			}
		}
		
		// 清空内存
		for ( int i = 0 ; i <nThreadNumber; ++i )
		{
			for ( int j = 0 ; j<nThreadSTRNumber; ++j )
			{
				for ( int k = 0 ; k<MAX_PATH; ++k)
					pThreadURL[i][j][k] = 0;
			}
		}


		int nThreadType = 0; // 哪一个线程分配字符串
		int nThreadSTRPos = 0; // 线程中子字符串分配的位置
		// 字符串分配
		for ( int i = 0; i<nURLLine; ++i )
		{
			// 拷贝字符串
			strcpy(&pThreadURL[nThreadType][nThreadSTRPos][0],&pURL[i][0]);
			
			// 给每个线程拷贝字符串
			if( ++nThreadType == nThreadNumber)
			{
				// 分配线程从头开始
				nThreadType = 0;
				// 每个线程中字符串的位置增加
				++nThreadSTRPos; 
			}
		}

		// 释放内存 二维数组
		for ( int  i = 0 ; i<nURLLine; ++i ) 
			delete [] pURL[i];
		delete[] pURL;
		pURL = NULL;
	}
	
BeginThread:
	
	// 开始定时器

	pDlg->SetTimer(1000,1000,NULL);
	
	// 根据 选择的线程个数 进行处理数据
	if ( nThreadNumber == 1 )
	{
		// 将线程需要处理的字数串数量 保存到全局变量中
		g_nThreadSTRNumber = nURLLine;

		HANDLE hThread;
		// 创建线程
		hThread = CreateThread(NULL,0,SubmitThreadProc,&pURL[0],0,NULL);
		// 等待线程
		WaitForSingleObject(hThread,INFINITE);
	}else
	{
		// 将线程需要处理的字数串数量 保存到全局变量中
		g_nThreadSTRNumber = nThreadSTRNumber;
		// 循环创建线程
		HANDLE hThreadArray[50];
		for ( int i = 0 ; i< nThreadNumber; ++i ) 
		{
			hThreadArray[i] = CreateThread(NULL,0,SubmitThreadProcs,&pThreadURL[i][0],0,NULL);
			
		}
		// 等待线程
		WaitForMultipleObjects(nThreadNumber,hThreadArray,TRUE,INFINITE);
	}
	
	// 结束掉定时器
	pDlg->KillTimer(1000);
	// 最后一次调用
	pDlg->OnTimer(1000);
	// 修改执行状态
	pDlg->m_bIsRun = FALSE;


	// 释放内存 二维数组
	if ( pURL != NULL)
	{
		for ( int  i = 0 ; i<nURLLine; ++i ) 
			delete [] pURL[i];
		delete[] pURL;
	}
	// 释放内存 三维数组
	if ( pThreadURL != NULL)
	{
		for(int i=0; i<nThreadNumber; i++) 
		{
			for(int j=0; j<nThreadSTRNumber; j++) 
			{   
				delete [] pThreadURL[i][j];   
			}   
		}       
		for( int i=0; i<nThreadNumber; i++)   
		{       
			delete [] pThreadURL[i];   
		}   
		delete [] pThreadURL;  
	}
	
	// 发送完成消息
	pDlg->SendMessage((WM_USER+3),0,0);
	
	

	return 0;
}

// 初始化URL数量
LRESULT CSubmitURLDlg::InitListURLNumber( WPARAM wParam, LPARAM lParam )
{
	CString strText;
	strText.Format(_T("%d"),(UINT32)wParam);

	// 判断添加
	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		// 及时更新到List
		m_wndListCtrl.SetItemText(0,3,strText);
		// 设置数字
		m_ArrayListNumber[0][2] = (UINT32)wParam;
	}
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		// 及时更新到List
		m_wndListCtrl.SetItemText(1,3,strText);
		// 设置数字
		m_ArrayListNumber[1][2] = (UINT32)wParam;
	}
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
		// 及时更新到List
		m_wndListCtrl.SetItemText(2,3,strText);
		// 设置数字
		m_ArrayListNumber[2][2] = (UINT32)wParam;
	}
	
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{
		// 及时更新到List
		m_wndListCtrl.SetItemText(3,3,strText);
		// 设置数字
		m_ArrayListNumber[3][2] = (UINT32)wParam;
	}
	
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
		// 及时更新到List
		m_wndListCtrl.SetItemText(4,3,strText);
		// 设置数字
		m_ArrayListNumber[4][2] = (UINT32)wParam;
	}

	return TRUE;
}

// 修改URL数量
LRESULT CSubmitURLDlg::ChangeListURLNumber( WPARAM wParam, LPARAM lParam )
{
	if( wParam>4||wParam<0 )
		return TRUE; // 错误项

	// 减掉未发送
	m_ArrayListNumber[wParam][2] = m_ArrayListNumber[wParam][2]-1;

	if ( lParam == 1)
	{ // 发送成功
		m_ArrayListNumber[wParam][0] = m_ArrayListNumber[wParam][0]+1;
	}else
	{ // 发送失败
		m_ArrayListNumber[wParam][1] = m_ArrayListNumber[wParam][1]+1;
	}
	
	return TRUE;
}
LRESULT CSubmitURLDlg::ChangeListURLNumberThreadError(WPARAM wParam, LPARAM lParam)// 整个线程失败
{
	if (static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		// 减掉未发送
		m_ArrayListNumber[0][2] = m_ArrayListNumber[0][2]-1;
		// 发送失败
		m_ArrayListNumber[0][1] = m_ArrayListNumber[0][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		// 减掉未发送
		m_ArrayListNumber[1][2] = m_ArrayListNumber[1][2]-1;
		// 发送失败
		m_ArrayListNumber[1][1] = m_ArrayListNumber[1][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
		// 减掉未发送
		m_ArrayListNumber[2][2] = m_ArrayListNumber[2][2]-1;
		// 发送失败
		m_ArrayListNumber[2][1] = m_ArrayListNumber[2][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{
		// 减掉未发送
		m_ArrayListNumber[3][2] = m_ArrayListNumber[3][2]-1;
		// 发送失败
		m_ArrayListNumber[3][1] = m_ArrayListNumber[3][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
		// 减掉未发送
		m_ArrayListNumber[4][2] = m_ArrayListNumber[4][2]-1;
		// 发送失败
		m_ArrayListNumber[4][1] = m_ArrayListNumber[4][1]+1;
	}
    return 0;
}

LRESULT CSubmitURLDlg::ChangStat(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_END)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
	
	// 启用所有可以控控件
	ControlStateControl(TRUE);

	return TRUE;
}

#ifdef _DEBUG
void PrintLog( char *pstrLog )
{
    HANDLE hAndle = GetStdHandle( STD_OUTPUT_HANDLE );// 获取句柄
    WriteConsoleA(hAndle, pstrLog , strlen(pstrLog ),NULL,NULL); // 输出内容
}
#endif // _DEBUG


// 提交线程,核心线程(单个线程)
DWORD WINAPI SubmitThreadProc(LPVOID pParam)
{
	char** ppArray = (char**)pParam;
    
    CURL *curl;  
    curl = curl_easy_init();

    if ( !curl )  
    { // 初始化CURL失败
        for ( int i = 0; i<g_nThreadSTRNumber; ++i )
            pDlg->SendMessage((WM_USER+4),0,0); // 循环发送错误消息
        return -1;
    }

    // 设置超时
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);

	for ( int i = 0; i<g_nThreadSTRNumber; ++i )
	{
		// 判断是否需要停止
		if (pDlg->m_bIsRun == FALSE)
			goto endthread; // 停止
		SearchEnginesMain(&ppArray[i][0],curl);
	
	}
    // 结束
endthread:
    // 释放CURL
    curl_easy_cleanup( curl );  

	return 0;
}

// 提交线程,核心线程(多个线程)
DWORD WINAPI SubmitThreadProcs(LPVOID pParam)
{
	char** ppArray = (char**)pParam;

	CURL *curl;  
	curl = curl_easy_init();

	if ( !curl )  
	{ // 初始化CURL失败
		for ( int i = 0; strcmp(&ppArray[i][0],"") != 0; ++i )
            pDlg->SendMessage((WM_USER+4),0,0); // 循环发送错误消息
		
		return -1;
	}
    // 设置超时
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	for ( int i = 0; strcmp(&ppArray[i][0],"") != 0; ++i )
	{
		// 判断是否需要停止
		if (pDlg->m_bIsRun == FALSE)
			goto endthread; // 停止
		SearchEnginesMain(&ppArray[i][0],curl);
	}
// 结束
endthread:
    // 释放CURL
    curl_easy_cleanup( curl );  

	return 0;
}

// 点击Check 按钮
void CSubmitURLDlg::OnCheckButton( UINT nID )
{
	// 发送消息, 取消选中
	if( nID != IDC_CHECK1 )
	{
		GetDlgItem(nID)->SendMessage(BM_SETCHECK,BST_UNCHECKED);
		if( AfxMessageBox(_T("该搜索引擎服务正在紧张编码中\r\n\r\n是否打开软件主页查看最新版?"),MB_YESNO) == IDYES )
		{
            ShellExecute(NULL, _T("open"),_T("http://www.pdling.com/thread-196-1-1.html"), NULL, NULL, SW_SHOW);
		}
		
	}
}


void CSubmitURLDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == 1000 )
	{
		for ( int i = 0 ; i<5; ++i )
		{
			for ( int j = 0; j<3; ++j )
			{
				CString strText;
				UINT nUmber = m_ArrayListNumber[i][j];
				strText.Format(_T("%d"),nUmber);
				// 设置文本
				m_wndListCtrl.SetItemText(i,j+1,strText);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void SearchEnginesMain( char* pURL, CURL* curl )
{

	// 判断是否需要停止
	if (pDlg->m_bIsRun == FALSE)
		return; // 停止
	if (static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		SearchEnginesBaidu(pURL,curl);
	}


	// 判断是否需要停止
	if (pDlg->m_bIsRun == FALSE)
		return; // 停止
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		SearchEngines360(pURL,curl);
	}
	

	// 判断是否需要停止
	if (pDlg->m_bIsRun == FALSE)
		return; // 停止
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
	}

	
	// 判断是否需要停止
	if (pDlg->m_bIsRun == FALSE)
		return; // 停止
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{

	}
	

	// 判断是否需要停止
	if (pDlg->m_bIsRun == FALSE)
		return; // 停止
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
	}
}
void SearchEnginesBaidu(char* pURL, CURL* curl )
{
    
    // 设置URL
    curl_easy_setopt( curl, CURLOPT_URL, &g_urllink[0][0]);  
    //设置返回html源码的方式
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_baidu);

    // 指定post内容
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pURL);    

    // 开始请求
    CURLcode res = curl_easy_perform( curl );  
    
    // 判断返回值 是否进行重试 重试3次
    for ( int i = 0 ; res!=CURLE_OK && i!=3; ++i)
        res = curl_easy_perform( curl ); // 开始请求

    // 判断在循环外面是否为 CURLE_OK
    if (res!=CURLE_OK)
    { 
        pDlg->SendMessage((WM_USER+2),0,0); // 发送失败
    }else
    {
        // 获取返回码
        long http_code = 0;
        curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_code);
        if(http_code != 200) // 参考ReadMe.txt
        { // 推送失败
            pDlg->SendMessage((WM_USER+2),0,0); // 发送失败
        }else
        { // 推送成功
            pDlg->SendMessage((WM_USER+2),0,1); // 发送成功
        }
    }


	
}

void SearchEngines360(char* pURL, CURL* curl )
{

}

void SearchEnginesSogou(char* pURL, CURL* curl )
{

}

void SearchEnginesGoogle(char* pURL, CURL* curl )
{

}

void SearchEnginesBing(char* pURL, CURL* curl )
{

}
// 百度 提交URL返回数据
size_t write_data_baidu(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	
	return size*nmemb;
}

void CSubmitURLDlg::OnClose()
{
    // 询问是否退出程序
    if ( AfxMessageBox(_T("您是否要退出程序?"),MB_YESNO) == IDYES)
    { // 退出
        CDialog::OnClose();
    }
}

LRESULT CSubmitURLDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    
    // 广告模块
    SoftwareADProc(message,wParam,lParam);

    return CDialog::WindowProc(message, wParam, lParam);
}

void CSubmitURLDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // 结束发送数据
    EnableSendOnLineData();
}


INT_PTR CSubmitURLDlg::DoModal()
{
    // 初始化信息
    ___SOFTWARE_AD_MainWnd = AfxGetApp()->GetMainWnd();;
    ___SOFTWARE_ID = 21;
    ___SOFTWARE_VersionsNumber = 1;

    return CDialog::DoModal();
}

void CSubmitURLDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    
    CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

BOOL CSubmitURLDlg::PreTranslateMessage(MSG* pMsg)
{
    if ( pMsg->message == WM_KEYDOWN )
    {
        if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN )
            return TRUE;

    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CSubmitURLDlg::WinHelp(DWORD dwData, UINT nCmd)
{
    // TODO: 在此添加专用代码和/或调用基类

    //CDialog::WinHelp(dwData, nCmd);
}

// 点击设置按钮
void CSubmitURLDlg::OnBnClickedSetting()
{
    CSettingDialog Dlg;
    Dlg.DoModal();
}

// 将宽字节字符集转换为多字节字符集
char* strWToM(CString str)
{
    // 转换后所需要的内存大小
    int nstrLen = WideCharToMultiByte(CP_ACP,0,str,wcslen(str),NULL,0,NULL,NULL);
    // 申请内存空间
    char* pmstr = (char*) malloc( sizeof(char)*(nstrLen+1) );
    // 清空内存
    ZeroMemory(pmstr,sizeof(char)*(nstrLen+1));
    // 转换字符串
    nstrLen = WideCharToMultiByte(CP_ACP,0,str,wcslen(str),pmstr,nstrLen,NULL,NULL);
    // 添加结尾
    pmstr[nstrLen] = 0;
    return pmstr;
}

// 构造URL连接
void CSubmitURLDlg::URLLink( )
{
    
    // 获取域名
    CString strDomainName = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    // 获取Key
    CString strKey1 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_1"),_T(""));
    CString strKey2 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_2"),_T(""));
    CString strKey3 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_3"),_T(""));
    CString strKey4 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_4"),_T(""));
    CString strKey5 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_5"),_T(""));

    // 转换后的Key
    char* pstrDomainName = NULL;
    char* pstrKey1 = NULL;
    char* pstrKey2 = NULL;
    char* pstrKey3 = NULL;
    char* pstrKey4 = NULL;
    char* pstrKey5 = NULL;

    // 转换Key字符串

    if ( wcscmp(strDomainName,_T("")) !=0 )
        pstrDomainName = strWToM(strDomainName);// 有数据
    
    if ( wcscmp(strKey1,_T("")) !=0 )
        pstrKey1 = strWToM(strKey1);// 有数据
    
    if ( wcscmp(strKey2,_T("")) !=0 )
        pstrKey2 = strWToM(strKey2);// 有数据
    
    if ( wcscmp(strKey3,_T("")) !=0 )
        pstrKey3 = strWToM(strKey3);// 有数据
    
    if ( wcscmp(strKey4,_T("")) !=0 )
        pstrKey4 = strWToM(strKey4);// 有数据

    if ( wcscmp(strKey5,_T("")) !=0 )
        pstrKey5 = strWToM(strKey5);// 有数据
    
    
        

    // 获取从注册表数据
    int nIsOriginal = AfxGetApp()->GetProfileInt(_T("SETTING"),_T("ORIGINAL"),0);
    // 构造URL判断
    if( nIsOriginal == 0 )
    { // 非原创文章
        // 转换 百度
        sprintf_s(&g_urllink[0][0],MAX_PATH,"http://data.zz.baidu.com/urls?site=%s&token=%s",pstrDomainName,pstrKey1);
    }else
    { // 原创文章
        // 转换 百度
        sprintf_s(&g_urllink[0][0],MAX_PATH,"http://data.zz.baidu.com/urls?site=%s&token=%s&type=original",pstrDomainName,pstrKey1);
    }

    // 释放内存
    if ( wcscmp(strDomainName,_T("")) !=0 )
        free(pstrDomainName); // 有数据
    
    if ( wcscmp(strKey1,_T("")) !=0 )
        free(pstrKey1);// 有数据
    
    if ( wcscmp(strKey2,_T("")) !=0 )
        free(pstrKey2);// 有数据
    
    if ( wcscmp(strKey3,_T("")) !=0 )
        free(pstrKey3);// 有数据
    
    if ( wcscmp(strKey4,_T("")) !=0 )
        free(pstrKey4);// 有数据
    
    if ( wcscmp(strKey5,_T("")) !=0 )
        free(pstrKey5);// 有数据

}
