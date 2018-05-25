// ImageView.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "MainFrm.h"
#include "ImageView.h"
// CImageView

IMPLEMENT_DYNCREATE(CImageView, CBCGPFormView )

CImageView::CImageView()
: CBCGPFormView(CImageView::IDD), m_nFPS(0.0)
{
	EnableVisualManagerStyle();
	EnableLayout();
	m_fRate         =1;
	m_fZoom			= 1;
}

CImageView::~CImageView()
{
}

void CImageView::DoDataExchange(CDataExchange* pDX)
{
	CBCGPFormView ::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageView, CBCGPFormView )
	ON_WM_TIMER()
	ON_WM_SIZE()	
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CImageView 诊断

#ifdef _DEBUG
void CImageView::AssertValid() const
{
	CBCGPFormView ::AssertValid();
}

#ifndef _WIN32_WCE
void CImageView::Dump(CDumpContext& dc) const
{
	CBCGPFormView ::Dump(dc);
}
#endif
#endif //_DEBUG


// CImageView 消息处理程序

void CImageView::OnInitialUpdate()
{
	CBCGPFormView ::OnInitialUpdate();
	CRect ClientRc; GetClientRect(ClientRc);
	// TODO: 在此添加专用代码和/或调用基类
}

void CImageView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
	switch (nIDEvent)
	{
	case 1:

		break;
	case 2:

		break;
	}
	CBCGPFormView::OnTimer(nIDEvent);
}


void CImageView::OnSize(UINT nType, int cx, int cy)
{
	CBCGPFormView::OnSize(nType, cx, cy);
	CRect ClientRc; GetClientRect(ClientRc);

	// TODO: 在此处添加消息处理程序代码
}

void CImageView::DrawCross(CPoint CnterPoint,CDC* pDC, FLOAT fZoom)
{
	if(pDC==NULL)
		return ;
	CPen NewPen(PS_SOLID,0,RGB(255,0,0));
	CPen* oldPen=pDC->SelectObject(&NewPen);

	pDC->MoveTo(INT(CnterPoint.x*fZoom-10),INT(CnterPoint.y*fZoom));

	pDC->LineTo(INT(CnterPoint.x*fZoom+10),INT(CnterPoint.y*fZoom));

	pDC->MoveTo(INT(CnterPoint.x*fZoom),INT(CnterPoint.y*fZoom-10));

	pDC->LineTo(INT(CnterPoint.x*fZoom),INT(CnterPoint.y*fZoom+10));

	pDC->SelectObject(oldPen);
	NewPen.DeleteObject();
	return ;
}






