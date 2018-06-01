
// APMView.cpp : CAPMView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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

// CAPMView 构造/析构

CAPMView::CAPMView(): CBCGPFormView(CAPMView::IDD)
{
	// TODO:  在此处添加构造代码
	EnableVisualManagerStyle(TRUE);
	EnableLayout();

}

CAPMView::~CAPMView()
{
}

void CAPMView::DoDataExchange(CDataExchange* pDX)
{
	// TODO:  在此添加专用代码和/或调用基类

	CBCGPFormView::DoDataExchange(pDX);
//  	DDX_Control(pDX, IDC_TAB_CONTAINER, m_wndTab);
}

BOOL CAPMView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
// 	int xsize = ::GetSystemMetrics(SM_CXSCREEN);
// 	int ysize = ::GetSystemMetrics(SM_CYSCREEN);
// 	cs.x = 0;
// 	cs.y = 0;
// 	cs.cx = xsize;
// 	cs.cy = ysize;
	return CBCGPFormView::PreCreateWindow(cs);
}

// CAPMView 绘制

void CAPMView::OnDraw(CDC* /*pDC*/)
{
	CAPMDoc* pDoc = GetDocument();
	CWnd*p=GetDlgItem(IDC_STATIC_CONTAIN);
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO:  在此处为本机数据添加绘制代码
}


// CAPMView 诊断

#ifdef _DEBUG
void CAPMView::AssertValid() const
{
	CBCGPFormView::AssertValid();
}

void CAPMView::Dump(CDumpContext& dc) const
{
	CBCGPFormView::Dump(dc);
}

CAPMDoc* CAPMView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAPMDoc)));
	return (CAPMDoc*)m_pDocument;
}
#endif //_DEBUG


// CAPMView 消息处理程序


void CAPMView::OnInitialUpdate()
{
	CBCGPFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
// 	ResizeParentToFit();

	// TODO:  在此添加专用代码和/或调用基类
	CRect ClientRc; GetClientRect(ClientRc);
	CRect rect1;    GetWindowRect(rect1);
	if (!m_wndTab.Create(CBCGPTabWnd::STYLE_3D, CRect(0, 0, 360, ClientRc.Height()), this, 1, CBCGPTabWnd::LOCATION_TOP))
	{
		TRACE0("\n未能创建tab\n");
		return ;      // 未能创建
	}
	m_MainCtrlDlg.Create(CMainCtrlDlg::IDD, &m_wndTab);
	m_VisionCtrlDlg.Create(CVisionCtrlDlg::IDD, &m_wndTab);
	m_MotionDlg.Create(CMotionDlg::IDD, &m_wndTab);
// 	m_wndTab.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	m_wndTab.AddTab(&m_MainCtrlDlg, _T("主界面"), (UINT)-1, FALSE);
	m_wndTab.AddTab(&m_MotionDlg, _T("运动平台"), (UINT)-1, FALSE);
	m_wndTab.AddTab(&m_VisionCtrlDlg, _T("视觉"), (UINT)-1, FALSE);
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
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->OnMouseMove(nFlags, point);

	CBCGPFormView::OnMouseMove(nFlags, point);
}

