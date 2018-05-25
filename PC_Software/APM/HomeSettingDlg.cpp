// HomeSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "HomeSettingDlg.h"
#include "MainFrm.h"


// CHomeSettingDlg 对话框

IMPLEMENT_DYNAMIC(CHomeSettingDlg, CBCGPDialog)

CHomeSettingDlg::CHomeSettingDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CHomeSettingDlg::IDD, pParent)
{
	ZeroMemory(&m_SpeedCfg,sizeof(m_SpeedCfg));
}

CHomeSettingDlg::~CHomeSettingDlg()
{
}

void CHomeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	int i(0);

	for (i = 0; i < 6; i++){
		DDX_Text(pDX, IDC_EDIT_MU_XVEL + i, m_SpeedCfg.Run_MU_VEL[i]);
		DDX_Text(pDX, IDC_EDIT_AU_XVEL + i, m_SpeedCfg.Run_AU_VEL[i]);
		DDX_Text(pDX, IDC_EDIT_XACVEL + i, m_SpeedCfg.Run_ACC[i]);
	}

}


BEGIN_MESSAGE_MAP(CHomeSettingDlg, CBCGPDialog)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_MU_XVEL, IDC_EDIT_VZACVEL, OnKillFocusEditVel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CHomeSettingDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CHomeSettingDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()

void CHomeSettingDlg::OnKillFocusEditVel(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT nIndex = idCtl - IDC_EDIT_MU_XVEL;
	double dvalue ;
	switch (nIndex)
	{
	case 0://IDC_EDIT_MU_XVEL
	case 1://IDC_EDIT_MU_LYVEL
	case 2://IDC_EDIT_MU_RYVEL
	case 3://IDC_EDIT_MU_ZVEL
	case 4://IDC_EDIT_MU_CXVEL
	case 5://IDC_EDIT_MU_VZVEL
		dvalue = m_SpeedCfg.Run_MU_VEL[nIndex];
		UpdateData();
		if (m_SpeedCfg.Run_MU_VEL[nIndex] < 1 || m_SpeedCfg.Run_MU_VEL[nIndex]>500){
			m_SpeedCfg.Run_MU_VEL[nIndex] = dvalue;
			UpdateData(FALSE);
		}

		break;
	case 6://IDC_EDIT_AU_XVEL
	case 7://IDC_EDIT_AU_LYVEL
	case 8://IDC_EDIT_AU_RYVEL
	case 9://IDC_EDIT_AU_ZVEL
	case 10://IDC_EDIT_AU_CXUVEL
	case 11://IDC_EDIT_AU_VZVEL
		nIndex -= 6;
		dvalue = m_SpeedCfg.Run_AU_VEL[nIndex];
		UpdateData();
		if (m_SpeedCfg.Run_AU_VEL[nIndex] < 1 || m_SpeedCfg.Run_AU_VEL[nIndex]>500){
			m_SpeedCfg.Run_AU_VEL[nIndex] = dvalue;
			UpdateData(FALSE);
		}

		break;
	case 12://IDC_EDIT_XACVEL
	case 13://IDC_EDIT_LYACVEL
	case 14://IDC_EDIT_RYACVEL
	case 15://IDC_EDIT_ZACVEL
	case 16://IDC_EDIT_CXACVEL
	case 17://IDC_EDIT_VZACVEL
		nIndex -= 12;
		dvalue = m_SpeedCfg.Run_ACC[nIndex];
		UpdateData();
		if (m_SpeedCfg.Run_ACC[nIndex] < 1||m_SpeedCfg.Run_ACC[nIndex] >1500){
			m_SpeedCfg.Run_ACC[nIndex] = dvalue;
			UpdateData(FALSE);
		}
		break;
	}
}


// CHomeSettingDlg 消息处理程序


void CHomeSettingDlg::OnBnClickedButtonApply()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString Tip = _T("即将更新速度参数信息\n请确认设置正确?\n");
	if (IDYES == MessageBox(Tip, _T("提示"), MB_YESNO))
	{
		theApp.m_bNeedSave = TRUE;
		pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg = m_SpeedCfg;
		pFrame->m_pMotionCtrller[0]->RuninConfig(0, m_SpeedCfg.Run_AU_VEL[0], m_SpeedCfg.Run_ACC[0]);
		pFrame->m_pMotionCtrller[1]->RuninConfig(0, m_SpeedCfg.Run_AU_VEL[4], m_SpeedCfg.Run_ACC[4]);
	}
}


BOOL CHomeSettingDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	m_SpeedCfg = pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg;
	UpdateData(FALSE);
	// TODO:  在此添加额外的初始化
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CHomeSettingDlg::OnBnClickedButtonCancel()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	m_SpeedCfg= pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg ;
	UpdateData(FALSE);
	CBCGPDialog::OnCancel();
}
