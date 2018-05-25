#pragma once
#include "afxwin.h"


// CProjectSettingDlg 对话框

class CProjectSettingDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CProjectSettingDlg)
	BYTE    bCommunicate;
public:
	CProjectSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CProjectSettingDlg();
// 对话框数据
	enum { IDD = IDD_DIALOG_PROJECTSETTING };
	int m_nCurrentPrjSel;

	UINT	nProductNum[2];
	UINT	nTotalVisionCheck;
	UINT	nCurrentVisionCheck;
	UINT	nTotalPoints;
	UINT	nCurrentPoint;
	float	fWeldingTimes;
	double	dProductSpacing[2];
	double	dPointSpacing[2];
	double	dClimbHeight[3];

	CBCGPComboBox  m_cbVisionType;
	CBCGPComboBox  m_cbRangingCfg;
	CBCGPSpinButtonCtrl				m_ctlPointSpin;
	CBCGPSpinButtonCtrl				m_ctlCheckSpin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	BOOL m_bOnCreatNew;

	DECLARE_MESSAGE_MAP()
public:
	CBCGPComboBox  m_cbPrjName;
	CBCGPButton m_btnCreate;
	CBCGPButton m_btnLoad;
	afx_msg void OnEnChangeEdit(UINT idCtl);
	afx_msg void OnCbnSelchangeComboVisionType();
	afx_msg void OnCbnSelchangeComboPrjname();
	afx_msg void OnKillFocusProductEdit(UINT idCtl);
	afx_msg void OnKillFocusPointsOffsetEdit(UINT idCtl);
	afx_msg void OnKillFocusOrderEdit(UINT idCtl);
	afx_msg void OnKillFocusOthersEdit(UINT idCtl);

	afx_msg void OnClose();
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedBtnCreate();
	afx_msg void OnBnClickedButtonCopy();
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);

	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	BOOL OnTransfer(BOOL bDownload);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonSetting();
};
