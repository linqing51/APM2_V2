#pragma once
#include "Label.h"


// CMotionDlg 对话框

class CMotionDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CMotionDlg)

public:
	CMotionDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMotionDlg();
	UINT						m_nCurrentAxis;
	BOOL						m_bInching;
	float						m_fPace;

// 对话框数据
	enum { IDD = IDD_DIALOG_MOTION };
	CBCGPComboBox				m_cPaceChs;
	CBCGPComboBox				m_cVisionOrderChs;
	CBCGPComboBox				m_cPositionChs;

	int							m_nCurrentWP;
	UINT						m_nCurrentProduct[2];
	UINT						m_nCurrentVisionCheck;
	CLabel						m_lStatus[7];
	BYTE						bMove;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnChangeWP(UINT idCtl);
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedMove(UINT idCtl);
	afx_msg void OnBnDownMove(UINT idCtl);
	afx_msg void OnCbnSelchangePositionType();
	afx_msg void OnCbnSelchangeProductCombo(UINT idCtl);
	afx_msg void OnCbnSelchangeCombo_VisionCheckOrder();
	afx_msg void OnBnClickedButtonGetPosition(UINT idCtl);
	afx_msg void OnBnClickedCheckInching();
	afx_msg void OnCbnKillfocusComboPace();

public:
	afx_msg void OnBnClickedButtonMovStop();
	afx_msg void OnBnClickedButtonShotCalibrate();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
