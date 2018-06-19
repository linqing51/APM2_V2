
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "APM.h"

#include "MainFrm.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
CString csz[2] = { _T("左边工位"), _T("右边工位") };

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CBCGPFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPFrameWnd)
	ON_REGISTERED_MESSAGE(BCGM_ON_CLOSEPOPUPWINDOW, OnClosePopup)
	ON_MESSAGE(WM_USER_TIPS, OnShowPopup)
	ON_MESSAGE(WM_USER_PROCESS_MESSAGE, OnShowResults)
	ON_MESSAGE(WM_USER_INVALIDATE, OnShowTestResults)
	ON_MESSAGE(WM_COMM_RXCHAR, OnCommRecvChar)
	ON_MESSAGE(WM_COMM_RXSTR, OnCommRecvString)
	ON_MESSAGE(WM_USER_UPDATEUI, &CMainFrame::OnUpdateDlg)
	ON_MESSAGE(WM_USER_SHOWIMAGE, &CMainFrame::OnUpdateDisplay)
	ON_MESSAGE(WM_USER_IOCHANGE, &CMainFrame::OnIochange)
	ON_MESSAGE(WM_USER_MOTIONPOS, &CMainFrame::OnPositionChange)
	ON_MESSAGE(WM_USER_MOTIONSTATUS, &CMainFrame::OnMotionStsChange)
	ON_MESSAGE(WM_USER_HOMED, &CMainFrame::OnHomed)
	ON_MESSAGE(WM_USER_SHOWERRORINFO, &CMainFrame::OnShowErrorInfo)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_TOPCAMERA, ID_VIEW_BOTTOMCAMERA, &CMainFrame::OnViewCCD)
	ON_COMMAND_RANGE(ID_OPERATION_HOME, ID_OPERATION_LASERTEST, &CMainFrame::OnOperationMode)
	ON_COMMAND_RANGE(ID_OPTION_GENERALSETTING, ID_OPTION_LASERSETTING, &CMainFrame::OnViewSetting)
	ON_COMMAND(ID_PROJECT_NEW, &CMainFrame::OnCommandNewProject)
	ON_COMMAND(ID_FILE_NEW, &CMainFrame::OnCommandNewProject)
	ON_COMMAND(ID_PROJECT_CONFIGURATION, &CMainFrame::OnViewProjectSetting)
	ON_COMMAND(ID_VIEW_IO, &CMainFrame::OnViewIODlg)
	ON_COMMAND(ID_OPTION_RANGING, &CMainFrame::OnViewRangingDlg)
	ON_COMMAND(ID_SYSTEM_LOGIN, &CMainFrame::OnLoginDlg)
	ON_COMMAND(ID_RESET_STATUS, &CMainFrame::ResetSts)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPTION_GENERALSETTING, ID_RESET_STATUS, &CMainFrame::OnUpdateCommand)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, &CMainFrame::OnUpdateFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, &CMainFrame::OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CMainFrame::OnUpdateFileSave)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_COMMAND(ID_BUTTON_STOP, &CMainFrame::OnButtonStop)
	ON_COMMAND(ID_BUTTON_LED, &CMainFrame::OnButtonLed)
	ON_COMMAND(ID_BUTTON_ZOOM, &CMainFrame::OnButtonZoom)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_ZOOM, &CMainFrame::OnUpdateButtonZoom)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_LABEL, &CMainFrame::OnUpdateIndicatorLabel)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_LABEL,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() :m_pDoc(NULL), m_pHomeProcessDlg(NULL), m_pErrorInfoDlg(NULL),
m_pIODlg(NULL), m_pGeneralSetDlg(NULL), m_pRunSettingDlg(NULL), m_pLaserSettingDlg(NULL), m_pPrjSettingDlg(NULL),
m_pLoginDlg(NULL), m_pView(NULL),m_pWaiteProcessDlg(NULL)
{
	// TODO:  在此添加成员初始化代码
	int i = 0;
	m_nBondAlarm = 0xff;
	m_nCurrentRunMode = 0;
	m_nCurrentWP[CAMERA] = 0x03;
	m_nCurrentWP[NOZZLE] = 0x03;
	m_nDetectSuccess = 0;
	m_nMatchSuccess = 0;
	m_bLightLed = 0;
	m_bGuideLight = 0;
	m_nSemiRun[0] = 0;
	m_nSemiRun[1] = 0;
	m_bSwiftMode = 1;
	m_nUserType = 0;
	m_nIsHomeEnd = 0;
	m_bIdling = TRUE;
	m_bWaitforTrigger = 0;
	m_bAutoThreadAlive = FALSE;
	m_bHomeThreadAlive[0] = FALSE;
	m_bHomeThreadAlive[1] = FALSE;
	m_bPollingAlive = FALSE;
	m_bFileThreadAlive = FALSE;
	m_nCameraAlive = FALSE;
	m_bLearningAlive = FALSE;
	m_bIsEmergencyStop = TRUE;
	m_bIsPowerOn = FALSE;
	m_bBinary = FALSE;
	m_bAppoint = FALSE;
	m_bAutoZoom = TRUE;
	m_pEstop = new CEvent(FALSE, TRUE);
	m_pStopRun = new CEvent(FALSE, TRUE);
	ZeroMemory(m_pImagePack, sizeof(m_pImagePack));
	ZeroMemory(m_pTemplate, sizeof(m_pTemplate));
	ZeroMemory(m_CoordSys, sizeof(m_CoordSys));
	ZeroMemory(m_nCurrentProduct, sizeof(m_nCurrentProduct));
	ZeroMemory(m_nCurrentVisionCheck, sizeof(m_nCurrentVisionCheck));
	ZeroMemory(m_nCurrentPin, sizeof(m_nCurrentPin));
	ZeroMemory(m_dLaserOffset, sizeof(m_dLaserOffset));
	ZeroMemory(m_nPinVisionSuccess, sizeof(m_nPinVisionSuccess));
	ZeroMemory(m_bSalveThreadAlive,sizeof(m_bSalveThreadAlive));
	ZeroMemory(m_nCurrentRanging,sizeof(m_nCurrentRanging));
	m_bCreateSpl = FALSE;
	m_bNeedClimb = FALSE;
	m_hRs485Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (i = 0; i < 7; i++)
	{
		m_hProcessEvent[0][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hProcessEvent[1][i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	for (i = 0; i < 2; i++)
	{
		m_pMotionCtrller[i] = new CAdvMotion(i);
		m_hSlaveEvent[i] = CreateEvent(NULL,TRUE,TRUE,NULL);
	}
	m_hImgProcessEvent[0] = m_pStopRun->m_hObject;
	for (i = 1; i < 9;i++)
		m_hImgProcessEvent[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
}

CMainFrame::~CMainFrame()
{

	m_NiVision.Camera_Close(0);
	m_NiVision.Camera_Close(1);
	m_hPollingStop.SetEvent();
	m_pStopRun->SetEvent();
//	m_LightControl.ClosePort();
	if (m_bPollingAlive || m_bHomeThreadAlive[0] || m_bAutoThreadAlive)
	{
		Sleep(500);
	}
	SAFE_DELETEDLG(m_pGeneralSetDlg);
	SAFE_DELETEDLG(m_pRunSettingDlg);
	SAFE_DELETEDLG(m_pLaserSettingDlg);
	SAFE_DELETEDLG(m_pPrjSettingDlg);
	SAFE_DELETEDLG(m_pIODlg);
	SAFE_DELETEDLG(m_pLoginDlg);
	SAFE_DELETEDLG(m_pErrorInfoDlg);
	SAFE_DELETEDLG(m_pHomeProcessDlg);
	SAFE_DELETE(m_pEstop);
	SAFE_DELETE(m_pStopRun);
	SAFE_DELETE(m_pMotionCtrller[0]);
	SAFE_DELETE(m_pMotionCtrller[1]);
	CloseHandle(m_hSlaveEvent[0]);
	CloseHandle(m_hSlaveEvent[1]);
	for (int i = 0; i < MAX_PinNumber; i++)
	{
		imaqDispose(m_pTemplate[i]);
		if (i&&i < 9)
			CloseHandle(m_hImgProcessEvent[i]);
		if (i < 7)
		{
			CloseHandle(m_hProcessEvent[0][i]);
			CloseHandle(m_hProcessEvent[1][i]);
		}
		if (i<8&&m_pImagePack[i])
		{
			SAFE_DELETEARRAY(m_pImagePack[i]->nIndex);
			if (m_pImagePack[i]->pData)
				imaqDispose((Image*)(m_pImagePack[i]->pData));
			SAFE_DELETE(m_pImagePack[i]);
		}
	}
	IVA_DisposeData(m_ivaData);
	SAFE_DELETEDLG(m_pWaiteProcessDlg);
}

HWND CMainFrame::m_hwdDesktopAlert = NULL;

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	CBCGPVisualManager::SetDefaultManager(RUNTIME_CLASS(CBCGPVisualManager2007));
	CBCGPVisualManager2007::SetStyle(CBCGPVisualManager2007::VS2007_Silver);
	CMenu menu;
	menu.LoadMenu(IDR_MENU_MAIN);  //加载自己的菜单  
	SetMenu(&menu);
	menu.Detach();   //该函数是用来将菜单句柄与菜单对象分离  
	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("Failed to create menubar\r\n");
		return -1;      // fail to create
	}
	m_wndMenuBar.EnablePopupMode(FALSE);
	m_wndMenuBar.SetBarStyle(m_wndMenuBar.GetBarStyle() | CBRS_SIZE_FIXED & ~CBRS_GRIPPER);

	m_wndToolBar.EnableLargeIcons(TRUE);
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\r\n");
		return -1;      // 未能创建
	}
	m_wndToolBar.GetImages()->Load(IDB_BITMAP1,0,TRUE);
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
	{
		TRACE0("未能创建状态栏\r\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetPaneTextColor(1, CBCGPColor::DarkRed);
	m_wndStatusBar.SetPaneWidth(1,130);
	m_wndToolBar.EnableTextLabels(TRUE);
	m_wndToolBar.SetButtonText(6, _T("适应窗口"));
	
	m_wndToolBar.SetLargeIcons(TRUE);
//	m_wndToolBar.SetButtonText(4, _T("Top Camera"));
// 	m_wndToolBar.GetButton(7)->SetImage(15);
	m_wndToolBar.GetButton(15)->SetImage(20);//project
	m_wndToolBar.GetButton(18)->SetImage(17);//login
	m_wndToolBar.GetButton(19)->SetImage(19);//about
	m_wndToolBar.GetButton(22)->SetImage(25);//reset
// 	m_wndToolBar.GetButton(22)->SetImage(17);//Stop

	// TODO:  如果不需要可停靠工具栏，则删除这三行
 	EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndMenuBar, AFX_IDW_DOCKBAR_TOP);
	DockControlBar(&m_wndToolBar, AFX_IDW_DOCKBAR_TOP);
	CenterWindow();
	return 0;
}

LRESULT CMainFrame::OnShowResults(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	static BYTE nCount(0);
	static CString info(_T(""));
	CString sz;
	CString sts[2] = {_T("识别失败") , _T("识别成功") };
	Point pt = { 20, 100 };
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	BYTE num = wParam >> 4;
	BYTE nIndex = wParam & 0x0f;
	if (nCount == 32)
	{
		info = _T("");
		nCount = 0;
	}
	sz.Format(_T("第%d个产品焊点:%d %s\r\n"), num + 1, nIndex + 1, sts[lParam ? 1 : 0]);
	info += sz;
	m_NiVision.DrawTextInfo(TempCanvas, info, pt, BASIC_INFO_LAYER,44);
	m_NiVision.UpdateOverlay((UINT)0, TempCanvas, CMainFrame::BASIC_INFO_LAYER + 1);
	nCount++;
	imaqDispose(TempCanvas);
	return 0;
}

LRESULT CMainFrame::OnShowTestResults(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CString info(_T("出球测试准备中"));
	double dtime;
	float fval;
	Point pt = {240, 60 };
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	switch (wParam)
	{
	case 0:
		break;
	case 1:
		if (lParam)
		{
			dtime = theApp.m_StopWatch.GetAverageTime() / 1000;
			fval = lParam ? dtime / lParam : 0;
			info.Format(_T("测试量：%d次,耗时：%.0f ms，平均%.0f ms"), lParam, dtime, fval);
		}
		else
			info.Format(_T("出球测试中，打球间隔%.0f ms"), theApp.m_StopWatch.GetTimeSpan() / 1000);
		break;
	}
	m_NiVision.DrawTextInfo(TempCanvas, info, pt, BASIC_INFO_LAYER, 44);
	m_NiVision.UpdateOverlay((UINT)0, TempCanvas, CMainFrame::BASIC_INFO_LAYER + 1);
	imaqDispose(TempCanvas);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO:  在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_MAXIMIZE | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX;
// 	int xsize = ::GetSystemMetrics(SM_CXSCREEN);
// 	int ysize = ::GetSystemMetrics(SM_CYSCREEN);
// 	cs.x = 0;
// 	cs.y = 0;
// 	cs.cx = xsize;
// 	cs.cy = ysize;
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::UpdateTitle()
{
	CString sz;
	CString str[] = { _T("操作员"), _T("技术员"), _T("工程师"),_T("超级？")};
	sz = m_pDoc->m_szCurParamName + _T("<---------") + str[m_nUserType];
	m_pDoc->SetTitle(sz);
}


void CMainFrame::ShowPopup(CString Info, BOOL bAutoClose, UINT nTime)
{
	CMyPopupWindow* pPopup = new CMyPopupWindow;
	pPopup->SetTheme((CBCGPPopupWindow::BCGPPopupWindowTheme)0);
	pPopup->SetAnimationType(CBCGPPopupMenu::SYSTEM_DEFAULT_ANIMATION );
	pPopup->SetAnimationSpeed(30);
	pPopup->SetTransparency((BYTE)252);
	pPopup->SetSmallCaption();
	pPopup->SetSmallCaptionGripper(TRUE);
	pPopup->SetAutoCloseTime(bAutoClose ? nTime * 1000 : 0);

	pPopup->SetRoundedCorners(TRUE);
	pPopup->SetShadow(TRUE);
	pPopup->EnablePinButton(TRUE);

	pPopup->m_params.m_strText = Info;
	pPopup->m_params.m_strURL = _T("OK");
	pPopup->m_params.m_nURLCmdID = 101;


/*
	pPopup->Create(m_hwd, params, NULL);
	m_hwdDesktopAlert = pPopup->GetSafeHwnd();
	((CMainFrame*)m_hwd)->params.m_strText = Info;
	((CMainFrame*)m_hwd)->params.m_strURL = _T("OK");
	((CMainFrame*)m_hwd)->params.m_nURLCmdID = 101;
*/
	if (theApp.m_pMainWnd&&theApp.m_pMainWnd->m_hWnd)
		::PostMessage(theApp.m_pMainWnd->m_hWnd, WM_USER_TIPS, 0, (LPARAM)pPopup);
	else
		SAFE_DELETEDLG(pPopup);
}

LRESULT CMainFrame::OnShowPopup(WPARAM wParam, LPARAM lParam)
{
	CMyPopupWindow* pPopup = (CMyPopupWindow*)lParam;
	if (pPopup)
	{

		pPopup->Create(theApp.m_pMainWnd);
		if (m_hwdDesktopAlert&&!pPopup->GetAutoCloseTime())
		{
			ClosePopup();
			m_hwdDesktopAlert = pPopup->GetSafeHwnd();
		}
	}
	return 0;
}

void CMainFrame::ClosePopup()
{
	if (m_hwdDesktopAlert != NULL && ::IsWindow(m_hwdDesktopAlert))
	{
		::SendMessage(m_hwdDesktopAlert, WM_CLOSE, 0, 0);
	}

}
LRESULT CMainFrame::OnClosePopup(WPARAM, LPARAM lp)
{
	CBCGPPopupWindow* pPopup = (CBCGPPopupWindow*)lp;
	ASSERT_VALID(pPopup);
	m_hwdDesktopAlert = NULL;
	
	return 0;
}

LRESULT CMainFrame::OnIochange(WPARAM wParam, LPARAM lParam)
{
	BOOL bNormal(TRUE);
	CString ErrMsg;
	int i(0);
	UINT nIndex = wParam;
	switch (nIndex)
	{
	case 0xff:
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 6, lParam);
		if (m_nIsHomeEnd&&(m_nBondAlarm^0x01)&& (m_nBondAlarm != 0x05))
		{
			if (m_bAutoThreadAlive)
				StopCommand();

			ErrMsg.Format(_T("Bond头状态错误:回零[%d],无球[%d],堵球[%d],清洗失败[%d]\r\n"), 
				(~lParam & 0x01), lParam & 0x02, lParam & 0x04, lParam & 0x08);
			AddedErrorMsg(ErrMsg);
		}
		break;
	default:
		if (m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_PLC_Input[0]])
			::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 8, 1);
		else
			::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 8, 0);

		if ((m_bSaftyDevice & 0x01 && !m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_GS_FSD1]) || (m_bSaftyDevice & 0x02 && !m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_Gate_Open]))
		{
			if (m_bAutoThreadAlive || m_bSalveThreadAlive[0] || m_bSalveThreadAlive[1] || m_bHomeThreadAlive[0] ){
				ErrMsg = _T("安全区域警报\r\n");
				AddedErrorMsg(ErrMsg);
				bNormal = FALSE;
				m_pStopRun->SetEvent();
			}
			else
				m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[1], TRUE);//黄状态灯
		}

		if (!bNormal)
		{
			StopCommand(TRUE);
			if (!bNormal)
			{
				m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[0], TRUE);//red状态灯/蜂鸣
// 				m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[3], TRUE);//red状态灯/蜂鸣
			}
		}
		else if (!m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_StopBtn])
		{
			StopCommand();

		}
		else if(m_bWaitforTrigger) 
		{
			if ((m_bWaitforTrigger & 0x01) && m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_StartLBtn])
				m_hOneCycleStart[0].PulseEvent();
			if ((m_bWaitforTrigger & 0x02) && m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_StartRBtn])
				m_hOneCycleStart[1].PulseEvent();
		}
		break;
	}
	if (m_pIODlg == NULL)
		m_pIODlg = new CIOStatusDlg;
	if (m_pIODlg && m_pIODlg->GetSafeHwnd() == NULL){
		m_pIODlg->Create(IDD_DIALOG_IO, this);
		m_pIODlg->ShowWindow(SW_HIDE);
	}
	::PostMessage(m_pIODlg->GetSafeHwnd(), WM_USER_IOCHANGE, wParam, lParam);
	return bNormal;
}

LRESULT CMainFrame::OnMotionStsChange(WPARAM wParam, LPARAM lParam)
{
	CString ErrMsg;
	static ULONG ntemp[2] = {0,0};
	if (m_pIODlg == NULL)
		m_pIODlg = new CIOStatusDlg;
	if (m_pIODlg && m_pIODlg->GetSafeHwnd() == NULL){
		m_pIODlg->Create(IDD_DIALOG_IO, this);
		m_pIODlg->ShowWindow(SW_HIDE);
	}
	BYTE nIndex = wParam & 0x01;
	if (ntemp[nIndex] ^ lParam)
	{
		BYTE nFlag = lParam & 0x0f;
		if (nFlag && !m_bHomeThreadAlive[0])
		{
			CString str[2][4] = { _T("X、"), _T("LY、"), _T("RY、"), _T("Z"), _T("CX、"), _T("VZ") };
			CString str_temp;
			for (BYTE j = 0; j < (0x04 >> nIndex); j++)
			{
				if (nFlag >> j)
				{
					if ((nFlag >> j) & 0x01)
					{
						CString str_num;
						str_num.Format(_T("%s"), str[nIndex][j]);
						str_temp += str_num;
					}
				}
				else
					break;
			}
			ErrMsg.Format(_T("运动卡%d:%s轴伺服报警,请检查是否上电 \r\n"), nIndex, str_temp);
			AddedErrorMsg(ErrMsg);
		}
		ntemp[nIndex] = lParam;
	}
	::PostMessage(m_pIODlg->GetSafeHwnd(), WM_USER_MOTIONSTATUS, nIndex, lParam);
	return 0;
}

LRESULT CMainFrame::OnPositionChange(WPARAM wParam, LPARAM lParam)
{
	::PostMessage(m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, wParam+2, lParam);

	return 1;
}

void CMainFrame::OnViewCCD(UINT idCtl)
{
	// TODO:  在此添加命令处理程序代码
	BOOL bSuccess(FALSE);
	UINT	nIndex = idCtl - ID_VIEW_TOPCAMERA;
	CRect ClientRc;
	m_pView->GetDlgItem(IDC_STATIC_CONTAIN)->GetClientRect(ClientRc);
	m_NiVision.ResizeDisplayWnd(0, ClientRc);

	SwitchCamera(nIndex);

}

void CMainFrame::OnViewSetting(UINT idCtl)
{
	// TODO:  在此添加命令处理程序代码

	switch (idCtl)
	{
	case ID_OPTION_GENERALSETTING:
		SAFE_DELETEDLG(m_pGeneralSetDlg);
		m_pGeneralSetDlg = new CGeneralSettingDlg;
		if (m_pGeneralSetDlg->GetSafeHwnd() == NULL)
		{
			m_pGeneralSetDlg->Create(CGeneralSettingDlg::IDD, this);
		}
		m_pGeneralSetDlg->ShowWindow(SW_SHOW);

		break;
	case ID_OPTION_HOMINGSETTING:
		SAFE_DELETEDLG(m_pRunSettingDlg);
		m_pRunSettingDlg = new CHomeSettingDlg;
		if (m_pRunSettingDlg->GetSafeHwnd() == NULL)
		{
			m_pRunSettingDlg->Create(CHomeSettingDlg::IDD, this);
		}
		m_pRunSettingDlg->ShowWindow(SW_SHOW);

		break;
	case ID_OPTION_LASERSETTING:
		SAFE_DELETEDLG(m_pLaserSettingDlg);
		if (m_pLaserSettingDlg == NULL)
		{
			m_pLaserSettingDlg = new CLaserSettingDlg;
		}
		if (m_pLaserSettingDlg->GetSafeHwnd() == NULL)
		{
			m_pLaserSettingDlg->Create(CLaserSettingDlg::IDD, this);
		}
		m_pLaserSettingDlg->ShowWindow(SW_SHOW);
		::PostMessage(m_pLaserSettingDlg->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);
		break;
	default:break;
	}

}

