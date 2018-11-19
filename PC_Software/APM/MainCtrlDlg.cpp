// MainCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "MainCtrlDlg.h"
#include "MainFrm.h"
#include "modbus.h"


// CMainCtrlDlg 对话框

IMPLEMENT_DYNAMIC(CMainCtrlDlg, CBCGPDialog)

CMainCtrlDlg::CMainCtrlDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CMainCtrlDlg::IDD, pParent)
, m_nCurrentWP(0)
, bIdling(TRUE)
, bAppointByMu(FALSE)
, m_total()
, m_cur()
{
// 	EnableVisualManagerStyle(TRUE, TRUE);
// 	EnableLayout(FALSE);
	m_lQuantity = 0;
	m_nCurrentPin = 0;
	m_nCurrentProduct[0] = 0;
	m_nCurrentProduct[1] = 0;
	
}

CMainCtrlDlg::~CMainCtrlDlg()
{
	CreateLabel(0);
}

void CMainCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_WORKBENCH_LEFT, m_nCurrentWP);
	for (int i = 0; i < 3; ++i)
	{
		DDX_Control(pDX, IDC_STATIC_LASERSTS + i, m_lStatus[i]);
		//DDX_Control(pDX, IDC_STATIC_CIRCLETIME + i, m_nStatus[i]);
	}
	DDX_Control(pDX, IDC_STATIC_BUSY, m_lStatus[3]);

	DDX_Check(pDX, IDC_CHECK_IDLING, bIdling);
	DDX_Text(pDX, IDC_STATIC_QUANTITY2, m_total);
	DDX_Text(pDX, IDC_STATIC_LIFECYCLE, m_cur);
	
}


BEGIN_MESSAGE_MAP(CMainCtrlDlg, CBCGPDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_WORKBENCH_LEFT, IDC_RADIO_WORKBENCH_RIGHT, OnBnChangeWP)
	ON_BN_CLICKED(IDC_BUTTON_WORKBENCHLOAD, &CMainCtrlDlg::OnBnClickedButtonWorkbenchload)
	ON_CBN_SELCHANGE(IDC_COMBO_JOINT_ORDER, &CMainCtrlDlg::OnCbnSelchangeComboJointOrder)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_BUTTON_PAD_CHECK, IDC_BUTTON_GETPOSITION, OnBnClickedButtonGetposition)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_PUSH_BALL, IDC_BUTTON_SHOT, OnBnClickedButtonPushBall)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_BOTTOMCHECK, IDC_BUTTON_BALL_SUPPLY, OnBnClickedButtonNozzlePos)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_RESET_COUNTER, IDC_BUTTON_COUNTER_N, OnBnClickedButtonReset)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_AUTO, IDC_BUTTON_STOP, OnBnClickedButtonMain)
	ON_BN_CLICKED(IDC_CHECK_IDLING, &CMainCtrlDlg::OnBnClickedCheckIdling)
	ON_MESSAGE(WM_USER_UPDATEUI, &CMainCtrlDlg::OnUpdateDlg)
	ON_BN_CLICKED(IDC_BUTTON_REDLIGHT, &CMainCtrlDlg::OnClickedButtonRedlight)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_APPOINT, &CMainCtrlDlg::OnBnClickedCheckAppoint)
	ON_BN_CLICKED(IDC_BUTTON_FIXER, &CMainCtrlDlg::OnBnClickedButtonFixer)
END_MESSAGE_MAP()

void CMainCtrlDlg::OnBnChangeWP(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
	CString str;
// 	pFrame->m_nCurrentWP[CMainFrame::CAMERA] = m_nCurrentWP;
	if (pFrame->IsDockReady(m_nCurrentWP))
		str = _T("释放夹具");
	else
		str = _T("紧固夹具");
	SetDlgItemText(IDC_BUTTON_FIXER, str);
	PostMessage(WM_USER_UPDATEUI, 7, MAX_ProductNumber);
}


// CMainCtrlDlg 消息处理程序

