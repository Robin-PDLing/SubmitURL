
// SubmitURL.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSubmitURLApp:
// �йش����ʵ�֣������ SubmitURL.cpp
//

class CSubmitURLApp : public CWinAppEx
{
public:
	CSubmitURLApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSubmitURLApp theApp;