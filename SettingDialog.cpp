// SettingDialog.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SubmitURL.h"
#include "SettingDialog.h"
#include <curl/curl.h>
#include "cJSON.h"

// �ٶ� URL�ύKey���
size_t write_key_baidu(void *ptr, size_t size, size_t nmemb, void *stream);

HANDLE g_hAndleEvent;

IMPLEMENT_DYNAMIC(CSettingDialog, CDialog)

CSettingDialog::CSettingDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDialog::IDD, pParent)
{

}

CSettingDialog::~CSettingDialog()
{
}

void CSettingDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSettingDialog, CDialog)
    ON_BN_CLICKED(IDC_ORIGINAL, &CSettingDialog::OnBnClickedOriginal)
    ON_CONTROL_RANGE(BN_CLICKED,IDC_KEY_1,IDC_KEY_5,CheckKey)
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CSettingDialog ��Ϣ�������


BOOL CSettingDialog::PreTranslateMessage(MSG* pMsg)
{
    switch( pMsg->message )
    {
    case WM_KEYDOWN:
        {
            if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN )
            {
                // �ƶ����뽹��
                SetFocus();
                // ����
                return TRUE;
            }
        }
        break;
    case WM_MOUSEMOVE:
        {
            m_wndTipCtrl.RelayEvent(pMsg);
        }
        break;
    }
   
   


    return CDialog::PreTranslateMessage(pMsg);
}

BOOL CSettingDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_wndTipCtrl.Create(this);
    m_wndTipCtrl.AddTool(GetDlgItem(IDC_DOMAINNAME),_T("��д�ύ������\r\n��ʽ:www.pdling.com"));
    m_wndTipCtrl.SetMaxTipWidth(130);
    m_wndTipCtrl.Activate(TRUE); //�����Ƿ�������ʾ
    CString strText;
    // ��ע����ȡ����
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    // ���õ�EDIT
    GetDlgItem(IDC_DOMAINNAME)->SetWindowText(strText);


    // �Ƿ�ԭ��
    
    static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->SetCheck(AfxGetApp()->GetProfileInt(_T("SETTING"),_T("ORIGINAL"),0));
   
    // ��ע����л�ȡKEY
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_1"),_T(""));
    GetDlgItem(IDC_EDIT_KEY_1)->SetWindowText(strText);

    
    
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_2"),_T(""));
    GetDlgItem(IDC_EDIT_KEY_2)->SetWindowText(strText);
    
    
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_3"),_T(""));
    GetDlgItem(IDC_EDIT_KEY_3)->SetWindowText(strText);

    
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_4"),_T(""));
    GetDlgItem(IDC_EDIT_KEY_4)->SetWindowText(strText);
    
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("KEY_5"),_T(""));
    GetDlgItem(IDC_EDIT_KEY_5)->SetWindowText(strText);
    
    // �����ź�
    g_hAndleEvent = CreateEvent(NULL,TRUE,FALSE,NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void CSettingDialog::OnOK()
{
    // ��ȡ�����ؼ�ֵ
    CString strText;
    // ����
    GetDlgItem(IDC_DOMAINNAME)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("DOMAINNAME"),strText);
    // �Ƿ�ԭ��
    AfxGetApp()->WriteProfileInt(_T("SETTING"),_T("ORIGINAL"),static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->GetCheck());

    // ��Keyд��ע���
    GetDlgItem(IDC_EDIT_KEY_1)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("KEY_1"),strText);
    
    GetDlgItem(IDC_EDIT_KEY_2)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("KEY_2"),strText);

    GetDlgItem(IDC_EDIT_KEY_3)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("KEY_3"),strText);

    GetDlgItem(IDC_EDIT_KEY_4)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("KEY_4"),strText);

    GetDlgItem(IDC_EDIT_KEY_5)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("KEY_5"),strText);



    CDialog::OnOK();
}

void CSettingDialog::OnCancel()
{
    // ѯ���Ƿ񱣴��޸ĵ�����
    if( AfxMessageBox(_T("�Ƿ񱣴沢�ر�?"),MB_YESNO) == IDYES )
        OnOK(); // ����
    

    CDialog::OnCancel();
}

void CSettingDialog::OnBnClickedOriginal()
{
    if(static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->GetCheck() == 1)
    {
        if( AfxMessageBox(_T("��ѡ��ȷ�����ύ��Ϊԭ��!\r\n\r\n�Ƿ��ύ���������վ�����޴��Ӱ��!"),MB_OKCANCEL) != IDOK )
            static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->SetCheck(0);
    }
}

void CSettingDialog::CheckKey(UINT nID) // ���Key�Ƿ���ȷ
{
    switch(nID)
    {
    case IDC_KEY_1:
        CheckKeyBaidu( );
        break;
    //case IDC_KEY_2:
    //    ;
    //    break;
    //case IDC_KEY_3:
    //    ;
    //    break;
    //case IDC_KEY_4:
    //    ;
    //    break;
    //case IDC_KEY_5:
    //    ;
    //    break;
    default:
        {
            if( AfxMessageBox(_T("����������������ڽ��ű�����\r\n\r\n�Ƿ�������ҳ�鿴���°�?"),MB_YESNO) == IDYES )
            {
                ShellExecute(NULL, _T("open"),_T("http://www.pdling.com/thread-216-1-1.html"), NULL, NULL, SW_SHOW);
            }
        }
        break;
    }
}
// �����ֽ��ַ���ת��Ϊ���ֽ��ַ���
char* WToM(CString str)
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

