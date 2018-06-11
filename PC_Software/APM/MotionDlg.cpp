// MotionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "MotionDlg.h"
#include "MainFrm.h"


// CMotionDlg 对话框

IMPLEMENT_DYNAMIC(CMotionDlg, CBCGPDialog)

CMotionDlg::CMotionDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CMotionDlg::IDD, pParent)
, m_nCurrentWP(0)
{
// 	EnableVisualManagerStyle(TRUE, TRUE);
// 	EnableLayout(FALSE);
	m_nCurrentVisionCheck = 0;
	m_nCurrentProduct[0] = 0;
	m_nCurrentProduct[1] = 0;
	bMove = 0;
	m_nCurrentAxis = 0;
	m_bInching = TRUE;
	m_fPace = 1;
}

CMotionDlg::~CMotionDlg()
{
}

void CMotionDlg::DoDataExchange(CDataExchange* pDX)
{
	int i = 0;
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_LEFT, m_nCurrentWP);
	for (i = 0; i < 6; i++)
		DDX_Control(pDX, IDC_STATIC_ACTUAL_X + i, m_lStatus[i]);
	DDX_Control(pDX, IDC_STATIC_TARGET, m_lStatus[6]);
	DDX_Control(pDX, IDC_COMBO_POSITION_TYPE, m_cPositionChs);
	DDX_Control(pDX, IDC_COMBO_CHECKTYPE, m_cVisionOrderChs);
	DDX_Control(pDX, IDC_COMBO_SPACING, m_cPaceChs);

}


BEGIN_MESSAGE_MAP(CMotionDlg, CBCGPDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_LEFT, IDC_RADIO_RIGHT, OnBnChangeWP)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_MOVE_XL, IDC_BUTTON_MOVE_RUD, OnBnClickedMove)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_GOPOSITION, IDC_BUTTON_SAVEPOSITION, OnBnClickedButtonGetPosition)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBO_PRODUCT_ROW, IDC_COMBO_PRODUCT_COL, OnCbnSelchangeProductCombo)
	ON_BN_CLICKED(IDC_CHECK_INCHING, &CMotionDlg::OnBnClickedCheckInching)
	ON_BN_CLICKED(IDC_BUTTON_MOV_STOP, &CMotionDlg::OnBnClickedButtonMovStop)
	ON_BN_CLICKED(IDC_BUTTON_SHOT_CALIBRATE, &CMotionDlg::OnBnClickedButtonShotCalibrate)
	ON_CBN_KILLFOCUS(IDC_COMBO_SPACING, &CMotionDlg::OnCbnKillfocusComboPace)
	ON_CBN_SELCHANGE(IDC_COMBO_POSITION_TYPE, &CMotionDlg::OnCbnSelchangePositionType)
	ON_CBN_SELCHANGE(IDC_COMBO_CHECKTYPE, &CMotionDlg::OnCbnSelchangeCombo_VisionCheckOrder)
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_USER_UPDATEUI, &CMotionDlg::OnUpdateDlg)
END_MESSAGE_MAP()


// CMotionDlg 消息处理程序

BOOL CMotionDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	m_cPaceChs.SetCurSel(6);
	m_cVisionOrderChs.SetCurSel(0);
	m_cPositionChs.SetCurSel(0);
	m_lStatus[6].SetBkColor(CBCGPColor::GreenYellow);
	((CButton*)GetDlgItem(IDC_CHECK_INCHING))->SetCheck(m_bInching);
	return TRUE;
}

