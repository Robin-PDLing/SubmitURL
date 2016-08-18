
// SubmitURL.cpp : 定义应用程序的类行为。
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


// CSubmitURLApp 构造

CSubmitURLApp::CSubmitURLApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSubmitURLApp 对象

CSubmitURLApp theApp;


// CSubmitURLApp 初始化

BOOL CSubmitURLApp::InitInstance()
{
    // 初始化Socket库
    WSADATA wsa = {0}; // 用于保存Socket的信息
    WSAStartup(MAKEWORD(2,2),&wsa);//函数初始化Socket库(Socket库的版本号 [Out]Socket的信息)


#ifdef _DEBUG
    AllocConsole( ); //显示
#endif // _DEBUG



	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("PDLing"));
    
	CSubmitURLDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}
