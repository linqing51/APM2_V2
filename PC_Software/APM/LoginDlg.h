#pragma once


// CLoginDlg �Ի���

class CLoginDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDlg();
	UINT nType;
	UINT CheckPassword(BOOL bOld=FALSE);
// �Ի�������
	enum { IDD = IDD_DIALOG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonLogin();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual void OnCancel();
};
