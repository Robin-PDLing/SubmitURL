// SettingDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "SubmitURL.h"
#include "SettingDialog.h"
#include <curl/curl.h>
#include "cJSON.h"

// 百度 URL提交Key检测
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


// CSettingDialog 消息处理程序


BOOL CSettingDialog::PreTranslateMessage(MSG* pMsg)
{
    switch( pMsg->message )
    {
    case WM_KEYDOWN:
        {
            if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN )
            {
                // 移动输入焦点
                SetFocus();
                // 返回
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
    m_wndTipCtrl.AddTool(GetDlgItem(IDC_DOMAINNAME),_T("填写提交的域名\r\n格式:www.pdling.com"));
    m_wndTipCtrl.SetMaxTipWidth(130);
    m_wndTipCtrl.Activate(TRUE); //设置是否启用提示
    CString strText;
    // 从注册表读取数据
    strText = AfxGetApp()->GetProfileString(_T("SETTING"),_T("DOMAINNAME"),_T(""));
    // 设置到EDIT
    GetDlgItem(IDC_DOMAINNAME)->SetWindowText(strText);


    // 是否原创
    
    static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->SetCheck(AfxGetApp()->GetProfileInt(_T("SETTING"),_T("ORIGINAL"),0));
   
    // 从注册表中获取KEY
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
    
    // 创建信号
    g_hAndleEvent = CreateEvent(NULL,TRUE,FALSE,NULL);


    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void CSettingDialog::OnOK()
{
    // 获取各个控件值
    CString strText;
    // 域名
    GetDlgItem(IDC_DOMAINNAME)->GetWindowText(strText);
    AfxGetApp()->WriteProfileString(_T("SETTING"),_T("DOMAINNAME"),strText);
    // 是否原创
    AfxGetApp()->WriteProfileInt(_T("SETTING"),_T("ORIGINAL"),static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->GetCheck());

    // 将Key写入注册表
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
    // 询问是否保存修改的内容
    if( AfxMessageBox(_T("是否保存并关闭?"),MB_YESNO) == IDYES )
        OnOK(); // 保存
    

    CDialog::OnCancel();
}

void CSettingDialog::OnBnClickedOriginal()
{
    if(static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->GetCheck() == 1)
    {
        if( AfxMessageBox(_T("勾选请确认您提交的为原创!\r\n\r\n非法提交会给您的网站带来巨大的影响!"),MB_OKCANCEL) != IDOK )
            static_cast<CButton*>(GetDlgItem(IDC_ORIGINAL))->SetCheck(0);
    }
}

void CSettingDialog::CheckKey(UINT nID) // 监测Key是否正确
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
            if( AfxMessageBox(_T("该搜索引擎服务正在紧张编码中\r\n\r\n是否打开软件主页查看最新版?"),MB_YESNO) == IDYES )
            {
                ShellExecute(NULL, _T("open"),_T("http://www.pdling.com/thread-216-1-1.html"), NULL, NULL, SW_SHOW);
            }
        }
        break;
    }
}
// 将宽字节字符集转换为多字节字符集
char* WToM(CString str)
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

void CSettingDialog::CheckKeyBaidu() // 监测百度
{
    
    
    CURL *curl;  
    curl = curl_easy_init();
    if ( curl == NULL)
    {
        AfxMessageBox(_T("CURL 创建失败!"));
        return;
    }
   
    // 在对话框中获取
    // 获取域名
    CString strDomainName;
    GetDlgItem(IDC_DOMAINNAME)->GetWindowText(strDomainName);
    // 获取百度的Key
    CString strKey_1;
    GetDlgItem(IDC_EDIT_KEY_1)->GetWindowText(strKey_1);

    // 转换字符集
    char* pstrDomainName = WToM(strDomainName);
    char* pstrKey_1 = WToM(strKey_1);
    
    // 构造URL
    char szURL[1024] = {0};
    sprintf_s(szURL,1024,"http://data.zz.baidu.com/urls?site=%s&token=%s",pstrDomainName,pstrKey_1);

    // 设置URL
    curl_easy_setopt( curl, CURLOPT_URL, szURL );  

    // 设置超时
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

    //设置返回html源码的方式
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_key_baidu);
    
    // 指定post内容
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,"www.pdling.com" );    

    // 开始请求
    CURLcode res = curl_easy_perform( curl );  

    // 判断返回值 是否进行重试 重试3次
    for ( int i = 0 ; res!=CURLE_OK && i!=3; ++i)
        res = curl_easy_perform( curl ); // 开始请求

    if (res!=CURLE_OK)
    {
        AfxMessageBox(_T("网络故障!"));
        SetEvent(g_hAndleEvent); // 设置信号
    }

    // 等待信号
    WaitForSingleObject(g_hAndleEvent,INFINITE);

    // 释放CURL
    curl_easy_cleanup( curl );  

    // 释放内存
    free( pstrDomainName );
    free( pstrKey_1 );
}

// 百度 URL提交Key检测
size_t write_key_baidu(void *ptr, size_t size, size_t nmemb, void *stream)  
{
   
    // 保存错误原因
    CString strText;

    // 判断是否有错误的item
    if( strstr((char*)ptr,"error") == 0)
    {
        AfxMessageBox(_T("校验成功!"));
        SetEvent(g_hAndleEvent); // 设置信号
        return size*nmemb; 
    }

    try
    {
        // 创建 json 解析
        cJSON * pJsonRoot = cJSON_Parse((char*)ptr); 
        // 判断json 是否创建失败
        if(NULL == pJsonRoot)
        {
            // 抛出异常0
            throw 1;
            return NULL;
        }
        

        // 解析数据
        cJSON* pJSONvalue = cJSON_GetObjectItem(pJsonRoot, "error");
        // 获取数字
        int nErrorNumber = pJSONvalue->valueint;
        if ( nErrorNumber == 400)
        {
            // 获取字符串
            pJSONvalue = cJSON_GetObjectItem(pJsonRoot, "message");
            char* pstrErrorMessage = pJSONvalue->valuestring;
            // 判断字符串解析错误
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
            throw 0; // 未知错误
        }

        // 释放json
        cJSON_Delete(pJsonRoot);
    }
    catch(int e) // 异常处理
    {
        
        switch(e)
        {
        case 0x00:
            strText = _T("未知错误");
            break;

        case 0x10:
            strText = _T("站点未在站长平台验证");
            break;

        case 0x11:
            strText = _T("post内容为空");
            break;

        case 0x12:
            strText = _T("每次最多只能提交2000条链接");
            break;

        case 0x13:
            strText = _T("超过每日配额了，超配额后再提交都是无效的");
            break;

        case 0x20:
            strText = _T("token错误");
            break;

        case 0x30:
            strText = _T("接口地址填写错误");
            break;

        case 0x40:
            strText = _T("服务器偶然异常，通常重试就会成功");
            break;

        default:
            strText = _T("未知错误");
            break;
        }
    }
    AfxMessageBox(strText);
    
    SetEvent(g_hAndleEvent); // 设置信号
    return size*nmemb;
}
void CSettingDialog::OnDestroy()
{
    CDialog::OnDestroy();

    // 关闭事件
    CloseHandle(g_hAndleEvent);
  
}
