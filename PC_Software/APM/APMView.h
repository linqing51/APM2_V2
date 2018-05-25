
// APMView.h : CAPMView 类的接口
//

#pragma once
#include "afxcmn.h"
#include "MainCtrlDlg.h"
#include "VisionCtrlDlg.h"
#include "MotionDlg.h"

class CAPMView : public CBCGPFormView
{
protected: // 仅从序列化创建
	CAPMView();
	DECLARE_DYNCREATE(CAPMView)

// 特性
public:
	enum{ IDD = IDD_CONTROLVIEW};
	CAPMDoc* GetDocument() const;
	CMainCtrlDlg				m_MainCtrlDlg;
	CVisionCtrlDlg				m_VisionCtrlDlg;
	CMotionDlg					m_MotionDlg;
// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CAPMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数Nozzle Check;Nozzle Clean;Try Shot;Loading Area;Z Safe;Product Position;P
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);
	CBCGPTabWnd m_wndTab;
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // APMView.cpp 中的调试版本
inline CAPMDoc* CAPMView::GetDocument() const
   { return reinterpret_cast<CAPMDoc*>(m_pDocument); }
#endif