void CMainFrame::OnOperationMode(UINT idCtl)
{
	switch (idCtl)
	{
	case ID_OPERATION_HOME:
		if (!m_nCurrentRunMode)
		{
			ShowHomeProcessDlg();
			if (!StartReturnHome())
				PostMessage(WM_USER_HOMED, (WPARAM)XYZU_AXIS, -1);
		}
		break;
	case ID_OPERATION_AUTO:
		if (m_nIsHomeEnd&&!m_nCurrentRunMode)
		{
			StartAuto(TRUE);
		}
		break;
	case ID_OPERATION_LASERTEST:
		if (!m_LaserCtrl.GetPLCRDY())
		{
			AddedErrorMsg(_T("PLC 未准备好\r\n"));
			return ;
		}
		if (m_nCurrentRunMode ==3)
		{
			m_LaserCtrl.SetLaserRDY(FALSE);
			m_pStopRun->SetEvent();
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[0], FALSE);
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[2], FALSE);
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);
		}
		else if (!m_nCurrentRunMode)
		{
			CString Tip=_T("可能处在相机上方，非安全操作!\r\n");
			m_pStopRun->ResetEvent();
			m_LaserCtrl.SetLaserRDY();
			if (!IsAboveCamera()){
				Tip.Format(_T("是否只测试激光而不转球打球，%d次？\r\n是（YES）：只测试激光；\r\n否（NO）：完整测试转球打球\r\n"), m_pDoc->m_cParam.PrjCfg.nTestBallNumber);
				if (IDYES == MessageBox(Tip, _T("重要提示-重要提示"), MB_YESNO))
				{
					m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[0], FALSE);
 					StartManualRunThread(0, 9);
				}
				else{
					m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[0], TRUE);
					PostMessage(WM_USER_INVALIDATE, 0, 0);
					StartManualRunThread(0, 8);

				}
			}
			else
				AddedErrorMsg(Tip);
		}
		break;
	default:break;
	}

}

void CMainFrame::OnCommandNewProject()
{
	// TODO:  在此添加命令处理程序代码
	 
	if (m_pPrjSettingDlg == NULL)
	{
		m_pPrjSettingDlg = new CProjectSettingDlg;
	}
	if (m_pPrjSettingDlg->GetSafeHwnd() == NULL)
	{
		m_pPrjSettingDlg->Create(CProjectSettingDlg::IDD, this);
	}
	m_pPrjSettingDlg->m_btnCreate.SetWindowTextW(_T("添加"));

	m_pPrjSettingDlg->ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->m_cbPrjName.ShowWindow(SW_HIDE);
	m_pPrjSettingDlg->m_btnLoad.ShowWindow(SW_HIDE);
	m_pPrjSettingDlg->GetDlgItem(IDC_EDIT_NEWPROJECT)->ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->GetDlgItem(IDC_EDIT_NEWPROJECT)->SetWindowTextW(_T(""));
	::PostMessage(m_pPrjSettingDlg->m_hWnd, WM_USER_UPDATEUI, 0, 1);
}

void CMainFrame::OnViewProjectSetting()
{
	// TODO:  在此添加命令处理程序代码
	if (m_pPrjSettingDlg == NULL)
	{
		m_pPrjSettingDlg = new CProjectSettingDlg;
	}
	if (m_pPrjSettingDlg->GetSafeHwnd() == NULL)
	{
		m_pPrjSettingDlg->Create(CProjectSettingDlg::IDD, this);
	}
//	m_pPrjSettingDlg->ChangeView(FALSE);
	m_pPrjSettingDlg->m_btnCreate.SetWindowTextW(_T("修改"));
	m_pPrjSettingDlg->ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->m_cbPrjName.ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->GetDlgItem(IDC_EDIT_NEWPROJECT)->ShowWindow(SW_HIDE);
	m_pPrjSettingDlg->m_btnLoad.ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->m_btnLoad.SetWindowText(_T("加载"));
	::PostMessage(m_pPrjSettingDlg->m_hWnd, WM_USER_UPDATEUI, 0, 1);
}

void CMainFrame::OnViewIODlg()
{
	// TODO:  在此添加命令处理程序代码
	if (m_pIODlg == NULL)
	{
		m_pIODlg = new CIOStatusDlg;
	}
	if (m_pIODlg->GetSafeHwnd() == NULL)
	{
		m_pIODlg->Create(CIOStatusDlg::IDD, this);
	}
	m_pIODlg->ShowWindow(SW_SHOW);

}

void CMainFrame::OnViewRangingDlg()
{
// 	SAFE_DELETEDLG(m_pRangingDlg);
// 	m_pRangingDlg = new CRangingDlg;
// 	if (m_pRangingDlg->GetSafeHwnd() == NULL)
// 	{
// 		m_pRangingDlg->Create(CRangingDlg::IDD, this);
// 	}
// 	m_pRangingDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnLoginDlg()
{
	// TODO:  在此添加命令处理程序代码
	SAFE_DELETEDLG(m_pLoginDlg);
	m_pLoginDlg = new CLoginDlg;
	if (m_pLoginDlg->GetSafeHwnd() == NULL)
	{
		m_pLoginDlg->Create(CLoginDlg::IDD, this);
	}
	m_pLoginDlg->ShowWindow(SW_SHOW);
}

void CMainFrame::OnUpdateCommand(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	UINT nSts(FALSE);
	switch (pCmdUI->m_nID)
	{
	case ID_OPTION_GENERALSETTING :

	case ID_OPTION_HOMINGSETTING:
	case ID_OPTION_LASERSETTING: 
	case ID_PROJECT_CONFIGURATION:
	case ID_PROJECT_NEW: 
		pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);
		break;


	case ID_VIEW_TOPCAMERA:
		pCmdUI->SetCheck(m_nCameraAlive&0X01);
		pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);
		break;
	case ID_VIEW_BOTTOMCAMERA:
		pCmdUI->SetCheck(m_nCameraAlive>>1);
		pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);
		break;
	case ID_OPERATION_HOME:
		pCmdUI->SetCheck(m_bHomeThreadAlive[0]);
		pCmdUI->Enable(!m_nCurrentRunMode);
		break;
	case ID_OPERATION_AUTO:
		pCmdUI->SetCheck(m_bAutoThreadAlive);
		pCmdUI->Enable(m_nIsHomeEnd&&!m_nCurrentRunMode);
		break;
	case ID_OPERATION_LASERTEST:
		pCmdUI->SetCheck(m_nCurrentRunMode==3);
		pCmdUI->Enable(!m_bIdling&&m_nUserType&&!(m_nCurrentRunMode & 0x03));

		break;
	case ID_VIEW_IO:  
		pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);

		break;
	case ID_SYSTEM_LOGIN :  
		pCmdUI->Enable(!m_nCurrentRunMode);
		break;
	case ID_RESET_STATUS :   
		pCmdUI->Enable(!m_bAutoThreadAlive);
		break;
	default:break;
	}

}

void CMainFrame::OnUpdateButtonZoom(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(!m_bAutoZoom);
}

void CMainFrame::OnUpdateFileNew(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);

}

void CMainFrame::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode);
}

void CMainFrame::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserType&&!m_nCurrentRunMode&&!m_bFileThreadAlive);
}

//BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
//{
//	// TODO:  在此添加专用代码和/或调用基类
///*
//	CRect rect1;    GetWindowRect(rect1);
//	CRect ClientRc; GetClientRect(ClientRc);
//	m_defSplitterWnd.CreateStatic(this, 1, 2, WS_CHILD | WS_VISIBLE);
//	m_defSplitterWnd.CreateView(0, 0, RUNTIME_CLASS(CControlView), CSize(240, ClientRc.Height()), pContext);
//	m_defSplitterWnd.CreateView(0, 1, RUNTIME_CLASS(CImageView), CSize(ClientRc.Width()-240, ClientRc.Height()), pContext);
//	m_pCtrlView = (CControlView*)m_defSplitterWnd.GetPane(0, 0);
//	m_pCCDView = (CImageView*)m_defSplitterWnd.GetPane(0, 1);
//	m_pCtrlView->GetClientRect(rect1);
//	m_bCreateSpl = TRUE;
//	if (!m_wndTab.Create(CBCGPTabWnd::STYLE_3D, rect1, m_pCtrlView, 1, CBCGPTabWnd::LOCATION_BOTTOM))
//	{
//		TRACE0("\n未能创建tab\n");
//		return -1;      // 未能创建
//	}
//	m_VisionCtrlDlg.Create(IDD_DIALOG_VISIONCTRL, &m_wndTab);
//	m_wndTab.AddTab(&m_VisionCtrlDlg, _T("One"), (UINT)-1, FALSE);
//	m_defSplitterWnd.SetColumnInfo(0, rect1.Width(),200 );
//	m_defSplitterWnd.RecalcLayout();
//	m_wndTab.RecalcLayout();
//	m_wndTab.RedrawWindow();
//	RecalcLayout(FALSE);
//	*/
//	return TRUE;
//}

BOOL CMainFrame::InitialDefine()
{
	if (m_pDoc == NULL)
		m_pDoc = (CAPMDoc*)GetActiveDocument();
	if (m_pView == NULL){
		m_pView = (CAPMView*)GetActiveView();

	}
 	m_NiVision.ShowMessage = NULL;

	InitEvent();
	m_ivaData = IVA_InitData(m_pDoc->m_cParam.PrjCfg.nPinNum, 0);
	return TRUE;

}

BOOL CMainFrame::InitHardware()
{
	BOOL bSucces(TRUE);
	UINT i(0);
	CString ErrMsg;
	CString cfgName = _T("PCI-1245V-0.ini");

	m_LaserCtrl.Initport(m_pDoc->m_cParam.nComPort[0], 115200, 'N', 8);
	LightControllerOpen(TRUE);
	bSucces = m_NiVision.PreInit(m_pView->GetDlgItem(IDC_STATIC_CONTAIN), 2);
	if (!bSucces)
	{
		AfxMessageBox(_T("找不到相机,请确认相机连接好!\r\n"));
		return bSucces;
	}
	m_NiVision.SetHandleWnd(this);
	m_NiVision.SetSameDisplayWnd(0x03);
	for (i = 0; i < 2; i++)
	{
		if (m_NiVision.Camera_Connect(i)){
// 			m_NiVision.Camera_EnumerateAttributes(i);
			m_NiVision.Camera_SetExposure(i, m_pDoc->m_cParam.PrjCfg.nExposure[i]);
			if (i==0)
				LoadCalibration(i);
		}
	}

	// 	if (m_pDoc->m_cParam.nFocusType)
	// 		m_NiVision.DrawRect(m_NiVision.m_pOverlay[0], m_pDoc->m_cParam.PrjCfg.FocusRc);
	// 	else
	CRect ClientRc;
	m_pView->GetDlgItem(IDC_STATIC_CONTAIN)->GetClientRect(ClientRc);
	m_NiVision.ResizeDisplayWnd(0, ClientRc);
	m_NiVision.m_nRotate = 0x03 << 2;

	cfgName = _T("1756-0.xml");
	bSucces = bSucces ? m_IoCtrller.InitCard(1756) : bSucces;
	bSucces = bSucces ? m_IoCtrller.LoadConfig(m_pDoc->m_szCurrentFolder + cfgName) : bSucces;
	if (!bSucces)
	{
		AfxMessageBox(_T("输入输出卡初始化失败\r\n"));
		return bSucces;
	}
	else
	{
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Driver_power[0], TRUE);
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Driver_power[1], TRUE);
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Driver_power[2], TRUE);
		Sleep(1000);
	}

// /*	
	bSucces = m_pMotionCtrller[0]->InitBoard(0, XYZU_AXIS);
	//	bSucces = bSucces ? m_MotionCtrller.LoadConfig(m_pDoc->m_szCurrentFolder + cfgName) : bSucces;
	if (!bSucces)
	{
		AfxMessageBox(m_pMotionCtrller[0]->m_ErrMsg);
		return bSucces;
	}
	bSucces = m_pMotionCtrller[1]->InitBoard(1, XY_AXIS);
	//	bSucces = bSucces ? m_MotionCtrller.LoadConfig(m_pDoc->m_szCurrentFolder + cfgName) : bSucces;
	if (!bSucces)
	{
		AfxMessageBox(m_pMotionCtrller[1]->m_ErrMsg);
		return bSucces;
	}
// 	*/
	bSucces = SetHardWareRun(TRUE);
	if (bSucces){
		CString str[9] = { _T("IO卡启动扫描失败;"), _T("启动实时图像失败;"), _T("传送参数至PLC通讯失败;"), _T("启动扫描PLC状态失败;"),
			_T("运动控制卡1初始化失败;"), _T("运动控制卡1初始化轴失败;"), _T("运动控制卡2初始化失败;"), _T("运动控制卡2初始化轴失败;"),
			_T("") };
		CString str_temp;
		for (BYTE j = 0; j < 8; j++)
		{
			if (bSucces >> j)
			{
				if ((bSucces >> j) & 0x01)
				{
					str_temp.Format(_T("%s"), str[j]);
					str[8] += str_temp;
				}
			}
			else
				break;
		}
		str_temp.Format(_T("初始化硬件失败，未完成的有\r\n%s\r\n"), str[8]);
		::AfxMessageBox(str_temp);
	}
	return !bSucces;
}

void CMainFrame::InitEvent()
{
	UINT i(0);
	m_pStopRun->SetEvent();
	Sleep(1);
	m_hOneCycleStart[0].ResetEvent();
	m_hOneCycleStart[1].ResetEvent();
	for (i = 0; i < 9; i++)
	{
		SetEvent(m_hImgProcessEvent[i]);
		if (i < 7)
		{
			ResetEvent(m_hProcessEvent[0][i]);
			ResetEvent(m_hProcessEvent[1][i]);

		}
	}
	SetEvent(m_hSlaveEvent[0]);
	SetEvent(m_hSlaveEvent[1]);
	m_pStopRun->ResetEvent();
	m_hCameraFree.SetEvent();
	m_hNozzleFree.SetEvent();

}

BOOL CMainFrame::SetHardWareRun(BOOL bEnable /*= TRUE*/)
{
	BOOL bSucces(FALSE);
	UINT bResult(0);
	bSucces = m_IoCtrller.StartIOMonitoring(this);
	if (bSucces)
	{
		m_bSaftyDevice = m_pDoc->m_cParam.nEnabeSafetyDev;
		SetButtonLamp(2, TRUE);

	}
	else
		bResult = 1;
	bSucces = SwitchCamera(0);
	if (bSucces)
	{
		m_nCameraAlive |= 0x01;
		m_NiVision.DrawCross(m_NiVision.m_pOverlay[0], imaqMakePoint(1024, 1024), 0xff, &IMAQ_RGB_YELLOW);

	}
	else
		bResult |= 2;
	if (!DownPlcConf())
	{
		m_LaserCtrl.SetbondServoRDY();
		m_LaserCtrl.SetLaserRDY();
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_DCF[2], TRUE);
		if(!StartPolling())
			bResult |= 8;
	}
	else
		bResult |= 4;

	if (m_pMotionCtrller[0]->m_bInit)
	{
		bSucces = m_pMotionCtrller[0]->CreateGroup(0x03, 0);
		bSucces = bSucces ? m_pMotionCtrller[0]->SetAxSvOn(TRUE) : bSucces;
		m_pMotionCtrller[0]->RuninConfig(0, m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[0]);
		if (!bSucces)
			bResult |= 0x020;
		if (m_pMotionCtrller[1]->m_bInit)
		{
			bSucces = m_pMotionCtrller[1]->CreateGroup(0x03, 0);
			bSucces = bSucces ? m_pMotionCtrller[1]->SetAxSvOn(TRUE) : bSucces;
			m_pMotionCtrller[1]->RuninConfig(0, m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[4], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[4]);
			if (!bSucces)
				bResult = 0x080;
		}
		else
			bResult |= 0x0C0;
	}
	else
		bResult |= 0x0F0;
	if (!(bResult >> 4))
	{
		UINT n=	SetTimer(5, 4000, NULL);
		if (n ^ 5)
			AfxMessageBox(_T("设备初始化:定时器5启动异常"));
	}
	m_bIsPowerOn = !bResult;
	return bResult;
}

BOOL CMainFrame::DownPlcConf()
{
	BOOL bSuccess=!m_LaserCtrl.Openport();

	if (!bSuccess)
	{
		if (m_LaserCtrl.GetPLCRDY())
		{
			if (!m_LaserCtrl.SetConfigList(m_pDoc->m_cParam.PrjCfg.fLaserPower, m_pDoc->m_cParam.PrjCfg.uLaserPressure / 10.0,
				m_pDoc->m_cParam.PrjCfg.uDetectPressure[0], m_pDoc->m_cParam.PrjCfg.uDetectPressure[1]))
				bSuccess = 1;
			if (!m_LaserCtrl.SetNGBallNum(m_pDoc->m_cParam.BallAlarmNum))
				bSuccess |= 2;
			if (!m_LaserCtrl.SetCleanPower(m_pDoc->m_cParam.PrjCfg.fCleanPower))
				bSuccess |= 4;
			if (!m_LaserCtrl.SetDroppingTime(m_pDoc->m_cParam.nDroppingTime[0],0 ))
				bSuccess |= 8;
			if (!m_LaserCtrl.SetDroppingTime(m_pDoc->m_cParam.nDroppingTime[1],1 ))
				bSuccess |= 0x010;
			for (int i = 0; i < MAX_PinNumber; i++)
			{
				if (!m_LaserCtrl.SetPowerTime(m_pDoc->m_cParam.PrjCfg.PinArray[i].uWeldingTimes /10.0, i))
					bSuccess |= 1 << (5 + i);
			}
		}
		else
			bSuccess = 0xffff;
	}
	return bSuccess;
}

void CMainFrame::AddedErrorMsg(CString Info)
{
	m_pErrorInfoDlg->AddedErrorMsg(Info);
	PostMessage(WM_USER_SHOWERRORINFO, 0, 0);
	return;

}

BOOL CMainFrame::LoadCalibration(UINT nIndex)
{
	BOOL bSuccess(FALSE);
	CString szFile;
	if (nIndex > 0)
		return bSuccess;
	szFile.Format(_T("Parameter\\Calibration%d-0514.png"), nIndex);
	szFile = m_pDoc->m_szCurrentFolder + szFile;
	if (m_pDoc->IsFileExisted(szFile))
	{
		Image* calibratedImage;
		calibratedImage = imaqCreateImage(IMAQ_IMAGE_U8, 2);
		if (!m_NiVision.m_pCalibration[nIndex])
			m_NiVision.m_pCalibration[nIndex] = imaqCreateImage(IMAQ_IMAGE_U8, 2);

		bSuccess = m_NiVision.RWVisionFile(calibratedImage, szFile, FALSE);
		bSuccess = !bSuccess ? bSuccess : m_NiVision.CameraCalib(nIndex, calibratedImage);

		imaqDispose(calibratedImage);
	}
	return bSuccess;
}

LRESULT CMainFrame::OnUpdateDisplay(WPARAM wParam, LPARAM lParam)
{
	static BYTE i(0);
	UINT nIndex = wParam;
	m_NiVision.Camera_Display(nIndex);
	// 	TRACE1("\n ------view ThreadID=%d\n", GetCurrentThreadId());
// 	if (0==nIndex)
// 	{
// 		if (i == 5)
// 			i = 0;
// 		if (m_ivaData->buffers[i] == NULL)
// 			m_ivaData->buffers[i] = imaqCreateImage(IMAQ_IMAGE_U8, 2);
// 		m_NiVision.GetImage(0, m_ivaData->buffers[i]);
// 		i++;
// 	}
	return 0;

}

BOOL CMainFrame::LoadTemplate(UINT nIndex, BOOL bSave /*= TRUE*/)
{
	if (nIndex > MAX_PinNumber || (bSave&&!m_pTemplate[nIndex]))
		return FALSE;
	CString sztemp = m_pDoc->m_szCurParamName;
	if (sztemp == _T(""))
		sztemp = _T("Default");
	CString FolderPathName;
	CString FilePathName;
	FolderPathName.Format(_T("%sParameter\\%s"), m_pDoc->m_szCurrentFolder, sztemp);
	FilePathName.Format(_T("%s\\Templates\\Template%d.png"), FolderPathName, nIndex);
	if (!m_pTemplate[nIndex])
		m_pTemplate[nIndex] = imaqCreateImage(IMAQ_IMAGE_U8, 2);

	return m_NiVision.RWVisionFile(m_pTemplate[nIndex], FilePathName, bSave);
}

void CMainFrame::EnableSaftyDev(UINT nIndex, BOOL bOn)
{
	if (m_pDoc->m_cParam.nEnabeSafetyDev >> nIndex & 0x01)
	{
		if (bOn)
			m_bSaftyDevice |= 0x01 << nIndex;
		else
			m_bSaftyDevice &= ~(0x01 << nIndex);
	}
}