BOOL CMainCtrlDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	m_lStatus[2].SetBkColor(CBCGPColor::LightGreen);
	
	
	//UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CMainCtrlDlg::OnBnClickedButtonWorkbenchload()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame->m_nIsHomeEnd || pFrame->m_nCurrentRunMode)
		return;

	if (pFrame->IsZSafe())
		pFrame->StartManualRunThread(m_nCurrentWP, 1);
	else
		pFrame->ShowPopup(_T("Z轴在不安全位置，请先抬升到安全高度！"));

}

LRESULT CMainCtrlDlg::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
		// TODO:  在此添加额外的
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;
	CString str;
	UINT num;
	UINT i(0), j(0);
	UINT nIndex = wParam;
	BOOL bEnable = pFrame->GetUserType() && !pFrame->m_nCurrentRunMode;

	switch (nIndex)
	{
	case 0://

		GetDlgItem(IDC_BUTTON_FIXER)->EnableWindow(bEnable);
		GetDlgItem(IDC_BUTTON_WORKBENCHLOAD)->EnableWindow(bEnable&&pFrame->m_nIsHomeEnd);
		GetDlgItem(IDC_BUTTON_BALL_SUPPLY)->EnableWindow(pFrame->m_nIsHomeEnd&&pFrame->GetUserType() && !(pFrame->m_nCurrentRunMode & 0x01));
		GetDlgItem(IDC_BUTTON_REDLIGHT)->EnableWindow(bEnable);
// 		GetDlgItem(IDC_BUTTON_AUTO)->EnableWindow((lParam >> 13) & 0x01);
		GetDlgItem(IDC_CHECK_IDLING)->EnableWindow(bEnable);
		GetDlgItem(IDC_CHECK_APPOINT)->EnableWindow(bEnable&&pFrame->m_nIsHomeEnd);
		for (i = 0; i < 2; i++)
		{
			GetDlgItem(IDC_RADIO_WORKBENCH_LEFT + i)->EnableWindow(bEnable);
			GetDlgItem(IDC_BUTTON_PAD_CHECK + i)->EnableWindow(bEnable&&pFrame->m_nIsHomeEnd);
			GetDlgItem(IDC_BUTTON_PUSH_BALL + i)->EnableWindow(bEnable);
			GetDlgItem(IDC_BUTTON_BOTTOMCHECK + i)->EnableWindow(bEnable&&pFrame->m_nIsHomeEnd);
		}
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_JOINT_ORDER);
		pCombox->EnableWindow(bEnable);
		break;
	case 1://
		for (i = 0; i < 2; i++)
		{
			m_nCurrentProduct[i] = pFrame->m_nCurrentProduct[m_nCurrentWP][i];

		}
		m_nCurrentPin = pFrame->m_nCurrentPin[m_nCurrentWP];
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_JOINT_ORDER);
		pCombox->SetCurSel(m_nCurrentPin);

// 		UpdateData(FALSE);
		break;
	case 2:
		num = pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0]*pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
		m_pStatusdata_left.resize(num);
		m_pStatusdata_right.resize(num);
		Typesetting(pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[0], pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1]);
		for (i = 0; i < 2; i++)
		{
			if (m_nCurrentProduct[i] >= pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[i])
				m_nCurrentProduct[i] = 0;
		}
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_JOINT_ORDER);
		pCombox->ResetContent();
		for (j = 0; j < pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum; j++)
		{
			str.Format(_T("%d"), j + 1);
			pCombox->AddString(str);
		}
		if (m_nCurrentPin >= j)
			m_nCurrentPin = 0;
		pCombox->SetCurSel(m_nCurrentPin);

		break;
	case 3:
		
		str.Format(_T("%5.2f s"), (double)lParam/1000.0);
		m_nStatus[0].SetText(str);
		
		break;
	case 4:
		if (lParam)
			m_lQuantity++;
		str.Format(_T("%d"), m_lQuantity);
		m_nStatus[1].SetText(str);
		break;
	case 5:
		j = pFrame->m_pDoc->m_cParam.nNozzleLife[0];
		str.Format(_T("%d"), j);
		m_nStatus[2].SetText(str);
		m_nStatus[2].FlashText(!j);
		m_nStatus[2].SetBkColor(j ? CBCGPColor::LightGreen:CBCGPColor::MediumSlateBlue  );

		break;
	case 6:
		if (lParam & 0x01)
		{
			str = _T("已回零");
			m_lStatus[0].SetBkColor(CBCGPColor::LightGreen);
		}
		else
		{
			str = _T("未回零");
			m_lStatus[0].SetBkColor(CBCGPColor::MediumSlateBlue);
		}
		m_lStatus[0].SetText(str);
		if (lParam >>1)
		{
			if(lParam&0x02)
				str = _T("无球");
			else if (lParam & 0x08)
				str = _T("清洗失败");
			else if (lParam & 0x04)
				str = _T("堵球");
			else
				str = _T("未知异常");

			m_lStatus[1].FlashText(TRUE);
			m_lStatus[1].SetBkColor(CBCGPColor::MediumSlateBlue);

		}
		else
		{
			str = _T("等待送球");
			m_lStatus[1].FlashText(FALSE);
			m_lStatus[1].SetBkColor(CBCGPColor::LightGreen);

		}
		m_lStatus[1].SetText(str);

