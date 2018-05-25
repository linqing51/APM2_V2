// ProjectSettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "ProjectSettingDlg.h"
#include "MainFrm.h"

// CProjectSettingDlg 对话框

IMPLEMENT_DYNAMIC(CProjectSettingDlg, CBCGPDialog)

CProjectSettingDlg::CProjectSettingDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CProjectSettingDlg::IDD, pParent), nTotalVisionCheck(1), nTotalPoints(1)
{
	int i(0);
	m_bOnCreatNew = FALSE;
	m_nCurrentPrjSel = 0;
	nCurrentPoint = 1;
	nCurrentVisionCheck = 0;
	nProductNum[0] = 1;
	nProductNum[1] = 1;
	ZeroMemory(dProductSpacing, sizeof(dProductSpacing));
	ZeroMemory(dPointSpacing, sizeof(dPointSpacing));
	bCommunicate = FALSE;
}

CProjectSettingDlg::~CProjectSettingDlg()
{
}

void CProjectSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	int i;
	DDX_Control(pDX, IDC_COMBO_DISTANCE, m_cbRangingCfg);
	DDX_Control(pDX, IDC_COMBO_PRJNAME, m_cbPrjName);
	DDX_Control(pDX, IDC_COMBO_VISIONTYPE, m_cbVisionType);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnCreate);
	DDX_Control(pDX, IDC_BUTTON_RELOAD, m_btnLoad);
	for (i = 0; i < 2;i++)
	{
		DDX_Text(pDX, IDC_EDIT_PRODUCTX + i, dProductSpacing[i]);
		DDX_Text(pDX, IDC_EDIT_JOINT_OFFSETX + i, dPointSpacing[i]);
		DDX_Text(pDX, IDC_EDIT_PRODUCTROW + i, nProductNum[i]);

	}

	DDX_Text(pDX, IDC_EDIT_ZHEIGHT, dClimbHeight[0]);
	DDX_Text(pDX, IDC_EDIT_ZHEIGHT_ROW, dClimbHeight[2]);
	DDX_Text(pDX, IDC_EDIT_ZHEIGHT_COL, dClimbHeight[1]);
	DDX_Text(pDX, IDC_EDIT_JOINTNUM, nTotalPoints);
	DDX_Text(pDX, IDC_EDIT_JOINT_ORDER, nCurrentPoint);
	DDX_Text(pDX, IDC_EDIT_VISIONNUM, nTotalVisionCheck);
	DDX_Text(pDX, IDC_EDIT_CHECK_ORDER, nCurrentVisionCheck);
	DDX_Text(pDX, IDC_EDIT_WELDING_TIMES, fWeldingTimes);
	DDX_Control(pDX, IDC_SPIN_CHECK_ORDER, m_ctlCheckSpin);
	DDX_Control(pDX, IDC_SPIN_JOINT_ORDER, m_ctlPointSpin);

}


BEGIN_MESSAGE_MAP(CProjectSettingDlg, CBCGPDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_PRJNAME, &CProjectSettingDlg::OnCbnSelchangeComboPrjname)
	ON_CBN_SELCHANGE(IDC_COMBO_VISIONTYPE, &CProjectSettingDlg::OnCbnSelchangeComboVisionType)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CProjectSettingDlg::OnClickedBtnCreate)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_PRODUCTROW, IDC_EDIT_PRODUCTY, OnKillFocusProductEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_JOINT_OFFSETX, IDC_EDIT_ZHEIGHT_COL, OnKillFocusPointsOffsetEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_VISIONNUM, IDC_EDIT_WELDING_TIMES, OnKillFocusOrderEdit)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_EDIT_CHECK_ORDER,IDC_EDIT_JOINT_ORDER , OnEnChangeEdit)
	ON_MESSAGE(WM_USER_UPDATEUI, &CProjectSettingDlg::OnUpdateDlg)

	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CProjectSettingDlg::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CProjectSettingDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CProjectSettingDlg::OnBnClickedButtonCancel)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CProjectSettingDlg::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CProjectSettingDlg::OnBnClickedButtonSetting)
END_MESSAGE_MAP()


// CProjectSettingDlg 消息处理程序


void CProjectSettingDlg::OnCbnSelchangeComboPrjname()
{
	// TODO:  在此添加控件通知处理程序代码
	m_nCurrentPrjSel = m_cbPrjName.GetCurSel();
}


void CProjectSettingDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CBCGPDialog::OnClose();
}


void CProjectSettingDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