BOOL CMainFrame::CamPosition(UINT nPos, BOOL bWait /*= TRUE*/)
{
	UINT	nOrder(0);
	int i(0);
	double* pdPos(NULL);

	BOOL bSuccess(TRUE);
	LaserPoint tempPoint;
	ZeroMemory(&tempPoint, sizeof(LaserPoint));
	UINT nID = nPos >> 4;
	UINT nCmd = nPos & 0x0f;
	if (nID >> 1 || m_nCurrentPin[nID] >= MAX_PinNumber)
		return FALSE;
	nOrder = m_nCurrentProduct[nID][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nID][1];
	i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;

	pdPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][m_nCurrentVisionCheck[nID]].EndPoint;
	switch (nCmd)
	{
	case ZSAFE_POSITION:
		break;
	case SNAPSHOT_POSITION:
		tempPoint.EndPoint[0] = pdPos[0];
		tempPoint.EndPoint[1] = pdPos[2];//Z轴坐标
		bSuccess = m_pMotionCtrller[1]->AddRuninTo(0, tempPoint, TRUE);
		if (bSuccess&&bWait)
		{
			Sleep(50);
			//
			switch (WaitMotionDone(0x10+4, 10000))//
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停

				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[1]->AxStopDec(5);
				break;
			}
			//
			bSuccess = FALSE;
		}
		break;
	case Calibration_CHECK:
		tempPoint.EndPoint[0] = m_pDoc->m_cParam.PrjCfg.dXPosition[3];
		tempPoint.EndPoint[1] = m_pDoc->m_cParam.PrjCfg.dZPosition[3];
		bSuccess = m_pMotionCtrller[1]->AddRuninTo(0, tempPoint, TRUE);
		if (bSuccess&&bWait)
		{
			Sleep(150);
			switch (WaitMotionDone(0x10+4, 10000))
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停

				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[1]->AxStopDec(5);
				break;
			}
			bSuccess = FALSE;
		}
		break;
	case RANGING_POSITION:
		if (!m_pDoc->m_cParam.PrjCfg.bRangingMode)//行首式
		{
			pdPos = m_pDoc->m_cParam.PrjCfg.dRangingRel[nID][m_nCurrentRanging[nID]];
		}
		else//三点式
			pdPos = m_pDoc->m_cParam.PrjCfg.dRangingFixed[nID][m_nCurrentRanging[nID] % 3];//%3防越界

		tempPoint.EndPoint[0] = pdPos[0];
		tempPoint.EndPoint[1] = pdPos[2];
		bSuccess = m_pMotionCtrller[1]->AddRuninTo(0, tempPoint, TRUE);
		if (bSuccess&&bWait)
		{
			Sleep(150);
			switch (WaitMotionDone(0x10 + 4, 10000))
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停

				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[1]->AxStopDec(5);
				break;
			}
			bSuccess = FALSE;
		}

		break;
	default:
		return FALSE;
	}
	return bSuccess;
}

BOOL CMainFrame::ZCheckClimbPos(UINT nPos)
{
	BOOL	bSuccess(TRUE);
	UINT	nOrder(0);
	double dPos(0);
	int		i(0);
	UINT nID = nPos >> 4;
	UINT nCmd = nPos & 0x0f;
	if (nID >> 1 || m_nCurrentPin[nID] >= MAX_PinNumber)
		return FALSE;
	double dCampare;
	nOrder = m_nCurrentProduct[nID][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nID][1];
	switch (nCmd)
	{
	case ZSAFE_POSITION://安全高度
		dPos = m_pDoc->m_cParam.PrjCfg.dZPosition[0];
		break;
	case CLIMB_POINT://爬升点间安全高度
		i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;
		if (!i)//忽略的点
			return FALSE;
		dPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint[2] + m_pDoc->m_cParam.dCCD_Nozzle[2] + m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[2] - m_pDoc->m_cParam.PrjCfg.fClimbHeight[0];
		break;
	case CLIMB_PRODUCT://爬升产品间安全高度
		i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;
		if (!i)//忽略的点
			return FALSE;
		dPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint[2] + m_pDoc->m_cParam.dCCD_Nozzle[2] + m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[2] - m_pDoc->m_cParam.PrjCfg.fClimbHeight[1];
		break;
	case CLIMB_FIXER://爬升行间安全高度
		i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;
		if (!i)//忽略的点
			return FALSE;
		dPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint[2] + m_pDoc->m_cParam.dCCD_Nozzle[2] + m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[2] - m_pDoc->m_cParam.PrjCfg.fClimbHeight[2];
		break;
	default:
		return FALSE;
	}
	dCampare = dPos - m_pMotionCtrller[0]->m_dAxisCurPos[3];
	if (abs(dCampare) <= 0.05 || (dCampare>0.05))
		return TRUE;
	bSuccess = m_pMotionCtrller[0]->Move(4, dPos, m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[3], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[3]);
	Sleep(50);
	if (m_pMotionCtrller[0]->CheckMotionDone(0x08, 10000))
		return TRUE;
	bSuccess = FALSE;
	return bSuccess;
}

BOOL CMainFrame::ZGoPosition(UINT nPos, BOOL bCheck, BOOL bWait)
{
	BOOL	bSuccess(TRUE);
	UINT	nOrder(0);
	double dPos(0);
	int		i(0);
	UINT nID = nPos >> 4;
	UINT nCmd = nPos & 0x0f;
	if (nID >> 1 || m_nCurrentPin[nID]>=MAX_PinNumber)
		return FALSE;
	CString ErrMsg=_T("");

	nOrder = m_nCurrentProduct[nID][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nID][1];
	switch (nCmd)
	{
	case ZSAFE_POSITION://安全高度
		dPos = m_pDoc->m_cParam.PrjCfg.dZPosition[0];
		break;
	case NOZZLE_CHECK_POSITION://焊嘴照相
		dPos = m_pDoc->m_cParam.PrjCfg.dZPosition[1];
		break;
	case NOZZLE_CLEAN_POSITION://清洗焊嘴
		dPos = m_pDoc->m_cParam.PrjCfg.dZPosition[2];
		break;
	case Calibration_LASER://校正台焊接
		dPos = m_pDoc->m_cParam.PrjCfg.dZPosition[4];
		break;
	case LASER_POSITION://焊点焊接
		i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;
		if (!i)//忽略的点
			return FALSE;
		dPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint[2] + m_pDoc->m_cParam.dCCD_Nozzle[2] + m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[2];
		break;
	default:
		return FALSE;
	}
	switch (bCheck)
	{
	case FALSE:
		if (ZGoPosition(nPos, TRUE))
			return TRUE;
		bSuccess = m_pMotionCtrller[0]->Move(4, dPos, m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[3], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[3]);
		Sleep(50);
		if (!bWait||!bSuccess)
			break;
	case TRUE:
		if (bCheck)
		{
			switch (WaitMotionDone(3, 1))
			{
			case 0://到位
				if (fabs(m_pMotionCtrller[0]->m_dAxisCurPos[3] - dPos) < 0.02)
					return TRUE;
				return FALSE;
				break;
			case 1://急停
				break;
			case 2://停止
				break;
			case 3://超时
			default://
				break;
			}

		}
		else
		{
			if (m_pMotionCtrller[0]->CheckMotionDone(0x08, 10000))
				return TRUE;
		}
		if (nCmd)
			m_pMotionCtrller[0]->AxStopDec(4);
		ErrMsg = _T("触发超时事件!\r\n");
		bSuccess = FALSE;
		break;
	}
	return bSuccess;
}

BOOL CMainFrame::GoPosition(UINT nPos, BOOL bWait)
{
	UINT	nOrder(0);
	int i(0);
	double* pdPos(NULL);
	PointFloat fPt;
	ROI* roi = imaqCreateROI();
	Point pt;
	CoordinateSystem coordsys[2];
	ZeroMemory(coordsys, sizeof(coordsys));

	BOOL bSuccess(TRUE);
	LaserPoint tempPoint;
	ZeroMemory(&tempPoint, sizeof(LaserPoint));
	UINT nID = nPos >> 4;
	UINT nCmd = nPos & 0x0f;
	if (nID >> 1)
		return FALSE;
	nOrder = m_nCurrentProduct[nID][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nID][1];
	i = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].uVisionIndex;

	pdPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][m_nCurrentVisionCheck[nID]].EndPoint;
	switch (nCmd)
	{
	case ZSAFE_POSITION:
		bSuccess = ZGoPosition(ZSAFE_POSITION, FALSE, bWait);
		return bSuccess;
		break;
	case LOAD_POSITION:
		pdPos = &m_pDoc->m_cParam.PrjCfg.dYPosition[nID][0];
		m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, FALSE);
		bSuccess = m_pMotionCtrller[0]->Move(nID + 2, pdPos[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nID + 1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nID + 1]);
		if (bSuccess&&bWait)
		{
			Sleep(50);
			if (m_pMotionCtrller[0]->CheckMotionDone(0x02 << nID, 10000))
				return TRUE;
			m_pMotionCtrller[0]->AxStopDec(nID + 2);
			bSuccess = FALSE;
		}
		break;
	case PRE_SNAPSHOT_POSITION:
	case RANGING_POSITION:
		if (m_pDoc->m_cParam.PrjCfg.bUseRanging)
		{
			m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, FALSE);
			if (!m_pDoc->m_cParam.PrjCfg.bRangingMode)//行首式
			{
				pdPos = m_pDoc->m_cParam.PrjCfg.dRangingRel[nID][m_nCurrentRanging[nID]];
			}
			else//三点式
				pdPos = m_pDoc->m_cParam.PrjCfg.dRangingFixed[nID][m_nCurrentRanging[nID]%3];//%3防越界
			bSuccess = m_pMotionCtrller[0]->Move(nID + 2, pdPos[1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nID + 1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nID + 1]);
			if (bSuccess&&nCmd == RANGING_POSITION)
				bSuccess = CamPosition((nID << 4) + RANGING_POSITION);
			if (bSuccess&&bWait)
			{
				Sleep(50);
				if (m_pMotionCtrller[0]->CheckMotionDone(0x02 << nID, 10000))
					return TRUE;

				m_pMotionCtrller[0]->AxStopDec(nID + 2);
				bSuccess = FALSE;
			}
			break;
		}
	case SNAPSHOT_POSITION:
		m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, FALSE);
		bSuccess = m_pMotionCtrller[0]->Move(nID + 2, pdPos[1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nID + 1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nID + 1]);
		if (bSuccess&&nCmd==SNAPSHOT_POSITION)
			bSuccess=CamPosition((nID << 4) + SNAPSHOT_POSITION);
		if (bSuccess&&bWait)
		{
			Sleep(50);
			if (m_pMotionCtrller[0]->CheckMotionDone(0x02 << nID, 10000))
				return TRUE;

			m_pMotionCtrller[0]->AxStopDec(nID + 2);
			bSuccess = FALSE;
		}
		break;
	case PRE_LASER_POSITION:
		if (!i)//忽略的点
			return FALSE;
		tempPoint.EndPoint[1] = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint[1] + m_pDoc->m_cParam.dCCD_Nozzle[1];
		m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, FALSE);
		bSuccess = m_pMotionCtrller[0]->Move(nID + 2, tempPoint.EndPoint[1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nID + 1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nID + 1]);
		if (bSuccess&&bWait)
		{
			Sleep(50);
			if (m_pMotionCtrller[0]->CheckMotionDone(0x02 << nID, 10000))
				return TRUE;
			m_pMotionCtrller[0]->AxStopDec(nID + 2);
			bSuccess = FALSE;
		}
		break;
	case LASER_POSITION:
		if (i)//非忽略的点
		{
			pdPos = m_pDoc->m_cParam.PrjCfg.ProductArray[nID][nOrder][i - 1].EndPoint;
			/*坐标变换*/
			pt.x = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[0] * 1000;
			pt.y = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nID]].CenterPt[1] * 1000;
			imaqAddPointContour(roi, pt);
			coordsys[1].angle = m_dLaserOffset[nID][nOrder][m_nCurrentPin[nID]][2];
			imaqTransformROI2(roi, &(coordsys[0]), &(coordsys[1]));
			ContourInfo2* pInf = imaqGetContourInfo2(roi, imaqGetContour(roi, 0));
			double doffsetx = (double)pInf->structure.point[0].x / 1000.0;
			double doffsety = (double)pInf->structure.point[0].y / 1000.0;
			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
			if (m_pView->m_MainCtrlDlg.bAppointByMu)
			{
				tempPoint.EndPoint[0] = pdPos[0] + m_pDoc->m_cParam.dCCD_Nozzle[0] + m_dLaserOffset[nID][nOrder][m_nCurrentPin[nID]][0];
				tempPoint.EndPoint[1] = pdPos[1] + m_pDoc->m_cParam.dCCD_Nozzle[1] + m_dLaserOffset[nID][nOrder][m_nCurrentPin[nID]][1];
				m_pView->m_MainCtrlDlg.bAppointByMu = FALSE;
				m_nPinVisionSuccess[nID][nOrder][m_nCurrentPin[nID]] = FALSE;
			}
			else
			{
				tempPoint.EndPoint[0] = pdPos[0] + m_pDoc->m_cParam.dCCD_Nozzle[0] + doffsetx + m_dLaserOffset[nID][nOrder][m_nCurrentPin[nID]][0];
				tempPoint.EndPoint[1] = pdPos[1] + m_pDoc->m_cParam.dCCD_Nozzle[1] + doffsety + m_dLaserOffset[nID][nOrder][m_nCurrentPin[nID]][1];
			}
			m_pMotionCtrller[0]->ModifyGroup(0, 2 >> nID, FALSE);
			if (m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, TRUE))
			{
				bSuccess = m_pMotionCtrller[0]->AddRuninTo(0, tempPoint, TRUE);
				if (bSuccess&&bWait)
				{
					Sleep(150);
					if (m_pMotionCtrller[0]->CheckMotionDone((0x02 << nID) + 1, 5000))
						return TRUE;
					m_pMotionCtrller[0]->AxStopDec(5);
					bSuccess = FALSE;
				}else
					AddedErrorMsg(m_pMotionCtrller[0]->m_ErrMsg);

			}
			break;
		}
		return FALSE;
	case Calibration_CHECK:
		tempPoint.EndPoint[1] = m_pDoc->m_cParam.PrjCfg.dYPosition[nID][1];
		m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, FALSE);

		bSuccess = m_pMotionCtrller[0]->Move(nID + 2, tempPoint.EndPoint[1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nID + 1], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nID + 1]);
		if (bSuccess&&bWait)
		{
			Sleep(50);
			switch (WaitMotionDone(nID + 1, 10000))
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停
				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[0]->AxStopDec(nID + 2);
				break;
			}
			bSuccess = FALSE;
		}
		break;
	case Calibration_LASER:
		tempPoint.EndPoint[0] = m_pDoc->m_cParam.PrjCfg.dXPosition[4];
		tempPoint.EndPoint[1] = m_pDoc->m_cParam.PrjCfg.dYPosition[nID][2];
		m_pMotionCtrller[0]->ModifyGroup(0, 2 >> nID, FALSE);
		if (m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, TRUE))
		{
			bSuccess = m_pMotionCtrller[0]->AddRuninTo(0, tempPoint, TRUE);
			if (bSuccess&&bWait)
			{
				Sleep(150);
				switch (WaitMotionDone(4, 10000))
				{
				case 0://到位
					Sleep(200);
					return TRUE;
					break;
				case 1://急停
					break;
				case 2://停止
					break;
				case 3://超时
				default://
					m_pMotionCtrller[0]->AxStopDec(5);
					break;
				}
				bSuccess = FALSE;
			}
		}
		break;
	case NOZZLE_CHECK_POSITION:
		pdPos = &m_pDoc->m_cParam.PrjCfg.dXPosition[0];
		bSuccess = m_pMotionCtrller[0]->Move(1, pdPos[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[0]);
		if (bSuccess&&bWait)
		{
			Sleep(150);
			switch (WaitMotionDone(0, 10000))
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停

				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[0]->AxStopDec(1);
				break;
			}
			bSuccess = FALSE;
		}
		break;
	case NOZZLE_CLEAN_POSITION:
		pdPos = &m_pDoc->m_cParam.PrjCfg.dXPosition[nID + 1];

		bSuccess = m_pMotionCtrller[0]->Move(1, pdPos[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[0], m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[0]);
		if (bSuccess&&bWait)
		{
			Sleep(150);
			switch (WaitMotionDone(0, 10000))
			{
			case 0://到位
				return TRUE;
				break;
			case 1://急停

				break;
			case 2://停止
				break;
			case 3://超时
			default://
				m_pMotionCtrller[0]->AxStopDec(1);
				break;
			}
			bSuccess = FALSE;
		}
		break;
	case 0xff://NOZZLE_CLEAN_POSITION-extend
		tempPoint.EndPoint[0] = m_pDoc->m_cParam.PrjCfg.dXPosition[1 + nID];
		tempPoint.EndPoint[1] = m_pDoc->m_cParam.PrjCfg.dYPosition[nID][3];
		m_pMotionCtrller[0]->ModifyGroup(0, 2 >> nID, FALSE);
		if (m_pMotionCtrller[0]->ModifyGroup(0, 1 << nID, TRUE))
		{
			bSuccess = m_pMotionCtrller[0]->AddRuninTo(0, tempPoint, TRUE);
			if (bSuccess&&bWait)
			{
				Sleep(150);
				switch (WaitMotionDone(4, 10000))
				{
				case 0://到位
					Sleep(200);
					return TRUE;
					break;
				case 1://急停

					break;
				case 2://停止
					break;
				case 3://超时
				default://
					m_pMotionCtrller[0]->AxStopDec(5);
					break;
				}
				bSuccess = FALSE;
			}
		}
		break;
	default:
		return FALSE;
	}
	return bSuccess;
}

UINT _cdecl CMainFrame::ImgProcessThread(LPVOID lpParam)
{
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	BYTE nWp = pOperateOrder->nIndex[0];
	BYTE nCurrent[2] = { pOperateOrder->nIndex[1], pOperateOrder->nIndex[2]};
	UINT nIndex = pOperateOrder->nIndex[3];
	UINT nEventID = pOperateOrder->nIndex[4];
	Image*  CurrentImg = (Image*)pOperateOrder->pData;
	CString ErrMsg;
	UINT i(0);
	UINT nResult(0);
// 	ErrMsg.Format(_T("第%d行%d列产品图像正在%d号处理资源进行处理"), nCurrent[0]+1,nCurrent[1]+1, nEventID+1);
// 	CLogHelper::WriteLog(ErrMsg);
	if (nEventID > 8)
		return nResult;
	if (CurrentImg!=NULL)
	{
		nResult = pFrame->ImageProcess(CurrentImg, nIndex, nWp, nCurrent[0], nCurrent[1]);
		imaqDispose(CurrentImg);
		pOperateOrder->pData = NULL;
	}
	if (!nResult)
	{
		ErrMsg.Format(_T("%s : <第%d 照相> 图像处理失败!\r\n"), csz[nWp], nCurrent[1] + 1);
// 		if (pFrame->m_bAppoint&&pFrame->m_NiVision.m_pCalibration[0])
// 		{
// 			PointFloat fPt;
// // 			if ()
// // 			{
// // 				TransformReport* pReport = imaqTransformPixelToRealWorld(pFrame->m_NiVision.m_pCalibration[0], &fPt, 1);
// // 				fPt = pReport->points[0];
// // 				imaqDispose(pReport);
// // 			}
// // 
// 		}else
// 			pFrame->ShowPopup(ErrMsg);
	}
	ErrMsg.Format(_T("第%d行%d列产品图像已在%d号处理资源处理完毕，资源释放"), nCurrent[0] + 1, nCurrent[1] + 1, nEventID + 1);
	CLogHelper::WriteLog(ErrMsg);
	SetEvent(pFrame->m_hImgProcessEvent[nEventID + 1]);

	return nResult;

}

BOOL CMainFrame::StartLearning(UINT nIndex, Image* mask)
{
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	pLuckyBag pOperateOrder = new LuckyBag;
	pOperateOrder->pOwner = this;
	pOperateOrder->nIndex = new UINT;
	*pOperateOrder->nIndex = nIndex;
	pOperateOrder->pData = mask;
	if (m_bLearningAlive || !(pThread = AfxBeginThread(LearningProcess, (LPVOID)pOperateOrder)))
	{
		bSuccess = FALSE;
		SAFE_DELETE(pOperateOrder->nIndex);
		SAFE_DELETE(pOperateOrder);
	}
	return bSuccess;
}

UINT _cdecl CMainFrame::LearningProcess(LPVOID lpParam)
{
	UINT nSuccess = FALSE;
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	UINT nID = *(pOperateOrder->nIndex);
	Image* mask = (Image*)pOperateOrder->pData;
	SAFE_DELETE(pOperateOrder->nIndex);
	SAFE_DELETE(pOperateOrder);
	if (pFrame)
	{
		::PostMessage(pFrame->m_pView->m_hWnd, WM_USER_UPDATEUI, 4<<4, 0);
		Sleep(5);
		pFrame->m_bLearningAlive = TRUE;
		nSuccess = pFrame->LearnTemplate(nID, mask);
		TRACE("\n-LearnTemplate over\n");
		pFrame->m_bLearningAlive = FALSE;
		Sleep(5);
		::PostMessage(pFrame->m_pView->m_hWnd, WM_USER_UPDATEUI, 4<<4, 1);
	}
	if (nSuccess)
	{
		pFrame->m_pDoc->m_cParam.PrjCfg.uTemplateSts |= 1 << nID;
		::AfxMessageBox(_T("学习完成"));
	}
	imaqDispose(mask);
	return nSuccess;
}

BOOL CMainFrame::LearnTemplate(UINT nIndex, Image* mask)
{

	BOOL bSuccess = FALSE;
	Image* CurrentImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	Image* TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	m_NiVision.GetImage(0, CurrentImg,FALSE);
	if (m_pDoc->m_cParam.PrjCfg.bUseBinary)
		m_NiVision.CreateBinary(CurrentImg, CurrentImg, 2);
	if (m_pDoc->m_cParam.PrjCfg.tTemplateArry[nIndex].bType)
		bSuccess = m_NiVision.CreateGeomTemplate(TempImg, CurrentImg, m_pDoc->m_cParam.PrjCfg.tTemplateArry[nIndex].rcTemp,
		m_pDoc->m_cParam.PrjCfg.tTemplateArry[nIndex].nContrast);
	else
		bSuccess = m_NiVision.CreateTemplate(TempImg, CurrentImg, m_pDoc->m_cParam.PrjCfg.tTemplateArry[nIndex].rcTemp);
	if (bSuccess){

		if (!m_pTemplate[nIndex])
			m_pTemplate[nIndex] = imaqCreateImage(IMAQ_IMAGE_U8, 2);
		imaqDuplicate(m_pTemplate[nIndex], TempImg);
		m_NiVision.DisplayImage(m_pTemplate[nIndex]);
	}
	
	imaqDispose(TempImg);
	imaqDispose(CurrentImg);

	return bSuccess;
}

