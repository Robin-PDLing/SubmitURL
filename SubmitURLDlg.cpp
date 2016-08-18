
// SubmitURLDlg.cpp : ʵ���ļ�
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



// �߳�
DWORD WINAPI InitThreadProc(LPVOID pParam);
DWORD WINAPI SubmitThreadProc(LPVOID pParam);
DWORD WINAPI SubmitThreadProcs(LPVOID pParam);

// �ύ���� , ����
void SearchEnginesMain( char* pURL, CURL* curl );
void SearchEnginesBaidu(char* pURL, CURL* curl );
void SearchEngines360(char* pURL, CURL* curl );
void SearchEnginesSogou(char* pURL, CURL* curl );
void SearchEnginesGoogle(char* pURL, CURL* curl );
void SearchEnginesBing(char* pURL, CURL* curl );

// ������
size_t write_data_baidu(void *ptr, size_t size, size_t nmemb, void *stream);


// ������ɵ�URL����
char g_urllink[5][MAX_PATH];

// CSubmitURLDlg �Ի���
CSubmitURLDlg* pDlg;

// ÿ���߳��ж�Ӧ�ַ���������
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


// CSubmitURLDlg ��Ϣ�������

BOOL CSubmitURLDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// Ĭ�����߳���ֹͣ����
	m_bIsRun = FALSE;

	// ����Ĭ���߳�����
	GetDlgItem(IDC_THREADNUMBER)->SetWindowText(_T("1"));

	// ����΢����ť
	static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN))->SetRange(1,50);

	// ����ֹͣ��ť��ֹ���
	GetDlgItem(IDC_END)->EnableWindow(FALSE);

	// ���÷�� List ���
	m_wndListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	
	// ��ʼ��List ��ͷ ��Ϣ
	m_wndListCtrl.InsertColumn(0,_T("��������"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(1,_T("�ɹ�"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(2,_T("ʧ��"),LVCFMT_LEFT,100);
	m_wndListCtrl.InsertColumn(3,_T("ʣ��"),LVCFMT_LEFT,100);
	
	// ��ʼ��List �б� ��Ϣ
	m_wndListCtrl.InsertItem(0,_T("�ٶ�����"));
	m_wndListCtrl.InsertItem(1,_T("360����"));
	m_wndListCtrl.InsertItem(2,_T("�ѹ�����"));
	m_wndListCtrl.InsertItem(3,_T("�ȸ�����"));
	m_wndListCtrl.InsertItem(4,_T("��Ӧ����"));

	// ��ʼ��List ��
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0; j<3; ++j )
		{
			m_wndListCtrl.SetItemText(i,j+1,_T("0"));
		}
	}
	// ��ʼ������ ����
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0 ; j<3; ++j )
			m_ArrayListNumber[i][j] = 0;
	}

	// ���Ի������ָ�뱣�浽ȫ�ֱ���
	pDlg = this;

   

    // �������߱�־
    BeginSendOnLineData();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSubmitURLDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSubmitURLDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSubmitURLDlg::OnBnClickedBegin()
{
	// �ж��Ƿ���Ե����ʼ��ť
	if( IsBegin() == FALSE )
		return ;// �����Ե��

	GetDlgItem(IDC_END)->EnableWindow(TRUE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(FALSE);

	// ��ֹ���п��Կؿؼ�
	ControlStateControl(FALSE);

	// �޸�ִ��״̬
	m_bIsRun =TRUE;

	// �����߳�(��ʼ���ļ���Ϣ)
	DWORD nThreadID = 0;
	CreateThread(NULL,0,InitThreadProc,this,0,&nThreadID);
}

void CSubmitURLDlg::OnBnClickedEnd()
{
	// ��ֹ��ť�����Ե����
	GetDlgItem(IDC_END)->EnableWindow(FALSE);

	// �޸�ִ��״̬
	m_bIsRun =FALSE;
	
}
// ����״̬
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
	CFileDialog FileDlg(TRUE,_T("txt"),NULL,OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT,_T("�ı��ĵ� (*.txt)|*.txt||"));

	// ��ʾ�ļ�ѡ��Ի���
	if( FileDlg.DoModal() != IDOK )
		return; // û�е����ѡ���ļ�

	// ���õ�EDIT
	GetDlgItem(IDC_FILEPATH)->SetWindowText(FileDlg.GetFileName());
	// ���浽��Ա������
	m_strFilePath = FileDlg.GetPathName();

}

