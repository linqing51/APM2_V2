// ImageView.cpp : ʵ���ļ�
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


// CImageView ���

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


// CImageView ��Ϣ�������

void CImageView::OnInitialUpdate()
{
	CBCGPFormView ::OnInitialUpdate();
	CRect ClientRc; GetClientRect(ClientRc);
	// TODO: �ڴ����ר�ô����/����û���
}

void CImageView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	// TODO: �ڴ˴������Ϣ����������
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