// 		j = lParam ^ 0x04;
// 		m_lStatus[2].FlashText(j);
// 		m_lStatus[2].SetBkColor(j ? CBCGPColor::MediumSlateBlue : CBCGPColor::LightGreen);
		break;
	case 7:////更新指示器状态
		num = m_plStatus.size();
		if (lParam < MAX_ProductNumber && lParam < num)
			i = lParam;
		do
		{
			BYTE bsts;
			COLORREF crbk(0xF0FAFF);
			if (m_nCurrentWP)
				bsts = m_pStatusdata_right.at(i);
			else
				bsts = m_pStatusdata_left.at(i);
			switch (bsts >> 1)
			{
			case 0://初始
				break;
			case 1://定位失败
			case 3://部分失败
				crbk = 0x0000FF;
				break;
			case 2://定位成功
				crbk = 0xffff00;
				break;
			case 4://焊接成功
				crbk = 0x00ff00;
				break;
			case 5://部分失败
			default:
				crbk = 0x0000FF;
				break;
			}
			m_plStatus.at(i)->SetBkColor(crbk);
			m_plStatus.at(i)->FlashBackground(bsts & 0x01);
			i++;
		} while ((i < num) && (lParam >= MAX_ProductNumber || lParam >= num));
		break;
	case  8:
		if (lParam ^ 0x01)
		{
			m_lStatus[2].FlashText(TRUE);
			m_lStatus[2].SetBkColor(CBCGPColor::MediumSlateBlue);
		}
		else
		{
			m_lStatus[2].FlashText(FALSE);
			m_lStatus[2].SetBkColor(CBCGPColor::LightGreen);
		}

		break;
	case 9:
		m_cur = pFrame->m_LaserCtrl.GetCurBalls();
		m_total = pFrame->m_LaserCtrl.GetTotalBalls();
		UpdateData(FALSE);
		break;
	}
	return 0;
}

void CMainCtrlDlg::OnCbnSelchangeProductCombo(UINT idCtl)
{
}


void CMainCtrlDlg::OnCbnSelchangeComboJointOrder()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_JOINT_ORDER);
	m_nCurrentPin = pCombox->GetCurSel();
	pFrame->m_nCurrentPin[m_nCurrentWP] = m_nCurrentPin;
}

void CMainCtrlDlg::OnBnClickedButtonGetposition(UINT idCtl)
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame->m_nIsHomeEnd || pFrame->m_nCurrentRunMode)
		return;

	BOOL bSuccess(FALSE);
	CBCGPComboBox* pCombox;
	CString Tip = _T("确定移动焊嘴至产品上？");
	switch (idCtl)
	{
	case IDC_BUTTON_PAD_CHECK:
		pFrame->SwitchCamera(0);
		pFrame->WorkProcess(m_nCurrentWP, CMainFrame::SNAPSHOT_MATCH);
		break;
	case IDC_BUTTON_GETPOSITION:
		if (IDYES == MessageBox(Tip, _T("重要提示-重要提示"), MB_YESNO))
		{

			pFrame->WorkProcess(m_nCurrentWP, CMainFrame::BONDING_READY);
		}
		break;

	}
}


