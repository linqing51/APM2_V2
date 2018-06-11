// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "GeneralSettingDlg.h"
#include "MainFrm.h"

// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CGeneralSettingDlg, CBCGPDialog)

CGeneralSettingDlg::CGeneralSettingDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CGeneralSettingDlg::IDD, pParent)
{
	EnableVisualManagerStyle();
	ZeroMemory(nExposure, sizeof(nExposure));
	ZeroMemory(nIntensity, sizeof(nIntensity));

}

CGeneralSettingDlg::~CGeneralSettingDlg()
{
}

void CGeneralSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SAFETYGRATING, btChecksafety[0]);
	DDX_Control(pDX, IDC_CHECK_SAFETYGATE, btChecksafety[1]);
	DDX_Control(pDX, IDC_CHECK_CLEANNOZZLE, btSetAutoClean);
	DDX_Control(pDX, IDC_CHECK_FIXER, btSetFixer);
	DDX_Control(pDX, IDC_CHECK_RANGING, btSetRanging);
	DDX_Text(pDX, IDC_EDIT_TOP_EXPOSURE, nExposure[0]);
	DDX_Text(pDX, IDC_EDIT_BOTTOM_EXPOSURE, nExposure[1]);
	DDX_Text(pDX, IDC_EDIT_TOP_LIGHT, nIntensity[0]);
	DDX_Text(pDX, IDC_EDIT_BOTTOM_LIGHT, nIntensity[1]);
	DDX_Text(pDX, IDC_EDIT_NOZZLE_OFFSETX, dOffset[0]);
	DDX_Text(pDX, IDC_EDIT_NOZZLE_OFFSETY, dOffset[1]);
	DDX_Text(pDX, IDC_EDIT_NOZZLE_OFFSETZ, dOffset[2]);
	DDX_Text(pDX, IDC_EDIT_NOZZLELIFE, nNozzleLife);
	DDX_Text(pDX, IDC_EDIT_BALL_NG, nBallAlarm);
	DDX_Control(pDX, IDC_SPIN_TOP_E, m_ctlExposureSpin[0]);
	DDX_Control(pDX, IDC_SPIN_BOTTOM_E, m_ctlExposureSpin[1]);
	DDX_Control(pDX, IDC_SPIN_TOP_L, m_ctlIntensitySpin[0]);
	DDX_Control(pDX, IDC_SPIN_BOTTOM_L, m_ctlIntensitySpin[1]);

}


BEGIN_MESSAGE_MAP(CGeneralSettingDlg, CBCGPDialog)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_SAFETYGRATING, IDC_CHECK_RANGING, OnBnSelect)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_TOP_EXPOSURE, IDC_EDIT_BOTTOM_EXPOSURE, OnKillFocusExposureEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_TOP_LIGHT, IDC_EDIT_BOTTOM_LIGHT, OnKillFocusIntensityEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_NOZZLE_OFFSETX, IDC_EDIT_NOZZLE_OFFSETZ, OnKillFocusCCD_NozzleEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_NOZZLELIFE, IDC_EDIT_BALL_NG, OnKillFocusOtherEdit)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_TOP_EXPOSURE, IDC_EDIT_BOTTOM_LIGHT, OnEnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CGeneralSettingDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CGeneralSettingDlg::OnBnClickedButtonCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CGeneralSettingDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i;
	// 	m_ctlCheckSpin.SetPos(1);

	for (i = 0; i < 2;i++)
	{
		nExposure[i] = pFrame->m_pDoc->m_cParam.PrjCfg.nExposure[i];
		nIntensity[i] = pFrame->m_pDoc->m_cParam.PrjCfg.bIntensity[i];
		dOffset[i] = (int)(pFrame->m_pDoc->m_cParam.dCCD_Nozzle[i] * 1000) / 1000.0;
	}
	dOffset[2] = (pFrame->m_pDoc->m_cParam.dCCD_Nozzle[2] * 1000) / 1000.0;
	nNozzleLife = pFrame->m_pDoc->m_cParam.nNozzleLife[1];
	nBallAlarm = pFrame->m_pDoc->m_cParam.BallAlarmNum;
	// TODO:  在此添加额外的初始化
	UpdateData(FALSE);
	for (i = 0; i < 2; i++)
	{
		m_ctlExposureSpin[i].SetBuddy(GetDlgItem(IDC_EDIT_TOP_EXPOSURE + i));
		m_ctlExposureSpin[i].SetBase(1);
		m_ctlExposureSpin[i].SetRange32(1, 60000);

		m_ctlIntensitySpin[i].SetBuddy(GetDlgItem(IDC_EDIT_TOP_LIGHT + i));
		m_ctlIntensitySpin[i].SetBase(1);
		m_ctlIntensitySpin[i].SetRange(0, 255);
	}
	btChecksafety[0].SetCheck(pFrame->m_pDoc->m_cParam.nEnabeSafetyDev & 0x01);
	btChecksafety[1].SetCheck(pFrame->m_pDoc->m_cParam.nEnabeSafetyDev >> 1 ? 1 : 0);
	btSetAutoClean.SetCheck(pFrame->m_pDoc->m_cParam.PrjCfg.bAutoClean);
	btSetFixer.SetCheck(pFrame->m_pDoc->m_cParam.PrjCfg.bUseFixer);
	btSetRanging.SetCheck(pFrame->m_pDoc->m_cParam.PrjCfg.bUseRanging);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CGeneralSettingDlg::OnKillFocusExposureEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i = idCtl-IDC_EDIT_TOP_EXPOSURE;
}