BOOL CSubmitURLDlg::IsBegin( VOID )
{
	
	// �ж��Ƿ�ѡ������ҪPing����������
	if ( static_cast<CButton*>(GetDlgItem(IDC_CHECK1))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK2))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK3))->GetCheck()==1 || 
		 static_cast<CButton*>(GetDlgItem(IDC_CHECK4))->GetCheck()==1 || static_cast<CButton*>(GetDlgItem(IDC_CHECK5))->GetCheck()==1 )
	{}else{
		AfxMessageBox(_T("��δѡ����Ҫ�ύURL����������"));
		return FALSE;
	}		
	
	// �ж��Ƿ�ѡ����ļ�
	if ( wcscmp(m_strFilePath,_T("")) == 0)
	{ // δѡ��
		AfxMessageBox(_T("��δѡ����Ҫ�ύ��URL�ļ�"));
		return FALSE;
	}

    CString strDomainName = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    if ( wcscmp(strDomainName,_T("")) == 0 )
    { //δ��������
        AfxMessageBox(_T("��δ��������, ���Ե�����ð�ť, ����������KEY"));
        return FALSE;
    }
    
	return TRUE;
}

// ��ʼ���߳�,���ڴ����̵߳��߳�
DWORD WINAPI InitThreadProc(LPVOID pParam)
{	
	CSubmitURLDlg* pDlg = (CSubmitURLDlg*)pParam;

	// ���ļ�
	CFile URLFile;
	URLFile.Open(pDlg->m_strFilePath,CFile::modeRead);

	// ��ȡ�ļ�����
	UINT FileLen =  0;
	FileLen = URLFile.GetLength();

	// ����ռ�
	char* pFiledata = new char[FileLen+1];
	// ����ڴ�
	memset(pFiledata,0,FileLen+1);

	// ��ȡ�ļ�
	URLFile.Read(pFiledata,FileLen);

	// �ر��ļ�
	URLFile.Close();
	
	// ����URL������
	int nURLLine = 1;
	for( int i = 0 ; i< FileLen; ++i) // waring
	{
		// �ж� \n
		if (pFiledata[i] == '\r')
			++nURLLine;
	}
	// Dlg��ʼ��
	//////////////////////////////////////////////////////////////////////////
	// ��ʼ��List ��
	for ( int i = 0 ; i<5; ++i )
	{ 
		for ( int j = 0; j<3; ++j )
		{
			pDlg->m_wndListCtrl.SetItemText(i,j+1,_T("0"));
		}
	}

	// ��ʼ������ ����
	for ( int i = 0 ; i<5; ++i )
	{
		for ( int j = 0 ; j<3; ++j )
			pDlg->m_ArrayListNumber[i][j] = 0;
	}

	// ������Ϣ,��List ����ʼ������
	pDlg->SendMessage((WM_USER+1),(WPARAM)nURLLine,0);

    // ��������URL
    pDlg->URLLink( );
    
	//////////////////////////////////////////////////////////////////////////

	// �ٴ�����ռ�, ���ַ����ֶ�
	char** pURL = NULL;
	pURL = new char *[nURLLine];
	for(int i=0;i<nURLLine; ++i )
		pURL[i] = new char [MAX_PATH];

	// ����ڴ�
	for ( int i = 0 ; i <nURLLine; ++i )
	{
		for ( int j = 0 ; j < MAX_PATH; ++j )
			pURL[i][j] = 0;
	}


	// ���ַ����ֶ�
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
		// ��\r\nȥ����
		nPos+=2;
		// ���ӵ���һ��
		++nURL;
	}
