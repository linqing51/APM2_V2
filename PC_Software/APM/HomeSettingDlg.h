#pragma once


// CHomeSettingDlg �Ի���

class CHomeSettingDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CHomeSettingDlg)

public:
	CHomeSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CHomeSettingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_HOMINGSETTING };
	AxSp			m_SpeedCfg;//0--5:�ֶ��ٶȣ�6-11���Զ��ٶȣ�12-17:���м��ٶȣ�

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnKillFocusEditVel(UINT idCtl);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
};
