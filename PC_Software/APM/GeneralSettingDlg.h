#pragma once
#include "afxwin.h"


// CSettingDlg 对话框

class CGeneralSettingDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CGeneralSettingDlg)

public:
	CGeneralSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGeneralSettingDlg();
// 对话框数据
	enum { IDD = IDD_DIALOG_SETTING };
	CBCGPButton		btChecksafety[2];
	CBCGPButton		btSetAutoClean;
	CBCGPButton		btSetFixer;
	CBCGPButton		btSetRanging;
	UINT		nExposure[2];
	UINT		nNozzleLife;
	UINT		nBallAlarm;
	UINT		nIntensity[2];
	double		dOffset[3];
	CBCGPSpinButtonCtrl				m_ctlExposureSpin[2];
	CBCGPSpinButtonCtrl				m_ctlIntensitySpin[2];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnKillFocusExposureEdit(UINT idCtl);
	afx_msg void OnKillFocusIntensityEdit(UINT idCtl);
	afx_msg void OnKillFocusCCD_NozzleEdit(UINT idCtl);
	afx_msg void OnKillFocusOtherEdit(UINT idCtl);
	afx_msg void OnBnSelect(UINT idCtl);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit(UINT idCtl);
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
