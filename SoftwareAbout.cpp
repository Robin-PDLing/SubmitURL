// SoftwareAbout.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SoftwareAbout.h"

#define URLPAGE  _T("http://www.pdling.com/thread-216-1-1.html")

// CSoftwareAbout �Ի���

IMPLEMENT_DYNAMIC(CSoftwareAbout, CDialog)

CSoftwareAbout::CSoftwareAbout(CWnd* pParent /*=NULL*/)
	: CDialog(CSoftwareAbout::IDD, pParent)
    , m_strVersion(_T(""))
{

}

CSoftwareAbout::~CSoftwareAbout()
{
}

void CSoftwareAbout::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, TEXT_VERSION_INFO, m_strVersion);
}


BEGIN_MESSAGE_MAP(CSoftwareAbout, CDialog)
	ON_BN_CLICKED(IDC_ALLSOFTWARE, &CSoftwareAbout::OnBnClickedAllsoftware)
	ON_BN_CLICKED(IDC_UPDATE, &CSoftwareAbout::OnBnClickedUpdate)
	ON_BN_CLICKED(IDC_CODEDOWNLOAD, &CSoftwareAbout::OnBnClickedCodeDownload)
	ON_BN_CLICKED(IDC_FEEDBACK, &CSoftwareAbout::OnBnClickedFeedback)
END_MESSAGE_MAP()


// CSoftwareAbout ��Ϣ�������

// ����ĸ������
void CSoftwareAbout::OnBnClickedAllsoftware()
{
	ShellExecute(NULL, _T("open"), _T("http://www.pdling.com/forum-46-1.html"), NULL, NULL, SW_SHOW);
}

// �������
void CSoftwareAbout::OnBnClickedUpdate()
{
	if( AfxMessageBox(_T("�Զ�������������Ŭ��������\r\n\r\n�Ƿ�������ҳ�鿴���°�?"),MB_YESNO) == IDYES )
	{
		ShellExecute(NULL, _T("open"),URLPAGE, NULL, NULL, SW_SHOW);
	}
	
}
// ��������
void CSoftwareAbout::OnBnClickedCodeDownload()
{
	ShellExecute(NULL, _T("open"),URLPAGE, NULL, NULL, SW_SHOW);
}

// �������
void CSoftwareAbout::OnBnClickedFeedback()
{
	CString strFeedbackURL;
	strFeedbackURL.Format(_T("https://jinshuju.net/f/FbbQH4?x_field_1=%d-%s"),___SOFTWARE_ID,m_strVersion);
	ShellExecute(NULL, _T("open"), strFeedbackURL, NULL, NULL, SW_SHOW);
}

BOOL CSoftwareAbout::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ���ð汾
    m_strVersion.LoadString(AfxGetApp()->m_hInstance,IDS_VERSION_INFO);
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}