LRESULT CMainFrame::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	int i = 0;
	CString str;
	static BOOL bFirst( TRUE);
	switch (wParam)
	{
	case 0://回零
		if (lParam != 0)//结束
		{
			m_nCurrentRunMode = 0x00;
			::PostMessage(m_pView->m_hWnd, WM_USER_UPDATEUI, 0, m_nUserType);
			if (bFirst&&m_nIsHomeEnd)
			{
				bFirst = FALSE;
				OnConfirmProject();
			}

		}
		else{
			m_pView->m_wndTab.SetActiveTab(0);
			m_nCurrentRunMode = 0x01;
			::PostMessage(m_pView->m_hWnd, WM_USER_UPDATEUI, 0, lParam);

		}
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0,0);
		UpdateTitle();
		break;
	case 1://自动
		if (lParam != 0)//结束
		{
			m_bWaitforTrigger = 0;
			SetButtonLamp(0, FALSE);
			SetButtonLamp(1, FALSE);
			SetButtonLamp(2, TRUE);
			m_nCurrentRunMode = 0x00;
			SetDockReady(0, FALSE);
			SetDockReady(1, FALSE);

		}
		else{
			m_pView->m_wndTab.SetActiveTab(0);
			SetButtonLamp(2, FALSE);
			m_nCurrentRunMode = 0x02;

		}
		::PostMessage(m_pView->m_hWnd, WM_USER_UPDATEUI, 0, lParam);
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0, 0);
		// 
// 		::PostMessage(m_pView->m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0, sts[2]);

		break;
	case 2:
		PostMessage(WM_USER_UPDATEUI,0,1);
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 0, 0);
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 2, 0);
		::PostMessage(m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 4, 0);
		::PostMessage(m_pView->m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 2, 0);
		::PostMessage(m_pView->m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 3, 0);
		::PostMessage(m_pView->m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 4, 0);
		::PostMessage(m_pView->m_VisionCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 5, 0);
		break;
	case 3:
		break;
	case 4:
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 1, 0);
		::PostMessage(m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 1, 0);

		break;
	case 5:
		str.Format(_T("气压实时：%d"),lParam);
		m_wndStatusBar.SetPaneText(1, str);
		break;
	case 6:

		break;
	case 7:
		ShowWaiteProcessDlg(lParam);
		break;
// 	case 7:
// 		m_pPrjSettingDlg->ShowWindow(SW_SHOW);
// 		break;
	}
	return 0;
}

BOOL CMainFrame::ShowHomeProcessDlg(BOOL bShow /*= TRUE*/)
{
	if (bShow){

		if (m_pHomeProcessDlg == NULL)
			m_pHomeProcessDlg = new CHomeProcessDlg;
		if (m_pHomeProcessDlg &&  m_pHomeProcessDlg->GetSafeHwnd() == NULL)
			m_pHomeProcessDlg->Create(CHomeProcessDlg::IDD, this);
		m_pHomeProcessDlg->ShowWindow(SW_RESTORE);
	}
	else if (m_pHomeProcessDlg != NULL)
	{
		m_pHomeProcessDlg->DestroyWindow();

		SAFE_DELETE(m_pHomeProcessDlg);
	}
	return TRUE;
}

BOOL CMainFrame::SetButtonLamp(BYTE nIndex, BYTE nOn/*=FALSE*/)
{
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Panel_lamp[nIndex], nOn);//状态灯/蜂鸣
	return 1;
}

BOOL CMainFrame::StartReturnHome()
{
	CString ErrorMsg;
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	m_LaserCtrl.SetbondServoRDY();
	if (!m_bHomeThreadAlive[0] )
	{
		for (UINT j = 0; j < m_pMotionCtrller[0]->m_ulAxisCount; j++)
		{
			m_pMotionCtrller[0]->m_dHomeVel[j] = m_pDoc->m_cParam.HOME_VEL[j];
			m_pMotionCtrller[0]->m_dHomeAcc[j] = m_pDoc->m_cParam.HOME_ACC[j];
			m_pMotionCtrller[1]->m_dHomeVel[j] = m_pDoc->m_cParam.HOME_VEL[j+4];
			m_pMotionCtrller[1]->m_dHomeAcc[j] = m_pDoc->m_cParam.HOME_ACC[j+4];

		}
		if (m_bAutoThreadAlive)
		{
			bSuccess = StartAuto(FALSE);
		}
		if (bSuccess)
		{
			m_pStopRun->ResetEvent();
			pThread = AfxBeginThread(HomeThread, this);
			if (!pThread)
			{
				ErrorMsg = _T("警告：启动复位失败！\r\n");
				AddedErrorMsg(ErrorMsg);
				bSuccess = FALSE;
			}
			else
				PostMessage(WM_USER_UPDATEUI, 0, 0);

		}
	}
	if (m_LaserCtrl.GetPLCRDY())
	{
		if (!m_LaserCtrl.IsHoming() && !m_LaserCtrl.IsHomed())
		{
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[0], FALSE);
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[1], TRUE);
			m_hBondHomeEnd.ResetEvent();
		}
	}
	return bSuccess;
}

UINT _cdecl CMainFrame::HomeThread(LPVOID lpParam)
{
	CMainFrame* pFrame = (CMainFrame*)(lpParam);
	pFrame->m_bHomeThreadAlive[0] = TRUE;
	pFrame->m_nIsHomeEnd = 0;
	BYTE dir = 0x00;
	int nResult(1);
	HWND hWnd = pFrame->m_pHomeProcessDlg->m_hWnd;

	::PostMessage(hWnd, WM_USER_HOMING, 0, 0);
	pFrame->m_hHomehExtThread.ResetEvent();
	CWinThread* pThread = AfxBeginThread(pFrame->HomeThreadExt, pFrame);
	if (pThread)
	{
		pFrame->m_bHomeThreadAlive[0] = 0x03;
		nResult = pFrame->m_pMotionCtrller[0]->AxisHSPHome(1 << U_AXIS, dir);
		::PostMessage(hWnd, WM_USER_HOMING, (WPARAM)(0x08), nResult);
		if (WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))
		{
			if (nResult == 1)
			{
				nResult = pFrame->m_pMotionCtrller[0]->AxisHSPHome(XYZ_AXIS, dir);
				::PostMessage(hWnd, WM_USER_HOMING, (WPARAM)XYZ_AXIS, nResult);
			}
			if (::WaitForSingleObject(pFrame->m_hHomehExtThread.m_hObject, 4500) == WAIT_TIMEOUT)
			{
				pFrame->m_pMotionCtrller[1]->SetEStop(TRUE);
				::PostMessage(hWnd, WM_USER_HOMING, (WPARAM)(0x30), 0);

				pFrame->AddedErrorMsg(_T("相机所在轴回零超时,请检查后重试!\r\n"));
			}
			else{
				BOOL bExtHomeResult = (pFrame->m_bHomeThreadAlive[0] >> 1) ? 0 : 1;
				::PostMessage(hWnd, WM_USER_HOMING, (WPARAM)(0x30),bExtHomeResult );
				if (WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))
				{
					if (nResult == 1 && bExtHomeResult)
					{
						Sleep(10);
						pFrame->m_pMotionCtrller[0]->SetCurrentPos(0);
						pFrame->m_pMotionCtrller[1]->SetCurrentPos(0);
						do
						{
							if (!pFrame->m_LaserCtrl.IsHomed())
							{
								if (pFrame->m_LaserCtrl.GetPLCRDY()&&pFrame->m_LaserCtrl.IsHoming())
								{
									HANDLE hEvent[2] = { pFrame->m_pStopRun->m_hObject, pFrame->m_hBondHomeEnd};
									DWORD dEvent = ::WaitForMultipleObjects(2, hEvent, FALSE, pFrame->m_pDoc->m_cParam.nBondHomeTime*1000);
									if (dEvent==(WAIT_OBJECT_0+1))
										break;
								}
								::AfxMessageBox(_T("焊头此刻未准备好,请检查并稍后再次手动复位!"));
							}
						} while (FALSE);
						pFrame->m_nIsHomeEnd = 1;
					}
					else
						pFrame->AddedErrorMsg(pFrame->m_pMotionCtrller[0]->m_ErrMsg);
				}
			}
		}
	}else
		::AfxMessageBox(_T("相机轴启动回零失败!"));
	pFrame->m_bHomeThreadAlive[0] = FALSE;
	::PostMessage(pFrame->m_hWnd, WM_USER_HOMED, (WPARAM)XYZU_AXIS, nResult);

	return 0;
}

UINT _cdecl CMainFrame::HomeThreadExt(LPVOID lpParam)
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, _T("HomeThreadExt"));
	if (GetLastError() == ERROR_ALREADY_EXISTS) { return FALSE; }
	CMainFrame* pFrame = (CMainFrame*)(lpParam);
	BYTE dir = 0x00;
	int nResult(0);
	nResult = pFrame->m_pMotionCtrller[1]->AxisHSPHome(XY_AXIS, dir);
	if (nResult != 1)
		pFrame->AddedErrorMsg(pFrame->m_pMotionCtrller[1]->m_ErrMsg);
	pFrame->m_bHomeThreadAlive[0] &= 0x03>>nResult;
	pFrame->m_hHomehExtThread.SetEvent();
	CloseHandle(hMutex);
	return 0;
}

LRESULT CMainFrame::OnHomed(WPARAM wParam, LPARAM lParam)
{
	LONG sts(0);
	if (wParam == XYZU_AXIS)//线程消息
	{
		m_pHomeProcessDlg->DestroyWindow();

		PostMessage(WM_USER_UPDATEUI, 0, lParam == 1 ? 1 : -1);
	}
	else{
		if (m_bHomeThreadAlive[0])
		{
			m_pMotionCtrller[0]->SetEStop(TRUE);
			m_pMotionCtrller[1]->SetEStop(TRUE);
		}else
			StopAutoRun();

	}
	return 0;
}

void CMainFrame::StopAutoRun()
{
	m_pStopRun->SetEvent();
	m_hOneCycleStart[0].ResetEvent();
	m_hOneCycleStart[1].ResetEvent();
	m_pMotionCtrller[0]->AxStopDec();
	m_pMotionCtrller[1]->AxStopDec();
	LaserOut(FALSE);//停止输出；
	m_pView->m_MainCtrlDlg.bAppointByMu = FALSE;
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[2], FALSE);
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);
	m_LaserCtrl.SetLaserRDY(FALSE);
	return;
}

BOOL CMainFrame::LightControllerOpen(BOOL bOpen/*=TRUE*/)
{
	BOOL bSuccess(!bOpen);
	int i(0);
	if (bOpen)
	{
		if (!m_LightControl.IsOpen())
		{
			if (m_LightControl.InitPort(m_hWnd, m_pDoc->m_cParam.nComPort[1], 19200, 'N', 8, 1, EV_RXFLAG | EV_RXCHAR))
			{
				bSuccess = m_LightControl.StartMonitoring();
			}
// 			else
// 				ShowPopup(_T("光源控制端口打开失败！"));
		}
		else
			bSuccess = TRUE;
	}else if (m_LightControl.IsOpen())
			m_LightControl.ClosePort();

	return bSuccess;
}

BOOL CMainFrame::LightControllerSet(BYTE nID, BYTE nValue, BYTE nMode)
{
	int i(0);
	int hexlenth(0);
	BYTE sum(0);
	CHAR str[2];
	CStringA szCmdHead;
	CStringA szCmdCode;
	CStringA szMode;
	CStringA result = "";
	switch (nMode)
	{
	case 0:
		if (nValue > 1)
			nValue = 0xff;
		szCmdCode.Format("%1d", nValue);
		szMode = "L";

		break;
	case 1:
		if (nValue > 10)
			nValue = 0;
		szMode = "S";

		szCmdCode.Format("%02d", nValue);
		break;
	case 2:
		if (nValue > m_pDoc->m_cParam.nLightLimit[nID])
			nValue = m_pDoc->m_cParam.nLightLimit[nID];

		szCmdCode.Format("%03d", nValue);
		szMode = "F";
		break;
	default:
		return 0;
	}
	szCmdHead.Format("@%02d%s%s00", nID, szMode, szCmdCode);
	hexlenth = szCmdHead.GetLength();
	for (i = 0; i < hexlenth; i++)
	{
		int converted = 0;
		str[0] = szCmdHead.GetAt(i);
		long lhex = str[0];
		sum += lhex;
	}
	result.Format("%02X", sum);
	szCmdHead += result+'\r'+ '\n';
	if (m_LightControl.IsOpen())
		m_LightControl.WriteToPort(szCmdHead);
	return 1;
}

void CMainFrame::SetUserType(BYTE nId)
{
	m_nUserType = nId; 
// 	if (m_nIsHomeEnd&&!m_nCurrentRunMode||nId==2)
		PostMessage(WM_USER_UPDATEUI, 0, 1);
}

BYTE CMainFrame::GetUserType()
{
	return m_nUserType;
}

LRESULT CMainFrame::OnCommRecvChar(WPARAM wParam, LPARAM port)
{
	CString strTemp;
	strTemp.Format(_T("%02X"), wParam);
//	strTemp.Format(_T("%c"), wParam);

	return 0;
}

LRESULT CMainFrame::OnCommRecvString(WPARAM wString, LPARAM lLength)
{
	DWORD nLength = (DWORD)lLength;
	CString strTemp;

// 	LPCTSTR lpBuffer = (LPCTSTR)wString;
// 	for (DWORD i = 0; i < nLength; i++)
// 		strTemp += (TCHAR)lpBuffer[0];
	PulseEvent(m_hRs485Event);
	strTemp = (LPCTSTR)wString;
	TRACE1("%S\r\n",strTemp);
	return 0;
}

UINT CMainFrame::WorkProcess(UINT nWp, UINT nStep)
{
	UINT nSuccess(0);
	UINT i(0);
	CString ErrorMsg=_T("");
	UINT nTotal = m_pDoc->m_cParam.PrjCfg.bProductNum[0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];
	// 	::PostMessage(pFrame->m_hWnd, WM_USER_SHOWSTATUS, MachineNo, 5);
	if (!m_bAutoThreadAlive)
		m_pStopRun->ResetEvent();
	DWORD dEvent(0);
	switch (nStep)
	{
	case START_LOAD:
		m_nCurrentProduct[nWp][0] = 0;
		m_nCurrentProduct[nWp][1] = 0;
		m_nCurrentVisionCheck[nWp] = 0;
		m_nCurrentPin[nWp] = 0;
		ZeroMemory(m_nPinVisionSuccess[nWp], sizeof(BYTE) * 1600);
		ZeroMemory(m_dLaserOffset[nWp], sizeof(double) * 4800);
		nSuccess = StartSlaveThread(nWp, nStep);
		if (!nSuccess)
			ErrorMsg.Format(_T("%s : 开始上料进程失败!\r\n"), csz[nWp]);
		break;
	case RANGING:
		if (!m_bAutoThreadAlive)
		{
			if (!IsZSafe())
			{
				if (!GoPosition(ZSAFE_POSITION))
				{
					AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
					break;
				}
			}
		}
		theApp.m_bHigh_Risk[nWp] = TRUE;
		nSuccess = StartSlaveThread(nWp, nStep);
		if (!nSuccess)
			ErrorMsg.Format(_T("%s : 开始测距进程失败!\r\n"), csz[nWp]);
		break;
	case SNAPSHOT_MATCH:
		if (!m_bAutoThreadAlive)
		{
			if (!IsZSafe())
			{
				if (!GoPosition(ZSAFE_POSITION))
				{
					AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
					break;
				}
			}
		}
		ZeroMemory(m_nPinVisionSuccess[nWp][nCurl], sizeof(BYTE) * MAX_PinNumber);
		ZeroMemory(m_dLaserOffset[nWp][nCurl], sizeof(double) * MAX_PinNumber*3);
		if (SwitchCamera(0, TRUE))
			nSuccess = StartSlaveThread(nWp, nStep);
		else
			ErrorMsg.Format(_T("%s : 激活相机失败!\r\n"), csz[nWp]);
		if (!nSuccess)
		{
			m_bAppoint = FALSE;
			ErrorMsg.Format(_T("%s : 启动照相进程失败!\r\n"), csz[nWp]);
		}
		break;
	case BONDING_READY:
		if (!m_bAutoThreadAlive)
		{
			if (!IsZSafe())
			{
				if (!GoPosition(ZSAFE_POSITION))
				{
					AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
					break;
				}
			}

		}
		ResetEvent(m_hProcessEvent[nWp][BONDING_READY_DONE]);
		for (i = m_nCurrentPin[nWp]; i < m_pDoc->m_cParam.PrjCfg.nPinNum; i++)
		{
			m_nCurrentPin[nWp] = i;
			if (m_pDoc->m_cParam.PrjCfg.PinArray[i].uVisionIndex)
			{

				if (!m_nPinVisionSuccess[nWp][nCurl][i])
				{
					CString sz;
					sz.Format(_T("注意 : <行 %d 列 %d 第%d 焊点> 未识别!\r\n"), m_nCurrentProduct[nWp][0] + 1, m_nCurrentProduct[nWp][1] + 1, i + 1);
					ErrorMsg += sz;
					nSuccess = 2;
					continue;
				}
				nSuccess = StartSlaveThread(nWp, nStep);
				if (!nSuccess)
					ErrorMsg.Format(_T("%s : 启动焊接准备失败!\r\n"), csz[nWp]);

				break;
			}
		}
		if (!m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nWp]].uVisionIndex)
			nSuccess = 2;
		break;
	case BALL_LASER:
		nSuccess = StartSlaveThread(nWp, nStep);
		if (!nSuccess)
			ErrorMsg.Format(_T("%s : 启动焊接失败!\r\n"), csz[nWp]);
		break;
	case NOZZLE_CLEAN:
	case BALLS_SUPPLY:
		nSuccess = StartSlaveThread(nWp, nStep);
		if (!nSuccess)
			ErrorMsg.Format(_T("%s : 启动清洗或加球失败!\r\n"), csz[nWp]);
		break;
	default:
		nSuccess = StartSlaveThread(nWp, WAIT_BTN_START);
		if (!nSuccess)
			ErrorMsg.Format(_T("%s : 启动等待开始按钮失败!\r\n"), csz[nWp]);
		return TRUE;
		break;
	}
	if (!ErrorMsg.IsEmpty())
	{
		AddedErrorMsg(ErrorMsg);
	}
	return nSuccess;
}

BOOL CMainFrame::StartAuto(BOOL bEnable /*= FALSE*/)
{
	CString ErrorMsg;
	CWinThread* pThread(NULL);
	BOOL bResult(!bEnable);
	m_pView->m_MainCtrlDlg.bAppointByMu = FALSE;
	if (bEnable)
	{
		if (!m_bAutoThreadAlive && !m_bHomeThreadAlive[0] && m_nIsHomeEnd&&SwitchCamera(0))
		{
			if (!m_LaserCtrl.GetPLCRDY())
			{
				AddedErrorMsg(_T("PLC 未准备好\r\n"));
				return FALSE;
			}
			m_LaserCtrl.SetLaserRDY();
			m_pStopRun->ResetEvent();
			m_pMotionCtrller[0]->ResetAxisErr();
			m_pMotionCtrller[1]->ResetAxisErr();
			m_nCurrentWP[CAMERA] = 0x03;
			m_hCameraFree.SetEvent();
			m_nCurrentWP[NOZZLE] = 0x03;
			m_hNozzleFree.SetEvent();
			pLuckyBag pOperateOrder = new LuckyBag;
			pOperateOrder->pOwner = this;
			pOperateOrder->nIndex = new UINT;
			*pOperateOrder->nIndex = 0;
			pThread = AfxBeginThread(AutoThread, (LPVOID)pOperateOrder);
			if (!pThread)
			{
				SAFE_DELETE(pOperateOrder->nIndex);
				SAFE_DELETE(pOperateOrder);
				ErrorMsg = _T("警告：启动自动模式线程失败！\r\n");
				AddedErrorMsg(ErrorMsg);
			}
			else{
				m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[0], TRUE);
				bResult = TRUE;
				PostMessage(WM_USER_UPDATEUI, 1, 0);

				QueryPerformanceCounter(&m_lTimestart);
				m_lQuantityStart = m_pView->m_MainCtrlDlg.m_lQuantity;
				UINT n=SetTimer(1, 1000, NULL);
				if (n ^ 1)
					AfxMessageBox(_T("定时器1启动异常"));
			}
		}
		else
			bResult = m_bAutoThreadAlive;
	}
	else{
		StopAutoRun();
	}
	return bResult;
}

UINT _cdecl CMainFrame::AutoThread(LPVOID lpParam)
{
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	UINT nIndex = pOperateOrder->nIndex[0];
	// 	UINT nId = pOperateOrder->nIndex[1];
	UINT nId = 0;
	SAFE_DELETE(pOperateOrder->nIndex);
	SAFE_DELETE(pOperateOrder);
	BOOL bExit(FALSE);
	pFrame->m_bAutoThreadAlive = TRUE;
	do
	{
		UINT i(0);
		UINT nStep(0);
		UINT nResult[2] = {3,3};
		if (!pFrame->IsZSafe())
		{
			if (!pFrame->GoPosition(ZSAFE_POSITION))
			{
				pFrame->AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
				break;
			}
		}
		for (i = 0; (i < 2); i++)
		{
			Sleep(1);
			if (pFrame->m_bSalveThreadAlive[i] || WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_hSlaveEvent[i], 0))
			{
				bExit = TRUE;
				break;
			}
			if (pFrame->m_nSemiRun[i])
			{
				pFrame->WorkProcess(i, WAIT_BTN_START);
			}
		}

		while ((!bExit)&&pFrame->MasterWaitFor(nId,nResult))
		{
			nResult[nId] = pFrame->WorkProcess(nId, pFrame->m_nSemiRun[nId]);
			bExit = !nResult[nId];
		}
	} while (FALSE);
	pFrame->m_pStopRun->SetEvent();
	pFrame->MasterWaitFor(0,0xff);

	pFrame->m_bAutoThreadAlive = FALSE;
	::PostMessage(pFrame->GetSafeHwnd(), WM_USER_UPDATEUI, 1, 1);
	return 0;
}