BOOL CProjectSettingDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	CString szNew;	// TODO:  在此添加额外的初始化
	m_ctlCheckSpin.SetBuddy(GetDlgItem(IDC_EDIT_CHECK_ORDER));
	m_ctlCheckSpin.SetBase(1);
	m_ctlCheckSpin.SetRange(0, 16);
// 	m_ctlCheckSpin.SetPos(1);

	m_ctlPointSpin.SetBuddy(GetDlgItem(IDC_EDIT_JOINT_ORDER));
	m_ctlPointSpin.SetBase(1);
	m_ctlPointSpin.SetRange(1, 16);
// 	m_ctlPointSpin.SetPos(1);

	POSITION pos = pFrame->m_pDoc->m_cListPrjName.GetHeadPosition();
	for (i = 0; (pos != NULL) && (i < pFrame->m_pDoc->m_cListPrjName.GetCount()); i++)
	{
		szNew = pFrame->m_pDoc->m_cListPrjName.GetNext(pos);
		int nItem = m_cbPrjName.AddString(szNew);
		if ((nItem != CB_ERR) && (nItem != CB_ERRSPACE) && (szNew == pFrame->m_pDoc->m_szCurParamName))
		{
			m_cbPrjName.SetCurSel(nItem);
			m_nCurrentPrjSel = nItem;
		}
	}
	m_cbVisionType.ResetContent();
	m_cbVisionType.AddString(_T("None"));
	for (i = 1; i < 17; i++)
	{
		szNew.Format(_T("T%d"), i);
		m_cbVisionType.AddString(szNew);

	}
	for (i = 1; i < 17; i++)
	{
		szNew.Format(_T("S%d"), i);
		m_cbVisionType.AddString(szNew);

	}

	PostMessage(WM_USER_UPDATEUI, 1, 1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

void CProjectSettingDlg::OnClickedBtnCreate()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString szNew;
	m_btnCreate.GetWindowText(szNew);
	if (szNew == _T("添加"))
	{
		GetDlgItem(IDC_EDIT_NEWPROJECT)->GetWindowText(szNew);
		if (szNew.TrimRight().IsEmpty())
			return;
		szNew.TrimLeft();
		if (CB_ERR != m_cbPrjName.FindString(-1, szNew))
			pFrame->ShowPopup(_T("项目名称已存在!"));
		else
		{
			int nItem = m_cbPrjName.AddString(szNew);
			if ((nItem != CB_ERR) && (nItem != CB_ERRSPACE))
			{
				m_cbPrjName.SetCurSel(nItem);
				m_nCurrentPrjSel = nItem;
				if (!pFrame->m_pDoc->CreateNewPrj(szNew))
				{
					pFrame->ShowPopup(_T("项目已存在!"));
				}
			}
			::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
		}
	}
	else if (szNew == _T("修改"))//modify
	{
		CString szold;
		m_cbPrjName.GetLBText(m_nCurrentPrjSel, szold);
		if (m_cbPrjName.IsWindowVisible())
		{
			m_cbPrjName.ShowWindow(SW_HIDE);
			GetDlgItem(IDC_EDIT_NEWPROJECT)->SetWindowText(szold);
			GetDlgItem(IDC_EDIT_NEWPROJECT)->ShowWindow(SW_SHOW);
		}
		else
		{

			GetDlgItem(IDC_EDIT_NEWPROJECT)->GetWindowText(szNew);
			int nIndex = m_cbPrjName.FindString(-1, szNew);
			if (CB_ERR != nIndex)
				pFrame->ShowPopup(_T("项目名称已存在!"));
			else
			{
				if (!pFrame->m_pDoc->PrjRename(szold, szNew))
				{
					pFrame->ShowPopup(_T("新项目名称已存在 或 \n 旧项目名不存在 !"));
					return;
				}
				m_cbPrjName.DeleteString(m_nCurrentPrjSel);
				m_cbPrjName.InsertString(m_nCurrentPrjSel, szNew);
				::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
			}
			::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
		}
	}
	else
	{
		::PostMessage(pFrame->GetSafeHwnd(),WM_CLOSE,NULL,NULL);
	}
}


void CProjectSettingDlg::OnBnClickedButtonCopy()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

}

void CProjectSettingDlg::OnCbnSelchangeComboVisionType()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

}

