#pragma once
#include "Label.h"


// CMainCtrlDlg 对话框

class CMainCtrlDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CMainCtrlDlg)

public:
	CMainCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainCtrlDlg();
	
// 对话框数据
	enum { IDD = IDD_DIALOG_NEWMAIN, IDC_STATIC_STATE_P1 = 1300 };
	enum { NORMAL = 0, VISION_FAIL = 1, VISION_OK = 2, VISION_FAIL_LIMIT = 3, WELDING_OK = 4, WELDING_FAIL = 5 };//
	enum { FLASH_DISABLE = 0, FLASH_ENABLE = 1};
	enum { SET_STS = 0, ADD_STS = 1, ERASING_STS = 2 };
	int							m_nCurrentWP;
	UINT						m_nCurrentProduct[2];
	UINT						m_nCurrentPin;
	CLabel						m_lStatus[6];
    CLabel						m_nStatus[3];
	ULONG						m_lQuantity;
	std::vector<BYTE>				m_pStatusdata_left;
	std::vector<BYTE>				m_pStatusdata_right;
	std::vector<CLabel*>				m_plStatus;
protected:
	BOOL	CreateLabel(BYTE nCount);
	void	Typesetting(BYTE nRow, BYTE nCol);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg void OnBnChangeWP(UINT idCtl);
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);

	DECLARE_MESSAGE_MAP()
public:
	void	OnUpdateSts(BYTE nWp, UINT nNum=100,BYTE nSts=0,BYTE bMode=SET_STS);
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeProductCombo(UINT idCtl);
	afx_msg void OnBnClickedButtonWorkbenchload();
	afx_msg void OnCbnSelchangeComboJointOrder();
	afx_msg void OnBnClickedButtonGetposition(UINT idCtl);
	afx_msg void OnBnClickedButtonPushBall(UINT idCtl);
	afx_msg void OnBnClickedButtonNozzlePos(UINT idCtl);
	afx_msg void OnBnClickedButtonReset(UINT idCtl);
	afx_msg void OnBnClickedButtonMain(UINT idCtl);
	afx_msg void OnBnClickedCheckIdling();
	afx_msg void OnClickedButtonRedlight();
	BOOL bIdling;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL bAppointByMu;
	void SelchangeProduct(UINT idCtl,BOOL breset);
	afx_msg void OnBnClickedCheckAppoint();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonFixer();
	virtual void OnCancel();
	virtual void OnOK();
	UINT m_total;
	UINT m_cur;
	
};