int CMainFrame::MasterWaitFor(UINT nWp, UINT nStep)
{
	UINT nResult(0);
	HANDLE hEvent[4] = { m_pEstop->m_hObject, m_pStopRun->m_hObject,0,0};
	DWORD dEvent(0);
	UINT nTotal = m_pDoc->m_cParam.PrjCfg.bProductNum[0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];

// 	switch (nStep)
// 	{
// 	default:
// 	}
	TRACE(" Wait Two slaveEvent \r\n");
	dEvent = ::WaitForMultipleObjects(2, m_hSlaveEvent, TRUE, 5000);
	switch (dEvent)
	{
	case WAIT_OBJECT_0:
		break;
	case WAIT_TIMEOUT:
		break;
	}
	if (!IsZSafe() && !ZGoPosition(ZSAFE_POSITION, FALSE, FALSE))
		AddedErrorMsg(_T("Nozzle Start Move to Safe_Heigh Position Fail!\r\n"));
	return nResult;
}

int CMainFrame::MasterWaitFor(UINT &nWp, UINT nFlage[2])
{
	UINT nResult(0);
	HANDLE hEvent[4] = { m_pEstop->m_hObject, m_pStopRun->m_hObject, m_hSlaveEvent[0], m_hSlaveEvent[1]};
	DWORD dEvent(0);
	UINT nTotal = m_pDoc->m_cParam.PrjCfg.bProductNum[0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	dEvent = ::WaitForMultipleObjects(4, hEvent, FALSE, INFINITE);
	switch (dEvent)
	{
	case WAIT_OBJECT_0://急停
	case WAIT_OBJECT_0 + 1://停止
	default:
		if (m_pDoc->m_cParam.PrjCfg.bUseRanging)//
		{
			if (!m_pDoc->m_cParam.PrjCfg.bRangingMode)//行首式
			{
				if (m_nSemiRun[0] == SNAPSHOT_MATCH)
				{
					m_nCurrentRanging[0] = (m_nCurrentProduct[0][0] << 1) | 0x01;
					m_nSemiRun[0] = RANGING;
				}
				if (m_nSemiRun[1] == SNAPSHOT_MATCH)
				{
					m_nCurrentRanging[1] = (m_nCurrentProduct[1][0] << 1) | 0x01;
					m_nSemiRun[1] = RANGING;
				}
			}
			else
			{
				m_nCurrentRanging[0] = 0;
				m_nCurrentRanging[1] = 0;
				if (m_nSemiRun[0] == SNAPSHOT_MATCH)
					m_nSemiRun[0] = RANGING;

				if (m_nSemiRun[1] == SNAPSHOT_MATCH)
					m_nSemiRun[1] = RANGING;
			}
		}
		if (m_nSemiRun[0] >BONDING_READY)
			m_nSemiRun[0] = BONDING_READY;
		if (m_nSemiRun[1] >BONDING_READY)
			m_nSemiRun[1] = BONDING_READY;

		return nResult;
		break;
	case WAIT_OBJECT_0 + 2://左ready
	case WAIT_OBJECT_0 + 3://右ready
		nWp = dEvent - WAIT_OBJECT_0 - 2;
		TRACE2(" Wait slavethread done event %d - %d \r\n", nWp, m_nSemiRun[nWp]);
		break;
	}
	UINT nStep = m_nSemiRun[nWp];
	UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];
	switch (nStep)
	{
	case LOAD_DONE:
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][LOAD_DONE], 0))
		{
			if (nFlage[nWp]>>1)
				nResult = 1;
			break;
		}
		if (m_pDoc->m_cParam.PrjCfg.bUseRanging)
		{
// 			if (m_pDoc->m_cParam.PrjCfg.bRangingMode)//三点式
// 				m_nCurrentRanging[nWp] = 0;
// 			else
// 				m_nCurrentRanging[nWp] = m_nCurrentProduct[nWp][0] << 1;
			m_nCurrentRanging[nWp] = 0;
			m_nSemiRun[nWp] = RANGING;
		}
		else
			m_nSemiRun[nWp] = SNAPSHOT_MATCH;
		nResult = 1;
		break;
	case RANGING_DONE:
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][RANGING_DONE], 0))
		{
			if (!(nFlage[nWp] >> 1))
				m_nSemiRun[nWp] = START_LOAD;
			nResult = 1;
			m_nCurrentWP[CAMERA] = 0x03;//释放相机轴
			m_hCameraFree.SetEvent();
			break;
		}
		if (m_pDoc->m_cParam.PrjCfg.bRangingMode)//三点式
		{
			if (m_nCurrentRanging[nWp] >>1)
			{
				m_nCurrentRanging[nWp] = 0;
				m_nSemiRun[nWp] = SNAPSHOT_MATCH;
			}
			else
				m_nCurrentRanging[nWp]++;
		}
		else
		{
			if ((m_nCurrentRanging[nWp]>>1) < (m_pDoc->m_cParam.PrjCfg.bProductNum[0] - 1))
				m_nCurrentRanging[nWp]+=0x02;
			else
			{
				m_nCurrentRanging[nWp] = 0;
				m_nSemiRun[nWp] = SNAPSHOT_MATCH;
			}
// 			if (!(m_nCurrentRanging[nWp] & 0x01))//倒序
// 				m_nSemiRun[nWp] = SNAPSHOT_MATCH;
// 			m_nCurrentRanging[nWp] ^= 0x01;

		}
		nResult = 1;
		break;
	case SNAPSHOT_MATCH_DONE:
		TRACE2(" Wait slaveEvent %d - %d \r\n", nWp, nStep);
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][SNAPSHOT_MATCH_DONE], 0))
		{
			if (theApp.m_bHigh_Risk[nWp])
			{
				nResult = 1;
				m_nSemiRun[nWp] = START_LOAD;
				break;
			}
			if (nFlage[nWp] >> 1)
				nResult = 1;
			break;
		}
		if (nCurl < (nTotal - 1))
		{
			nCurl++;
			m_nCurrentProduct[nWp][0] = nCurl / m_pDoc->m_cParam.PrjCfg.bProductNum[1];
			m_nCurrentProduct[nWp][1] = nCurl - (m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1]);
// 			if (m_pDoc->m_cParam.PrjCfg.bUseRanging&&!m_nCurrentProduct[nWp][1])//行结束
// 			{
// 				if (!m_pDoc->m_cParam.PrjCfg.bRangingMode)//行首式
// 				{
// 					m_nCurrentRanging[nWp] = (m_nCurrentProduct[nWp][0] << 1)|0x01;
// 					m_nSemiRun[nWp] = RANGING;
// 				}
// 			}

		}
		else
		{
			m_nCurrentProduct[nWp][0] = 0;
			m_nCurrentProduct[nWp][1] = 0;
			m_nSemiRun[nWp] = BONDING_READY;
			m_nCurrentWP[CAMERA] = 0x03;
			m_hCameraFree.SetEvent();
		}
		PostMessage(WM_USER_UPDATEUI, 4, 0);

		nResult = 1;
		break;
	case BONDING_READY_DONE:
		TRACE2(" Wait event %d - %d \r\n", nWp, nStep);

		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][BONDING_READY_DONE], 0))
		{
			if (!IsZSafe() && !ZGoPosition(ZSAFE_POSITION, FALSE, TRUE))
			{
				AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
				break;
			}
			if (m_nBondAlarm ^ 0x01)//异常
			{
				if (m_nBondAlarm ^ 0x05)//其他异常与无球一样处理
					m_nSemiRun[nWp] = BALLS_SUPPLY;
				else//仅仅堵球
					m_nSemiRun[nWp] = NOZZLE_CLEAN;
				nResult = 1;
			}
			else
			{
				if (nFlage[nWp] >> 1)
				{
					nResult = 1;
					if (nFlage[nWp] == 2)
					{
						m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ERASING_STS);
						if (nCurl < (nTotal - 1))
						{
							nCurl++;
							m_nCurrentPin[nWp] = 0;
							m_nCurrentProduct[nWp][0] = nCurl / m_pDoc->m_cParam.PrjCfg.bProductNum[1];
							m_nCurrentProduct[nWp][1] = nCurl - (m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1]);
							m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ADD_STS);
						}
						else
						{
							m_nCurrentProduct[nWp][0] = 0;
							m_nCurrentProduct[nWp][1] = 0;
							m_nCurrentPin[nWp] = 0;
							m_nSemiRun[nWp] = START_LOAD;
						}

					}
				}
			}
			break;
		}
		m_nSemiRun[nWp] = BALL_LASER;
		nResult = 1;
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);//清除sq

		break;
	case BALL_LASER_DONE:
		TRACE2(" Wait event %d - %d \r\n", nWp, nStep);
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][BALL_LASER_DONE], 0))
		{
			m_nSemiRun[nWp] = BONDING_READY;
			if (!IsZSafe() && !ZGoPosition(ZSAFE_POSITION, FALSE, TRUE))
			{
				AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
				break;
			}
			if (m_nBondAlarm ^ 0x01)//异常
			{
				if (m_nBondAlarm ^ 0x05)//其他异常与无球一样处理
					m_nSemiRun[nWp] = BALLS_SUPPLY;
				else//仅仅堵球
					m_nSemiRun[nWp] = NOZZLE_CLEAN;
				nResult = 1;

			}
			else{
				if (nFlage[nWp] >> 1)
					nResult = 1;
			}
			break;
		}
		if (m_nCurrentPin[nWp] >= (m_pDoc->m_cParam.PrjCfg.nPinNum - 1))//一个产品结束
		{
			m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ERASING_STS);
			m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::WELDING_OK << 1, CMainCtrlDlg::ADD_STS);
			if (nCurl < (nTotal - 1))
			{
				nCurl++;
				m_nCurrentProduct[nWp][0] = nCurl / m_pDoc->m_cParam.PrjCfg.bProductNum[1];
				m_nCurrentProduct[nWp][1] = nCurl - (m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1]);
				m_nCurrentPin[nWp] = 0;
				m_nSemiRun[nWp] = BONDING_READY;
				m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ADD_STS);
				if (m_nCurrentProduct[nWp][1])
					m_bNeedClimb = CLIMB_PRODUCT;
				else
					m_bNeedClimb = CLIMB_FIXER;
			}
			else
			{
				if (m_nCurrentWP[NOZZLE] == nWp&&!IsZSafe() && !ZGoPosition(ZSAFE_POSITION, FALSE, TRUE))
				{
					AddedErrorMsg(_T("焊嘴移动至安全高度失败!\r\n"));
					break;
				}
				m_nCurrentProduct[nWp][1] = 0;
				m_nCurrentProduct[nWp][0] = 0;
				m_nCurrentVisionCheck[nWp] = 0;
				m_nCurrentPin[nWp] = 0;

				m_nSemiRun[nWp] = START_LOAD;
				m_nCurrentWP[NOZZLE] = 0x03;
				m_hNozzleFree.SetEvent();

			}
			::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 4, 1);

		}
		else
		{
			m_bNeedClimb = CLIMB_POINT;
			m_nCurrentPin[nWp]++;
			m_nSemiRun[nWp] = BONDING_READY;
		}
		PostMessage(WM_USER_UPDATEUI, 4, 0);
		nResult = 1;
		break;
	case NOZZLE_CLEAN_DONE:
	case SUPPLY_DONE:
		TRACE2(" Wait event %d - %d \r\n", nWp, nStep);
		m_nSemiRun[nWp] = BONDING_READY;

		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_hProcessEvent[nWp][nStep], 0))
		{
			if (nFlage[nWp] >> 1)
				nResult = 1;
			break;
		}
		nResult = 1;
		break;
	default:;
	}
	return nResult;
}

int CMainFrame::SlaveWaitFor(UINT nCmd)
{
	UINT nResult(0);
	HANDLE hEvent[2] = { m_pStopRun->m_hObject, m_hCameraFree};
	switch (nCmd)
	{
	case NOZZLE:
		hEvent[1] = m_hNozzleFree;
	case CAMERA:
		break;
	case SUPPLY_DONE:
		hEvent[1] = m_hBondRecovery;
		break;
	default:
		return nResult;  
	}
	DWORD dEvent = ::WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
	switch (dEvent)
	{
	case WAIT_OBJECT_0://停止
	default://
		break;
	case WAIT_OBJECT_0 + 1://
		nResult = 1;
		break;
	}
	return nResult;
}

BOOL CMainFrame::RangingProcess(UINT nWp)
{
	CString ErrMsg = _T("");
	DWORD dEvent;
	BOOL bSuccess(TRUE);
	UINT i(0);
	UINT nTotal = m_pDoc->m_cParam.PrjCfg.bProductNum[0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];
	if (m_nCurrentWP[CAMERA] != nWp)
	{
		bSuccess = SlaveWaitFor(CAMERA);
		if (bSuccess)
			m_nCurrentWP[CAMERA] = nWp;
	}
	if (bSuccess)
	{
		if (!m_pDoc->m_cParam.PrjCfg.bUseRanging)
		{
			theApp.m_bHigh_Risk[nWp] = FALSE;
			return TRUE;
		}
		if (m_pDoc->m_cParam.PrjCfg.bRangingMode)//三点式
		{
			for (BYTE i = 0; bSuccess&&i < 3; i++)
			{
				m_nCurrentRanging[nWp] = i;
				if (!GoPosition((nWp << 4) + RANGING_POSITION))
				{
					ErrMsg.Format(_T("%s : <第%d 行测距> 移动至测距位置失败!\r\n"), csz[nWp], i+1);
					bSuccess = FALSE;
					break;
				}
				Sleep(150);
				theApp.m_bHigh_Risk[nWp] = !IsFixerSafe();
				if (theApp.m_bHigh_Risk[nWp])
				{
					ErrMsg.Format(_T("%s : <第%d 行测距> 测距结果异常!\r\n"), csz[nWp], i + 1);
					bSuccess = FALSE;
				}
			}

		}
		else
		{
			for (BYTE i = 0; bSuccess&&i < 2; i++)
			{
// 				if (i)
// 					m_nCurrentRanging[nWp] ^= 0x01;
// 				else
// 					m_nCurrentRanging[nWp] |= 0x01;
				if (!GoPosition((nWp << 4) + RANGING_POSITION))
				{
					ErrMsg.Format(_T("%s : <第%d 行测距> 移动至测距位置失败!\r\n"), csz[nWp], i + 1);
					bSuccess = FALSE;
					break;
				}
				Sleep(150);
				theApp.m_bHigh_Risk[nWp] = !IsFixerSafe();
				if (theApp.m_bHigh_Risk[nWp])
				{
					ErrMsg.Format(_T("%s : <第%d 行测距> 测距结果异常!\r\n"), csz[nWp], i + 1);
					bSuccess = FALSE;
				}
				else
				{
					if (!i)
						m_nCurrentRanging[nWp] ^= 0x01;
				}
			}

		}

	}
	if (!bSuccess || !m_bAutoThreadAlive)
	{
		m_nCurrentWP[CAMERA] = 0x03;
		m_hCameraFree.SetEvent();
	}
	if (!ErrMsg.IsEmpty())
		AddedErrorMsg(ErrMsg);
	return bSuccess;
}

BOOL CMainFrame::SnapshotProcess(UINT nWp)
{
	CString ErrMsg = _T("");
	DWORD dEvent;
	BOOL bSuccess(TRUE);
	UINT i(0);
	UINT nTotal = m_pDoc->m_cParam.PrjCfg.bProductNum[0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];
	if (m_nCurrentWP[CAMERA] != nWp)
	{
		bSuccess = SlaveWaitFor(CAMERA);
		if (bSuccess)
			m_nCurrentWP[CAMERA] = nWp;
	}
	if (bSuccess)
		m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE);

	for (i = 0; i < m_pDoc->m_cParam.PrjCfg.uInspection &&bSuccess; i++)
	{
		m_nCurrentVisionCheck[nWp] = i;
		if (!GoPosition((nWp << 4) + SNAPSHOT_POSITION))
		{
			ErrMsg.Format(_T("%s : <第%d 行照相> 移动至照相位置失败!\r\n"), csz[nWp], i + 1);
			bSuccess = FALSE;
			break;
		}
		Sleep(150);
		bSuccess = GetImg2Buffers(nWp,nCurl,i);
		Sleep(10);
	}
	if (bSuccess)
	{
		if (m_bAutoThreadAlive)//
		{
			if (nCurl == (nTotal - 1))
			{
				DWORD dEvent = ::WaitForMultipleObjects(8, &m_hImgProcessEvent[1], TRUE, 10000);
				if (dEvent == WAIT_TIMEOUT)
				{
					bSuccess = FALSE;
					ErrMsg = _T("图像处理超时,未全部处理完!\r\n");
				}
				else
				{
// 					m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nTotal, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ERASING_STS);
					m_nCurrentWP[CAMERA] = 0x03;
					m_hCameraFree.SetEvent();
				}
			}
		}
		else
			/************************************************************************/
			/*                        手动状态下返回初始照相位                         */
			/************************************************************************/
		{
			BYTE nNum = m_pDoc->m_cParam.PrjCfg.PinArray[m_nCurrentPin[nWp]].uVisionIndex;
			if (nNum)
			{
				m_nCurrentVisionCheck[nWp] = nNum - 1;
				if (!GoPosition((nWp << 4) + SNAPSHOT_POSITION))
				{
					ErrMsg.Format(_T("%s : <第%d 行照相> 回到初始照相位失败!\r\n"), csz[nWp], i + 1);
					bSuccess = FALSE;
				}
			}
			DWORD dEvent = ::WaitForMultipleObjects(8, &m_hImgProcessEvent[1], TRUE, 4000);
			if (dEvent == WAIT_TIMEOUT)
			{
				bSuccess = FALSE;
				ErrMsg+=_T("图像处理超时,有流程未返回结果!\r\n");
			}
		}
	}
	if (!bSuccess||!m_bAutoThreadAlive)
	{
		m_nCurrentWP[CAMERA] = 0x03;
		m_hCameraFree.SetEvent();
	}
	if (!ErrMsg.IsEmpty())
		AddedErrorMsg(ErrMsg);
	return bSuccess;
}
BOOL CMainFrame::BondingProcess(UINT nWp)
{
	CString ErrMsg = _T("");
	DWORD dEvent;
	BOOL bSuccess(TRUE);
	UINT i(0);
	if (m_nCurrentWP[NOZZLE] != nWp)
	{
		bSuccess = SlaveWaitFor(NOZZLE);
		if (bSuccess)
		{
			m_nCurrentWP[NOZZLE] = nWp;
			if (!IsZSafe())
				if (!ZGoPosition(ZSAFE_POSITION, FALSE))
				{
					ErrMsg.Format(_T("%s : 焊嘴移动至安全高度失败!\r\n"), csz[nWp]);
					bSuccess = FALSE;
				}
		}
	}
	if (bSuccess)
	{
		if (bSuccess)
		{
			UINT nCurl = m_nCurrentProduct[nWp][0] * m_pDoc->m_cParam.PrjCfg.bProductNum[1] + m_nCurrentProduct[nWp][1];
			m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ADD_STS);
		}
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);
		if (m_nBondAlarm ^ 0x01)//异常
			return FALSE;
		SetCurrentLaserGP(m_nCurrentPin[nWp]);
		switch (m_bNeedClimb)
		{
		case CLIMB_POINT:
		case CLIMB_PRODUCT:
		case CLIMB_FIXER:
			bSuccess = ZCheckClimbPos((nWp << 4) + m_bNeedClimb);
			break;
		case ZSAFE_POSITION:
		default:
			bSuccess = ZCheckClimbPos((nWp << 4) + ZSAFE_POSITION);
			break;
		}
		if (!bSuccess)
			ErrMsg.Format(_T("%s :焊嘴降到焊接位失败!\r\n"), csz[nWp]);
		bSuccess = GoPosition((nWp << 4) + LASER_POSITION);
		if (bSuccess)
		{
			bSuccess = ZGoPosition((nWp << 4) + LASER_POSITION, FALSE, TRUE);
			if (!bSuccess)
				ErrMsg.Format(_T("%s :焊嘴降到焊接位失败!\r\n"), csz[nWp]);
		}
		else
			ErrMsg.Format(_T("%s : XY 移动到焊接位失败!\r\n"), csz[nWp]);

		if (!bSuccess&&!IsZSafe())
		{
			if (!ZGoPosition(ZSAFE_POSITION, FALSE))
				ErrMsg += _T("@焊嘴移动至安全高度失败!\r\n");
		}
	}
	if (!bSuccess || !m_bAutoThreadAlive)
	{
		m_nCurrentWP[NOZZLE] = 0x03;
		m_hNozzleFree.SetEvent();
	}
	if (!ErrMsg.IsEmpty())
		AddedErrorMsg(ErrMsg);

	return bSuccess;
}

BOOL CMainFrame::LaserOut(BOOL bOnOff /*= FALSE*/, UINT nMode /*= 0*/)
{
	BOOL bSuccess(!bOnOff);
	if (bOnOff&&m_bIdling)
		return TRUE;
	UINT ndelay(100);
	if (bOnOff)
	{
		// 		Sleep(250);
		if (m_nBondAlarm ^ 0x01)//异常
			return FALSE;

// 		if (m_IoCtrller.WaitSensorSignal(m_pDoc->m_cParam.In_PLC_Input[0], TRUE, 500))
		if (CheckLaserDone())
		{
			bSuccess = m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, TRUE);
			if (m_pDoc->m_cParam.nNozzleLife[0])
			{
				m_pDoc->m_cParam.nNozzleLife[0]--;
				::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 5, 0);
			}
			// 			bSuccess = m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_WeldingTrigger, TRUE);
			Sleep(100);
			m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);
			if (bSuccess&&nMode)//等待
			{
				// 				bSuccess = m_IoCtrller.WaitSensorSignal(m_pDoc->m_cParam.In_WeldingReady, TRUE, 2000);
				Sleep(1);
				// 				bSuccess = m_IoCtrller.WaitSensorSignal(m_pDoc->m_cParam.In_PLC_Input[0], TRUE, 2000);
				bSuccess = CheckLaserDone(2000);
			}
		}
	}
	else
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);
	return bSuccess;
}

