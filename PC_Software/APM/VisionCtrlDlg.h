#pragma once


// CVisionCtrlDlg 对话框

class CVisionCtrlDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CVisionCtrlDlg)

public:
	CVisionCtrlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVisionCtrlDlg();
	BOOL	m_bUseMask;
	BYTE	m_nCurrentPin;
	BYTE	m_nCurrentSearch;
	BYTE	m_nCurrentTM;
	BYTE	m_nCurrentSS;

	BYTE	m_nTMContrast;
	BYTE	m_nSSStrength;
	UINT	m_nTMScore;
	UINT	m_nSSGap;

	// 对话框数据
	enum { IDD = IDD_DIALOG_VISIONCTRL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);
	void ShowContour(UINT nIndex,int nThreshold,UINT nGap=10);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboPinSearch(UINT idCtl);
	afx_msg void OnCbnSelchangeTMCombo(UINT idCtl);
	afx_msg void OnCbnSelchangeSSCombo(UINT idCtl);
	afx_msg void OnKillFocusTMEdit(UINT idCtl);
	afx_msg void OnKillFocusSSEdit(UINT idCtl);
	afx_msg void OnImageEditTool(UINT idCtl);
	afx_msg void OnBnClickedButtonMatch();
	afx_msg void OnBnClickedButtonFindtest();
	afx_msg void OnCbnSelchangeComboCirculardirect();
	BOOL m_bShowContour;
	BOOL m_bUseBinary;
	afx_msg void OnBnClickedCheckContour();
	afx_msg void OnBnClickedCheckBinary();
};
