// IOStatusDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "IOStatusDlg.h"
#include "MainFrm.h"

// CIOStatusDlg 对话框

IMPLEMENT_DYNAMIC(CIOStatusDlg, CBCGPDialog)

CIOStatusDlg::CIOStatusDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CIOStatusDlg::IDD, pParent), m_pIoHandler(NULL)
{
	EnableVisualManagerStyle(TRUE,TRUE);
	m_nStyle = 0;
	m_hRedBitmap = NULL;
	m_hGreenBitmap = NULL;
}

CIOStatusDlg::~CIOStatusDlg()
{
	TRACE0("done!\n");

}

void CIOStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	for (int i = 0; i < 32;++i)
	{
		DDX_Control(pDX, IDC_CHECK00+i, m_hCheck[i]);
		if (i<24)
			DDX_Control(pDX, IDC_STATIC_XNLIMIT + i, m_lStatus[i]);
	}

}


BEGIN_MESSAGE_MAP(CIOStatusDlg, CBCGPDialog)
	ON_MESSAGE(WM_USER_MOTIONSTATUS, &CIOStatusDlg::OnAxisStsChange)
	ON_MESSAGE(WM_USER_IOCHANGE, &CIOStatusDlg::OnIochange)
	ON_REGISTERED_MESSAGE(BCGM_ON_GAUGE_CLICK, &CIOStatusDlg::OnGaugeClick)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CIOStatusDlg 消息处理程序


BOOL CIOStatusDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();

	CBCGPSwitchColors colorsHorz;
	colorsHorz.m_brFillOn = CBCGPBrush(CBCGPColor::Green, CBCGPColor::YellowGreen, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL);
	colorsHorz.m_brFillOff = CBCGPBrush(CBCGPColor::DarkRed, CBCGPColor::Red, CBCGPBrush::BCGP_GRADIENT_HORIZONTAL);
	colorsHorz.m_brFillThumb = CBCGPBrush(CBCGPColor::LightGray, CBCGPColor::White, CBCGPBrush::BCGP_GRADIENT_RADIAL_TOP_LEFT);

	CBCGPSwitchColors colorsHorzLabel = colorsHorz;
	colorsHorzLabel.m_brLabelOff = CBCGPBrush(CBCGPColor::White);
	int i;
	CString sz[] = { _T("断开"), _T("闭合") };
	for (i=0;i<32;i++)
	{
		CString label;
		m_hCheck[i].GetSwitch()->SetID(i);
		m_hCheck[i].GetSwitch()->EnableOnOffLabels();
		m_hCheck[i].GetSwitch()->SetStyle(CBCGPSwitchImpl::BCGP_SWITCH_CIRCLE);
		m_hCheck[i].GetSwitch()->SetColors(colorsHorzLabel);
		for (int j = 0; j < 2; ++j)
		{
			label.Format(_T("%-2d%s"), i,sz[j]);
			m_hCheck[i].GetSwitch()->SetLabel(label, j);
		}
	}

	CStatic* pStatic=NULL;
	pStatic = (CStatic*)GetDlgItem(IDC_PICT01);
	m_hRedBitmap = pStatic->GetBitmap();
	pStatic = (CStatic*)GetDlgItem(IDC_PICT00);
	m_hGreenBitmap = pStatic->GetBitmap();
	UINT n=SetTimer(1, 1000, NULL);
	if (n ^ 1)
		AfxMessageBox(_T("IO 状态窗:定时器1启动异常"));
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

afx_msg LRESULT CIOStatusDlg::OnIochange(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
	UINT nIndex = wParam;

	BOOL bBitState = FALSE;
	CStatic* pState = NULL;
	UINT uOutValue(0);
	int i = 0;
	if (!nIndex)
	{
// 		for (i = 0; i < 4; i++)
// 		{
// 			pState = (CStatic*)GetDlgItem(IDC_PICT38 + i);
// 			bBitState = (lParam>>i)&(i!=3?0x01:0x1f);
// 			pState->SetBitmap(!bBitState ? m_hGreenBitmap : m_hRedBitmap);
// 		}
		uOutValue = pFrame->m_IoCtrller.ReadOutputByPort(0, 4);
		for (i = 0; i < 32; i++)
		{
			pState = (CStatic*)GetDlgItem(IDC_PICT00 + i);
			bBitState = pFrame->m_IoCtrller.m_bInput[i];
			pState->SetBitmap(!bBitState ? m_hGreenBitmap : m_hRedBitmap);

			m_hCheck[i].GetSwitch()->SetOn(uOutValue >> i & 0x01, TRUE);
		}
	}
		
	return 0;
}


afx_msg LRESULT CIOStatusDlg::OnGaugeClick(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();

	int nID = (int)wParam;
	BOOL bBitState;
	bBitState = m_hCheck[nID].GetSwitch()->IsOn();
	if (pFrame->GetUserType())
		pFrame->m_IoCtrller.WriteOutputByBit(nID, bBitState);

	return 0;
}

void CIOStatusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
	switch (nIDEvent)
	{
	case 1:
		PostMessage(WM_USER_IOCHANGE, 0, 0);
		break;
	case 2:
		break;
	}
	CBCGPDialog::OnTimer(nIDEvent);
}

void CIOStatusDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CBCGPDialog::OnClose();
}


void CIOStatusDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBCGPDialog::OnShowWindow(bShow, nStatus);
	CMainFrame* pFrame=(CMainFrame*)AfxGetMainWnd();
}

LRESULT CIOStatusDlg::OnAxisStsChange(WPARAM wParam, LPARAM lParam)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	int i(0);
	BOOL bStatus(FALSE);
	static UINT nsts[2] = {0xff,0xff};

	switch (wParam)
	{
	case 0:
// 		if (nsts[0] ^ lParam)
// 		{
// 			nsts[0] = lParam;
			for (i = 0; i < 4; i++)
			{
				bStatus = pFrame->m_pMotionCtrller[0]->GetSensorState(i + 1, NegSensor);
				m_lStatus[i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
				bStatus = pFrame->m_pMotionCtrller[0]->GetSensorState(i + 1, HomeSensor);
				m_lStatus[6 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
				bStatus = pFrame->m_pMotionCtrller[0]->GetSensorState(i + 1, PosSensor);
				m_lStatus[12 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
				bStatus = pFrame->m_pMotionCtrller[0]->GetSensorState(i + 1, Alarm);
				m_lStatus[18 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
			}
// 		}
		break;
	case 1:
// 		if (nsts[1] ^ lParam)
// 		{
// 			nsts[1] = lParam;

			for (i = 0; i < 2; i++)
			{
				bStatus = pFrame->m_pMotionCtrller[1]->GetSensorState(i + 1, NegSensor);
				m_lStatus[4+i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
				bStatus = pFrame->m_pMotionCtrller[1]->GetSensorState(i + 1, HomeSensor);
				m_lStatus[10 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);

				bStatus = pFrame->m_pMotionCtrller[1]->GetSensorState(i + 1, PosSensor);
				m_lStatus[16 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
				bStatus = pFrame->m_pMotionCtrller[1]->GetSensorState(i + 1, Alarm);
				m_lStatus[22 + i].SetBkColor(bStatus ? CBCGPColor::DarkBlue : CBCGPColor::DarkGreen);
			}
// 		}
		break;
	}

	return 0;
}