BOOL CMainFrame::CheckLaserDone(UINT ntime /*= 1000*/)
{
	BOOL bSuccess(FALSE);
	for (UINT i = 0; i < ntime;i++)
	{
		if (WAIT_TIMEOUT != ::WaitForSingleObject(m_pStopRun->m_hObject, 0))
			break;
		Sleep(1);
		if (m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_PLC_Input[0]])
		{
			bSuccess = TRUE;
			break;
		}
	}
	return bSuccess;
}

BOOL CMainFrame::StartSlaveThread(UINT nWP, UINT nIndex)
{
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	if (m_bSalveThreadAlive[nWP])
	{
		TRACE2(" Warning m_bSalveThreadAlive is true %d - %d \r\n", nWP, nIndex);
		return bSuccess;
	}
	pLuckyBag pOperateOrder = new LuckyBag;
	pOperateOrder->pOwner = this;
	pOperateOrder->nIndex = new UINT[2];
	pOperateOrder->nIndex[0] = nWP;
	pOperateOrder->nIndex[1] = nIndex;
	ResetEvent(m_hSlaveEvent[nWP]);
	if (nIndex == WAIT_BTN_START)
		ResetEvent(m_hProcessEvent[nWP][START_LOAD]);
	else
		ResetEvent(m_hProcessEvent[nWP][nIndex]);

	TRACE2(" clear event %d - %d \r\n",nWP,nIndex);
	if (!(pThread = AfxBeginThread(SlaveThread, (LPVOID)pOperateOrder)))
	{
		bSuccess = FALSE;
		SAFE_DELETEARRAY(pOperateOrder->nIndex);
		SAFE_DELETE(pOperateOrder);
		SetEvent(m_hSlaveEvent[nWP]);
	}
	return bSuccess;
}

UINT _cdecl CMainFrame::SlaveThread(LPVOID lpParam)
{
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	UINT nId = pOperateOrder->nIndex[0];
	UINT nCMD = pOperateOrder->nIndex[1];
	pFrame->m_bSalveThreadAlive[nId] = TRUE;
	SAFE_DELETEARRAY(pOperateOrder->nIndex);
	SAFE_DELETE(pOperateOrder);

	CString ErrMsg = _T("");
	CString id_CMD = _T("BALLS_SUPPLY");
	BOOL bSuccess(FALSE);
	UINT nsts(0);
	UINT i(0);
	switch (nCMD)
	{
	case WAIT_BTN_START:
		{
			pFrame->SetButtonLamp(nId, FALSE);
			pFrame->m_CriticalData.Lock();
			pFrame->m_bWaitforTrigger |= 1 << nId;
			pFrame->m_CriticalData.Unlock();

			ResetEvent(pFrame->m_hOneCycleStart[nId]);
			HANDLE hEvent[2] = { pFrame->m_pStopRun->m_hObject, pFrame->m_hOneCycleStart[nId] };
			DWORD dEvent = ::WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
			switch (dEvent)
			{
			case WAIT_OBJECT_0://停止
				break;
			case WAIT_OBJECT_0 + 1://
				if (pFrame->SetDockReady(nId, TRUE))
				{
					if (pFrame->m_IoCtrller.WaitSensorSignal(pFrame->m_pDoc->m_cParam.In_GS_FSD1, TRUE, 1))//
					{
						Sleep(100);//延时滤波
						if (!pFrame->m_IoCtrller.WaitSensorSignal(pFrame->m_pDoc->m_cParam.In_GS_FSD1, FALSE, 1))
						{
							pFrame->SetButtonLamp(nId, TRUE);
							pFrame->m_CriticalData.Lock();
							pFrame->m_bWaitforTrigger &= ~(1 << nId);
							pFrame->m_CriticalData.Unlock();

							SetEvent(pFrame->m_hProcessEvent[nId][LOAD_DONE]);
							TRACE2(" set event %d - %d \r\n", nId, nCMD);
							break;
						}
					}
					ErrMsg.Format(_T("%s :安全光幕区域异常!\r\n"), csz[nId]);

				}else
					ErrMsg.Format(_T("%s :夹具固定异常!\r\n"), csz[nId]);

				SetEvent(pFrame->m_pStopRun);
				break;
			default:
				break;
			}
		}
		break;
	case START_LOAD:
		if (pFrame->m_IoCtrller.WaitSensorSignal(pFrame->m_pDoc->m_cParam.In_GS_FSD1, TRUE, 10000))//
		{
			if (!pFrame->m_IoCtrller.WaitSensorSignal(pFrame->m_pDoc->m_cParam.In_GS_FSD1, FALSE, 500))
			{
				if (pFrame->GoPosition((nId << 4) + LOAD_POSITION))
				{
					pFrame->SetButtonLamp(nId, FALSE);
					pFrame->SetDockReady(nId, FALSE);
					pFrame->m_CriticalData.Lock();
					pFrame->m_bWaitforTrigger |= 1 << nId;
					if ((pFrame->m_bWaitforTrigger& (0x02 >> nId)) || pFrame->m_nSemiRun[1 >> nId])
					{
						pFrame->EnableSaftyDev(0, FALSE);
					}
					pFrame->m_CriticalData.Unlock();

					ResetEvent(pFrame->m_hOneCycleStart[nId]);
					HANDLE hEvent[2] = { pFrame->m_pStopRun->m_hObject, pFrame->m_hOneCycleStart[nId] };
					DWORD dEvent = ::WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
					switch (dEvent)
					{
					case WAIT_OBJECT_0://停止
						break;
					case WAIT_OBJECT_0 + 1://
						if (!pFrame->SetDockReady(nId, TRUE))
						{
							ErrMsg.Format(_T("%s :固定夹具动作失败!\r\n"), csz[nId]);
							break;
						}
						pFrame->SetButtonLamp(nId, TRUE);
						pFrame->m_pView->m_MainCtrlDlg.OnUpdateSts(nId);
						Sleep(1000);
						pFrame->m_CriticalData.Lock();
						pFrame->m_bWaitforTrigger &= ~(1 << nId);
						pFrame->EnableSaftyDev(0, TRUE);
						if (pFrame->GoPosition((nId << 4) + PRE_SNAPSHOT_POSITION) && WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))
						{
							if (pFrame->m_bWaitforTrigger& (0x02 >> nId))
								pFrame->EnableSaftyDev(0, FALSE);
							pFrame->m_CriticalData.Unlock();

							SetEvent(pFrame->m_hProcessEvent[nId][LOAD_DONE]);
							TRACE2(" set event %d - %d \r\n", nId, nCMD);
							break;
						}
						pFrame->m_CriticalData.Unlock();
						ErrMsg.Format(_T("%s :移动至照相位失败!\r\n"), csz[nId]);
					default:
						break;
					}
				}
				else
					ErrMsg.Format(_T("%s :移动至上料位失败!\r\n"), csz[nId]);
			}
		}
		break;
	case RANGING:
		if (pFrame->RangingProcess(nId))
		{
			SetEvent(pFrame->m_hProcessEvent[nId][RANGING_DONE]);
			TRACE2(" set event %d - %d \r\n", nId, nCMD);
		}
		break;
	case SNAPSHOT_MATCH:
		if (pFrame->SnapshotProcess(nId))
		{
			SetEvent(pFrame->m_hProcessEvent[nId][SNAPSHOT_MATCH_DONE]);
			TRACE2(" set event %d - %d \r\n", nId, nCMD);
		}
		break;
	case BONDING_READY:
		if (!pFrame->m_bAutoThreadAlive)
		{
			if (!pFrame->IsZSafe())
				if (!pFrame->ZGoPosition(ZSAFE_POSITION,FALSE))
				{
					ErrMsg.Format(_T("%s : 焊嘴移动至安全高度失败!\r\n"), csz[nId]);
					break;
				}
		}
		bSuccess = pFrame->BondingProcess(nId);
		if (bSuccess)
		{
			SetEvent(pFrame->m_hProcessEvent[nId][BONDING_READY_DONE]);
			Sleep(0);
			TRACE2(" set event %d - %d \r\n", nId, nCMD);
		}
		break;
	case BALL_LASER:
		bSuccess = pFrame->LaserOut(TRUE, TRUE);	
		if (bSuccess)
		{
			if (!pFrame->ZCheckClimbPos((nId << 4) + CLIMB_POINT))//爬升指定距离
			{
				bSuccess = FALSE;
				ErrMsg.Format(_T("%s :焊嘴点间爬升安全高度失败!\r\n"), csz[nId]);
			}
		}
		else
			ErrMsg.Format(_T("注意!%s :出光焊接失败! \r\n"), csz[nId]);

		if (bSuccess)
		{
			SetEvent(pFrame->m_hProcessEvent[nId][BALL_LASER_DONE]);
			Sleep(0);
			TRACE2(" set event %d - %d \r\n", nId, nCMD);
		}
		else
		{
			if (!pFrame->ZGoPosition(ZSAFE_POSITION, FALSE, TRUE))
			{
				CString sz;
				sz.Format(_T("@焊嘴移动至安全高度失败!\r\n"), csz[nId]);
				ErrMsg += sz;
			}
		}
		break;
	case NOZZLE_CLEAN:
		id_CMD = _T("NOZZLE_CLEAN");
	case BALLS_SUPPLY:
		if (!pFrame->ZGoPosition(ZSAFE_POSITION, FALSE, TRUE) && WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))
		{
			ErrMsg.Format(_T("%s :焊嘴移动至安全高度失败!\r\n"), csz[nId]);
			break;
		}
		else if (!pFrame->GoPosition((nId << 4) + NOZZLE_CLEAN_POSITION) && WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0)){
			ErrMsg.Format(_T("%s :xy 移动至 %s 位置失败!\r\n"), csz[nId],id_CMD);
			break;
		}
		else if (!pFrame->ZGoPosition(NOZZLE_CLEAN_POSITION, FALSE, TRUE) && WAIT_TIMEOUT == ::WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))	{
			ErrMsg.Format(_T("%s :Z 移动至 %s 位置失败!\r\n"), csz[nId],id_CMD);
			break;
		}
		if (nCMD == NOZZLE_CLEAN)
		{
			if (!pFrame->m_LaserCtrl.SetTriggerCleanMode()){
				ErrMsg.Format(_T("%s :焊嘴清洗命令发送失败!\r\n"), csz[nId]);
				break;
			}
			Sleep(2000);
			if (!pFrame->ZGoPosition(ZSAFE_POSITION, FALSE, TRUE))
			{
				ErrMsg.Format(_T("%s :Z 移动至安全高度失败!\r\n"), csz[nId]);
				break;
			}
		}
		else if (pFrame->m_bAutoThreadAlive)
		{
			pFrame->EnableSaftyDev(1, FALSE);
			if(!pFrame->SlaveWaitFor(SUPPLY_DONE))
				break;
		}
		if (!(pFrame->m_LaserCtrl.GetAlarm()^0x01))
			SetEvent(pFrame->m_hProcessEvent[nId][nCMD]);

		break;
	default:
		break;
	}
	if (nCMD == BALLS_SUPPLY)
		pFrame->EnableSaftyDev(1, TRUE);

	if (!ErrMsg.IsEmpty())
		pFrame->AddedErrorMsg(ErrMsg);
	pFrame->m_bSalveThreadAlive[nId] = FALSE;
	TRACE2(" set m_bSalveThreadAlive FALSE %d - %d \r\n", nId, nCMD);

	SetEvent(pFrame->m_hSlaveEvent[nId]);
	TRACE2(" set SlaveEvent %d - %d \r\n", nId, nCMD);

	return 0;
}

BOOL CMainFrame::StartManualRunThread(UINT nWP, UINT nIndex)
{
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	pLuckyBag pOperateOrder = new LuckyBag;
	pOperateOrder->pOwner = this;
	pOperateOrder->nIndex = new UINT[2];
	pOperateOrder->nIndex[0] = nWP;
	pOperateOrder->nIndex[1] = nIndex;
	m_pStopRun->ResetEvent();
	ClearOverlayInfo();
	if (m_bAutoThreadAlive || !(pThread = AfxBeginThread(ManualRunThread, (LPVOID)pOperateOrder)))
	{
		bSuccess = FALSE;
		SAFE_DELETEARRAY(pOperateOrder->nIndex);
		SAFE_DELETE(pOperateOrder);
	}
	return bSuccess;
}

UINT _cdecl CMainFrame::ManualRunThread(LPVOID lpParam)
{
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	UINT nId = pOperateOrder->nIndex[0];
	UINT nIndex = pOperateOrder->nIndex[1];
	SAFE_DELETEARRAY(pOperateOrder->nIndex);
	SAFE_DELETE(pOperateOrder);
	pFrame->m_bAutoThreadAlive = TRUE;
	::PostMessage(pFrame->m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 0, 0);
	pFrame->m_nCurrentRunMode = 2;
	BOOL bSuccess(0);
	UINT i(0);
	if (pFrame)
	{
		LaserPoint tempPoint;
		ZeroMemory(&tempPoint, sizeof(LaserPoint));
		switch (nIndex&0x0f)
		{
		case 0://Z safe position
			bSuccess = pFrame->ZGoPosition(ZSAFE_POSITION, FALSE, TRUE);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("z没到安全位\r\n"));
			break;
		case 1://loading area
			bSuccess = pFrame->GoPosition((nId << 4) + LOAD_POSITION);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("y没到位\r\n"));
			break;
		case 2://Nozzle Check
			bSuccess = pFrame->GoPosition(ZSAFE_POSITION);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("z没到安全位\r\n"));
			else
			{
				bSuccess = pFrame->GoPosition(NOZZLE_CHECK_POSITION);
				if (!bSuccess)
					pFrame->AddedErrorMsg(_T("x没到位\r\n"));
				else
				{
					bSuccess = pFrame->ZGoPosition(NOZZLE_CHECK_POSITION, FALSE, TRUE);//Assume Safety
					if (!bSuccess)
						pFrame->AddedErrorMsg(_T("z没到位\r\n"));
				}
			}
			break;
		case 3://Nozzle Clean
			bSuccess = pFrame->GoPosition(ZSAFE_POSITION);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("z没到安全位\r\n"));
			else
			{
				bSuccess = pFrame->GoPosition((nId << 4) + NOZZLE_CLEAN_POSITION);
				if (!bSuccess)
					pFrame->AddedErrorMsg(_T("xy没到位\r\n"));
				else
				{
					bSuccess = pFrame->ZGoPosition(NOZZLE_CLEAN_POSITION, FALSE, TRUE);
					if (!bSuccess)
						pFrame->AddedErrorMsg(_T("z没到位\r\n"));
				}
			}
			break;
		case 4://Try Check
			::PostMessage(pFrame->m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 1, 1);
			bSuccess = pFrame->CamPosition(Calibration_CHECK);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("Camera没到位\r\n"));
			else
			{
				bSuccess = pFrame->GoPosition((nId << 4)+ Calibration_CHECK);
				if (!bSuccess)
					pFrame->AddedErrorMsg(_T("xy没到位\r\n"));
			}

			break;
		case 5://Try Shot
			bSuccess = pFrame->GoPosition(ZSAFE_POSITION);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("z没到安全位\r\n"));
			else
			{
				bSuccess = pFrame->GoPosition((nId << 4) + Calibration_LASER);
				if (!bSuccess)
					pFrame->AddedErrorMsg(_T("xy没到位\r\n"));
				else
				{
					bSuccess = pFrame->ZGoPosition(Calibration_LASER, FALSE, TRUE);
					if (!bSuccess)
						pFrame->AddedErrorMsg(_T("z没到位\r\n"));
				}
			}
			break;
		case 6://Product Position
			bSuccess = pFrame->CamPosition((nId << 4) + SNAPSHOT_POSITION);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("Camera没到位\r\n"));
			else
			{
				bSuccess = pFrame->GoPosition((nId << 4) + SNAPSHOT_POSITION);
				if (!bSuccess)
					pFrame->AddedErrorMsg(_T("z没到位\r\n"));
			}
			break;
		case 7://测距
			pFrame->m_pMotionCtrller[0]->ModifyGroup(0, 1 << nId, FALSE);
			if (nIndex >> 4)//三点式
			{
				//cx vz
				tempPoint.EndPoint[0] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingFixed[nId][pFrame->m_nCurrentRanging[nId] % 3][0];//%3防越界
				tempPoint.EndPoint[1] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingFixed[nId][pFrame->m_nCurrentRanging[nId] % 3][2];//%3防越界
				//Y
				tempPoint.EndPoint[2] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingFixed[nId][pFrame->m_nCurrentRanging[nId] % 3][1];//%3防越界
			}
			else
			{
				tempPoint.EndPoint[0] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingRel[nId][pFrame->m_nCurrentRanging[nId]][0];
				tempPoint.EndPoint[1] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingRel[nId][pFrame->m_nCurrentRanging[nId]][2];
				tempPoint.EndPoint[2] = pFrame->m_pDoc->m_cParam.PrjCfg.dRangingRel[nId][pFrame->m_nCurrentRanging[nId]][1];
			}
			bSuccess = pFrame->m_pMotionCtrller[0]->Move(nId + 2, tempPoint.EndPoint[2], pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_AU_VEL[nId + 1], pFrame->m_pDoc->m_cParam.PrjCfg.SpeedCfg.Run_ACC[nId + 1]);
			if (bSuccess)
			{
				bSuccess = pFrame->m_pMotionCtrller[1]->AddRuninTo(0, tempPoint, TRUE);
				if (bSuccess)
				{
					Sleep(150);
					bSuccess = FALSE;
					switch (pFrame->WaitMotionDone(0x10 + 4, 10000))
					{
					case 0://到位
						bSuccess= TRUE;
						break;
					case 1://急停

						break;
					case 2://停止
						break;
					case 3://超时
					default://
						pFrame->m_pMotionCtrller[1]->AxStopDec(5);
						AfxMessageBox(_T("CX VZ轴到位超时"));
						break;
					}
				}else
					AfxMessageBox(_T("CX VZ 轴移动失败"));

			}else
				AfxMessageBox(_T("Y轴移动失败"));
			if (bSuccess)
			{
				Sleep(50);
				if (!pFrame->m_pMotionCtrller[0]->CheckMotionDone(0x02 << nId, 10000))
				{

					pFrame->m_pMotionCtrller[0]->AxStopDec(nId + 2);
					bSuccess = FALSE;
					AfxMessageBox(_T("Y轴到位超时"));
				}
			}
			break;
		case 8://测试打球
			pFrame->m_nCurrentRunMode = 0x03;
			theApp.m_StopWatch.Start();
			theApp.m_StopWatch.Reset();
			theApp.m_StopWatch.GetTimeSpan();
			for (i = 0; i < pFrame->m_pDoc->m_cParam.PrjCfg.nTestBallNumber;i++)
			{
				if (WAIT_TIMEOUT == WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 0))
				{
					bSuccess=pFrame->LaserOut(TRUE, TRUE);
					::PostMessage(pFrame->m_hWnd,WM_USER_INVALIDATE, 1, 0);
					if (bSuccess)
						Sleep(pFrame->m_pDoc->m_cParam.nLaserSpan);
					else
						break;
				}
				else
				{
					break;
				}
			}
			theApp.m_StopWatch.Stop();
			::PostMessage(pFrame->m_hWnd, WM_USER_INVALIDATE, 1, i);
			if (!bSuccess)
				pFrame->AddedErrorMsg(_T("测试未完成!bond头异常或完成信号超时\r\n"));
			break;
		case 9://测试打激光
			pFrame->m_nCurrentRunMode = 0x03;
			for (i = 0; i < pFrame->m_pDoc->m_cParam.PrjCfg.nTestBallNumber; i++)
			{
				if (WAIT_TIMEOUT == WaitForSingleObject(pFrame->m_pStopRun->m_hObject, 500))
				{
					if (!pFrame->IsAboveCamera()||pFrame->GetUserType())
					{
						pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[2], TRUE);
						Sleep(100);
						pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[2], FALSE);
					}
				}
				else
				{
					break;
				}
			}
			pFrame->m_LaserCtrl.SetLaserRDY(FALSE);
			pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[2], FALSE);

			break;
		default:
			break;
		}
	}
	::PostMessage(pFrame->m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 0, 0x07fff);
	pFrame->m_nCurrentRunMode = 0;
	pFrame->m_bAutoThreadAlive = FALSE;
	return 0;
}

BOOL CMainFrame::StartPolling()
{
	CString ErrorMsg;
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	m_hPollingStop.ResetEvent();
	if (!(pThread = AfxBeginThread(PollingThread, (LPVOID)this)))
		return FALSE;
	TRACE("------------->modbus Polling Thread start\r\n");
	return TRUE;
}

UINT _cdecl CMainFrame::PollingThread(LPVOID lpParam)
{
	CMainFrame* pFrame = (CMainFrame*)lpParam;
	pFrame->m_bPollingAlive = TRUE;
	DWORD Event = 0;

	while (::WaitForSingleObject(pFrame->m_hPollingStop, 250) == WAIT_TIMEOUT)
	{
		pFrame->PollingHandle();
	}
	Sleep(50);

	pFrame->m_bPollingAlive = FALSE;
	TRACE("---------------->Polling Thread Stop\r\n");

	return 0;
}

BOOL CMainFrame::PollingHandle()
{
	UINT nGp(0);

	UINT nTemp(0);
	nTemp = m_LaserCtrl.GetAlarm();
	if (m_nBondAlarm^nTemp)
	{
		m_nBondAlarm = nTemp;
		if (::IsWindow(m_hWnd))
			PostMessage(WM_USER_IOCHANGE, 0xff, nTemp);
	}
	Sleep(10);
	if (m_LaserCtrl.IsHoming())
	{
		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Welding[1],FALSE);
	}
	else
	{
		if (m_LaserCtrl.IsHomed()&&m_nCurrentRunMode==0x01)
			m_hBondHomeEnd.SetEvent();
	}
	if (::IsWindow(m_hWnd))
		PostMessage(WM_USER_UPDATEUI,5,m_LaserCtrl.GetLaserPressure());
	return 0;
}

