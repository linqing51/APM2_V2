// HomeProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "HomeProcessDlg.h"
#include "MainFrm.h"


// CHomeProcessDlg 对话框

IMPLEMENT_DYNAMIC(CHomeProcessDlg, CBCGPDialog)

CHomeProcessDlg::CHomeProcessDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CHomeProcessDlg::IDD, pParent)
{
	EnableVisualManagerStyle();
}

CHomeProcessDlg::~CHomeProcessDlg()
{
}

void CHomeProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_wndTips);
}


BEGIN_MESSAGE_MAP(CHomeProcessDlg, CBCGPDialog)
	ON_MESSAGE(WM_USER_HOMING, &CHomeProcessDlg::OnHoming)
END_MESSAGE_MAP()


// CHomeProcessDlg 消息处理程序


void CHomeProcessDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类
	::PostMessage(GetParent()->m_hWnd, WM_USER_HOMED, 0, 0);

	CBCGPDialog::OnCancel();
}


BOOL CHomeProcessDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	// 	::SetWindowPos(this->m_hWnd, HWND_TOP, 0, 0, 640, 480, SWP_NOZORDER);
	CenterWindow();
	// TODO:  在此添加额外的初始化
// 	m_wndAnimate.Open(IDR_ANIMATION);
	
	m_wndAnimate.Open(pFrame->m_pDoc->m_szCurrentFolder+_T("APM.Bin"));
	m_wndAnimate.Play(0, -1, -1);
	m_wndTips.m_clrText = 0xff11ad;
	m_wndTips.SetText(_T("平台等待复位！"));
	m_ctlProgress.SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

LRESULT CHomeProcessDlg::OnHoming(WPARAM wParam, LPARAM lParam)
{
	static UINT nProcess=0;
	int nSts = lParam;
	switch ((UINT)wParam)
	{
	case 0:
		m_wndTips.SetText(_T("平台正在复位，请稍后…………\n"));
		m_ctlProgress.SetPos(1);
		break;
	case 1:
		if (nSts == 1){
			m_ctlProgress.SetPos(99);
			m_wndTips.SetText(_T("X轴复位结束…………"));
		}
		else
			m_wndTips.SetText(_T("X轴复位失败…………"));

		break;
	case 2:

		if (nSts == 1){
			m_ctlProgress.SetPos(66);
			m_wndTips.SetText(_T("LY轴复位结束…………"));
		}
		else
			m_wndTips.SetText(_T("LY轴复位失败…………"));

		break;
	case 3:

		if (nSts == 1){
			m_ctlProgress.SetPos(66);
			m_wndTips.SetText(_T("XY轴复位结束…………"));
		}
		else
			m_wndTips.SetText(_T("XY轴复位失败…………"));

		break;
	case 4:

		if (nSts == 1){
			m_ctlProgress.SetPos(33);
			m_wndTips.SetText(_T("RY轴复位结束…………"));
		}
		else
			m_wndTips.SetText(_T("RY轴复位失败…………"));

		break;
	case 7:
		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("X、LY、RY轴复位结束…………等待Bond头碟片复位"));
		}
		else
			m_wndTips.SetText(_T("X、LY、RY轴复位失败…………"));

		break;
	case 8:

		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("Z轴复位结束…………等待X LY RY复位"));
		}
		else
			m_wndTips.SetText(_T("Z轴复位失败…………"));

		break;
	case 0x30:

		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("CX、VZ轴复位结束…………等待Bond头碟片复位"));
		}
		else
			m_wndTips.SetText(_T("CX、VZ轴复位失败…………"));

		break;

	case 0xff:
		break;
	}
	return 0;
}