void CMainCtrlDlg::OnBnClickedButtonPushBall(UINT idCtl)
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;
	if (!pFrame->m_LaserCtrl.GetPLCRDY())
	{
		pFrame->AddedErrorMsg(_T("PLC 未准备好\r\n"));
		return ;
	}
	GetDlgItem(idCtl)->EnableWindow(FALSE);
	switch (idCtl)
	{
	case IDC_BUTTON_PUSH_BALL:
		pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[0], FALSE);
		pFrame->m_LaserCtrl.SetBallRDY();
		break;
	case IDC_BUTTON_SHOT:
		//！check位不能出光！
		if (!pFrame->IsAboveCamera()||pFrame->GetUserType()==2)
		{
			pFrame->m_LaserCtrl.SetLaserRDY();
			pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[0], FALSE);
			pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[2], TRUE);
			if (pFrame->m_pDoc->m_cParam.nNozzleLife[0])
			{
				pFrame->m_pDoc->m_cParam.nNozzleLife[0]--;
				PostMessage(WM_USER_UPDATEUI, 5, 0);
			}

		}
		break;
	}
	UINT n=SetTimer(1, 500, NULL);
	if (n ^ 1)
		AfxMessageBox(_T("手动操作:定时器1启动异常"));

}

void CMainCtrlDlg::OnBnClickedButtonNozzlePos(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame->m_nIsHomeEnd||pFrame->m_nCurrentRunMode)
		return;
	int nId(0);
	switch (idCtl)
	{
	case IDC_BUTTON_BOTTOMCHECK:
		pFrame->SwitchCamera(1);
		pFrame->StartManualRunThread(m_nCurrentWP, 2);

		break;
	case IDC_BUTTON_BALL_SUPPLY:
		if (pFrame->m_bAutoThreadAlive)
			pFrame->m_hBondRecovery.PulseEvent();
		else if (pFrame->GetUserType())
		{
			if (IDYES == MessageBox(_T("确认移动至加锡球位置，等待加球？"), _T("重要提示-重要提示"), MB_YESNO))
			{

				pFrame->WorkProcess(m_nCurrentWP, CMainFrame::BALLS_SUPPLY);
			}
		}
		break;
	case IDC_BUTTON_NOZZLE_CLEAN:
		pFrame->WorkProcess(m_nCurrentWP, CMainFrame::NOZZLE_CLEAN);
		break;
	}

}

void CMainCtrlDlg::OnBnClickedButtonReset(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	switch (idCtl)
	{
	case IDC_BUTTON_RESET_COUNTER:
		m_lQuantity = 0;
		PostMessage(WM_USER_UPDATEUI, 4, 0);
		break;
	case IDC_BUTTON_COUNTER_N:
		pFrame->m_pDoc->m_cParam.nNozzleLife[0] = pFrame->m_pDoc->m_cParam.nNozzleLife[1];
		PostMessage(WM_USER_UPDATEUI, 5, 0);
		break;
	default:
		return;
	}


}

void CMainCtrlDlg::OnBnClickedButtonMain(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	switch (idCtl)
	{
	case IDC_BUTTON_AUTO:
		if (!pFrame->m_nIsHomeEnd || pFrame->m_nCurrentRunMode)
			return;

		pFrame->StartAuto(TRUE);
		break;
	case IDC_BUTTON_STOP:
		pFrame->StopCommand();
		break;
	}


}


void CMainCtrlDlg::OnBnClickedCheckIdling()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
	pFrame->m_bIdling = bIdling;
}


void CMainCtrlDlg::OnClickedButtonRedlight()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bon = pFrame->m_IoCtrller.GetDOBit(pFrame->m_pDoc->m_cParam.Ou_Welding[3]);
	pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[3], !bon);
	if (bon)
	{
		SetDlgItemText(IDC_BUTTON_REDLIGHT,_T("开红光"));
	}else
		SetDlgItemText(IDC_BUTTON_REDLIGHT,_T("关红光"));

}


void CMainCtrlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	static int num(4);
	switch (nIDEvent)
	{
	case 1:
		if (!num||pFrame->m_IoCtrller.m_bInput[pFrame->m_pDoc->m_cParam.In_PLC_Input[0]])
		{
			if (!pFrame->m_IoCtrller.m_bInput[pFrame->m_pDoc->m_cParam.In_PLC_Input[0]])
				pFrame->AddedErrorMsg(_T("动作超时\r\n"));
			pFrame->m_LaserCtrl.SetBallRDY(FALSE);
			//
			pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[2], FALSE);
			//
			GetDlgItem(IDC_BUTTON_PUSH_BALL)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SHOT)->EnableWindow(TRUE);
			num = 4;
			KillTimer(nIDEvent);
		}else
			num--;

		break;
	case 2:
		break;
	}
	CBCGPDialog::OnTimer(nIDEvent);
}


void CMainCtrlDlg::OnBnClickedCheckAppoint()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bSuccess(FALSE);
	UINT nCurl = m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[1];
	BYTE nNum = pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[pFrame->m_nCurrentPin[m_nCurrentWP]].uVisionIndex;
	if (!nNum)
		return;
	if (pFrame->m_nCurrentVisionCheck[m_nCurrentWP] != (nNum - 1))
	{
		pFrame->ShowPopup(_T("不在当前焊点照相位，先尝试自动视觉定位！"));
		return;
	}
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
				pFrame->m_nPinVisionSuccess[m_nCurrentWP][nCurl][m_nCurrentPin] = TRUE;
				pFrame->m_dLaserOffset[m_nCurrentWP][nCurl][m_nCurrentPin][0] = fPt.x;
				pFrame->m_dLaserOffset[m_nCurrentWP][nCurl][m_nCurrentPin][1] = fPt.y;
				bAppointByMu = TRUE;
			}
			imaqDispose(pInf);

		}
	}
	imaqDispose(CurrentImg);

	imaqDispose(roi);

}

BOOL CMainCtrlDlg::CreateLabel(BYTE nCount)
{
	CString info;
	BOOL bSuccess(TRUE);
	BYTE nSize = m_plStatus.size();
	if (nSize < nCount)
	{
		for (BYTE i = m_plStatus.size(); i < nCount; i++)
		{
			info.Format(_T("产品%d"), i + 1);
			CLabel* pLabel = new CLabel;
			DWORD dwStyle = (WS_CHILD | WS_BORDER | SS_CENTER | SS_NOTIFY | SS_CENTERIMAGE)&(~WS_VISIBLE);
			if (!pLabel || !pLabel->Create(info, dwStyle, CRect(10, 10, 100, 80), this, IDC_STATIC_STATE_P1 + i))
			{
				bSuccess = FALSE;
				break;
			}
			m_plStatus.push_back(pLabel);
			pLabel->SetFontSize(6, FALSE);
			pLabel->SetFontName(_T("微软雅黑"));
			pLabel->SetBkColor(0xF0FAFF);
		}
	}
	else
	{
		for (BYTE i = nCount; i < nSize; i++)
		{
			SAFE_DELETEDLG(m_plStatus.back());
			m_plStatus.pop_back();
		}
	}
	return bSuccess;

}

