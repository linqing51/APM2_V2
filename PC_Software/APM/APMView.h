
// APMView.h : CAPMView ��Ľӿ�
//

#pragma once
#include "afxcmn.h"
#include "MainCtrlDlg.h"
#include "VisionCtrlDlg.h"
#include "MotionDlg.h"

class CAPMView : public CBCGPFormView
{
protected: // �������л�����
	CAPMView();
	DECLARE_DYNCREATE(CAPMView)

// ����
public:
	enum{ IDD = IDD_CONTROLVIEW};
	CAPMDoc* GetDocument() const;
	CMainCtrlDlg				m_MainCtrlDlg;
	CVisionCtrlDlg				m_VisionCtrlDlg;
	CMotionDlg					m_MotionDlg;
// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CAPMView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��Nozzle Check;Nozzle Clean;Try Shot;Loading Area;Z Safe;Product Position;P
protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange* pDX);
	CBCGPTabWnd m_wndTab;
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // APMView.cpp �еĵ��԰汾
inline CAPMDoc* CAPMView::GetDocument() const
   { return reinterpret_cast<CAPMDoc*>(m_pDocument); }
#endif

