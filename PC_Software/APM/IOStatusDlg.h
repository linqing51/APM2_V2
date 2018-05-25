#pragma once
#include "IOControl.h"
#include "Label.h"

// CIOStatusDlg 对话框

class CIOStatusDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CIOStatusDlg)

public:
	CIOStatusDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIOStatusDlg();

	int		m_nStyle;
	HBITMAP m_hGreenBitmap;
	HBITMAP m_hRedBitmap;

// 对话框数据
	enum { IDD = IDD_DIALOG_IO };
public:
	CLabel			m_lStatus[24];
	CBCGPSwitchCtrl m_hCheck[32];
	CIOControl*		m_pIoHandler;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnAxisStsChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIochange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGaugeClick(WPARAM wParam, LPARAM lParam);
// 	afx_msg LRESULT OnUpdateDlg(WPARAM wParam, LPARAM lParam=0);
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
