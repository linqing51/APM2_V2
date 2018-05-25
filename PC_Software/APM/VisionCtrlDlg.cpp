// VisionCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "VisionCtrlDlg.h"
#include "MainFrm.h"


// CVisionCtrlDlg 

IMPLEMENT_DYNAMIC(CVisionCtrlDlg, CBCGPDialog)

CVisionCtrlDlg::CVisionCtrlDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CVisionCtrlDlg::IDD, pParent)
, m_bShowContour(FALSE), m_bUseBinary(FALSE)
{
// 	EnableVisualManagerStyle(TRUE, TRUE);
// 	EnableLayout(FALSE);
	m_bUseMask = FALSE;
	m_nCurrentSearch = 0;
	m_nCurrentTM = 0;
	m_nCurrentSS = 0;
	m_nCurrentPin = 0;
}

CVisionCtrlDlg::~CVisionCtrlDlg()
{
}

void CVisionCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_MASK, m_bUseMask);
	DDX_Text(pDX, IDC_EDIT_CONTRAST, m_nTMContrast);
	DDX_Text(pDX, IDC_EDIT_STRENGTH, m_nSSStrength);
	DDX_Text(pDX, IDC_EDIT_CIRCULARGAP, m_nSSGap);
	DDX_Text(pDX, IDC_EDIT_SCORES, m_nTMScore);
	DDX_Check(pDX, IDC_CHECK_CONTOUR, m_bShowContour);
	DDX_Check(pDX, IDC_CHECK_BINARY, m_bUseBinary);
}


BEGIN_MESSAGE_MAP(CVisionCtrlDlg, CBCGPDialog)
	ON_MESSAGE(WM_USER_UPDATEUI, &CVisionCtrlDlg::OnUpdateDlg)
	ON_CONTROL_RANGE(CBN_SELCHANGE,IDC_COMBO_SEARCHAREA, IDC_COMBO_JOINTS,OnCbnSelchangeComboPinSearch)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBO_ORDER_TM, IDC_COMBO_TM_TYPE, OnCbnSelchangeTMCombo)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_COMBO_ORDER_SS, IDC_COMBO_SS_TYPE, OnCbnSelchangeSSCombo)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_CONTRAST, IDC_EDIT_SCORES, OnKillFocusTMEdit)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_STRENGTH, IDC_EDIT_TARGET_MAXH, OnKillFocusSSEdit)
	ON_BN_CLICKED(IDC_BUTTON_MATCH, &CVisionCtrlDlg::OnBnClickedButtonMatch)
	ON_BN_CLICKED(IDC_BUTTON_FINDTEST, &CVisionCtrlDlg::OnBnClickedButtonFindtest)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_BUTTON_SEARCHRECT, IDC_BUTTON_TEMPLATERECT, OnImageEditTool)
	ON_CBN_SELCHANGE(IDC_COMBO_CIRCULARDIRECT, &CVisionCtrlDlg::OnCbnSelchangeComboCirculardirect)
	ON_BN_CLICKED(IDC_CHECK_CONTOUR, &CVisionCtrlDlg::OnBnClickedCheckContour)
	ON_BN_CLICKED(IDC_CHECK_BINARY, &CVisionCtrlDlg::OnBnClickedCheckBinary)
END_MESSAGE_MAP()


// CVisionCtrlDlg 消息处理程序


void CVisionCtrlDlg::OnCbnSelchangeComboPinSearch(UINT idCtl)
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;

 
	pCombox = (CBCGPComboBox*)GetDlgItem(idCtl);
	BYTE nSelec = pCombox->GetCurSel();
	switch (idCtl)
	{
	case IDC_COMBO_SEARCHAREA:
		m_nCurrentSearch = nSelec;
		pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin].uVisionType.nSearchNum = nSelec;
		break;
	case IDC_COMBO_JOINTS:
		m_nCurrentPin = nSelec;
		PostMessage(WM_USER_UPDATEUI, 5, 0);
		break;
	}

}

