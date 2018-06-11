
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "AdvMotion.h"
#include "IOControl.h"
#include "NiVisionCtl.h"
#include "SerialPort.h"
#include "PLCModbus.h"
#include "GeneralSettingDlg.h"
#include "HomeSettingDlg.h"
#include "LaserSettingDlg.h"
#include "ProjectSettingDlg.h"
#include "LoginDlg.h"
#include "HomeProcessDlg.h"
#include "IOStatusDlg.h"
#include "ErrorInfoDlg.h"
#include "WaiteProcessDlg.h"
#include "APMDoc.h"
#include "APMView.h"
class CMainFrame : public CBCGPFrameWnd
{
	class CMyPopupWindow : public CBCGPPopupWindow
	{
		virtual BOOL OnClickLinkButton(UINT)
		{
			SendMessage(WM_CLOSE);
			return TRUE;
		}
	public:
		CBCGPPopupWndParams		m_params;
		virtual BOOL Create(CWnd* pWndOwner,HMENU hMenu = NULL, CPoint ptPos = CPoint(-1, -1))
		{
			return CBCGPPopupWindow::Create(pWndOwner, m_params, hMenu, ptPos);
		}
		void SetAutoCloseTime(int nTime)	// ms
		{
			m_nAutoCloseTime = nTime?nTime:60000;
		}

	};


protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:
	CAPMDoc*					m_pDoc;
	CAPMView*					m_pView;
	//Child Window
	CGeneralSettingDlg*			m_pGeneralSetDlg;
	CHomeSettingDlg*			m_pRunSettingDlg;
	CLaserSettingDlg*			m_pLaserSettingDlg;
	CProjectSettingDlg*			m_pPrjSettingDlg;
	CIOStatusDlg*				m_pIODlg;
	CErrorInfoDlg*				m_pErrorInfoDlg;
	CHomeProcessDlg*			m_pHomeProcessDlg;
	CLoginDlg*					m_pLoginDlg;
// 	CRangingDlg*				m_pRangingDlg;
	CWaiteProcessDlg*			m_pWaiteProcessDlg;

	CAdvMotion*					m_pMotionCtrller[2];
	CIOControl					m_IoCtrller;
	CNiVisionCtl				m_NiVision;
	CSerialPort					m_LightControl;
	CPLCModbus					m_LaserCtrl;
	Image*						m_pTemplate[MAX_PinNumber];
	Image*						m_pResult;
	CoordinateSystem			m_CoordSys[2];
	UINT						m_nMatchSuccess;
	UINT						m_nDetectSuccess;
	UINT						m_nIsHomeEnd;
	BOOL						m_bAppoint;
	BOOL						m_bAutoZoom;
	BOOL						m_bIdling;
	BOOL						m_bPollingAlive;
	BOOL						m_bAutoThreadAlive;
	BOOL						m_bHomeThreadAlive[2];
	BOOL						m_bIsEmergencyStop;
	BOOL						m_bIsPowerOn;
	BOOL						m_bSwiftMode;
	BOOL						m_bLearningAlive;
	BOOL						m_bFileThreadAlive;
	BOOL						m_bImgProcessAlive;
	BOOL						m_bSalveThreadAlive[2];
	BOOL						m_nBondAlarm;
	BOOL						m_bBinary;
	BYTE						m_bNeedClimb;

	UINT						m_bWaitforTrigger;
	UINT					m_bSaftyDevice;
	UINT					m_bLightLed;
	UINT					m_bGuideLight;

	UINT					m_nCurrentRunMode;//0:待机； 1:回零中； 2:自动中； 3:测试出球；
	UINT					m_nCurrentWP[2];//当前操作工位
	UINT					m_nCurrentProduct[2][2];
	UINT					m_nCurrentVisionCheck[2];
	UINT					m_nCurrentPin[2];
	UINT					m_nCurrentRanging[2];
	BYTE					m_nPinVisionSuccess[2][MAX_ProductNumber][16];
	double					m_dLaserOffset[2][MAX_ProductNumber][16][3];

	UINT					m_nCameraAlive;
	double					m_dCenterPosition[2];
	USHORT					m_nSemiRun[2];
	IVA_Data*				m_ivaData;
	pLuckyBag				m_pImagePack[8];

