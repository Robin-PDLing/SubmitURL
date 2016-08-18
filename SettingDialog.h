#pragma once


// CSettingDialog �Ի���

class CSettingDialog : public CDialog
{
	DECLARE_DYNAMIC(CSettingDialog)

public:
	CSettingDialog(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettingDialog();

// �Ի�������
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
    CToolTipCtrl m_wndTipCtrl;
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnInitDialog();
protected:
    virtual void OnOK();
    virtual void OnCancel();
    void CheckKey(UINT nID); // ���Key�Ƿ���ȷ
    void CheckKeyBaidu(); // ���ٶ�
public:
    afx_msg void OnBnClickedOriginal();
    afx_msg void OnDestroy();
};
