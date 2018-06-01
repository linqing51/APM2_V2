
// APMView.cpp : CAPMView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "APM.h"
#endif

#include "APMDoc.h"
#include "APMView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPMView

IMPLEMENT_DYNCREATE(CAPMView, CBCGPFormView)

BEGIN_MESSAGE_MAP(CAPMView, CBCGPFormView)
	ON_MESSAGE(WM_USER_UPDATEUI, &CAPMView::OnUpdateDlg)
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CAPMView ����/����

CAPMView::CAPMView(): CBCGPFormView(CAPMView::IDD)
{
	// TODO:  �ڴ˴���ӹ������
	EnableVisualManagerStyle(TRUE);
	EnableLayout();

}

CAPMView::~CAPMView()
{
}

void CAPMView::DoDataExchange(CDataExchange* pDX)
{
	// TODO:  �ڴ����ר�ô����/����û���

	CBCGPFormView::DoDataExchange(pDX);
//  	DDX_Control(pDX, IDC_TAB_CONTAINER, m_wndTab);
}

BOOL CAPMView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
// 	int xsize = ::GetSystemMetrics(SM_CXSCREEN);
// 	int ysize = ::GetSystemMetrics(SM_CYSCREEN);
// 	cs.x = 0;
// 	cs.y = 0;
// 	cs.cx = xsize;
// 	cs.cy = ysize;
	return CBCGPFormView::PreCreateWindow(cs);
}

// CAPMView ����

void CAPMView::OnDraw(CDC* /*pDC*/)
{
	CAPMDoc* pDoc = GetDocument();
	CWnd*p=GetDlgItem(IDC_STATIC_CONTAIN);
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CAPMView ���

#ifdef _DEBUG
void CAPMView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

void CAPMView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}

CAPMDoc* CAPMView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPMDoc)));
	return (CAPMDoc*)m_pDocument;
}
#endif //_DEBUG


// CAPMView ��Ϣ�������


void CAPMView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
// 	ResizeParentToFit();

	// TODO:  �ڴ����ר�ô����/����û���
	CRect ClientRc; GetClientRect(ClientRc);
	CRect rect1;    GetWindowRect(rect1);
	if (!m_wndTab.Create(CBCGPTabWnd::STYLE_3D, CRect(0, 0, 360, ClientRc.Height()), this, 1, CBCGPTabWnd::LOCATION_TOP))
	{
		TRACE0("\nδ�ܴ���tab\n");
		return ;      // δ�ܴ���
	}
	m_MainCtrlDlg.Create(CMainCtrlDlg::IDD, &m_wndTab);
	m_VisionCtrlDlg.Create(CVisionCtrlDlg::IDD, &m_wndTab);
	m_MotionDlg.Create(CMotionDlg::IDD, &m_wndTab);
// 	m_wndTab.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	m_wndTab.AddTab(&m_MainCtrlDlg, _T("������"), (UINT)-1, FALSE);
	m_wndTab.AddTab(&m_MotionDlg, _T("�˶�ƽ̨"), (UINT)-1, FALSE);
	m_wndTab.AddTab(&m_VisionCtrlDlg, _T("�Ӿ�"), (UINT)-1, FALSE);
	m_wndTab.ShowTab(1, FALSE);
	m_wndTab.ShowTab(2, FALSE);

	m_wndTab.RecalcLayout();
	m_wndTab.RedrawWindow();
	GetDlgItem(IDC_STATIC_CONTAIN)->MoveWindow(360, 0, ClientRc.Width() - 360, ClientRc.Height());

}

LRESULT CAPMView::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	LONG sts[2] = {0};
	CString str;
	UINT i(0), j(2);
	UINT nIndex = wParam>>4;
	int nId(0);
	BOOL bEnable = pFrame->GetUserType() && !pFrame->m_nCurrentRunMode ;
	switch (nIndex)
	{
	case 0://
// 		m_wndTab.EnableWindow(lParam);
// 		nId = m_wndTab.GetActiveTab();
		m_wndTab.ShowTab(1, bEnable);
		m_wndTab.ShowTab(2, bEnable);
		if (bEnable){
			::PostMessage(m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0, 1);
			::PostMessage(m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 0, 0);
		}
		break;
	case 1://
		break;
	case 2://MainCtrlDlg
		break;
	case 3://MotionDlg
		break;
	case 4://VisionCtrlDlg
		::PostMessage(m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0, lParam);

		break;
	case 5:
		break;
	default:
		break;
	}
	return 0;

}

void CAPMView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnMouseMove(nFlags, point);

	CBCGPFormView::OnMouseMove(nFlags, point);
}