	HANDLE					m_hImgProcessEvent[9];//
	HANDLE					m_hProcessEvent[2][7];//
	HANDLE					m_hSlaveEvent[2];//
	HANDLE					m_hRs485Event;//
	CEvent					m_hHomehExtThread;
	CEvent					m_hCameraFree;
	CEvent					m_hNozzleFree;
	CEvent					m_hBondHomeEnd;
	CEvent					m_hOneCycleStart[2];
	CEvent					m_hPollingStop;
	CEvent					m_hBondRecovery;
	CEvent*					m_pEstop;
	CEvent*					m_pStopRun;
	static HWND				m_hwdDesktopAlert;
	CCriticalSection		m_CriticalData;
// 操作
public:
	enum { LOAD_DONE = 0, RANGING_DONE, SNAPSHOT_MATCH_DONE, BONDING_READY_DONE, BALL_LASER_DONE, NOZZLE_CLEAN_DONE, SUPPLY_DONE };
	enum { START_LOAD = 0, RANGING,SNAPSHOT_MATCH, BONDING_READY, BALL_LASER, NOZZLE_CLEAN,BALLS_SUPPLY,WAIT_BTN_START};
	enum { ZSAFE_POSITION = 0, LOAD_POSITION, PRE_SNAPSHOT_POSITION, SNAPSHOT_POSITION, LASER_POSITION, Calibration_CHECK, Calibration_LASER, NOZZLE_CHECK_POSITION, NOZZLE_CLEAN_POSITION, PRE_LASER_POSITION,RANGING_POSITION };
	enum { BASIC_INFO_LAYER = 0, MATCH_INFO_LAYER, CALCULATED_INFO_LAYER, DETECT_INFO_LAYER, ASSISANT_INFO_LAYER };
	enum { CAMERA = 0, NOZZLE = 1, };
	enum { CLIMB_POINT=1, CLIMB_PRODUCT , CLIMB_FIXER};
public:
	BOOL ShowWaiteProcessDlg(BOOL bShow = TRUE);
	void UpdateTitle();
	void OnConfirmProject();
	void InitEvent();
	BOOL InitialDefine();
	BOOL InitHardware();
	BOOL LightControllerOpen(BOOL bOpen = TRUE);
	BOOL LightControllerSet(BYTE nID,BYTE nValue,BYTE nMode=0);
	void SetUserType(BYTE nId);
	BYTE GetUserType();
	BOOL SetHardWareRun(BOOL bEnable = TRUE);
	void AddedErrorMsg(CString Info);
	void UserLogin(int nType);
	void EnableSaftyDev(UINT nIndex, BOOL bOn);
	BOOL ShowHomeProcessDlg(BOOL bShow = TRUE);
	void EStop(BOOL bState);
	void StopAutoRun();
	void StopCommand(BOOL bEmg=FALSE);
	BOOL SwitchCamera(UINT nIndex,BOOL bLive=TRUE);
	int	 WaitMotionDone(UINT nIndex, DWORD dwMilliseconds = 150000);
	int	 MasterWaitFor(UINT &nWp,UINT nFlage[2]);
	int	 MasterWaitFor(UINT nWp, UINT nStep);
	int	 SlaveWaitFor(UINT nCmd);
	BOOL SetCurrentLaserGP(UINT nIndex);
	BOOL LaserOut(BOOL bOnOff = FALSE, UINT nMode = 0);//
	BOOL CheckLaserDone(UINT ntime = 1000);//
	BOOL BlowAct(UINT nIndex, BOOL bOnOff, BOOL bWait = FALSE);
	BOOL SetDockReady(UINT nIndex, BOOL bLoad = TRUE);
	BOOL IsDockReady(UINT nIndex);

	BOOL CamPosition(UINT nPos,BOOL bWait = TRUE);
//  	BOOL NozzlePosition(UINT nPos, BOOL bCheck, BOOL bWait = TRUE);
	BOOL GoPosition(UINT nPos, BOOL bWait = TRUE);
	BOOL ZGoPosition(UINT nPos, BOOL bCheck, BOOL bWait = TRUE);
	BOOL ZCheckClimbPos(UINT nPos);
	BOOL IsZSafe();
	BOOL IsAboveCamera();
	BOOL IsFixerSafe();
	BOOL DownPlcConf();
	void AutoCalculatePos1(BYTE nWp);
	void AutoCalculatePos2(BYTE nWp);

	
	BOOL LoadCalibration(UINT nIndex);
	BOOL LoadTemplate(UINT nIndex, BOOL bSave = TRUE);
	BOOL LearnTemplate(UINT nIndex, Image* mask=NULL);