void CSettingDialog::CheckKeyBaidu() // ���ٶ�
{
    
    
    CURL *curl;  
    curl = curl_easy_init();
    if ( curl == NULL)
    {
        AfxMessageBox(_T("CURL ����ʧ��!"));
        return;
    }
   
    // �ڶԻ����л�ȡ
    // ��ȡ����
    CString strDomainName;
    GetDlgItem(IDC_DOMAINNAME)->GetWindowText(strDomainName);
    // ��ȡ�ٶȵ�Key
    CString strKey_1;
    GetDlgItem(IDC_EDIT_KEY_1)->GetWindowText(strKey_1);

    // ת���ַ���
    char* pstrDomainName = WToM(strDomainName);
    char* pstrKey_1 = WToM(strKey_1);
    
    // ����URL
    char szURL[1024] = {0};
    sprintf_s(szURL,1024,"http://data.zz.baidu.com/urls?site=%s&token=%s",pstrDomainName,pstrKey_1);

    // ����URL
    curl_easy_setopt( curl, CURLOPT_URL, szURL );  

    // ���ó�ʱ
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

    //���÷���htmlԴ��ķ�ʽ
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_key_baidu);
    
    // ָ��post����
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"www.pdling.com" );    

    // ��ʼ����
    CURLcode res = curl_easy_perform( curl );  

    // �жϷ���ֵ �Ƿ�������� ����3��
    for ( int i = 0 ; res!=CURLE_OK && i!=3; ++i)
        res = curl_easy_perform( curl ); // ��ʼ����

    if (res!=CURLE_OK)
    {
        AfxMessageBox(_T("�������!"));
        SetEvent(g_hAndleEvent); // �����ź�
    }

    // �ȴ��ź�
    WaitForSingleObject(g_hAndleEvent,INFINITE);

    // �ͷ�CURL
    curl_easy_cleanup( curl );  

    // �ͷ��ڴ�
    free( pstrDomainName );
    free( pstrKey_1 );
}

// �ٶ� URL�ύKey���
size_t write_key_baidu(void *ptr, size_t size, size_t nmemb, void *stream)  
{
   
    // �������ԭ��
    CString strText;

    // �ж��Ƿ��д����item
    if( strstr((char*)ptr,"error") == 0)
    {
        AfxMessageBox(_T("У��ɹ�!"));
        SetEvent(g_hAndleEvent); // �����ź�
        return size*nmemb; 
    }

    try
    {
        // ���� json ����
        cJSON * pJsonRoot = cJSON_Parse((char*)ptr); 
        // �ж�json �Ƿ񴴽�ʧ��
        if(NULL == pJsonRoot)
        {
            // �׳��쳣0
            throw 1;
            return NULL;
        }
        

        // ��������
        cJSON* pJSONvalue = cJSON_GetObjectItem(pJsonRoot, "error");
        // ��ȡ����
        int nErrorNumber = pJSONvalue->valueint;
        if ( nErrorNumber == 400)
        {
            // ��ȡ�ַ���
            pJSONvalue = cJSON_GetObjectItem(pJsonRoot, "message");
            char* pstrErrorMessage = pJSONvalue->valuestring;
            // �ж��ַ�����������
            if( strcmp( pstrErrorMessage, "site error" ) == 0)
            {
                throw 0x10;
            }else if(strcmp(pstrErrorMessage , "empty content") == 0)
            {
                throw 0x11;
            }else if( strcmp(pstrErrorMessage, "only 2000 urls are allowed once") == 0 )
            {
                throw 0x12;
            }
            else if ( strcmp(pstrErrorMessage, "over quota") == 0)
            {
                throw 0x13;
            }
            else
            {
                throw 0;
            }
        }else if(nErrorNumber == 401 ) 
        {
            throw 0x20;
        }else if( nErrorNumber == 404 )
        {
            throw 0x30;
        }else if( nErrorNumber == 500 )
        {
            throw 0x40;
        }else
        {
            throw 0; // δ֪����
        }

        // �ͷ�json
        cJSON_Delete(pJsonRoot);
    }
    catch(int e) // �쳣����
    {
        
        switch(e)
        {
        case 0x00:
            strText = _T("δ֪����");
            break;

        case 0x10:
            strText = _T("վ��δ��վ��ƽ̨��֤");
            break;

        case 0x11:
            strText = _T("post����Ϊ��");
            break;

        case 0x12:
            strText = _T("ÿ�����ֻ���ύ2000������");
            break;

        case 0x13:
            strText = _T("����ÿ������ˣ����������ύ������Ч��");
            break;

        case 0x20:
            strText = _T("token����");
            break;

        case 0x30:
            strText = _T("�ӿڵ�ַ��д����");
            break;

        case 0x40:
            strText = _T("������żȻ�쳣��ͨ�����Ծͻ�ɹ�");
            break;

        default:
            strText = _T("δ֪����");
            break;
        }
    }
    AfxMessageBox(strText);
    
    SetEvent(g_hAndleEvent); // �����ź�
    return size*nmemb;
}
void CSettingDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // �ر��¼�
    CloseHandle(g_hAndleEvent);
  
}