BOOL CVisionCtrlDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	CBCGPComboBox* pComboxTM = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_ORDER_TM);
	pComboxTM->ResetContent();
	CBCGPComboBox* pComboxSS = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_ORDER_SS);
	pComboxSS->ResetContent();
	for (int i = 1; i < 17; i++)
	{
		str.Format(_T("%d"), i);
		pComboxTM->AddString(str);
		pComboxSS->AddString(str);
	}
	pComboxTM->SetCurSel(m_nCurrentTM);
	pComboxSS->SetCurSel(m_nCurrentSS);

	return TRUE;
}

LRESULT CVisionCtrlDlg::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	CBCGPComboBox* pCombox;
	UINT i(0), j(2);
	UINT nIndex = wParam;
	BOOL bEnable = pFrame->GetUserType() && !pFrame->m_nCurrentRunMode ;

	switch (nIndex)
	{
	case 0:
		GetDlgItem(IDC_BUTTON_TEMPLATERECT)->EnableWindow(bEnable&&lParam);
		GetDlgItem(IDC_BUTTON_MATCH)->EnableWindow(bEnable&&lParam);

		break;
	case 1:
		break;
	case 2:
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_JOINTS);
		pCombox->ResetContent();
		for (j = 0; j < pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum; j++)
		{
			str.Format(_T("%d"), j + 1);
			pCombox->AddString(str);
		}
		if (m_nCurrentPin >= j)
			m_nCurrentPin = j-1;
		pCombox->SetCurSel(m_nCurrentPin);


		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_SEARCHAREA);
		pCombox->ResetContent();
		for (j = 0; j < pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum; j++)
		{
			str.Format(_T("%d"), j + 1);
			pCombox->AddString(str);
		}
		if (m_nCurrentSearch >= j)
			m_nCurrentSearch = j - 1;
		pCombox->SetCurSel(m_nCurrentSearch);
		break;
	case 3:
		j = pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].bType;
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_TM_TYPE);
		pCombox->SetCurSel(j);
		GetDlgItem(IDC_EDIT_CONTRAST)->EnableWindow(j);

		m_nTMContrast = pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].nContrast;
		m_nTMScore = pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].nScore;
		break;
	case 4:
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_SS_TYPE);
		pCombox->SetCurSel(pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].bType);
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_CIRCULARDIRECT);
		pCombox->SetCurSel(pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nDir);
		m_nSSStrength = pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nEdgeStrength;
		m_nSSGap = pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nGap;
		m_bUseBinary = pFrame->m_pDoc->m_cParam.PrjCfg.bUseBinary;
		UpdateData(FALSE);
		str.Format(_T("%d"), pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nWidth);
		GetDlgItem(IDC_EDIT_CIRCULARWIDTH)->SetWindowText(str);
		str.Format(_T("%d"), pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nKernelSize);
		GetDlgItem(IDC_EDIT_CIRCULARKERNEL)->SetWindowText(str);
		str.Format(_T("%0.f"), pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].maxHeight);
		GetDlgItem(IDC_EDIT_TARGET_MAXH)->SetWindowText(str);
		str.Format(_T("%0.f"), pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].minHeight);
		GetDlgItem(IDC_EDIT_TARGET_MINH)->SetWindowText(str);
		str.Format(_T("%0.f"), pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].maxWidth);
		GetDlgItem(IDC_EDIT_TARGET_MAXW)->SetWindowText(str);
		str.Format(_T("%0.f"), pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].minWidth);
		GetDlgItem(IDC_EDIT_TARGET_MINW)->SetWindowText(str);
		break;
	case 5:
		j=pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin].uVisionType.nSearchNum;
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_SEARCHAREA);
		if (j >= pFrame->m_pDoc->m_cParam.PrjCfg.nPinNum)
		{
			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin].uVisionType.nSearchNum = m_nCurrentPin;
			j = m_nCurrentPin;
		}
		pCombox->SetCurSel(j);
		break;
	}
	return 0;
}