void CMotionDlg::OnBnClickedMove(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int nDir(0);
	if (!m_bInching)
		return;
	switch (idCtl)
	{
	case IDC_BUTTON_MOVE_XR:
		nDir = 1;
	case IDC_BUTTON_MOVE_XL:
		m_nCurrentAxis = 0;

		break;
	case IDC_BUTTON_MOVE_LYD:
		nDir = 1;
	case IDC_BUTTON_MOVE_LYU:
		m_nCurrentAxis = 1;
		break;
	case IDC_BUTTON_MOVE_RYD:
		nDir = 1;
	case IDC_BUTTON_MOVE_RYU:
		m_nCurrentAxis = 2;
		break;
	case IDC_BUTTON_MOVE_ZD:
		nDir = 1;
		if (m_fPace > 10)
		{
			AfxMessageBox(_T("该轴限制步进长度不超过10mm，为了安全请减小步进长度"));
			return;
		}
	case IDC_BUTTON_MOVE_ZU:
		if (IDNO == MessageBox(_T("确定移动焊嘴高度?"), _T("重要提示"), MB_YESNO))
			return;
		m_nCurrentAxis = 3;
		break;
	case IDC_BUTTON_MOVE_LUD://CX
		nDir = 1;
	case IDC_BUTTON_MOVE_LUU:
		m_nCurrentAxis = 0x10;
		break;
	case IDC_BUTTON_MOVE_RUD://VZ
		nDir = 1;
		if (m_fPace > 10)
		{
			AfxMessageBox(_T("该轴限制步进长度不超过10mm，为了安全请减小步进长度"));
			return;
		}
	case IDC_BUTTON_MOVE_RUU:
		m_nCurrentAxis = 0x11;
		if (m_fPace>15&&IDNO == MessageBox(_T("确定移动相机轴高度?"), _T("重要提示"), MB_YESNO))
			return;
		break;
	default:
		return;
	}
	if (!pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->m_bAxisMotionDone[m_nCurrentAxis & 0x0f])
	{
		pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->AxStopDec();
		return;
	}
	double dvel = pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_MU_VEL[(m_nCurrentAxis >> 4) * 4 + (m_nCurrentAxis & 0x0f)] / 2;
	double dAcc = pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[(m_nCurrentAxis >> 4) * 4 + (m_nCurrentAxis & 0x0f)]/2;

	if (m_bInching)
		pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->Move((m_nCurrentAxis & 0x0f) + 1, ((nDir << 1) - 1)*m_fPace, dvel, dAcc, FALSE);

}

void CMotionDlg::OnBnDownMove(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int nDir(0);
	if (m_bInching || bMove||pFrame->m_nCurrentRunMode||!pFrame->m_nIsHomeEnd)
		return;
	static long c(0);
	CString str;
// 	bMove = 1;
// 	return;
// 
	if (idCtl<IDC_BUTTON_MOVE_XL || idCtl>IDC_BUTTON_MOVE_RUD||!GetDlgItem(idCtl)->IsWindowEnabled())
		return;
	switch (idCtl)
	{
	case IDC_BUTTON_MOVE_XR:
		nDir = 1;
	case IDC_BUTTON_MOVE_XL:
		m_nCurrentAxis = 0;

		break;
	case IDC_BUTTON_MOVE_LYD:
		nDir = 1;
	case IDC_BUTTON_MOVE_LYU:
		m_nCurrentAxis = 1;
		break;
	case IDC_BUTTON_MOVE_RYD:
		nDir = 1;
	case IDC_BUTTON_MOVE_RYU:
		m_nCurrentAxis = 2;
		break;
	case IDC_BUTTON_MOVE_ZD:
		nDir = 1;
	case IDC_BUTTON_MOVE_ZU:
		m_nCurrentAxis = 3;
		return;//为了安全
		break;
	case IDC_BUTTON_MOVE_LUD:
		nDir = 1;
	case IDC_BUTTON_MOVE_LUU:
		m_nCurrentAxis = 0x10;
		break;
	case IDC_BUTTON_MOVE_RUD:
		nDir = 1;
	case IDC_BUTTON_MOVE_RUU:
		m_nCurrentAxis = 0x11;
		return;//为了安全
		break;
	default:
		return;
	}
	if (!pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->m_bAxisMotionDone[m_nCurrentAxis & 0x0f])
	{
		pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->AxStopDec();
		return;
	}
	GetDlgItem(IDC_CHECK_INCHING)->EnableWindow(FALSE);

	double dvel = pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_MU_VEL[(m_nCurrentAxis >> 4) * 4 + (m_nCurrentAxis & 0x0f)] / 2;
	double dAcc = pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[(m_nCurrentAxis >> 4) * 4 + (m_nCurrentAxis & 0x0f)]/2;
	bMove = TRUE;
	m_lStatus[6].SetBkColor(CBCGPColor::DarkBlue);
	if (!pFrame->m_nIsHomeEnd)
	{
		dvel = 0.5;
		dAcc = 1;
	}
	pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->Move((m_nCurrentAxis & 0x0f) + 1, ((nDir << 1) - 1) * 650, dvel, dAcc);

}

void CMotionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPDialog::OnLButtonUp(nFlags, point);
}