void CProjectSettingDlg::OnKillFocusProductEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT i(0),j(0),k(0);
	UINT	nNum(pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0] * (pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1]));
	switch (idCtl)
	{
	case IDC_EDIT_PRODUCTROW:
	case IDC_EDIT_PRODUCTCOL:
		i = idCtl - IDC_EDIT_PRODUCTROW;
		UpdateData();
		if (nProductNum[i] == 0 || nProductNum[i] > 100)
			nProductNum[i] = 1;

		if (nProductNum[0] * nProductNum[1] > MAX_ProductNumber)
		{
			nProductNum[1 >> i] = MAX_ProductNumber / nProductNum[i];
			UpdateData(FALSE);
		}
		break;
	case IDC_EDIT_PRODUCTX:
	case IDC_EDIT_PRODUCTY:
		i = idCtl - IDC_EDIT_PRODUCTX;
		UpdateData();
		break;
	}
}

void CProjectSettingDlg::OnKillFocusPointsOffsetEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	BOOL bNeedUpdate=FALSE;
	UpdateData();
	switch (idCtl)
	{
	case IDC_EDIT_JOINT_OFFSETX:
	case IDC_EDIT_JOINT_OFFSETY:
		if (dPointSpacing[idCtl-IDC_EDIT_JOINT_OFFSETX]>15)
		{
			dPointSpacing[idCtl - IDC_EDIT_JOINT_OFFSETX] = 0.1;
			bNeedUpdate = TRUE;
		}
		break;
	case IDC_EDIT_ZHEIGHT:
	case IDC_EDIT_ZHEIGHT_COL:
	case IDC_EDIT_ZHEIGHT_ROW:
		if (dClimbHeight < 0)
		{
			dClimbHeight[0] = 1;
			bNeedUpdate = TRUE;
		}
		if (dClimbHeight[1] < dClimbHeight[0])
		{
			dClimbHeight[1] = dClimbHeight[0];
			bNeedUpdate = TRUE;
		}
		if (dClimbHeight[2] < dClimbHeight[0])
		{
			dClimbHeight[2] = dClimbHeight[0];
			bNeedUpdate = TRUE;
		}
		break;
	}
	if (bNeedUpdate)
		UpdateData(FALSE);
}

void CProjectSettingDlg::OnKillFocusOrderEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT	nNum(0);
	switch (idCtl)
	{
	case IDC_EDIT_VISIONNUM:
		nNum = nTotalVisionCheck;
		UpdateData();
		if (nTotalVisionCheck > 16 || nTotalVisionCheck == 0)
		{
			nTotalVisionCheck = nNum;
			UpdateData(FALSE);
		}
		m_ctlCheckSpin.SetRange(0, nTotalVisionCheck);
		PostMessage(WM_USER_UPDATEUI, 2, 0);
		break;
	case IDC_EDIT_JOINTNUM:
		nNum = nTotalPoints;
		UpdateData();
		if (nTotalPoints > 16)
		{
			nTotalPoints = nNum;
			UpdateData(FALSE);
		}
		m_ctlPointSpin.SetRange(1, nTotalPoints);
		PostMessage(WM_USER_UPDATEUI, 2, 0);
		break;
	case IDC_EDIT_WELDING_TIMES:
		nNum = fWeldingTimes*10;
		UpdateData();
		if (fWeldingTimes > 20)
		{
			AfxMessageBox(_T("出光时间太长了，再想想？"));
			fWeldingTimes = nNum/10.0;
			UpdateData(FALSE);
		}
		break;

	}
}

void CProjectSettingDlg::OnKillFocusOthersEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
}

void CProjectSettingDlg::OnEnChangeEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();

	switch (idCtl)
	{
	case IDC_EDIT_JOINT_ORDER:
		PostMessage(WM_USER_UPDATEUI, 2, 0);
		pFrame->SetCurrentLaserGP(nCurrentPoint-1);
		break;
	case IDC_EDIT_CHECK_ORDER:

		break;
	default:
		return;
		break;
	}

}