void CVisionCtrlDlg::OnCbnSelchangeTMCombo(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int sts(FALSE);
	CBCGPComboBox* pCombox;
	pCombox = (CBCGPComboBox*)GetDlgItem(idCtl);
	switch (idCtl)
	{
	case IDC_COMBO_ORDER_TM:
		m_nCurrentTM = pCombox->GetCurSel();
		PostMessage(WM_USER_UPDATEUI, 3, 0);

		break;
	case IDC_COMBO_TM_TYPE:
		sts = pCombox->GetCurSel();
		pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].bType = sts;
		GetDlgItem(IDC_EDIT_CONTRAST)->EnableWindow(sts);

		break;
	}

}

void CVisionCtrlDlg::OnCbnSelchangeSSCombo(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;
	pCombox = (CBCGPComboBox*)GetDlgItem(idCtl);
	switch (idCtl)
	{
	case IDC_COMBO_ORDER_SS:
		m_nCurrentSS = pCombox->GetCurSel();
		PostMessage(WM_USER_UPDATEUI, 4, 0);

		break;
	case IDC_COMBO_SS_TYPE:

		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].bType = pCombox->GetCurSel();
		break;
	case IDC_COMBO_CIRCULARDIRECT:
		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nDir = pCombox->GetCurSel();

		break;
	}

}

void CVisionCtrlDlg::OnKillFocusTMEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT num;
	switch (idCtl)
	{
	case IDC_EDIT_CONTRAST:
		num = m_nTMContrast;
		UpdateData();
		if (m_nTMContrast > 255)
		{
			m_nTMContrast = num;
			UpdateData(FALSE);
		}
		pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].nContrast = m_nTMContrast;
		if (m_bShowContour)
		{
			ShowContour(0, m_nTMContrast);
		}

		break;
	case IDC_EDIT_SCORES:
		num = m_nTMScore;
		UpdateData();
		if (m_nTMScore > 1000)
		{
			m_nTMScore = num;
			UpdateData(FALSE);
		}

		pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].nScore = m_nTMScore;
		break;
	}

}

void CVisionCtrlDlg::OnKillFocusSSEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	UINT num;
	switch (idCtl)
	{
	case IDC_EDIT_STRENGTH:
		num = m_nSSStrength;
		UpdateData();
		if (m_nSSStrength > 255)
		{
			m_nSSStrength = num;
			UpdateData(FALSE);
		}
		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nEdgeStrength = m_nSSStrength;

		break;
	case IDC_EDIT_CIRCULARGAP:
		num = m_nSSGap;
		UpdateData();
		if (m_nSSGap > 255)
		{
			m_nSSGap = num;
			UpdateData(FALSE);
		}
		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nGap = m_nSSGap;

		break;
	case IDC_EDIT_CIRCULARWIDTH:
		GetDlgItemText(idCtl, str);
		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nWidth = _ttoi(str);
		break;
	case IDC_EDIT_CIRCULARKERNEL:
		GetDlgItemText(idCtl, str);

		pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nKernelSize = _ttoi(str);
		break;
	case IDC_EDIT_TARGET_MAXH:
		GetDlgItemText(idCtl, str);
		pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].maxHeight = _ttoi(str);

		break;
	case IDC_EDIT_TARGET_MAXW:
		GetDlgItemText(idCtl, str);
		pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].maxWidth = _ttoi(str);

		break;
	case IDC_EDIT_TARGET_MINH:
		GetDlgItemText(idCtl, str);
		pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].minHeight = _ttoi(str);
		break;
	case IDC_EDIT_TARGET_MINW:
		GetDlgItemText(idCtl, str);
		pFrame->m_pDoc->m_cParam.PrjCfg.rcMinMax[m_nCurrentSS].minWidth = _ttoi(str);
		break;
	}
	if (m_bShowContour)
	{
		ShowContour(pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].bType, m_nSSStrength, m_nSSGap);
	}

}

