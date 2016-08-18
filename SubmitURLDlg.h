
// SubmitURLDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"


// CSubmitURLDlg �Ի���
class CSubmitURLDlg : public CDialog
{
// ����
public:
	CSubmitURLDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SubmitURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	// ����״̬
	void ControlStateControl( BOOL nBool );
	BOOL IsBegin( VOID );

	// ��ʼ��URL����
	void InitListURLNumber( );
	LRESULT InitListURLNumber( WPARAM wParam, LPARAM lParam );
	LRESULT ChangeListURLNumber( WPARAM wParam, LPARAM lParam );
	LRESULT ChangStat(WPARAM wParam, LPARAM lParam);
	LRESULT ChangeListURLNumberThreadError(WPARAM wParam, LPARAM lParam);// �����߳�ʧ��_
public:
	// ����List��������Ϣ
	UINT m_ArrayListNumber[5][3];
	// �Ƿ�����
	BOOL m_bIsRun;
    // ��������URL
    void URLLink();
	// ʵ��
protected:
	HICON m_hIcon;
    
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// �������߳�����
	UINT m_nThreadNumber;
	// �����б�
	CListCtrl m_wndListCtrl;
	// �ļ�·��
	CString m_strFilePath;

	afx_msg void OnBnClickedBegin();
	afx_msg void OnBnClickedEnd();
	afx_msg void OnBnClickedOpenfile();
	afx_msg void OnCheckButton( UINT nID );
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnClose();
protected:
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
    afx_msg void OnDestroy();
    virtual INT_PTR DoModal();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
    afx_msg void OnBnClickedSetting();
};
