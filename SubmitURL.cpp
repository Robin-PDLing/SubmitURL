
// SubmitURL.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "SubmitURL.h"
#include "SubmitURLDlg.h"
#include <curl/curl.h>

#pragma comment(lib, "ws2_32.lib")  
#pragma comment ( lib, "winmm.lib" )  
#pragma comment ( lib, "wldap32.lib" )  

#ifdef _DEBUG
#pragma comment ( lib, "libcurld_imp" )
#else
#pragma comment ( lib, "libcurl.lib" )
#endif// _DEBUG

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSubmitURLApp

BEGIN_MESSAGE_MAP(CSubmitURLApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSubmitURLApp ����

CSubmitURLApp::CSubmitURLApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CSubmitURLApp ����

CSubmitURLApp theApp;


// CSubmitURLApp ��ʼ��

BOOL CSubmitURLApp::InitInstance()
{
    // ��ʼ��Socket��
    WSADATA wsa = {0}; // ���ڱ���Socket����Ϣ
    WSAStartup(MAKEWORD(2,2),&wsa);//������ʼ��Socket��(Socket��İ汾�� [Out]Socket����Ϣ)


#ifdef _DEBUG
    AllocConsole( ); //��ʾ
#endif // _DEBUG



	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("PDLing"));
    
	CSubmitURLDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
