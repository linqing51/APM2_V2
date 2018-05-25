#pragma once


// CHomeSettingDlg 对话框

class CHomeSettingDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CHomeSettingDlg)

public:
	CHomeSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CHomeSettingDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_HOMINGSETTING };
	AxSp			m_SpeedCfg;//0--5:手动速度；6-11：自动速度；12-17:运行加速度；

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnKillFocusEditVel(UINT idCtl);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
};
