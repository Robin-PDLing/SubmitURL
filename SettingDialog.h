#pragma once


// CSettingDialog 对话框

class CSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingDialog)

public:
	CSettingDialog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDialog();

// 对话框数据
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
    CToolTipCtrl m_wndTipCtrl;
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
    virtual void OnCancel();
    void CheckKey(UINT nID); // 监测Key是否正确
    void CheckKeyBaidu(); // 监测百度
public:
    afx_msg void OnBnClickedOriginal();
    afx_msg void OnDestroy();
};