UINT CMainFrame::ImageProcess(Image* image, BYTE nIndex, BYTE nWP, BYTE nRow, BYTE nCol)
{
	BOOL bSuccess(TRUE);
	UINT i(0),j(0);
	BOOL bStart(TRUE);
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	BYTE nOrder = nRow*m_pDoc->m_cParam.PrjCfg.bProductNum[1] + nCol;
	ImageProcessBinder ipb = {0xff,0xff};
	BYTE bResult(0);
	CString str;
	if (m_pDoc->m_cParam.PrjCfg.bUseBinary)
	{
		bSuccess = m_NiVision.CreateBinary(image, image, 2);
	}
	for (i = 0; (i < m_pDoc->m_cParam.PrjCfg.nPinNum)&&bSuccess;i++)
	{
		if (m_pDoc->m_cParam.PrjCfg.PinArray[i].uVisionIndex==(nIndex+1))
		{
			if (bStart)
			{
				m_dLaserOffset[nWP][nOrder][i][0] = 0;
				m_dLaserOffset[nWP][nOrder][i][1] = 0;
				m_dLaserOffset[nWP][nOrder][i][2] = 0;
				m_nPinVisionSuccess[nWP][nOrder][i] = FALSE;

				bStart = FALSE;
			}
			if (ipb.nProcessNum != m_pDoc->m_cParam.PrjCfg.PinArray[i].uVisionType.nProcessNum || ipb.nSearchNum != m_pDoc->m_cParam.PrjCfg.PinArray[i].uVisionType.nSearchNum)
			{
				ipb = m_pDoc->m_cParam.PrjCfg.PinArray[i].uVisionType;
				j = i;

				BYTE nType = ipb.nProcessNum;
				UINT minScore;
				UINT nCount(0);
				BYTE mode;
				if (!nType)//无需视觉
				{
					m_dLaserOffset[nWP][nOrder][i][0] = 0;
					m_dLaserOffset[nWP][nOrder][i][1] = 0;
					m_dLaserOffset[nWP][nOrder][i][2] = 0;
					m_nPinVisionSuccess[nWP][nOrder][i] = TRUE;
				}
				else
				{
					IVA_StepResults MatchResults;
					MatchResults.results = NULL;
					MatchResults.stepIndex = nOrder|(nWP<<7);
					if (nType < 17)//T
					{
						Rect rc = m_pDoc->m_cParam.PrjCfg.rcSearch[ipb.nSearchNum];
						minScore = m_pDoc->m_cParam.PrjCfg.tTemplateArry[nType - 1].nScore;
						mode = m_pDoc->m_cParam.PrjCfg.tTemplateArry[nType - 1].bType ? m_pDoc->m_cParam.PrjCfg.tTemplateArry[nType - 1].nContrast : 0;
						str.Format(_T("产品%d图像处理开始"), nOrder+1);
						CLogHelper::WriteLog(str);
						nCount = MatchProcessing(image, nType - 1, rc, minScore, mode, &MatchResults);
						str.Format(_T("产品%d图像处理结果:%d"), nOrder + 1,nCount);
						CLogHelper::WriteLog(str);
						m_NiVision.UpdateOverlay(TempCanvas, image, MATCH_INFO_LAYER + 1, FALSE);
						bResult |=0x01;
					}
					else if(nType<33)//S
					{
						nType = nType - 17;
						mode = m_pDoc->m_cParam.PrjCfg.cCircleArry[nType].bType;
						if (mode >> 1)
							nCount = DetectCircleProcessing(image, nType, m_pDoc->m_cParam.PrjCfg.arcSearch[ipb.nSearchNum], &MatchResults);
						else
							nCount = DetectShapeProcessing(image, nType, m_pDoc->m_cParam.PrjCfg.rcSearch[ipb.nSearchNum], 800, mode, &MatchResults);
						m_NiVision.UpdateOverlay(TempCanvas, image, DETECT_INFO_LAYER + 1, FALSE);
						bResult |= 0x02;

					}
					if (nCount)
					{
						m_nPinVisionSuccess[nWP][nOrder][i] = TRUE;
						m_dLaserOffset[nWP][nOrder][i][0] = (abs(MatchResults.results[0].resultVal.numVal) < 6) ? MatchResults.results[0].resultVal.numVal : 0;
						m_dLaserOffset[nWP][nOrder][i][1] = (abs(MatchResults.results[1].resultVal.numVal) < 6) ? MatchResults.results[1].resultVal.numVal : 0;
						m_dLaserOffset[nWP][nOrder][i][2] = 360-MatchResults.results[2].resultVal.numVal;
						m_pView->m_MainCtrlDlg.OnUpdateSts(nWP, nOrder, CMainCtrlDlg::VISION_OK << 1, CMainCtrlDlg::ADD_STS);
					}
					else
					{
						m_nPinVisionSuccess[nWP][nOrder][i] = FALSE;
						m_pView->m_MainCtrlDlg.OnUpdateSts(nWP, nOrder, CMainCtrlDlg::VISION_FAIL << 1, CMainCtrlDlg::ADD_STS);
					}
					m_pView->m_MainCtrlDlg.OnUpdateSts(nWP, nOrder, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ERASING_STS);
					SAFE_DELETEARRAY(MatchResults.results);
				}
			}
			else
			{
				m_dLaserOffset[nWP][nOrder][i][0] = m_dLaserOffset[nWP][nOrder][j][0];
				m_dLaserOffset[nWP][nOrder][i][1] = m_dLaserOffset[nWP][nOrder][j][1];
				m_dLaserOffset[nWP][nOrder][i][2] = m_dLaserOffset[nWP][nOrder][j][2];
				m_nPinVisionSuccess[nWP][nOrder][i] = m_nPinVisionSuccess[nWP][nOrder][j];
			}
			PostMessage(WM_USER_PROCESS_MESSAGE, (nOrder << 4) + i, m_nPinVisionSuccess[nWP][nOrder][i]);
		}
		
	}
	if (!m_bAutoThreadAlive)
	{
		if (bResult & 0x01)
			m_NiVision.UpdateOverlay((UINT)0, TempCanvas, MATCH_INFO_LAYER + 1, TRUE);
		if (bResult & 0x02)
			m_NiVision.UpdateOverlay((UINT)0, TempCanvas, DETECT_INFO_LAYER + 1, TRUE);
	}
	imaqDispose(TempCanvas);
	str.Format(_T("产品%d图像处理结束返回"), nOrder + 1);
	CLogHelper::WriteLog(str);
	return bSuccess;
}

UINT CMainFrame::MatchProcessing(Image* image, BYTE nIndex, Rect rc, UINT nScore, BYTE nMode, IVA_StepResults* stepResults)
{
	UINT nCount(1);
	Point pt = { 100, 60 };
	CString info = _T("Not Match");
	CString loginfo = _T("Match Failure");
	if (!m_pDoc->m_cParam.PrjCfg.uTemplateSts >> nIndex & 0x01)
		return 0;
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	Image* TempImg = image;
	CString str;
	if (TempImg == NULL)
	{
		TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
		m_NiVision.GetImage(0, TempImg,TRUE);
		if (m_pDoc->m_cParam.PrjCfg.bUseBinary)
			m_NiVision.CreateBinary(TempImg, TempImg, 2);
	}
	// First, delete all the results of this step (from a previous iteration)
	if (stepResults)
		SAFE_DELETEARRAY(stepResults->results);
	IVA_Result* MatchResults(NULL);
	UINT i(0);

	if (nMode)
	{
		GeometricPatternMatch3* GmatchInfo = m_NiVision.GTemplatemSearch(TempImg, m_pTemplate[nIndex], rc, nMode, &nCount, nScore);
		if (nCount){
			pt.x = GmatchInfo->position.x;
			pt.y = GmatchInfo->position.y;
			info.Format(_T("X=%f ;Y=%f;\r\n 评分:%f;\r\nCX:%f,CY:%f;\r\n 相对角度:%f;"),
				GmatchInfo->position.x, GmatchInfo->position.y, GmatchInfo->score, GmatchInfo->calibratedPosition.x, GmatchInfo->calibratedPosition.y,
				GmatchInfo->calibratedRotation);
			if (stepResults)
			{
				stepResults->numResults = nCount * 3 + 1;
				MatchResults = new IVA_Result[stepResults->numResults];
				stepResults->results = MatchResults;
				ZeroMemory(MatchResults, sizeof(MatchResults));
				for (i = 0; i < nCount; i++)
				{
					MatchResults[i * 3].resultVal.numVal = GmatchInfo[i].calibratedPosition.x;
					MatchResults[i * 3 + 1].resultVal.numVal = GmatchInfo[i].calibratedPosition.y;
					MatchResults[i * 3 + 2].resultVal.numVal = GmatchInfo[i].calibratedRotation;
				}
			}
		}
		imaqDispose(GmatchInfo);

	}
	else
	{
		str.Format(_T("图像:%X模板匹配开始"), TempImg);
		CLogHelper::WriteLog(str);
		PatternMatchReport* matchInfo = m_NiVision.TemplatemSearch(TempImg, m_pTemplate[nIndex], rc, &nCount, nScore);
		if (nCount){
			pt.x = matchInfo->position.x;
			pt.y = matchInfo->position.y;
			info.Format(_T("X=%f ;Y=%f;\r\n 评分:%f;\r\nCX:%f,CY:%f;\r\n 相对角度:%f;"),
				matchInfo->position.x, matchInfo->position.y, matchInfo->score, matchInfo->calibratedPosition.x, matchInfo->calibratedPosition.y,
				matchInfo->calibratedRotation);
			if (stepResults)
			{
				stepResults->numResults = nCount * 3 + 1;
				MatchResults = new IVA_Result[stepResults->numResults];
				stepResults->results = MatchResults;
				ZeroMemory(MatchResults, sizeof(MatchResults));
				for (i = 0; i < nCount; i++)
				{
					MatchResults[i * 3].resultVal.numVal = matchInfo[i].calibratedPosition.x;
					MatchResults[i * 3 + 1].resultVal.numVal = matchInfo[i].calibratedPosition.y;
					MatchResults[i * 3 + 2].resultVal.numVal = matchInfo[i].calibratedRotation;
				}
			}
		}
		imaqDispose(matchInfo);
		str.Format(_T("图像:%X模板匹配结果:%d"), TempImg,nCount);
		CLogHelper::WriteLog(str);

	}
	CString FilePathName;
	if (nCount)
	{
// 		FilePathName.Format(m_pDoc->m_szCurrentFolder + _T("Parameter\\match%d.png"), nIndex);
		m_NiVision.DrawCross(TempCanvas, pt, MATCH_INFO_LAYER);
	}
	else if (stepResults&&(GetUserType()==2||0))
	{
		FilePathName.Format(m_pDoc->m_szCurrentFolder + _T("NG\\%d-%dnomatch%d.png"), stepResults->stepIndex >> 7,(stepResults->stepIndex&0x07f), nIndex);
		m_NiVision.RWVisionFile(TempImg, FilePathName);

	}
	m_NiVision.DrawTextInfo(TempCanvas, info, pt, MATCH_INFO_LAYER,44);
	if (image == NULL)
	{
		m_NiVision.UpdateOverlay((UINT)0, TempCanvas, CMainFrame::MATCH_INFO_LAYER + 1);
		PostMessage( WM_USER_SHOWIMAGE, 0, 0);
		imaqDispose(TempImg);

	}
	else
		m_NiVision.UpdateOverlay(TempImg, TempCanvas, MATCH_INFO_LAYER + 1);
	/*
	m_pDoc->Writelog(1, szTime + loginfo);
	loginfo.Format(_T("%d"), _mm_popcnt_u32(m_nMatchSuccess));

	m_pDoc->Writelog(0x00, loginfo);
	*/
	//	::PostMessage(GetSafeHwnd(), WM_USER_SHOWIMAGE, 0, 0);
	imaqDispose(TempCanvas);
	return nCount;
}

int CMainFrame::DetectShapeProcessing(Image* image, BYTE nIndex, Rect rc, UINT nScore, BYTE nMode, IVA_StepResults* stepResults)
{
	int nCount(0);
	BYTE i(0);
	Point pt = { 100, 60 };
	CString info = _T("Not Detect");
	CString loginfo = _T("Detect Failure");
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	Image* TempImg = image;
	if (TempImg == NULL)
	{
		TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
		m_NiVision.GetImage(0, TempImg);

	}
	// First, delete all the results of this step (from a previous iteration)
	if (stepResults)
		SAFE_DELETEARRAY(stepResults->results);
	IVA_Result* MatchResults(NULL);

	int nThreshold = m_pDoc->m_cParam.PrjCfg.cCircleArry[nIndex].nEdgeStrength;
	if (nMode & 0x01)
	{
		CircleMatch* matchedCircles = NULL;
		matchedCircles = m_NiVision.FindShapeCircle(TempImg, rc, nThreshold, m_pDoc->m_cParam.PrjCfg.rcMinMax[nIndex].minHeight,
			m_pDoc->m_cParam.PrjCfg.rcMinMax[nIndex].maxHeight, nScore, nCount);
		if (nCount){
			pt.x = matchedCircles->position.x;
			pt.y = matchedCircles->position.y;
			info.Format(_T("[X=%f ;Y=%f ;半径：%fmm\r\n 圆度:%f; AvgStrength:%f;]\r\n"),
				matchedCircles->position.x, matchedCircles->position.y, matchedCircles->radius, matchedCircles->score);
			if (stepResults)
			{
				stepResults->numResults = nCount * 3 + 1;
				MatchResults = new IVA_Result[stepResults->numResults];
				stepResults->results = MatchResults;
				ZeroMemory(MatchResults, sizeof(MatchResults));
				for (i = 0; i < nCount; i++)
				{
					MatchResults[i * 3].resultVal.numVal = matchedCircles[i].position.x;
					MatchResults[i * 3 + 1].resultVal.numVal = matchedCircles[i].position.y;
					MatchResults[i * 3 + 2].resultVal.numVal = 0;
					MatchResults[i * 3 + 3].resultVal.numVal = matchedCircles[i].radius;
				}
			}
			rc.height = rc.width = matchedCircles->radius*2;
			rc.top = matchedCircles->position.y - rc.height;
			rc.left = matchedCircles->position.x - rc.height;
		}
		imaqDispose(matchedCircles);
	}
	else
	{
		RectangleMatch* matchedRects = NULL;
		matchedRects = m_NiVision.FindShapeRect(TempImg, rc, nThreshold, (double*)&m_pDoc->m_cParam.PrjCfg.rcMinMax[nIndex], nScore, nCount);
		if (nCount){
			pt.x = matchedRects->corner[0].x;
			pt.y = matchedRects->corner[0].y;
			info.Format(_T("X=%f ;Y=%f;\r\n Width:%f ;Height:%f\r\n Angle:%f ;Score:%f;"),
				matchedRects->corner[0].x, matchedRects->corner[0].y, matchedRects->width,
				matchedRects->height, matchedRects->rotation, matchedRects->score);
			if (stepResults)
			{
				stepResults->numResults = nCount * 3 + 1;
				MatchResults = new IVA_Result[stepResults->numResults];
				stepResults->results = MatchResults;
				ZeroMemory(MatchResults, sizeof(MatchResults));
				for (i = 0; i < nCount; i++)
				{
					MatchResults[i * 3].resultVal.numVal = matchedRects[i].corner[0].x + matchedRects->width/2;
					MatchResults[i * 3 + 1].resultVal.numVal = matchedRects[i].corner[0].y + matchedRects->height/2;
					MatchResults[i * 3 + 2].resultVal.numVal = matchedRects[i].rotation;
				}

			}

		}
		imaqDispose(matchedRects);
	}

	if (nCount)
	{
		m_NiVision.DrawCross(TempCanvas, pt, DETECT_INFO_LAYER);
		m_NiVision.DrawRect(TempCanvas, imaqMakeRotatedRect(rc.top, rc.left, rc.height, rc.width, 0), DETECT_INFO_LAYER);
	}
	m_NiVision.DrawTextInfo(TempCanvas, info, pt, DETECT_INFO_LAYER,44);
	if (image == NULL)
	{
		m_NiVision.UpdateOverlay((UINT)0, TempCanvas, DETECT_INFO_LAYER + 1);
		::PostMessage(GetSafeHwnd(), WM_USER_SHOWIMAGE, 0, 0);
		imaqDispose(TempImg);

	}
	else
		m_NiVision.UpdateOverlay(TempImg, TempCanvas, DETECT_INFO_LAYER + 1);
	imaqDispose(TempCanvas);
	return nCount;
}

int CMainFrame::DetectCircleProcessing(Image* image, BYTE nIndex, Annulus ann, IVA_StepResults* stepResults)
{
	int nCount(0);
	Point pt = { 100, 60 };
	CString info = _T("Not Detect");
	CString loginfo = _T("Detect Failure");
	Image*  TempCanvas = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	Image* TempImg = image;
	if (TempImg == NULL)
	{
		TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
		m_NiVision.GetImage(0, TempImg);
	}
	// First, delete all the results of this step (from a previous iteration)
	if (stepResults)
		SAFE_DELETEARRAY(stepResults->results);
	IVA_Result* MatchResults(NULL);

	FindCircularEdgeReport* circularEdgeReport = NULL;
	EdgeData	 Tcircle = m_pDoc->m_cParam.PrjCfg.cCircleArry[nIndex];
	Tcircle.SearchArea.annulus = &ann;
	circularEdgeReport = m_NiVision.DetectCircle(TempImg, Tcircle);
	if (circularEdgeReport&&circularEdgeReport->circleFound)
	{
		nCount = 1;
		pt.x = circularEdgeReport->center.x;
		pt.y = circularEdgeReport->center.y;
		m_CoordSys[1].origin = circularEdgeReport->center;
		info.Format(_T("[X=%f ;Y=%f ;半径：%fmm\r\n 圆度:%f; AvgStrength:%f;]\r\n"),
			circularEdgeReport->center.x, circularEdgeReport->center.y, circularEdgeReport->radius, circularEdgeReport->roundness, circularEdgeReport->avgStrength);
		if (stepResults)
		{
			stepResults->numResults = nCount * 3 + 1;
			MatchResults = new IVA_Result[stepResults->numResults];
			stepResults->results = MatchResults;
			ZeroMemory(MatchResults, sizeof(MatchResults));
			MatchResults[0].resultVal.numVal = circularEdgeReport->center.x;
			MatchResults[1].resultVal.numVal = circularEdgeReport->center.y;
			MatchResults[2].resultVal.numVal = 0;
			MatchResults[3].resultVal.numVal = circularEdgeReport->radius;
		}

	}
	imaqDispose(circularEdgeReport);
	if (nCount)
		m_NiVision.DrawCross(TempCanvas, pt, DETECT_INFO_LAYER);
	m_NiVision.DrawTextInfo(TempCanvas, info, pt, DETECT_INFO_LAYER,44);
	if (image == NULL)
	{
		m_NiVision.UpdateOverlay((UINT)0, TempCanvas, DETECT_INFO_LAYER + 1);
		::PostMessage(GetSafeHwnd(), WM_USER_SHOWIMAGE, 0, 0);
		imaqDispose(TempImg);

	}
	else
		m_NiVision.UpdateOverlay(TempImg, TempCanvas, DETECT_INFO_LAYER + 1);
	imaqDispose(TempCanvas);
	return nCount;
}

BOOL CMainFrame::GetImg2Buffers(UINT nWp, UINT nCurl, UINT nIndex)
{
	BOOL bSuccess(FALSE);
	CString ErrMsg = _T("");
	CString strTemp = _T("");
	DWORD dEvent(0);
	Image* TempImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
	if (TempImg)
		bSuccess = m_NiVision.GetImage(0, TempImg, TRUE);
	if (bSuccess)
	{
		strTemp.Format(_T("第%d个产品图像正在等待进入处理资源池"), nCurl+1);
		CLogHelper::WriteLog(strTemp);
		dEvent = ::WaitForMultipleObjects(9, m_hImgProcessEvent, FALSE, 10000);
		switch (dEvent)
		{
		case WAIT_OBJECT_0://停止
			bSuccess = FALSE;
			break;
		default:
			if (dEvent < 9)
			{
				strTemp.Format(_T("%d号处理资源空闲，第%d个产品图像准备进入处理"), dEvent, nCurl+1);
				CLogHelper::WriteLog(strTemp);
				dEvent--;
				CWinThread* pThread(NULL);
				if (!m_pImagePack[dEvent])
				{
					m_pImagePack[dEvent] = new LuckyBag;
					m_pImagePack[dEvent]->pOwner = this;
					m_pImagePack[dEvent]->nIndex = new UINT[5];
					m_pImagePack[dEvent]->pData = NULL;
				}
				m_pImagePack[dEvent]->nIndex[0] = nWp;
				m_pImagePack[dEvent]->nIndex[1] = nCurl / m_pDoc->m_cParam.PrjCfg.bProductNum[1];;
				m_pImagePack[dEvent]->nIndex[2] = nCurl - (m_pImagePack[dEvent]->nIndex[1] * m_pDoc->m_cParam.PrjCfg.bProductNum[1]);
				m_pImagePack[dEvent]->nIndex[3] = nIndex;
				m_pImagePack[dEvent]->nIndex[4] = dEvent;
				if (m_pImagePack[dEvent]->pData)
					imaqDispose((Image*)(m_pImagePack[dEvent]->pData));
				m_pImagePack[dEvent]->pData = TempImg;
				ResetEvent(m_hImgProcessEvent[dEvent+1]);
				pThread = AfxBeginThread(ImgProcessThread, (LPVOID)m_pImagePack[dEvent]);
				if (pThread==NULL)
				{
					ErrMsg.Format(_T("%s :产品%d <第%d 次照相> 图像处理启动失败!\r\n"), csz[nWp],nCurl+1, nIndex + 1);
					SetEvent(m_hImgProcessEvent[dEvent+1]);
					m_pImagePack[dEvent]->pData = NULL;
					bSuccess = FALSE;
					CLogHelper::WriteLog(ErrMsg);
				}
				SetThreadAffinityMask(pThread->m_hThread, 1<<((dEvent%3)+1));
				break;
			}
			ErrMsg.Format(_T("%s :产品%d <第%d 次照相> 等待空闲处理资源超时!\r\n"), csz[nWp], nCurl + 1, nIndex + 1);
			bSuccess = FALSE;
// 			CLogHelper::WriteLog(ErrMsg);
			break;
		}
	}else
		ErrMsg.Format(_T("%s :产品%d <第%d 次照相> 抓取图像失败!\r\n"), csz[nWp], nCurl + 1, nIndex + 1);
	if (!bSuccess)
	{
		imaqDispose(TempImg);
		m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::VISION_FAIL<<1);
		m_pView->m_MainCtrlDlg.OnUpdateSts(nWp, nCurl, CMainCtrlDlg::FLASH_ENABLE, CMainCtrlDlg::ERASING_STS);
	}
	if (!ErrMsg.IsEmpty())
		AddedErrorMsg(ErrMsg);

	return bSuccess;
}