LRESULT CProjectSettingDlg::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	UINT nIndex = wParam;
	CStatic* pStatic = NULL;
	BOOL bEable = (lParam&0x01)&&pFrame->GetUserType();
	switch (nIndex)
	{
	case 0:
		m_cbPrjName.EnableWindow(bEable||lParam>>1);
		m_cbVisionType.EnableWindow(bEable);
		m_btnCreate.EnableWindow(bEable || lParam >> 1);
		m_btnLoad.EnableWindow(bEable || lParam >> 1);
		GetDlgItem(IDC_EDIT_NEWPROJECT)->EnableWindow(bEable);
		GetDlgItem(IDC_BUTTON_SETTING)->EnableWindow(bEable);
		m_cbRangingCfg.EnableWindow(bEable);
		for (i = 0; i < 2; i++)
		{
			GetDlgItem(IDC_EDIT_PRODUCTX + i)->EnableWindow(bEable);
			GetDlgItem(IDC_EDIT_JOINT_OFFSETX + i)->EnableWindow(bEable);
			GetDlgItem(IDC_EDIT_PRODUCTROW + i)->EnableWindow(bEable);
		}
		GetDlgItem(IDC_EDIT_ZHEIGHT)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_ZHEIGHT_ROW)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_ZHEIGHT_COL)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_JOINTNUM)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_JOINT_ORDER)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_VISIONNUM)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_CHECK_ORDER)->EnableWindow(bEable);
		GetDlgItem(IDC_EDIT_WELDING_TIMES)->EnableWindow(bEable);
		GetDlgItem(IDC_SPIN_CHECK_ORDER)->EnableWindow(bEable);
		GetDlgItem(IDC_SPIN_JOINT_ORDER)->EnableWindow(bEable);
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEable);
		GetDlgItem(IDC_BUTTON_CANCEL)->EnableWindow(!(lParam & 0x02));
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(bEable&&theApp.m_bHasLocation);
		break;
	case 1:
	case 2:
		if (lParam)
		{
			nTotalVisionCheck = pFrame->m_pDoc->m_cParam.PrjCfg.uInspection;
			nTotalPoints = pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum;
			dClimbHeight[0] = pFrame->m_pDoc->m_cParam.PrjCfg.fClimbHeight[0];
			dClimbHeight[1] = pFrame->m_pDoc->m_cParam.PrjCfg.fClimbHeight[1];
			dClimbHeight[2] = pFrame->m_pDoc->m_cParam.PrjCfg.fClimbHeight[2];
			m_ctlCheckSpin.SetRange(0, nTotalVisionCheck);
			m_ctlPointSpin.SetRange(1, nTotalPoints);
		}
		if (nCurrentPoint > nTotalPoints)
		{
			nCurrentPoint = nTotalPoints;
		}
		nCurrentVisionCheck = pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uVisionIndex;
		if (nCurrentVisionCheck > nTotalVisionCheck)
		{
			nCurrentVisionCheck = 0;
			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uVisionIndex = nTotalVisionCheck;
		}
		m_cbVisionType.SetCurSel(pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uVisionType.nProcessNum);
		m_cbRangingCfg.SetCurSel(pFrame->m_pDoc->m_cParam.PrjCfg.bRangingMode&0x01);
		fWeldingTimes = pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uWeldingTimes/10.0;

		for (i = 0; i < 2; i++)
		{
			if (lParam)
			{
				nProductNum[i] = pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[i];
				dProductSpacing[i] = pFrame->m_pDoc->m_cParam.PrjCfg.fProductSpace[i];
			}
			dPointSpacing[i] = pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].CenterPt[i];

		}
		UpdateData(FALSE);
		break;
	}
	return 0;
}

void CProjectSettingDlg::OnBnClickedButtonReload()
{
		// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	CString prjname;
	CString szNew;
	m_cbPrjName.GetLBText(m_nCurrentPrjSel, prjname);
	pFrame->m_pDoc->LoadPrjData(prjname);
	PostMessage(WM_USER_UPDATEUI, 2, 1);
	::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
	i = pFrame->DownPlcConf();
	CString str[5] = { _T("能量、气压;"), _T("无球报警数;"), _T("清洗能量;"), _T("激光时间;"), _T("") };
	if (i)
	{
		for (BYTE j = 0; j < 4;j++)
		{
			if (i >> j)
			{
				if ((i >> j) & 0x01)
				{
					szNew.Format(_T("%s"), str[i]);
					str[4] += szNew;
				}

			}else
				break;
		}
		szNew.Format(_T("参数写入PLC失败，未完成写入的有\r\n%s\r\n"), str[4]);
		pFrame->AddedErrorMsg(szNew);
	}
	m_btnLoad.GetWindowText(szNew);
	if (szNew == _T("确认"))
	{
		pFrame->ShowWindow(SW_SHOWMAXIMIZED);
// 		m_btnLoad.SetWindowText(_T("加载"));
// 		m_btnCreate.SetWindowText(_T("修改"));
		CBCGPDialog::OnCancel();
	}

}