void CGeneralSettingDlg::OnKillFocusIntensityEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i = idCtl - IDC_EDIT_TOP_LIGHT;
}

void CGeneralSettingDlg::OnKillFocusCCD_NozzleEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i;
// 	if (IDYES == MessageBox(_T("此参数极其重要，确定更新焊嘴与CCD偏移距离?"), _T("重要提示"), MB_YESNO))
// 	{
// 		UpdateData();
// 	}
// 	else
// 		UpdateData(FALSE);
}

void CGeneralSettingDlg::OnKillFocusOtherEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
// 	UpdateData();
	switch (idCtl)
	{
	case IDC_EDIT_NOZZLELIFE:
		break;
	case IDC_EDIT_BALL_NG:

		break;
	default:
		break;
	}
	return;
}

void CGeneralSettingDlg::OnBnSelect(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bOn(FALSE);
	UINT nIndex = idCtl - IDC_CHECK_SAFETYGRATING;
	if (((CButton*)GetDlgItem(idCtl))->GetCheck() == BST_CHECKED)
		bOn = TRUE;
	AfxMessageBox(_T("此操作修改即生效"));
	switch (nIndex)
	{
	case 0://IDC_CHECK_SAFETYGRATING
	case 1://IDC_CHECK_SAFETYGATE
		if (bOn)
			pFrame->m_pDoc->m_cParam.nEnabeSafetyDev |= 0x01 << nIndex;
		else
			pFrame->m_pDoc->m_cParam.nEnabeSafetyDev &= ~(0x01 << nIndex);

		pFrame->EnableSaftyDev(nIndex, bOn);

		break;
	case 2://IDC_CHECK_CLEANNOZZLE
		pFrame->m_pDoc->m_cParam.PrjCfg.bAutoClean = bOn;
		break;
	case 3://IDC_CHECK_fixer
		pFrame->m_pDoc->m_cParam.PrjCfg.bUseFixer = bOn;
		break;
	case 4://IDC_CHECK_ranging
		pFrame->m_pDoc->m_cParam.PrjCfg.bUseRanging = bOn;
		if (0)
		{
			if (!pFrame->m_pDoc->m_cParam.PrjCfg.bRangingMode)
			{
				if (IDYES == MessageBox(_T("当前<行首尾测距>模式下切换使能，拍照路径有不同优化，\r\n使用：Z 字形走位，即每行拍照起始为左1;\r\n不使用：弓 字形走位，即每行拍照起始衔接上一行\r\n是否启用此优化并重新计算产品坐标(以首产品为基准)?"), _T("提示"), MB_YESNO))
				{
					pFrame->AutoCalculatePos1(0);
					pFrame->AutoCalculatePos1(1);
					pFrame->AutoCalculatePos2(0);
					pFrame->AutoCalculatePos2(1);
				}
			}
		}
		break;
	}

}