void CMotionDlg::OnBnClickedCheckInching()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPEdit* pEdit(NULL);
	m_bInching = !m_bInching;
	pEdit = (CBCGPEdit*)GetDlgItem(IDC_COMBO_SPACING);
	pEdit->EnableWindow(m_bInching);

}

void CMotionDlg::OnBnClickedButtonMovStop()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->AxStopDec();
	bMove = FALSE;
}

void CMotionDlg::OnCbnKillfocusComboPace()
{
	CString str;
	float fvalue = m_fPace;
	UpdateData();
	m_cPaceChs.GetWindowText(str);
	m_fPace = _tstof(str);
	if (m_fPace > 500.0 || m_fPace < 0.001)
	{
		m_fPace = fvalue;
		str.Format(_T("%f"), m_fPace);
		m_cPaceChs.SetWindowText(str);
		UpdateData(FALSE);
	}
}

void CMotionDlg::OnBnChangeWP(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
// 	pFrame->m_nCurrentWP = m_nCurrentWP;
	PostMessage(WM_USER_UPDATEUI, 5, 0);

}

LRESULT CMotionDlg::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	CString str2 = m_nCurrentWP? _T("右边R"):_T("左边L");
	UINT nOrder(0);
	double* pdPos;

	CBCGPComboBox* pCombox;
	int i(0), j(2);
	UINT nIndex = wParam;
	UINT num;
	BOOL bEnable = pFrame->GetUserType() && !pFrame->m_nCurrentRunMode  && pFrame->m_nIsHomeEnd;

	switch (nIndex)
	{
	case 0:
		for (i = 0; i < 12;i++)
			GetDlgItem(IDC_BUTTON_MOVE_XL+i)->EnableWindow(bEnable);
		GetDlgItem(IDC_RADIO_LEFT)->EnableWindow(bEnable);
		m_cPositionChs.EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_GOPOSITION)->EnableWindow(bEnable);
		j = m_cPositionChs.GetCurSel();
		m_cVisionOrderChs.EnableWindow(j == 6 ? bEnable : FALSE);
		GetDlgItem(IDC_COMBO_PRODUCT_ROW)->EnableWindow(j == 6 ? bEnable : FALSE);
		GetDlgItem(IDC_COMBO_PRODUCT_COL)->EnableWindow(j == 7 ? bEnable : FALSE);
		GetDlgItem(IDC_BUTTON_SHOT_CALIBRATE)->EnableWindow(j == 4 ? bEnable : FALSE);
		GetDlgItem(IDC_BUTTON_SAVEPOSITION)->EnableWindow(bEnable);

		break;
	case 1:
		// 		m_nCurrentWP = pFrame->m_nCurrentWP;
		for (i = 0; i < 2; i++)
		{
			m_nCurrentProduct[i] = pFrame->m_nCurrentProduct[m_nCurrentWP][i];
// 			pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_ROW + i);
// 			pCombox->SetCurSel(m_nCurrentProduct[i]);

		}
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_ROW );
		pCombox->SetCurSel(m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[1]);

		m_nCurrentVisionCheck = pFrame->m_nCurrentVisionCheck[m_nCurrentWP];
		m_cVisionOrderChs.SetCurSel(m_nCurrentVisionCheck);
		UpdateData(FALSE);


		break;
	case 2:
		j = 4;
	case 3://更新坐标
		for ( i = 0; i < j; i++)
		{
			str = DOUBLE_STR07(pFrame->m_pMotionCtrller[nIndex-2]->m_dAxisCurPos[i]);
			m_lStatus[i+(nIndex&0x01)*4].SetText(str);
		}

		break;
	case 4://项目参数更新
		num = pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
		if (m_nCurrentProduct[0] >= pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0])
			m_nCurrentProduct[0] = 0;
		if (m_nCurrentProduct[1] >= pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1])
			m_nCurrentProduct[1] = 0;
		// 		for (i = 0; i < 2; i++)
		// 		{
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_ROW);
		pCombox->ResetContent();
		for (j = 0; j < num; j++)
		{
			str.Format(_T("%d"), j + 1);
			pCombox->AddString(str);
		}
		pCombox->SetCurSel(m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[1]);

		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_COL);
		pCombox->ResetContent();
		pCombox->AddString(_T("固定位置1"));
		pCombox->AddString(_T("固定位置2"));
		pCombox->AddString(_T("固定位置3"));

		for (j = 0; j < pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0]; j++)
		{
			str.Format(_T("第%d行首"), j + 1);
			pCombox->AddString(str);
			str.Format(_T("第%d行尾"), j + 1);
			pCombox->AddString(str);
		}
		pCombox->SetCurSel(0);

		// 		}
		m_cVisionOrderChs.ResetContent();
		for (j = 0; j < pFrame->m_pDoc->m_cParam.PrjCfg.uInspection; j++)
		{
			str.Format(_T("%d"), j + 1);
			m_cVisionOrderChs.AddString(str);
			if (m_nCurrentVisionCheck >= j)
				m_nCurrentVisionCheck = 0;
			m_cVisionOrderChs.SetCurSel(m_nCurrentVisionCheck);

		}
	case 5:
		j = m_cPositionChs.GetCurSel();
		GetDlgItem(IDC_BUTTON_SHOT_CALIBRATE)->EnableWindow(j == 4 ? TRUE : FALSE);
		bEnable = FALSE;
		switch (j)
		{
		case 0://Z safe position
			str.Format(_T("Z_安全高度:\n \nZ %8.3f"), pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[0]);
			m_lStatus[6].SetText(str);
			break;
		case 1://loading area
			pdPos = &pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][0];

			str.Format(_T("上料位:\n \n%sY %8.3f"), str2, pdPos[0]);
			m_lStatus[6].SetText(str);

			break;
		case 2://Nozzle Check
			str.Format(_T("焊嘴检测:\n \nX %8.3f, Z %8.3f"), pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[0], pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[1]);
			m_lStatus[6].SetText(str);
			break;
		case 3://Nozzle Clean
			str.Format(_T("焊嘴清洗:\n \nX %8.3f, Z %8.3f"), pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[1+m_nCurrentWP], pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[2]);
			m_lStatus[6].SetText(str);
			break;
		case 4://Try Check
			pdPos = &pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][1];
			str.Format(_T("校正检视:\n \nCX %8.3f, %sY %8.3f\nVZ %8.3f"), pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[3], str2, pdPos[0], pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[3]);
			m_lStatus[6].SetText(str);
			break;
		case 5://Try Shot
			pdPos = &pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][2];
			str.Format(_T("校正出球:\n \nX %8.3f, %sY %8.3f\nZ %8.3f"), pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[4], str2, pdPos[0], pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[4]);
			m_lStatus[6].SetText(str);
			break;
		case 6://Product Position
			bEnable = TRUE;
			nOrder = m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[1];
			pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.ProductArray[m_nCurrentWP][nOrder][m_nCurrentVisionCheck].EndPoint;
			str.Format(_T("产品检视:\n \nCX %8.3f, %sY %8.3f, VZ %8.3f"), pdPos[0], str2, pdPos[1], pdPos[2]);
			m_lStatus[6].SetText(str);
			break;
		case 7://测距位置
			pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_COL);
			i=pCombox->GetCurSel();
			switch (i)
			{
			case 0:
			case 1:
			case 2:
				pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingFixed[m_nCurrentWP][i];

				str.Format(_T("固定测距位置%d:\n \nCX %8.3f, %sY %8.3f\nVZ %8.3f"),i+1, pdPos[0], str2, pdPos[1], pdPos[2]);
				m_lStatus[6].SetText(str);
				break;
			case -1:
				str=_T("异常：未选择");
				m_lStatus[6].SetText(str);
				break;
			default:
				i -= 3;
				pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingRel[m_nCurrentWP][i];
				if (i&0x01)
					str.Format(_T("行尾测距位置:\n \nCX %8.3f, %sY %8.3f\nVZ %8.3f"), pdPos[0], str2, pdPos[1], pdPos[2]);
				else
					str.Format(_T("行首测距位置:\n \nCX %8.3f, %sY %8.3f\nVZ %8.3f"), pdPos[0], str2, pdPos[1], pdPos[2]);
				m_lStatus[6].SetText(str);
				break;
			}

		default:
			break;
		}
		m_cVisionOrderChs.EnableWindow(bEnable);
		GetDlgItem(IDC_COMBO_PRODUCT_ROW)->EnableWindow(bEnable);
		GetDlgItem(IDC_COMBO_PRODUCT_COL)->EnableWindow(j == 7 ? TRUE : FALSE);
	case 6:
	default:
		break;
	}
	return 0;
}
//Z Safe;Loading Area;Nozzle Check;Nozzle Clean;Cali Check;Cali Shot;Product Position;
void CMotionDlg::OnCbnSelchangePositionType()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT nIndex = m_cPositionChs.GetCurSel();
	PostMessage(WM_USER_UPDATEUI, 5, 0);
}