void CMainCtrlDlg::Typesetting(BYTE nRow, BYTE nCol)
{
	CRect rc[2];
	UINT nWidth, nHeight;
	UINT nRowstep, nColstep;
	UINT nStart;
	if (nRow*nCol > MAX_ProductNumber || nRow*nCol == 0)
		return;
	if (CreateLabel(nRow*nCol))
	{
		GetDlgItem(IDC_STATIC_PRODUCT)->GetWindowRect(&rc[0]);
		m_plStatus.at(0)->GetWindowRect(&rc[1]);
		rc[0].DeflateRect(5, 35, 0, 10);
		nWidth = rc[0].Width();
		nHeight = rc[0].Height();
		ScreenToClient(rc[0]);
		if (theApp.m_bSortingDir & 0x01)
		{
			nRowstep = nHeight / nRow;
			nColstep = nWidth / nCol;
			rc[1].DeflateRect(0, 0, rc[1].Width() - nColstep * 9.0/10, rc[1].Height() - nRowstep*9.0 / 10);
			switch (theApp.m_bSortingDir >> 1)
			{
			case 0:
				/********/
				/* 123  */
				/* 654  */
				/* 789  */
				/********/
				for (int i = 0; i < nRow; i++)
				{
					nStart = rc[0].top + i*nRowstep;
					for (int j = 0; j < nCol; j++)
					{
						if (i&0x01)
							rc[1].MoveToXY(rc[0].left + nColstep*(nCol-j-1), nStart);
						else
							rc[1].MoveToXY(rc[0].left + nColstep*j, nStart);
						m_plStatus.at(i*nCol + j)->MoveWindow(rc[1]);
						m_plStatus.at(i*nCol + j)->ShowWindow(TRUE);

					}
				}
				break;
			case 1:
				/********/
				/* 123  */
				/* 456  */
				/* 789  */
				/********/
				for (int i = 0; i < nRow; i++)
				{
					nStart = rc[0].top + i*nRowstep;
					for (int j = 0; j < nCol; j++)
					{
						rc[1].MoveToXY(rc[0].left + nColstep*j, nStart);
						m_plStatus.at(i*nCol + j)->MoveWindow(rc[1]);
						m_plStatus.at(i*nCol + j)->ShowWindow(TRUE);
					}
				}
				break;
			}
		}
		else
		{
			nRowstep = nWidth / nRow;
			nColstep = nHeight / nCol;
			rc[1].DeflateRect(0, 0, rc[1].Width() - nRowstep + 20, rc[1].Height() - nColstep + 10);
			switch (theApp.m_bSortingDir >> 1)
			{
			case 0:
				/********/
				/* 963  */
				/* 852  */
				/* 741  */
				/********/
				for (int i = 0; i < nRow; i++)
				{
					UINT nStart = rc[0].right - (i + 1)*nRowstep;
					for (int j = 0; j < nCol; j++)
					{
						rc[1].MoveToXY(nStart, rc[0].bottom - nColstep*(j + 1));
						m_plStatus.at(i*nCol + j)->MoveWindow(rc[1]);
						m_plStatus.at(i*nCol + j)->ShowWindow(TRUE);
					}
				}

				break;
			case 1:
				/********/
				/* 369  */
				/* 258  */
				/* 147  */
				/********/
				for (int i = 0; i < nRow; i++)
				{
					UINT nStart = rc[0].left + i*nRowstep;
					for (int j = 0; j < nCol; j++)
					{
						rc[1].MoveToXY(nStart, rc[0].bottom - nColstep*(j + 1));
						m_plStatus.at(i*nCol + j)->MoveWindow(rc[1]);
						m_plStatus.at(i*nCol + j)->ShowWindow(TRUE);
					}
				}
				break;
			case 2:
				/********/
				/* 741  */
				/* 852  */
				/* 963  */
				/********/
				break;
			case 3:
				/********/
				/* 147  */
				/* 258  */
				/* 369  */
				/********/
				break;
			}

		}
		InvalidateRect(rc[0]);
	}

}

