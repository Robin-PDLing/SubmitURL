#pragma once


// CSoftwareAbout �Ի���

class CSoftwareAbout : public CDialog
{
	DECLARE_DYNAMIC(CSoftwareAbout)

public:
	CSoftwareAbout(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSoftwareAbout();

// �Ի�������
	enum { IDD = IDD_SOFTWAREABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAllsoftware();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedCodeDownload();
	afx_msg void OnBnClickedFeedback();
    CString m_strVersion;
    virtual BOOL OnInitDialog();
};