ok:

	// �ͷž��ڴ�
	delete []pFiledata;

	// ��ȡ�̵߳�����
	CString strThreadNumber;
	pDlg->GetDlgItem(IDC_THREADNUMBER)->GetWindowText(strThreadNumber);
	// ת��������
	UINT8 nThreadNumber = _ttoi(strThreadNumber);
	// ÿ���̶߳�Ӧ�Ŀռ�
	char*** pThreadURL = NULL;
	// ÿ���̶߳�Ӧ���ַ�������
	int nThreadSTRNumber = 0;
	// �ж��Ƿ��߳�������URL������
	if(nThreadNumber > nURLLine )
	{
		AfxMessageBox(_T("����URL���������̷߳����\r\n\r\n���ǽ��߳������޸�Ϊ1��"));
		// ���߳����޸�Ϊ1
		pDlg->GetDlgItem(IDC_THREADNUMBER)->SetWindowText(_T("1"));
		nThreadNumber = 1;
	}else
	{ 
		if( nThreadNumber == 1)
			goto BeginThread; // 1���߳�,��û��Ҫ����Щ������

		// ÿ���̶߳�Ӧ���ַ�������
		nThreadSTRNumber = ( nURLLine/nThreadNumber)+2 ;// +1��ֹ���ֲ�����  ; +1�����жϿհ׽���

		// ������ά�ռ�
		pThreadURL = new char** [nThreadNumber];
		for ( int i = 0; i< nThreadNumber; ++i)
		{
			pThreadURL[i] = new char*[nThreadSTRNumber];
			for ( int j = 0; j<nThreadSTRNumber; ++j)
			{
				pThreadURL[i][j] = new char[MAX_PATH];
			}
		}
		
		// ����ڴ�
		for ( int i = 0 ; i <nThreadNumber; ++i )
		{
			for ( int j = 0 ; j<nThreadSTRNumber; ++j )
			{
				for ( int k = 0 ; k<MAX_PATH; ++k)
					pThreadURL[i][j][k] = 0;
			}
		}


		int nThreadType = 0; // ��һ���̷߳����ַ���
		int nThreadSTRPos = 0; // �߳������ַ��������λ��
		// �ַ�������
		for ( int i = 0; i<nURLLine; ++i )
		{
			// �����ַ���
			strcpy(&pThreadURL[nThreadType][nThreadSTRPos][0],&pURL[i][0]);
			
			// ��ÿ���߳̿����ַ���
			if( ++nThreadType == nThreadNumber)
			{
				// �����̴߳�ͷ��ʼ
				nThreadType = 0;
				// ÿ���߳����ַ�����λ������
				++nThreadSTRPos; 
			}
		}

		// �ͷ��ڴ� ��ά����
		for ( int  i = 0 ; i<nURLLine; ++i ) 
			delete [] pURL[i];
		delete[] pURL;
		pURL = NULL;
	}
	
BeginThread:
	
	// ��ʼ��ʱ��

	pDlg->SetTimer(1000,1000,NULL);
	
	// ���� ѡ����̸߳��� ���д�������
	if ( nThreadNumber == 1 )
	{
		// ���߳���Ҫ��������������� ���浽ȫ�ֱ�����
		g_nThreadSTRNumber = nURLLine;

		HANDLE hThread;
		// �����߳�
		hThread = CreateThread(NULL,0,SubmitThreadProc,&pURL[0],0,NULL);
		// �ȴ��߳�
		WaitForSingleObject(hThread,INFINITE);
	}else
	{
		// ���߳���Ҫ��������������� ���浽ȫ�ֱ�����
		g_nThreadSTRNumber = nThreadSTRNumber;
		// ѭ�������߳�
		HANDLE hThreadArray[50];
		for ( int i = 0 ; i< nThreadNumber; ++i ) 
		{
			hThreadArray[i] = CreateThread(NULL,0,SubmitThreadProcs,&pThreadURL[i][0],0,NULL);
			
		}
		// �ȴ��߳�
		WaitForMultipleObjects(nThreadNumber,hThreadArray,TRUE,INFINITE);
	}
	
	// ��������ʱ��
	pDlg->KillTimer(1000);
	// ���һ�ε���
	pDlg->OnTimer(1000);
	// �޸�ִ��״̬
	pDlg->m_bIsRun = FALSE;


	// �ͷ��ڴ� ��ά����
	if ( pURL != NULL)
	{
		for ( int  i = 0 ; i<nURLLine; ++i ) 
			delete [] pURL[i];
		delete[] pURL;
	}
	// �ͷ��ڴ� ��ά����
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
	
	// ���������Ϣ
	pDlg->SendMessage((WM_USER+3),0,0);
	
	

	return 0;
}