	BOOL GetImg2Buffers(UINT nWp, UINT nCurl, UINT nIndex);
	BOOL ClearOverlayInfo(UINT nScreen = 0, UINT nIndex = 0);
	UINT  MatchProcessing(Image* image, BYTE nIndex, Rect rc, UINT nScore, BYTE nMode, IVA_StepResults* stepResults);
	int  DetectShapeProcessing(Image* image, BYTE nIndex, Rect rc, UINT nScore, BYTE nMode, IVA_StepResults* stepResults);
	int  DetectCircleProcessing(Image* image, BYTE nIndex, Annulus ann, IVA_StepResults* stepResults);
	BOOL SnapshotProcess(UINT nWp);
	BOOL RangingProcess(UINT nWp);
	BOOL BondingProcess(UINT nWp);
	UINT  ImageProcess(Image* image, BYTE nIndex, BYTE nWP, BYTE nRow, BYTE nCol);
	UINT  WorkProcess(UINT nWp, UINT nStep);
	BOOL PollingHandle();
	void ResultLog();
	BOOL SetButtonLamp(BYTE nIndex,BYTE nOn=FALSE);
	BOOL StartReturnHome();
	BOOL StartPolling();
	BOOL StartAuto(BOOL bEnable = FALSE);//
	BOOL StartTemplateRW(BOOL bSave = TRUE,BYTE nMode=0);//
	BOOL StartManualRunThread(UINT nWP, UINT nIndex);
	BOOL StartSlaveThread(UINT nWP, UINT nIndex);
	BOOL StartLearning(UINT nIndex, Image* mask=NULL);
protected:
	static UINT _cdecl PollingThread(LPVOID lpParam);
	static UINT _cdecl HomeThread(LPVOID lpParam);
	static UINT _cdecl HomeThreadExt(LPVOID lpParam);
	static UINT _cdecl ManualRunThread(LPVOID lpParam);
	static UINT _cdecl AutoThread(LPVOID lpParam);
	static UINT _cdecl SlaveThread(LPVOID lpParam);
	static UINT _cdecl LearningProcess(LPVOID lpParam);
	static UINT _cdecl FileRWProcess(LPVOID lpParam);
	static UINT _cdecl ImgProcessThread(LPVOID lpParam);

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
 	CBCGPToolBar			m_wndToolBar;
	CBCGPStatusBar			m_wndStatusBar;
	CBCGPMenuBar			m_wndMenuBar;
	BOOL					m_bCreateSpl;
	UINT					m_nUserType;
	LARGE_INTEGER			m_lTimestart;
	ULONG					m_lQuantityStart;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnShowResults(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnShowTestResults(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT	OnShowErrorInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIochange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnPositionChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnMotionStsChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateDisplay(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnShowPopup(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT OnClosePopup(WPARAM, LPARAM);
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);
	afx_msg LRESULT	OnHomed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCommRecvChar(WPARAM wParam, LPARAM port);
	afx_msg LRESULT OnCommRecvString(WPARAM wString, LPARAM lLength);

public:
	afx_msg void OnFileSave(BYTE nFlag=0);
	afx_msg void ResetSts();
	afx_msg void OnViewCCD(UINT idCtl);
	afx_msg void OnOperationMode(UINT idCtl);
	afx_msg void OnViewSetting(UINT idCtl);
	afx_msg void OnCommandNewProject();
	afx_msg void OnViewIODlg();
	afx_msg void OnViewRangingDlg();
	afx_msg void OnLoginDlg();
	afx_msg void OnViewProjectSetting();
	afx_msg void OnUpdateCommand(CCmdUI *pCmdUI);
	static void  _cdecl ShowPopup(CString Info, BOOL bAutoClose = TRUE, UINT nTime = 2);
	void ClosePopup();
	afx_msg void OnUpdateFileNew(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileOpen(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonZoom();
	afx_msg void OnButtonLed();
	afx_msg void OnUpdateButtonZoom(CCmdUI *pCmdUI);
	afx_msg void OnUpdateIndicatorLabel(CCmdUI *pCmdUI);
	void	UpdateWaiteProcessDlg(WPARAM wParam = 0, LPARAM lParam = 0);
};