void CMainCtrlDlg::OnUpdateSts(BYTE nWp, UINT nNum, BYTE nSts, BYTE bMode)
{
	UINT i(0);
	if (!nWp)
	{
		if (nNum >= MAX_ProductNumber || nNum >= m_pStatusdata_left.size())
		{
			for (i = 0; i < m_pStatusdata_left.size();i++)
			{
				switch (bMode)
				{
				case 0://设置
					m_pStatusdata_left.at(i) = nSts;
					break;
				case 1://加
					m_pStatusdata_left.at(i) |= nSts;
					break;
				case 2://减
					m_pStatusdata_left.at(i) ^= (m_pStatusdata_left.at(i)&nSts);
					break;
				}
			}
			
		}
		else
		{
			switch (bMode)
			{
			case 0://设置
				m_pStatusdata_left.at(nNum) = nSts;
				break;
			case 1://加
				m_pStatusdata_left.at(nNum) |= nSts;
				break;
			case 2://减
				m_pStatusdata_left.at(nNum) ^= (m_pStatusdata_left.at(nNum)&nSts);
				break;
			}
		}
	}
	else
	{
		if (nNum >= MAX_ProductNumber || nNum >= m_pStatusdata_right.size())
		{
			for (i = 0; i < m_pStatusdata_right.size(); i++)
			{
				switch (bMode)
				{
				case 0://设置
					m_pStatusdata_right.at(i) = nSts;
					break;
				case 1://加
					m_pStatusdata_right.at(i) |= nSts;
					break;
				case 2://减
					m_pStatusdata_right.at(i) ^= (m_pStatusdata_right.at(i)&nSts);
					break;
				}
			}

		}
		else
		{
			switch (bMode)
			{
			case 0://设置
				m_pStatusdata_right.at(nNum) = nSts;
				break;
			case 1://加
				m_pStatusdata_right.at(nNum) |= nSts;
				break;
			case 2://减
				m_pStatusdata_right.at(nNum) ^= (m_pStatusdata_right.at(nNum)&nSts);
				break;
			}
		}

	}
	if (nWp == m_nCurrentWP)
		PostMessage(WM_USER_UPDATEUI, 7, nNum);
}

BOOL CMainCtrlDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pFrame->m_bAutoThreadAlive)
	{
		int buID;
		if (pMsg->message == WM_LBUTTONDBLCLK)//
		{
			buID = GetWindowLong(pMsg->hwnd, GWL_ID);
			SelchangeProduct(buID, FALSE);
		}
		else if (pMsg->message == WM_RBUTTONDBLCLK)
		{
			buID = GetWindowLong(pMsg->hwnd, GWL_ID);
			SelchangeProduct(buID, TRUE);

		}
	}
	return CBCGPDialog::PreTranslateMessage(pMsg);
}

void CMainCtrlDlg::SelchangeProduct(UINT idCtl, BOOL breset)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (idCtl > (IDC_STATIC_STATE_P1 - 1) && idCtl < (IDC_STATIC_STATE_P1 + m_plStatus.size()))
	{
		if (!breset)
		{
			m_nCurrentProduct[0] = (idCtl - IDC_STATIC_STATE_P1) / pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
			m_nCurrentProduct[1] = (idCtl - IDC_STATIC_STATE_P1) - m_nCurrentProduct[0] * pFrame->m_pDoc->m_cParam.PrjCfg.bProductNum[1];
			pFrame->m_nCurrentProduct[m_nCurrentWP][0] = m_nCurrentProduct[0];
			pFrame->m_nCurrentProduct[m_nCurrentWP][1] = m_nCurrentProduct[1];
			OnUpdateSts(m_nCurrentWP, MAX_ProductNumber, FLASH_ENABLE, ERASING_STS);
			OnUpdateSts(m_nCurrentWP, idCtl - IDC_STATIC_STATE_P1, FLASH_ENABLE, ADD_STS);
		}else
			OnUpdateSts(m_nCurrentWP);
	}
}


void CMainCtrlDlg::OnBnClickedButtonFixer()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	GetDlgItemText(IDC_BUTTON_FIXER,str);
	if (str == _T("紧固夹具"))
	{
		if (pFrame->BlowAct(m_nCurrentWP, TRUE))
		{
			str = _T("释放夹具");
			SetDlgItemText(IDC_BUTTON_FIXER, str);
		}
	}
	else{
		if (pFrame->BlowAct(m_nCurrentWP, FALSE))
		{
			str = _T("紧固夹具");
			SetDlgItemText(IDC_BUTTON_FIXER, str);
		}
	}
}


void CMainCtrlDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

// 	CBCGPDialog::OnCancel();
}


void CMainCtrlDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类

// 	CBCGPDialog::OnOK();
}