// ��ʼ��URL����
LRESULT CSubmitURLDlg::InitListURLNumber( WPARAM wParam, LPARAM lParam )
{
	CString strText;
	strText.Format(_T("%d"),(UINT32)wParam);

	// �ж����
	if (static_cast<CButton*>(GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		// ��ʱ���µ�List
		m_wndListCtrl.SetItemText(0,3,strText);
		// ��������
		m_ArrayListNumber[0][2] = (UINT32)wParam;
	}
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		// ��ʱ���µ�List
		m_wndListCtrl.SetItemText(1,3,strText);
		// ��������
		m_ArrayListNumber[1][2] = (UINT32)wParam;
	}
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
		// ��ʱ���µ�List
		m_wndListCtrl.SetItemText(2,3,strText);
		// ��������
		m_ArrayListNumber[2][2] = (UINT32)wParam;
	}
	
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{
		// ��ʱ���µ�List
		m_wndListCtrl.SetItemText(3,3,strText);
		// ��������
		m_ArrayListNumber[3][2] = (UINT32)wParam;
	}
	
	if(static_cast<CButton*>(GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
		// ��ʱ���µ�List
		m_wndListCtrl.SetItemText(4,3,strText);
		// ��������
		m_ArrayListNumber[4][2] = (UINT32)wParam;
	}

	return TRUE;
}