BOOL CMainFrame::ClearOverlayInfo(UINT nScreen, UINT nIndex)
{
	if (nIndex == 0){
		for (int i = 1; i < 4; i++)
			m_NiVision.ClearOverlay(nScreen, CMainFrame::MATCH_INFO_LAYER + i);
	}
	else
	{
		m_NiVision.ClearOverlay(nScreen, CMainFrame::MATCH_INFO_LAYER + nIndex);

	}
	return TRUE;

}

void CMainFrame::ResetSts()
{
	InitEvent();
	m_pMotionCtrller[0]->ResetAxisErr();
	m_pMotionCtrller[1]->ResetAxisErr();
	m_nSemiRun[0] = 0;
	m_nSemiRun[1] = 0;
	m_nCurrentWP[CAMERA] = 0x03;
	m_nCurrentWP[NOZZLE] = 0x03;
	m_hCameraFree.SetEvent();
	m_hNozzleFree.SetEvent();
	theApp.m_bHasLocation = FALSE;
	ZeroMemory(theApp.m_bHigh_Risk, sizeof(theApp.m_bHigh_Risk));
	ZeroMemory(theApp.m_dLocated, sizeof(theApp.m_dLocated));
	ZeroMemory(m_dLaserOffset, sizeof(m_dLaserOffset));
	ZeroMemory(m_nPinVisionSuccess, sizeof(m_nPinVisionSuccess));
	// 	::PostMessage(m_pView->m_MotionDlg.m_hWnd, WM_USER_UPDATEUI, 2, 0);
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[0], FALSE);//状态灯/蜂鸣
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[1], FALSE);//状态灯
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Tower[3], FALSE);//状态灯
	m_LaserCtrl.ResetStatus();
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_Trigger, FALSE);//
	m_pView->m_MainCtrlDlg.OnUpdateSts(0);
	m_pView->m_MainCtrlDlg.OnUpdateSts(1);
	m_LaserCtrl.SetLaserRDY();
}

int CMainFrame::WaitMotionDone(UINT nIndex, DWORD dwMilliseconds /*= 150000*/)
{
	int nMotionDone(3);
	UINT nID = nIndex>>4;
	UINT AxisNum = nIndex & 0x0f;
	if (nID >> 1 || AxisNum>5)
		return FALSE;
	HANDLE hEvent[3] = { m_pEstop->m_hObject, m_pStopRun->m_hObject, m_pMotionCtrller[nID]->m_hMotionDone[AxisNum] };
	DWORD dEvent = ::WaitForMultipleObjects(3, &hEvent[0], FALSE, dwMilliseconds);
	switch (dEvent)
	{
	case WAIT_OBJECT_0://急停
		nMotionDone--;
	case WAIT_OBJECT_0 + 1://停止
		nMotionDone--;
		m_pMotionCtrller[nID]->AxStopDec((dEvent - WAIT_OBJECT_0)*(AxisNum+1));

		break;
	case WAIT_OBJECT_0 + 2://到位
		nMotionDone = 0;
		break;
	case WAIT_TIMEOUT:
		break;
	default://
		nMotionDone = 4;
		break;
	}
	return nMotionDone;
}

LRESULT CMainFrame::OnShowErrorInfo(WPARAM wParam, LPARAM lParam)
{
	if (!m_pErrorInfoDlg)
		m_pErrorInfoDlg = new CErrorInfoDlg;
	if (m_pErrorInfoDlg && m_pErrorInfoDlg->GetSafeHwnd() == NULL)
		m_pErrorInfoDlg->Create(IDD_DIALOG_ERRORINFO, this);
	::PostMessage(m_pErrorInfoDlg->m_hWnd, WM_USER_SHOWERRORINFO, wParam, lParam);

	return 0;

}

BOOL CMainFrame::IsZSafe()
{
	if (m_pMotionCtrller[0]->m_bAxisMotionDone[3] && (m_pDoc->m_cParam.PrjCfg.dZPosition[0] - m_pMotionCtrller[0]->m_dAxisCurPos[3]) >=-0.1)
		return TRUE;
	return FALSE;
}

BOOL CMainFrame::IsAboveCamera()
{
	if (fabsf(m_pDoc->m_cParam.PrjCfg.dXPosition[0] - m_pMotionCtrller[0]->m_dAxisCurPos[0]) <= 20)
		return TRUE;
	return FALSE;

}

void CMainFrame::StopCommand(BOOL bEmg)
{
	if (m_bHomeThreadAlive[0] || bEmg)
	{
		m_pMotionCtrller[0]->SetEStop(TRUE);
		m_pMotionCtrller[1]->SetEStop(TRUE);
	}
	StopAutoRun();
	EnableSaftyDev(0, TRUE);
	EnableSaftyDev(1, TRUE);
}

BOOL CMainFrame::SwitchCamera(UINT nIndex, BOOL bLive)
{
	BOOL bSuccess(FALSE);
// 	LightControllerSet(1>>nIndex,FALSE);
	m_NiVision.StartImageMonitor(1 >> nIndex, FALSE);
	m_nCameraAlive &= ~(0x01 << (1 >> nIndex));

	bSuccess = m_NiVision.StartImageMonitor(nIndex, bLive);
	if (bSuccess)
		m_nCameraAlive |= 0x01 << nIndex;
	else
		m_nCameraAlive &= ~(0x01 << nIndex);
	LightControllerSet(nIndex, bLive);
	return bSuccess;
}




void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	static BYTE nTemp(0);
	static LARGE_INTEGER freq;
	LARGE_INTEGER end;
	QueryPerformanceFrequency(&freq);
	ULONG diff(1);
	CString str[] = { _T("测距：无效"), _T("测距：偏高"), _T("测距：偏低"), _T("测距：合适")};
	switch (nIDEvent)
	{
	case 5:
		StartTemplateRW(FALSE,1);
		if (m_pMotionCtrller[0]->m_bInit&&m_pMotionCtrller[1]->m_bInit)
		{
			m_pMotionCtrller[0]->ResetAxisErr();
			m_pMotionCtrller[1]->ResetAxisErr();
			UINT n=SetTimer(2, 500, NULL);
			if (n ^ 2)
				AfxMessageBox(_T("设备初始化:定时器2启动异常"));
			n=SetTimer(3, 1000, NULL);
			if (n ^ 3)
				AfxMessageBox(_T("设备初始化:定时器3启动异常"));
		}
		else
			ShowWindow(SW_SHOWMAXIMIZED);
		KillTimer(nIDEvent);
		break;
	case 1:
		diff = m_pView->m_MainCtrlDlg.m_lQuantity-m_lQuantityStart;
		if (diff)
		{
			QueryPerformanceCounter(&end);
			UINT tm=(end.QuadPart - m_lTimestart.QuadPart)*1000 / freq.QuadPart;
			ULONG sp = tm / diff;
			::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 3, sp);

		}
		if (!m_bAutoThreadAlive)
			KillTimer(nIDEvent);
		break;
	case 2:
		if (m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_JL_Status[0]])
			theApp.m_nRangingSts = 1;
		else
			if (m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_JL_Status[1]])
				theApp.m_nRangingSts = 2;
			else
				if (m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_JL_Status[2]])
					theApp.m_nRangingSts = 3;
				else
					theApp.m_nRangingSts = 0;
		
		m_wndStatusBar.SetPaneText(0, str[theApp.m_nRangingSts]);

		break;
	case 3:
		m_pMotionCtrller[0]->StartMotionCardMonitor(this);
		m_pMotionCtrller[1]->StartMotionCardMonitor(this);
		m_pMotionCtrller[0]->ResetAxisErr();
		m_pMotionCtrller[1]->ResetAxisErr();
		ShowWindow(SW_SHOWMAXIMIZED);
		UpdateTitle();
		KillTimer(nIDEvent);
		break;
	case 4:
		break;
	}
	CBCGPFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (theApp.m_bNeedSave)
	{
		if (IDYES == MessageBox(_T("有参数更改，需要保存至当前项目文件吗?"), _T("重要提示"), MB_YESNO))
		{
			StartTemplateRW();
			return;
		}
		else if (IDNO == MessageBox(_T("确认退出程序码?"), _T("重要提示"), MB_YESNO))
			return;
	}
	LightControllerSet(0, FALSE);
	::WaitForSingleObject(m_hRs485Event, 2000);
	LightControllerSet(1, FALSE);
	::WaitForSingleObject(m_hRs485Event, 2000);
	m_LaserCtrl.SetLaserRDY(FALSE);
	m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_DCF[2], FALSE);
	m_LaserCtrl.SetbondServoRDY(FALSE);

	m_hPollingStop.SetEvent();
	m_pStopRun->SetEvent();
	CBCGPFrameWnd::OnClose();
}

void CMainFrame::OnButtonLed()
{
}

void CMainFrame::OnButtonStop()
{
	// TODO:  在此添加命令处理程序代码
	StopCommand();
}


void CMainFrame::OnButtonZoom()
{
	// TODO:  在此添加命令处理程序代码
	CString btntext[2] = { _T("Adaptive"), _T("Zoom In") };
	m_bAutoZoom = !m_bAutoZoom;
	m_wndToolBar.SetButtonText(6, btntext[!m_bAutoZoom]);
	m_NiVision.EnableZoomDisplay(0, !m_bAutoZoom);
}

void CMainFrame::OnUpdateIndicatorLabel(CCmdUI *pCmdUI)
{
	// TODO:  在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable();
}

BOOL CMainFrame::SetCurrentLaserGP(UINT nIndex)
{
// 	for (BYTE i = 0; i < 4;i++)
// 	{
// 		m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_BCD[i], (nIndex >> i) & 0x01);
// 	}
	m_IoCtrller.WriteOutputByPort((m_pDoc->m_cParam.Ou_BCD[0]+1)/8, nIndex);
	return TRUE;
}

BOOL CMainFrame::IsFixerSafe()
{
	BOOL bSuccess(TRUE);
	BYTE sts(FALSE);
	if (m_pDoc->m_cParam.PrjCfg.bUseRanging)
	{
		sts = m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_JL_Status[2]];
		return sts;
	}
	return bSuccess;
}

BOOL CMainFrame::SetDockReady(UINT nIndex, BOOL bLoad /*= TRUE*/)
{
	BOOL bSuccess(TRUE);
	if (m_pDoc->m_cParam.PrjCfg.bUseFixer)
	{
		bSuccess = BlowAct(nIndex, bLoad & 0x01, TRUE);
	}
	return bSuccess;
}

BOOL CMainFrame::IsDockReady(UINT nIndex)
{
	BOOL bSuccess(TRUE);
	if (m_pDoc->m_cParam.PrjCfg.bUseFixer)
	{
		bSuccess = m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_Cyder[nIndex * 2 + 1]] == TRUE
			&& m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_Cyder[nIndex * 2]] == FALSE;
	}
	return bSuccess;
}

BOOL CMainFrame::BlowAct(UINT nIndex, BOOL bOnOff, BOOL bWait /*= FALSE*/)
{
	BOOL bSuccess(TRUE);
	CString ErrMsg;
	CString str[] = { _T("左边夹具"), _T("右边夹具") };
	CString str1[2] = { _T("松弛"), _T("紧固") };
	UINT nPort;
	bOnOff = bOnOff?0x01:0x00;
	if (bWait&&m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_Cyder[nIndex * 2 + bOnOff]] == TRUE
		&& m_IoCtrller.m_bInput[m_pDoc->m_cParam.In_Cyder[nIndex*2+(1>>bOnOff)]] == FALSE)
			return TRUE;
	bSuccess = m_IoCtrller.WriteOutputByBit(m_pDoc->m_cParam.Ou_DCF[nIndex], bOnOff);
	if (bSuccess&&bWait)
	{
		if (!m_IoCtrller.WaitSensorSignal(m_pDoc->m_cParam.In_Cyder[nIndex * 2 + (1 >> bOnOff)], FALSE))
			ErrMsg.Format(_T("警告：%s气缸%s动作后仍有%s感应信号\r\n"), str[nIndex], str1[bOnOff], str1[1 >> bOnOff]);
		else if (!m_IoCtrller.WaitSensorSignal(m_pDoc->m_cParam.In_Cyder[nIndex * 2+bOnOff], TRUE))
			ErrMsg.Format(_T("警告：%s气缸%s动作后没等到%s感应信号\r\n"), str[nIndex], str1[bOnOff], str1[bOnOff]);
		else
			return TRUE;
		AddedErrorMsg(ErrMsg);
		return FALSE;
	}
	return bSuccess;
}

void CMainFrame::OnConfirmProject()
{
	if (m_pPrjSettingDlg == NULL)
	{
		m_pPrjSettingDlg = new CProjectSettingDlg;
	}
	if (m_pPrjSettingDlg->GetSafeHwnd() == NULL)
	{
		m_pPrjSettingDlg->Create(CProjectSettingDlg::IDD, this);
	}
	//	m_pPrjSettingDlg->ChangeView(FALSE);
	m_pPrjSettingDlg->m_btnCreate.SetWindowText(_T("退出"));
	m_pPrjSettingDlg->m_btnLoad.SetWindowText(_T("确认"));
	m_pPrjSettingDlg->ShowWindow(SW_SHOW);
	m_pPrjSettingDlg->GetDlgItem(IDC_EDIT_NEWPROJECT)->ShowWindow(SW_HIDE);
	m_pPrjSettingDlg->m_btnLoad.ShowWindow(SW_SHOW);
	::PostMessage(m_pPrjSettingDlg->m_hWnd,WM_USER_UPDATEUI, 0, 2);

	ShowWindow(SW_HIDE);
}

void CMainFrame::AutoCalculatePos1(BYTE nWp)
{
	UINT i(0), j(0), k(0), nNum(1);
	UINT nRow, nCol;
	nRow = m_pDoc->m_cParam.PrjCfg.bProductNum[0];
	nCol = m_pDoc->m_cParam.PrjCfg.bProductNum[1];
	nWp &= 0x01;
	//测量与否影响拍照路径
	BYTE PathMode = m_pDoc->m_cParam.PrjCfg.bUseRanging&&!m_pDoc->m_cParam.PrjCfg.bRangingMode;
	BYTE bTYpe = /*(0x02 << PathMode)-*/1;
	if (theApp.m_bSortingDir^bTYpe)
	{
		theApp.m_bSortingDir = bTYpe;
		m_pDoc->m_cParam.PrjCfg.bSortingDir = bTYpe;
		::PostMessage(m_pView->m_MainCtrlDlg.m_hWnd, WM_USER_UPDATEUI, 2, 0);
	}
	for (i = 0; i < nRow; i++)
	{
		for (k = 0; k < nCol; k++)
		{
			if ((i | k) == 0)
				continue;
			for (j = 0; j < MAX_PinNumber; j++)
			{
				if ((i & 0x01)/*&&!PathMode*/)
				{
					m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][nNum][j].EndPoint[0] = m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][0][j].EndPoint[0] + (nCol - k - 1) * m_pDoc->m_cParam.PrjCfg.fProductSpace[0];
				}
				else
				{
					m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][nNum][j].EndPoint[0] = m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][0][j].EndPoint[0] + k * m_pDoc->m_cParam.PrjCfg.fProductSpace[0];
				}
				m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][nNum][j].EndPoint[1] = m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][0][j].EndPoint[1] + i * m_pDoc->m_cParam.PrjCfg.fProductSpace[1];
				m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][nNum][j].EndPoint[2] = m_pDoc->m_cParam.PrjCfg.ProductArray[nWp][0][j].EndPoint[2];
			}
			nNum++;
		}
	}
}

void CMainFrame::AutoCalculatePos2(BYTE nWp)
{
	UINT i(0), j(0), k(0), nNum(1);
	UINT nRow;
	nRow = m_pDoc->m_cParam.PrjCfg.bProductNum[0];
	if (nRow > 20)
		AfxMessageBox(_T("行参数不正确，超过20行的限制"));
	nWp &= 0x01;
	for (i = 1; i < nRow; i++)
	{
		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i][0] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][0][0];
		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i][1] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][0][1] + i * m_pDoc->m_cParam.PrjCfg.fProductSpace[1];
		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i][2] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][0][2];

		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i + 1][0] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][1][0];
		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i + 1][1] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][1][1] + i * m_pDoc->m_cParam.PrjCfg.fProductSpace[1];
		m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][2 * i + 1][2] = m_pDoc->m_cParam.PrjCfg.dRangingRel[nWp][1][2];
	}
}

BOOL CMainFrame::ShowWaiteProcessDlg(BOOL bShow /*= TRUE*/)
{
	static BYTE nCount(0);
	if (bShow){
		if (m_pWaiteProcessDlg == NULL)
			m_pWaiteProcessDlg = new CWaiteProcessDlg;
		if (m_pWaiteProcessDlg &&  m_pWaiteProcessDlg->GetSafeHwnd() == NULL)
			m_pWaiteProcessDlg->Create(CWaiteProcessDlg::IDD, theApp.m_pMainWnd);
		m_pWaiteProcessDlg->ShowWindow(SW_RESTORE);
		nCount++;
	}
	else if (m_pWaiteProcessDlg != NULL)
	{
		if (nCount)
			nCount--;
		if (!nCount)
		{
			SAFE_DELETEDLG(m_pWaiteProcessDlg);
		}
	}
	return nCount;
}

void CMainFrame::UpdateWaiteProcessDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	if (m_pWaiteProcessDlg&&m_pWaiteProcessDlg->GetSafeHwnd())
		::PostMessage(m_pWaiteProcessDlg->m_hWnd, WM_USER_SHOWPROCESS, wParam, lParam);
}

UINT _cdecl CMainFrame::FileRWProcess(LPVOID lpParam)
{
	LuckyBag* pOperateOrder = (LuckyBag*)lpParam;
	CMainFrame* pFrame = (CMainFrame*)(pOperateOrder->pOwner);
	UINT bSaveOrLoad = pOperateOrder->nIndex[0];
	UINT nMode = pOperateOrder->nIndex[1];

	SAFE_DELETEARRAY(pOperateOrder->nIndex);
	SAFE_DELETE(pOperateOrder);
	if (pFrame)
	{
		pFrame->m_bFileThreadAlive = TRUE;
		pFrame->UpdateWaiteProcessDlg(0, _mm_popcnt_u32(pFrame->m_pDoc->m_cParam.PrjCfg.uTemplateSts));
		for (BYTE i = 0; i < MAX_PinNumber; i++)
		{
			if (pFrame->m_pDoc->m_cParam.PrjCfg.uTemplateSts & (1 << i))
			{
				if (!pFrame->LoadTemplate(i, bSaveOrLoad))
					pFrame->m_pDoc->m_cParam.PrjCfg.uTemplateSts &= ~(0x01 << i);
				pFrame->UpdateWaiteProcessDlg(1, 0);
			}
		}
		theApp.ShowWaiteProcessDlg(FALSE);
		switch (nMode)
		{
		case 0:
			pFrame->OnFileSave(0);
		default:
			break;
		}
		pFrame->m_bFileThreadAlive = FALSE;
	}
	return 0;
}

BOOL CMainFrame::StartTemplateRW(BOOL bSave /*= TRUE*/, BYTE nMode)
{
	BOOL bSuccess(TRUE);
	CWinThread* pThread(NULL);
	pLuckyBag pOperateOrder = new LuckyBag;
	pOperateOrder->pOwner = this;
	pOperateOrder->nIndex = new UINT[2];
	pOperateOrder->nIndex[0] = bSave;
	pOperateOrder->nIndex[1] = nMode;
	if (!m_bFileThreadAlive)
		ShowWaiteProcessDlg(TRUE);

	if (m_bFileThreadAlive || !(pThread = AfxBeginThread(FileRWProcess, (LPVOID)pOperateOrder)))
	{
		bSuccess = FALSE;
		SAFE_DELETE(pOperateOrder->nIndex);
		SAFE_DELETE(pOperateOrder);
	}
	return bSuccess;

}

void CMainFrame::OnFileSave(BYTE nFlag)
{
	UINT i(1);
	CString str;
	CStopWatch sw;
	// 	str.Format(_T("保存模板耗时：%.1f 微秒"),sw.GetTimeSpan());
	m_pDoc->LoadPrjData(m_pDoc->m_szCurParamName, FALSE);
	m_pDoc->m_cParam.DefaultProjectName = m_pDoc->m_szCurParamName;
	// 	str.Format(_T("保存二进制文件耗时：%.1f 微秒"), sw.GetTimeSpan());
	// 	pFrame->ShowPopup(str,1,5);
	m_pDoc->SaveParameter();
	// 	str.Format(_T("保存ini文件耗时：%.1f 微秒"), sw.GetTimeSpan());
	// 	pFrame->ShowPopup(str);
	theApp.m_bNeedSave = FALSE;
}