// CSettingDlg 消息处理程序

void CGeneralSettingDlg::OnBnClickedButtonConnect()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

}

void CGeneralSettingDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CBCGPDialog::OnClose();
}

void CGeneralSettingDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}

void CGeneralSettingDlg::OnBnClickedButtonApply()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (IDYES == MessageBox(_T("确定更新参数吗?"), _T("重要提示"), MB_YESNO))
	{
		UpdateData();
		theApp.m_bNeedSave = TRUE;
		int i;
		for (i = 0; i < 2; i++)
		{
			if (pFrame->m_NiVision.Camera_SetExposure(i, nExposure[i]))
				pFrame->m_pDoc->m_cParam.PrjCfg.nExposure[i] = nExposure[i];
			pFrame->m_pDoc->m_cParam.PrjCfg.bIntensity[i] = nIntensity[i];
			pFrame->LightControllerSet(i, nIntensity[i], 2);
		}
		for (i = 0; i < 3; i++)
			pFrame->m_pDoc->m_cParam.dCCD_Nozzle[i] = dOffset[i];
		pFrame->m_pDoc->m_cParam.nNozzleLife[1] = nNozzleLife;
		pFrame->m_pDoc->m_cParam.BallAlarmNum = nBallAlarm;
		pFrame->m_LaserCtrl.SetNGBallNum(nBallAlarm);
		UINT n=SetTimer(2, 500, NULL);
		if (n ^ 2)
			AfxMessageBox(_T("通用参数回读:定时器2启动异常"));
	}
}

void CGeneralSettingDlg::OnBnClickedButtonCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i;
	for (i = 0; i < 2; i++)
	{
		nExposure[i] = pFrame->m_pDoc->m_cParam.PrjCfg.nExposure[i];
		pFrame->m_NiVision.Camera_SetExposure(i, nExposure[i]);
		nIntensity[i] = pFrame->m_pDoc->m_cParam.PrjCfg.bIntensity[i];
		pFrame->LightControllerSet(i, nIntensity[i], 2);
		dOffset[i] = (int)(pFrame->m_pDoc->m_cParam.dCCD_Nozzle[i] * 1000) / 1000.0;
	}
	dOffset[2] = (int)(pFrame->m_pDoc->m_cParam.dCCD_Nozzle[2] * 1000) / 1000.0;
	nNozzleLife = pFrame->m_pDoc->m_cParam.nNozzleLife[1];
	nBallAlarm = pFrame->m_pDoc->m_cParam.BallAlarmNum;
	// TODO:  在此添加额外的初始化
	UpdateData(FALSE);
	CBCGPDialog::OnCancel();
}


void CGeneralSettingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	switch (nIDEvent)
	{
	case 2:
		nBallAlarm=pFrame->m_LaserCtrl.GetNGBallNum();
		UpdateData(FALSE);
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(TRUE);
		KillTimer(nIDEvent);
		break;
	}

	CBCGPDialog::OnTimer(nIDEvent);
}

void CGeneralSettingDlg::OnEnChangeEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
	BYTE nIndex;
	switch (idCtl)
	{
	case IDC_EDIT_TOP_EXPOSURE:
	case IDC_EDIT_BOTTOM_EXPOSURE:
		nIndex = idCtl - IDC_EDIT_TOP_EXPOSURE;
		if (nExposure[nIndex] < 60000)
			pFrame->m_NiVision.Camera_SetExposure(nIndex, nExposure[nIndex]);

		break;
	case IDC_EDIT_TOP_LIGHT:
	case IDC_EDIT_BOTTOM_LIGHT:
		nIndex = idCtl - IDC_EDIT_TOP_LIGHT;
		if(nIntensity[nIndex]<255)
			pFrame->LightControllerSet(nIndex, nIntensity[nIndex], 2);
		break;
	default:
		return;
		break;
	}

}