// �޸�URL����
LRESULT CSubmitURLDlg::ChangeListURLNumber( WPARAM wParam, LPARAM lParam )
{
	if( wParam>4||wParam<0 )
		return TRUE; // ������

	// ����δ����
	m_ArrayListNumber[wParam][2] = m_ArrayListNumber[wParam][2]-1;

	if ( lParam == 1)
	{ // ���ͳɹ�
		m_ArrayListNumber[wParam][0] = m_ArrayListNumber[wParam][0]+1;
	}else
	{ // ����ʧ��
		m_ArrayListNumber[wParam][1] = m_ArrayListNumber[wParam][1]+1;
	}
	
	return TRUE;
}
LRESULT CSubmitURLDlg::ChangeListURLNumberThreadError(WPARAM wParam, LPARAM lParam)// �����߳�ʧ��
{
	if (static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		// ����δ����
		m_ArrayListNumber[0][2] = m_ArrayListNumber[0][2]-1;
		// ����ʧ��
		m_ArrayListNumber[0][1] = m_ArrayListNumber[0][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		// ����δ����
		m_ArrayListNumber[1][2] = m_ArrayListNumber[1][2]-1;
		// ����ʧ��
		m_ArrayListNumber[1][1] = m_ArrayListNumber[1][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
		// ����δ����
		m_ArrayListNumber[2][2] = m_ArrayListNumber[2][2]-1;
		// ����ʧ��
		m_ArrayListNumber[2][1] = m_ArrayListNumber[2][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{
		// ����δ����
		m_ArrayListNumber[3][2] = m_ArrayListNumber[3][2]-1;
		// ����ʧ��
		m_ArrayListNumber[3][1] = m_ArrayListNumber[3][1]+1;
	}


	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
		// ����δ����
		m_ArrayListNumber[4][2] = m_ArrayListNumber[4][2]-1;
		// ����ʧ��
		m_ArrayListNumber[4][1] = m_ArrayListNumber[4][1]+1;
	}
    return 0;
}

LRESULT CSubmitURLDlg::ChangStat(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_END)->EnableWindow(FALSE);
	GetDlgItem(IDC_BEGIN)->EnableWindow(TRUE);
	
	// �������п��Կؿؼ�
	ControlStateControl(TRUE);

	return TRUE;
}

#ifdef _DEBUG
void PrintLog( char *pstrLog )
{
    HANDLE hAndle = GetStdHandle( STD_OUTPUT_HANDLE );// ��ȡ���
    WriteConsoleA(hAndle, pstrLog , strlen(pstrLog ),NULL,NULL); // �������
}
#endif // _DEBUG


// �ύ�߳�,�����߳�(�����߳�)
DWORD WINAPI SubmitThreadProc(LPVOID pParam)
{
	char** ppArray = (char**)pParam;
    
    CURL *curl;  
    curl = curl_easy_init();

    if ( !curl )  
    { // ��ʼ��CURLʧ��
        for ( int i = 0; i<g_nThreadSTRNumber; ++i )
            pDlg->SendMessage((WM_USER+4),0,0); // ѭ�����ʹ�����Ϣ
        return -1;
    }

    // ���ó�ʱ
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);

	for ( int i = 0; i<g_nThreadSTRNumber; ++i )
	{
		// �ж��Ƿ���Ҫֹͣ
		if (pDlg->m_bIsRun == FALSE)
			goto endthread; // ֹͣ
		SearchEnginesMain(&ppArray[i][0],curl);
	
	}
    // ����
endthread:
    // �ͷ�CURL
    curl_easy_cleanup( curl );  

	return 0;
}

// �ύ�߳�,�����߳�(����߳�)
DWORD WINAPI SubmitThreadProcs(LPVOID pParam)
{
	char** ppArray = (char**)pParam;

	CURL *curl;  
	curl = curl_easy_init();

	if ( !curl )  
	{ // ��ʼ��CURLʧ��
		for ( int i = 0; strcmp(&ppArray[i][0],"") != 0; ++i )
            pDlg->SendMessage((WM_USER+4),0,0); // ѭ�����ʹ�����Ϣ
		
		return -1;
	}
    // ���ó�ʱ
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
	for ( int i = 0; strcmp(&ppArray[i][0],"") != 0; ++i )
	{
		// �ж��Ƿ���Ҫֹͣ
		if (pDlg->m_bIsRun == FALSE)
			goto endthread; // ֹͣ
		SearchEnginesMain(&ppArray[i][0],curl);
	}
// ����
endthread:
    // �ͷ�CURL
    curl_easy_cleanup( curl );  

	return 0;
}

// ���Check ��ť
void CSubmitURLDlg::OnCheckButton( UINT nID )
{
	// ������Ϣ, ȡ��ѡ��
	if( nID != IDC_CHECK1 )
	{
		GetDlgItem(nID)->SendMessage(BM_SETCHECK,BST_UNCHECKED);
		if( AfxMessageBox(_T("����������������ڽ��ű�����\r\n\r\n�Ƿ�������ҳ�鿴���°�?"),MB_YESNO) == IDYES )
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
				// �����ı�
				m_wndListCtrl.SetItemText(i,j+1,strText);
			}
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void SearchEnginesMain( char* pURL, CURL* curl )
{

	// �ж��Ƿ���Ҫֹͣ
	if (pDlg->m_bIsRun == FALSE)
		return; // ֹͣ
	if (static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK1))->GetCheck()==1)
	{
		SearchEnginesBaidu(pURL,curl);
	}


	// �ж��Ƿ���Ҫֹͣ
	if (pDlg->m_bIsRun == FALSE)
		return; // ֹͣ
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK2))->GetCheck()==1)
	{
		SearchEngines360(pURL,curl);
	}
	

	// �ж��Ƿ���Ҫֹͣ
	if (pDlg->m_bIsRun == FALSE)
		return; // ֹͣ
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK3))->GetCheck()==1)
	{
	}

	
	// �ж��Ƿ���Ҫֹͣ
	if (pDlg->m_bIsRun == FALSE)
		return; // ֹͣ
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK4))->GetCheck()==1)
	{

	}
	

	// �ж��Ƿ���Ҫֹͣ
	if (pDlg->m_bIsRun == FALSE)
		return; // ֹͣ
	if(static_cast<CButton*>(pDlg->GetDlgItem(IDC_CHECK5))->GetCheck()==1)
	{
	}
}
void SearchEnginesBaidu(char* pURL, CURL* curl )
{
    
    // ����URL
    curl_easy_setopt( curl, CURLOPT_URL, &g_urllink[0][0]);  
    //���÷���htmlԴ��ķ�ʽ
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_baidu);

    // ָ��post����
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, pURL);    

    // ��ʼ����
    CURLcode res = curl_easy_perform( curl );  
    
    // �жϷ���ֵ �Ƿ�������� ����3��
    for ( int i = 0 ; res!=CURLE_OK && i!=3; ++i)
        res = curl_easy_perform( curl ); // ��ʼ����

    // �ж���ѭ�������Ƿ�Ϊ CURLE_OK
    if (res!=CURLE_OK)
    { 
        pDlg->SendMessage((WM_USER+2),0,0); // ����ʧ��
    }else
    {
        // ��ȡ������
        long http_code = 0;
        curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&http_code);
        if(http_code != 200) // �ο�ReadMe.txt
        { // ����ʧ��
            pDlg->SendMessage((WM_USER+2),0,0); // ����ʧ��
        }else
        { // ���ͳɹ�
            pDlg->SendMessage((WM_USER+2),0,1); // ���ͳɹ�
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
// �ٶ� �ύURL��������
size_t write_data_baidu(void *ptr, size_t size, size_t nmemb, void *stream)  
{
	
	return size*nmemb;
}

void CSubmitURLDlg::OnClose()
{
    // ѯ���Ƿ��˳�����
    if ( AfxMessageBox(_T("���Ƿ�Ҫ�˳�����?"),MB_YESNO) == IDYES)
    { // �˳�
        CDialog::OnClose();
    }
}

LRESULT CSubmitURLDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    
    // ���ģ��
    SoftwareADProc(message,wParam,lParam);

    return CDialog::WindowProc(message, wParam, lParam);
}

void CSubmitURLDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // ������������
    EnableSendOnLineData();
}


INT_PTR CSubmitURLDlg::DoModal()
{
    // ��ʼ����Ϣ
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
    // TODO: �ڴ����ר�ô����/����û���

    //CDialog::WinHelp(dwData, nCmd);
}

// ������ð�ť
void CSubmitURLDlg::OnBnClickedSetting()
{
    CSettingDialog Dlg;
    Dlg.DoModal();
}

// �����ֽ��ַ���ת��Ϊ���ֽ��ַ���
char* strWToM(CString str)
{
    // ת��������Ҫ���ڴ��С
    int nstrLen = WideCharToMultiByte(CP_ACP,0,str,wcslen(str),NULL,0,NULL,NULL);
    // �����ڴ�ռ�
    char* pmstr = (char*) malloc( sizeof(char)*(nstrLen+1) );
    // ����ڴ�
    ZeroMemory(pmstr,sizeof(char)*(nstrLen+1));
    // ת���ַ���
    nstrLen = WideCharToMultiByte(CP_ACP,0,str,wcslen(str),pmstr,nstrLen,NULL,NULL);
    // ��ӽ�β
    pmstr[nstrLen] = 0;
    return pmstr;
}

// ����URL����
void CSubmitURLDlg::URLLink( )
{
    
    // ��ȡ����
    CString strDomainName = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    // ��ȡKey
    CString strKey1 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_1"),_T(""));
    CString strKey2 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_2"),_T(""));
    CString strKey3 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_3"),_T(""));
    CString strKey4 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_4"),_T(""));
    CString strKey5 = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_5"),_T(""));

    // ת�����Key
    char* pstrDomainName = NULL;
    char* pstrKey1 = NULL;
    char* pstrKey2 = NULL;
    char* pstrKey3 = NULL;
    char* pstrKey4 = NULL;
    char* pstrKey5 = NULL;

    // ת��Key�ַ���

    if ( wcscmp(strDomainName,_T("")) !=0 )
        pstrDomainName = strWToM(strDomainName);// ������
    
    if ( wcscmp(strKey1,_T("")) !=0 )
        pstrKey1 = strWToM(strKey1);// ������
    
    if ( wcscmp(strKey2,_T("")) !=0 )
        pstrKey2 = strWToM(strKey2);// ������
    
    if ( wcscmp(strKey3,_T("")) !=0 )
        pstrKey3 = strWToM(strKey3);// ������
    
    if ( wcscmp(strKey4,_T("")) !=0 )
        pstrKey4 = strWToM(strKey4);// ������

    if ( wcscmp(strKey5,_T("")) !=0 )
        pstrKey5 = strWToM(strKey5);// ������
    
    
        

    // ��ȡ��ע�������
    int nIsOriginal = AfxGetApp()->GetProfileInt(_T("SETTING"),_T("ORIGINAL"),0);
    // ����URL�ж�
    if( nIsOriginal == 0 )
    { // ��ԭ������
        // ת�� �ٶ�
        sprintf_s(&g_urllink[0][0],MAX_PATH,"http://data.zz.baidu.com/urls?site=%s&token=%s",pstrDomainName,pstrKey1);
    }else
    { // ԭ������
        // ת�� �ٶ�
        sprintf_s(&g_urllink[0][0],MAX_PATH,"http://data.zz.baidu.com/urls?site=%s&token=%s&type=original",pstrDomainName,pstrKey1);
    }

    // �ͷ��ڴ�
    if ( wcscmp(strDomainName,_T("")) !=0 )
        free(pstrDomainName); // ������
    
    if ( wcscmp(strKey1,_T("")) !=0 )
        free(pstrKey1);// ������
    
    if ( wcscmp(strKey2,_T("")) !=0 )
        free(pstrKey2);// ������
    
    if ( wcscmp(strKey3,_T("")) !=0 )
        free(pstrKey3);// ������
    
    if ( wcscmp(strKey4,_T("")) !=0 )
        free(pstrKey4);// ������
    
    if ( wcscmp(strKey5,_T("")) !=0 )
        free(pstrKey5);// ������

}