void CVisionCtrlDlg::OnImageEditTool(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox;
	CString str;
	BOOL bSetOffset(FALSE);
	Rect rc = pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].rcTemp;
	PointFloat fPt[2];
	UINT nCount(0);
	int nw(0);
	int nl(0);
	UINT nTool = 12311;
	UINT nType = idCtl - IDC_BUTTON_SEARCHRECT;
	switch (nType)
	{
	case 0://IDC_BUTTON_SEARCHRECT
		break;
	case 1://IDC_BUTTON_SEARCH_AREA
		pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_SS_TYPE);
		if (pCombox->GetCurSel() >> 1)
			nTool = 13319;
		break;
	case 2://IDC_BUTTON_TEMPLATERECT
		fPt[0].x = rc.left + rc.width / 2;
		fPt[0].y = rc.top + rc.height / 2;
		UpdateData();
		break;
	}
	ROI* roi = imaqCreateROI();
	Image* CurrentImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	pFrame->m_NiVision.GetImage(0, CurrentImg,FALSE);
	if (m_bUseBinary)
		pFrame->m_NiVision.CreateBinary(CurrentImg, CurrentImg, 2);
	imaqGetImageSize(CurrentImg, &nw, &nl);
	if (pFrame->m_NiVision.CreateROI(CurrentImg, roi,0, nTool))
	{
		nCount = imaqGetContourCount(roi);
		for (UINT i = 0; i < nCount&&nCount < 3; ++i)
		{
			ContourInfo2* pInf = imaqGetContourInfo2(roi, imaqGetContour(roi, i));
			if (pInf == NULL)
				break;
			switch (pInf->type)
			{
			case IMAQ_POINT:
				if (nType >> 1)
				{
					bSetOffset = TRUE;
					fPt[1].x = pInf->structure.point->x;
					fPt[1].y = pInf->structure.point->y;
				}
				break;
			case IMAQ_RECT:
				rc = *(pInf->structure.rect);
				if (rc.top < 0 || rc.top >= nl)
					rc.top = 0;
				if (rc.left < 0 || rc.left >= nw)
					rc.left = 0;
				if ((rc.width + rc.left)>nw)
					rc.width = nw - rc.left;
				if ((rc.height + rc.top)>nl)
					rc.height = nl - rc.top;
				if (nType >> 1)
				{
					fPt[0].x = rc.left + rc.width / 2;
					fPt[0].y = rc.top + rc.height / 2;

					pFrame->m_pDoc->m_cParam.PrjCfg.tTemplateArry[m_nCurrentTM].rcTemp = rc;
					Image* TempImg = NULL;
					if (m_bUseMask)
					{
						TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
						pFrame->m_NiVision.CreateMaskImage(TempImg, CurrentImg, rc);
					}
					if (!pFrame->StartLearning(m_nCurrentTM,TempImg))
						pFrame->ShowPopup(_T("没有成功，可能是前次模板学习未完成，请稍后再试！"));
				}
				else
					pFrame->m_pDoc->m_cParam.PrjCfg.rcSearch[m_nCurrentSearch] = rc;
				break;
			case IMAQ_ANNULUS:
				pFrame->m_pDoc->m_cParam.PrjCfg.arcSearch[m_nCurrentSearch] = *(pInf->structure.annulus);
				break;
			case IMAQ_EMPTY_CONTOUR:
			default:

				break;
			}
			imaqDispose(pInf);
		}
		if (bSetOffset)
		{
			if (pFrame->m_NiVision.m_pCalibration[0])
			{
				TransformReport* pReport = imaqTransformPixelToRealWorld(pFrame->m_NiVision.m_pCalibration[0], fPt, 2);
				fPt[0].x = pReport->points[0].x * pFrame->m_NiVision.m_dCalibrationUnit[0];
				fPt[0].y = pReport->points[0].y * pFrame->m_NiVision.m_dCalibrationUnit[0];
				fPt[1].x = pReport->points[1].x * pFrame->m_NiVision.m_dCalibrationUnit[0];
				fPt[1].y = pReport->points[1].y * pFrame->m_NiVision.m_dCalibrationUnit[0];
				imaqDispose(pReport);
			}
			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin].CenterPt[0] = fPt[1].x - fPt[0].x;
			pFrame->m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin].CenterPt[1] = -fPt[1].y + fPt[0].y;
			if (pFrame->m_pPrjSettingDlg)
				::PostMessage(pFrame->m_pPrjSettingDlg->m_hWnd, WM_USER_UPDATEUI,2, 0);


		}

	}
	imaqDispose(roi);
	imaqDispose(CurrentImg);
}