void CMotionDlg::OnCbnSelchangeProductCombo(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;
	pCombox = (CBCGPComboBox*)GetDlgItem(idCtl);
	UINT nIndex;
	switch (idCtl)
	{
	case IDC_COMBO_PRODUCT_ROW://产品序号
		m_nCurrentProduct[0] = pCombox->GetCurSel() / pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
		m_nCurrentProduct[1] = pCombox->GetCurSel() - m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
		pFrame->m_nCurrentProduct[m_nCurrentWP][0] = m_nCurrentProduct[0];
		pFrame->m_nCurrentProduct[m_nCurrentWP][1] = m_nCurrentProduct[1];
		break;
	case IDC_COMBO_PRODUCT_COL://测距序号
		nIndex = pCombox->GetCurSel();
		if (nIndex > 2)
			nIndex -= 3;
		pFrame->m_nCurrentRanging[m_nCurrentWP] = nIndex;
		break;
	}

	PostMessage(WM_USER_UPDATEUI, 5, 0);

}

void CMotionDlg::OnCbnSelchangeCombo_VisionCheckOrder()
{
	m_nCurrentVisionCheck = m_cVisionOrderChs.GetCurSel();
	PostMessage(WM_USER_UPDATEUI, 5, 0);

}
//Z Safe;Loading Area;Nozzle Check;Nozzle Clean;Cali Check;Cali Shot;Product Position;
void CMotionDlg::OnBnClickedButtonGetPosition(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame->m_nIsHomeEnd || pFrame->m_nCurrentRunMode)
		return;
	CBCGPComboBox* pCombox;
	BOOL	bSave(TRUE);
	UINT	nOrder(0);
	int i(0);
	double* pdPos;
	bSave = idCtl - IDC_BUTTON_GOPOSITION;
	UINT nIndex = m_cPositionChs.GetCurSel();
	if (bSave)
	{
		if (IDYES == MessageBox(_T("确定更新当前位置坐标吗?（即刻生效）"), _T("重要提示"), MB_YESNO))
		{
			switch (nIndex)
			{
			case 0://Z safe position
				pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[0] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[3];
				break;
			case 1://loading area
				pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][0] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				break;
			case 3://Nozzle Clean
				for (i = 0; i < 2; i++)
					pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[i + 1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[0];
				pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][3] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[nIndex - 1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[3];
				break;
			case 2://Nozzle Check
				pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[nIndex - 2] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[0];
				pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[nIndex - 1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[3];
				break;
			case 4://Try Check
				pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[nIndex - 1] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[0];
				pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][nIndex - 3] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[nIndex - 1] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[1];;
				break;
			case 5://Try Shot
				pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[nIndex - 1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[0];
				pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][nIndex - 3] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[nIndex - 1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[3];
				break;
			case 6://Product Position
				nOrder = m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[1];
				pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.ProductArray[m_nCurrentWP][nOrder][m_nCurrentVisionCheck].EndPoint;
				pdPos[0] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[0];
				pdPos[1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				pdPos[2] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[1];
				if (!nOrder&&IDYES == MessageBox(_T("首位置更新完成，需要自动计算后序产品排布位置坐标吗?"), _T("重要提示"), MB_YESNO))
				{
					pFrame->AutoCalculatePos1(m_nCurrentWP);
					pFrame->AutoCalculatePos2(m_nCurrentWP);
				}
				break;
			case 7://测距
				pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_PRODUCT_COL);
				i = pCombox->GetCurSel();
				switch (i)
				{
				case 0:
				case 1:
				case 2:
					pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingFixed[m_nCurrentWP][i];
					break;
				case -1:
					return;
				default:
					pdPos = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingRel[m_nCurrentWP][i-3];
					break;
				}
				pdPos[0] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[0];
				pdPos[1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[m_nCurrentWP + 1];
				pdPos[2] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[1];
				if ((4==i||i==3)&&IDYES == MessageBox(_T("首行测距位置更新完成，需要自动计算后序行测距位置坐标吗?"), _T("重要提示"), MB_YESNO))
				{
					pFrame->AutoCalculatePos2(m_nCurrentWP);
				}

			default:
				break;
			}
			PostMessage(WM_USER_UPDATEUI, 5, 0);
		}
	}
	else {
		if (IDYES == MessageBox(_T("确定移动至该位置吗?"), _T("重要提示"), MB_YESNO))
		{
			switch (nIndex)
			{
			case 7://测距
			case 1://loading area
			case 4://Try Check
			case 6://Product Position
				if (!pFrame->IsZSafe())
				{
					pFrame->ShowPopup(_T("Z轴在不安全位置，请先抬升到安全高度！"));
					break;
				}
			case 0://Z safe position
			case 2://Nozzle Check
			case 3://Nozzle Clean
			case 5://Try Shot
				pFrame->StartManualRunThread(m_nCurrentWP, nIndex);
				break;
			default:break;
			}
		}
	}
}

void CMotionDlg::OnBnClickedButtonShotCalibrate()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bSuccess(FALSE);

	ROI* roi = imaqCreateROI();
	UINT nCount;
	PointFloat fPt;
	Image* CurrentImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	pFrame->m_NiVision.GetImage(0, CurrentImg);
	UINT nTool = 12311;

	if (pFrame->m_NiVision.CreateROI(CurrentImg, roi,0, nTool))
	{
		nCount = imaqGetContourCount(roi);
		if (nCount)
		{
			ContourInfo2* pInf = imaqGetContourInfo2(roi, imaqGetContour(roi, 0));
			switch (pInf->type)
			{
			case IMAQ_EMPTY_CONTOUR:
				break;
			case IMAQ_POINT:
				fPt.x = pInf->structure.point->x;
				fPt.y = pInf->structure.point->y;
				break;
			case IMAQ_ROTATED_RECT:
				fPt.x = pInf->structure.rotatedRect->left + pInf->structure.rotatedRect->width / 2;
				fPt.y = pInf->structure.rotatedRect->top + pInf->structure.rotatedRect->height / 2;
			default:
				break;
			}
			if (pInf->type == IMAQ_POINT || pInf->type == IMAQ_ROTATED_RECT)
			{
				if (pFrame->m_NiVision.m_pCalibration[0])
				{
					TransformReport* pReport = imaqTransformPixelToRealWorld(pFrame->m_NiVision.m_pCalibration[0], &fPt, 1);
					fPt.x = pReport->points[0].x * pFrame->m_NiVision.m_dCalibrationUnit[0];
					fPt.y = pReport->points[0].y * pFrame->m_NiVision.m_dCalibrationUnit[0];
					imaqDispose(pReport);
				}
				pFrame->m_pDoc->m_cParam.dCCD_Nozzle[0] = pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[4] - pFrame->m_pDoc->m_cParam.PrjCfg.dXPosition[3];
				pFrame->m_pDoc->m_cParam.dCCD_Nozzle[1] = pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][2] - pFrame->m_pDoc->m_cParam.PrjCfg.dYPosition[m_nCurrentWP][1];
				pFrame->m_pDoc->m_cParam.dCCD_Nozzle[2] = pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[4] - pFrame->m_pDoc->m_cParam.PrjCfg.dZPosition[3];

				pFrame->m_pDoc->m_cParam.dCCD_Nozzle[0] -= fPt.x;
				pFrame->m_pDoc->m_cParam.dCCD_Nozzle[1] -= fPt.y;

			}
			imaqDispose(pInf);

		}
	}
	imaqDispose(CurrentImg);

	imaqDispose(roi);
}

BOOL CMotionDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		int buID;
		buID = GetWindowLong(pMsg->hwnd, GWL_ID);
		if (buID)
			OnBnDownMove(buID);
	}
	else if (pMsg->message == WM_LBUTTONUP&&bMove)
	{
		pFrame->m_pMotionCtrller[m_nCurrentAxis >> 4]->AxStopDec();
		GetDlgItem(IDC_CHECK_INCHING)->EnableWindow(TRUE);
		bMove = FALSE;
		m_lStatus[6].SetBkColor(CBCGPColor::GreenYellow);

	}

	return CBCGPDialog::PreTranslateMessage(pMsg);
}

