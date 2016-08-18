#pragma once


// CSoftwareAbout 对话框

class CSoftwareAbout : public CDialog
{
	DECLARE_DYNAMIC(CSoftwareAbout)

public:
	CSoftwareAbout(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSoftwareAbout();

// 对话框数据
	enum { IDD = IDD_SOFTWAREABOUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAllsoftware();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedCodeDownload();
	afx_msg void OnBnClickedFeedback();
    CString m_strVersion;
    virtual BOOL OnInitDialog();
};