void CVisionCtrlDlg::OnBnClickedButtonMatch()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_TM_TYPE);
	int j = pCombox->GetCurSel();
	theApp.m_bHasLocation = FALSE;
	pFrame->ClearOverlayInfo();
	if(pFrame->MatchProcessing(NULL, m_nCurrentTM, pFrame->m_pDoc->m_cParam.PrjCfg.rcSearch[m_nCurrentSearch],
		m_nTMScore,j?m_nTMContrast:0,NULL))
	{
		CString Tip = _T("需要记住当前位置坐标吗?\r\n如果稍后要以此修正焊点偏移值请点击:[是]");
		if (IDYES == MessageBox(Tip, _T("提示"), MB_YESNO))
		{
			theApp.m_dLocated[0] = pFrame->m_pMotionCtrller[1]->m_dAxisCurPos[0];
			if (theApp.m_dLocated[0]<pFrame->m_pDoc->m_cParam.PrjCfg.ProductArray[1][0][0].EndPoint[0])
				theApp.m_bHasLocation = 1;
			else
				theApp.m_bHasLocation = 2;
			theApp.m_dLocated[1] = pFrame->m_pMotionCtrller[0]->m_dAxisCurPos[theApp.m_bHasLocation];
		}
	}

}


void CVisionCtrlDlg::OnBnClickedButtonFindtest()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_SS_TYPE);
	int j = pCombox->GetCurSel();
	pFrame->ClearOverlayInfo();
	if (j >> 1)
		pFrame->DetectCircleProcessing(NULL, m_nCurrentSS, pFrame->m_pDoc->m_cParam.PrjCfg.arcSearch[m_nCurrentSearch],NULL);
	else
		pFrame->DetectShapeProcessing(NULL, m_nCurrentSS, pFrame->m_pDoc->m_cParam.PrjCfg.rcSearch[m_nCurrentSearch],
		m_nTMScore, j ,NULL);

}



void CVisionCtrlDlg::OnCbnSelchangeComboCirculardirect()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CBCGPComboBox* pCombox = (CBCGPComboBox*)GetDlgItem(IDC_COMBO_CIRCULARDIRECT);
	pFrame->m_pDoc->m_cParam.PrjCfg.cCircleArry[m_nCurrentSS].nDir = pCombox->GetCurSel();

}


void CVisionCtrlDlg::OnBnClickedCheckContour()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();

}

void CVisionCtrlDlg::OnBnClickedCheckBinary()
{
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
	pFrame->m_pDoc->m_cParam.PrjCfg.bUseBinary = m_bUseBinary;
}

void CVisionCtrlDlg::ShowContour(UINT nIndex, int nThreshold, UINT nGap)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	ROI* roi = imaqCreateROI();
	Image* CurrentImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	pFrame->m_NiVision.GetImage(0, CurrentImg);
	switch (nIndex)
	{
	case 0:
	case 1:
		imaqAddRectContour(roi, pFrame->m_pDoc->m_cParam.PrjCfg.rcSearch[m_nCurrentSearch]);
		break;
	case 2:
		imaqAddAnnulusContour(roi, pFrame->m_pDoc->m_cParam.PrjCfg.arcSearch[m_nCurrentSearch]);
		break;
	default:
		break;
	} 

	CurveParameters cp;
	cp.extractionMode = IMAQ_NORMAL_IMAGE;
	cp.filterSize = IMAQ_NORMAL;
	cp.maxEndPointGap =nGap;
	cp.minLength = 10;
	cp.searchStep =5 ;
	cp.subpixel =1 ;
	cp.threshold = nThreshold;
	pFrame->m_NiVision.DrawContour(TempCanvas, CurrentImg, roi, 1,&cp, CMainFrame::ASSISANT_INFO_LAYER);
	pFrame->m_NiVision.UpdateOverlay((UINT)0, TempCanvas, CMainFrame::ASSISANT_INFO_LAYER + 1);
	::PostMessage(pFrame->GetSafeHwnd(), WM_USER_SHOWIMAGE, 0, 0);
	imaqDispose(TempCanvas);
	imaqDispose(CurrentImg);
	imaqDispose(roi);

}