BOOL CProjectSettingDlg::OnTransfer(BOOL bDownload)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT tem(0);
	int i;
	if (bDownload)
	{
		UpdateData(TRUE);
		if (pFrame->m_LaserCtrl.SetPowerTime(fWeldingTimes,nCurrentPoint - 1 ))
		{
			GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(FALSE);
			SetTimer(2, 600, NULL);
		}
		else
			pFrame->AddedErrorMsg(_T("出光时间写入PLC失败\r\n"));
	}
	else
	{
		fWeldingTimes = pFrame->m_LaserCtrl.GetPowerTime(nCurrentPoint - 1);
		UpdateData(FALSE);
	}
	return TRUE;
}

void CProjectSettingDlg::OnBnClickedButtonApply()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT i(0), j(0), k(0), nNum(1);
	if (IDYES == MessageBox(_T("确定更新参数吗?"), _T("重要提示"), MB_YESNO))
	{
		theApp.m_bNeedSave = TRUE;

		for (i = 0; i < 2; i++)
		{
			pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[i] = nProductNum[i];
			pFrame->m_pDoc->m_cParam.PrjCfg.fProductSpace[i] = dProductSpacing[i];

			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].CenterPt[i] = dPointSpacing[i];

			pFrame->m_pDoc->m_cParam.PrjCfg.fClimbHeight[i] = abs(dClimbHeight[i]);
		}
		pFrame->m_pDoc->m_cParam.PrjCfg.fClimbHeight[2] = abs(dClimbHeight[2]);
		pFrame->m_pDoc->m_cParam.PrjCfg.uInspection = nTotalVisionCheck;
		if (pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum ^ nTotalPoints)
		{
			pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum = nTotalPoints;
			IVA_DisposeData(pFrame->m_ivaData);
			pFrame->m_ivaData = IVA_InitData(nTotalPoints, 0);
		}
		pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uWeldingTimes = fWeldingTimes*10;
		pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uVisionIndex = nCurrentVisionCheck;
		i = m_cbVisionType.GetCurSel();
		if (i >= 0)
			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[nCurrentPoint - 1].uVisionType.nProcessNum = i;
		i = m_cbRangingCfg.GetCurSel();
		if (i >= 0)
			pFrame->m_pDoc->m_cParam.PrjCfg.bRangingMode= i;
		OnTransfer(TRUE);
		if (IDYES == MessageBox(_T("是否重新计算产品阵列坐标?"), _T("重要提示"), MB_YESNO))
		{
			pFrame->AutoCalculatePos1(0);
			pFrame->AutoCalculatePos1(1);
			pFrame->AutoCalculatePos2(0);
			pFrame->AutoCalculatePos2(1);
		}
		::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
	}
}


void CProjectSettingDlg::OnBnClickedButtonCancel()
{
	// TODO:  在此添加控件通知处理程序代码
	PostMessage(WM_USER_UPDATEUI, 2, 1);
	CBCGPDialog::OnCancel();
}


void CProjectSettingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 2:
		OnTransfer(FALSE);
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(TRUE);
		KillTimer(nIDEvent);
		break;
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void CProjectSettingDlg::OnBnClickedButtonUpdate()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString Tip ;
	CString str[] = { _T("左边载具"), _T("右边载具") };
	Tip.Format(_T("确认在%s更新焊点偏移位置 ？\r\n务必在定位测试正常后的状态下使用此自动计算"), str[theApp.m_bHasLocation>>1]);
	if (nCurrentPoint)
	{
		if (IDYES == MessageBox(Tip, _T("重要提示"), MB_YESNO))
		{
			if (theApp.m_bHasLocation)
			{
				dPointSpacing[0] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[0] - theApp.m_dLocated[0];
				dPointSpacing[1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[theApp.m_bHasLocation] - theApp.m_dLocated[1];
				
				Tip = _T("计算完成!该定位基准下是否还有其它焊点稍后要更新偏移位置 ？");
				if (IDNO == MessageBox(Tip, _T("重要提示"), MB_YESNO))
				{
					theApp.m_bHasLocation = FALSE;
				}
			}
			int nTemp = dPointSpacing[0] * 1000;
			dPointSpacing[0] = nTemp / 1000.0;
			nTemp = dPointSpacing[1] * 1000;
			dPointSpacing[1] = nTemp / 1000.0;
			UpdateData(FALSE);
		}
	}

}


void CProjectSettingDlg::OnBnClickedButtonSetting()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 6, 0);
}
