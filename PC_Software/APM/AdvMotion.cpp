// AdvMotion.cpp: implementation of the CAdvMotion class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "AdvMotion.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAdvMotion::CAdvMotion(int nID)
{
	m_bThreadAlive=FALSE;
	m_bInit = FALSE;
	m_hEventStart=NULL;
	m_hEventEnd=NULL;
	m_hEventArray[0]=NULL;
	m_hEventArray[1]=NULL;

	m_ErrMsg = _T("");
	m_bEStop=false;
	m_nCardID=nID;
	m_hDev = NULL;
	ZeroMemory(m_Axishand, sizeof(m_Axishand));
	ZeroMemory(m_dHomeVel, sizeof(m_dHomeVel));
	ZeroMemory(m_dHomeAcc, sizeof(m_dHomeAcc));
	ZeroMemory(m_dAxisCurPos,sizeof(m_dAxisCurPos));
	ZeroMemory(m_nAxisResolution,sizeof(m_nAxisResolution));
	ZeroMemory(m_AxisStatus, sizeof(m_AxisStatus));
	ZeroMemory(m_MotionStatus, sizeof(m_MotionStatus));
	m_dwDevNum = 0;	//设备卡数量
	m_nAxisUsed = 0;
	for (int i = 0; i < 6; i++){
		if (i < 2)
		{
			m_GpID[i] = 0;
			m_GpHand[i] = NULL;
			m_hEventArray[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		m_hMotionDone[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
	}
	for (int j = 0; j < 4; j++)
		m_bAxisMotionDone[j] = TRUE;
	m_hEventEnd = m_hEventArray[0];
	m_hEventStart = m_hEventArray[1];
	SetEvent(m_hEventStart);
	ResetEvent(m_hEventEnd);
}


CAdvMotion::~CAdvMotion()
{
	CloseCard();
}
/*********Check Device should be called first**********/
/*
BOOL CAdvMotion::GetDeviceInf(DEVLIST* DeviceList)
{
	ULONG	nResult(1);
	nResult=Acm_GetAvailableDevs(DeviceList,MAX_DEVICES,&m_dwDevNum);
	if (nResult!=SUCCESS)
	{
		Errcode="no available device in system";
		return FALSE;
	}
	return TRUE;
}*/


BOOL CAdvMotion::InitBoard(ULONG DeviceNumber, UINT AxisUsed)
{
	UINT i, j(0);
	ULONG	buffLen=4;
	ULONG	AxisNumber=0,AxisPerDev=0;
	ULONG	PropertyVal=0;
	U32 lErrorID = Acm_GetAvailableDevs(m_AvaDevs, 4, &m_dwDevNum);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	
	if(DeviceNumber > (m_dwDevNum-1))
	{
		m_ErrMsg.Format(_T("%d控制卡未找到！\n"),DeviceNumber);
		return FALSE;
	}
	lErrorID = Acm_DevOpen(m_AvaDevs[DeviceNumber].dwDeviceNum,&m_hDev);
	if (lErrorID!=SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	lErrorID = Acm_GetU32Property(m_hDev, FT_DevAxesCount, &AxisPerDev);
	if (lErrorID!=SUCCESS)
	{
		Acm_DevClose(&(m_hDev));
		ErrorInfo(lErrorID);
		return FALSE;
	}
	m_ulAxisCount = AxisPerDev>MAX_AXIS ? MAX_AXIS : AxisPerDev;	//获取可用总轴数
	AxisNumber = _mm_popcnt_u32(AxisUsed);//使用轴数
	if (AxisNumber>m_ulAxisCount)
	{
		Acm_DevClose(&(m_hDev));
		m_ErrMsg.Format(_T("欲使用%d轴大于支持轴数%d！\n"), AxisNumber,m_ulAxisCount);
		return FALSE;
	}
	m_nAxisUsed = AxisUsed;
	for (i = 0; i < m_ulAxisCount; ++i)
	{
		//Open every Axis and get the each Axis Handle
		lErrorID = Acm_AxOpen(m_hDev, (USHORT)i,
			&(m_Axishand[i]));
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			return FALSE;
		}
		Acm_SetU32Property(m_hDev, CFG_DevEmgLogic, 0);
		// 			Acm_SetU32Property(m_Axishand[i], CFG_AxPPU, 10000);

		Acm_SetU32Property(m_Axishand[i], CFG_AxPulseInMaxFreq, 3);
		Acm_SetU32Property(m_Axishand[i], CFG_AxPulseOutMode, 8);

		Acm_SetU32Property(m_Axishand[i], CFG_AxAlmEnable, 1);
		Acm_SetU32Property(m_Axishand[i], CFG_AxAlmLogic, 1);

		// 			Acm_SetU32Property(m_Axishand[i], CFG_AxInpEnable, 1);
		// 			Acm_SetU32Property(m_Axishand[i], CFG_AxInpLogic, 0);

		Acm_SetU32Property(m_Axishand[i], CFG_AxErcEnableMode, 1);
		Acm_SetU32Property(m_Axishand[i], CFG_AxErcLogic, 1);

		// 			Acm_SetU32Property(m_Axishand[i], CFG_AxElLogic, 1);

		Acm_SetU32Property(m_Axishand[i], CFG_AxSwMelEnable, 1);
		Acm_SetU32Property(m_Axishand[i], CFG_AxSwPelEnable, 1);
		Acm_SetU32Property(m_Axishand[i], CFG_AxSwMelReact, 0);
		Acm_SetU32Property(m_Axishand[i], CFG_AxSwPelReact, 0);

		// 			Acm_SetU32Property(m_Axishand[i], CFG_AxEzLogic, EZ_ACT_LOW);

		//Reset Command Counter
		Acm_AxSetCmdPosition(m_Axishand[i], 0);
		Acm_AxResetError(m_Axishand[i]);

	}
	SetUnitPulse(ALL_AXIS, 2000);
	InpConfig(ALL_AXIS, INP_ACT_LOW);
	HardLimitConfig(ALL_AXIS, HLMT_EN, HLMT_ACT_HIGH);
	HomeConfig(ALL_AXIS, ORG_ACT_LOW);

	m_bInit = TRUE;
	m_bEStop=false;
	                                      
	m_bThreadAlive = FALSE;
	return TRUE;
}
void CAdvMotion::SetEStop(bool Estop)
{

	m_bEStop=Estop;
	if (m_bEStop)
		AxStopEmg();
}

BOOL CAdvMotion::CreateGroup(UINT AxisNum, UINT nGp)
{
	U32		lErrorID;
	U32		AxEnableEvtArray[4] = { 1, 1, 1, 1 };
	U32		GpEnableEvt[3] = {1,1,1};
	U32 RefPlane = 0;
	int i;
	if (nGp > 1)
		return FALSE;
	//Add an axis to the specified group
	if (AxisNum==0)
	{
		if (m_GpHand[nGp])
		{
			Acm_GpClose(&m_GpHand[nGp]);
			m_GpHand[nGp] = NULL;
		}
		return TRUE;
	}
	for (i=0; i < 4; i++)
	{
		if (AxisNum >> i & 0x01)
		{
			lErrorID = Acm_GpAddAxis(&m_GpHand[nGp], m_Axishand[i]);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
		}
	}

	lErrorID = Acm_SetU32Property(m_GpHand[nGp], PAR_GpRefPlane, RefPlane);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	lErrorID = Acm_GetU32Property(m_GpHand[nGp], PAR_GpGroupID, &m_GpID[nGp]);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	GpEnableEvt[0] |= EVT_GP1_MOTION_DONE << m_GpID[nGp];
	GpEnableEvt[1] |= EVT_GP1_VH_START << m_GpID[nGp];//Enable VHStart
	GpEnableEvt[2] |= EVT_GP1_VH_END << m_GpID[nGp];//Enable VHEnd
	lErrorID = Acm_EnableMotionEvent(m_hDev, AxEnableEvtArray, GpEnableEvt, m_ulAxisCount, 3);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	return TRUE;
}

BOOL CAdvMotion::ModifyGroup(BYTE nGp, BYTE AxisNum, BYTE bAdd)
{
	U32		lErrorID;
	U32		ExistAxis(0);
	BOOL  bResult(FALSE);
	if (nGp > 1 || !m_GpHand[nGp])
		return bResult;
	m_CriticalCard.Lock();
	Acm_GetU32Property(m_GpHand[nGp], CFG_GpAxesInGroup, &ExistAxis);
	if ((ExistAxis >> AxisNum) & 0x01)
	{
		if (bAdd)
			bResult = TRUE;
		else
			Acm_GpRemAxis(m_GpHand[nGp], m_Axishand[AxisNum]);
	}
	else
	{
		if (!bAdd)
			bResult = TRUE;
		else{

			lErrorID = Acm_GpAddAxis(&m_GpHand[nGp], m_Axishand[AxisNum]);
			if (lErrorID != SUCCESS)
				ErrorInfo(lErrorID);
			else
				bResult = TRUE;
		}

	}
	m_CriticalCard.Unlock();
	return bResult;
}

//Close Device should be called before Exit
BOOL CAdvMotion::CloseCard()
{
	UINT AxisNum;
	USHORT usAxisState[4];
	StopThread();
	//Stop Every Axes

	if (m_bInit == TRUE)
	{
		for (AxisNum = 0; AxisNum<m_ulAxisCount; AxisNum++)
		{
			Acm_AxGetState(m_Axishand[AxisNum], &usAxisState[AxisNum]);
			//if axis is in error state , reset it first. then Stop Axes
			if (!(m_MotionStatus[AxisNum]>>6&0x01) &&usAxisState[AxisNum] == STA_AX_ERROR_STOP)
			{
				Acm_AxResetError(m_Axishand[AxisNum]);
			}	
			Acm_AxStopDec(m_Axishand[AxisNum]);
			Acm_AxSetSvOn(m_Axishand[AxisNum], FALSE);
		}
		
		//Close Axes
		for (AxisNum = 0; AxisNum<m_ulAxisCount; AxisNum++)
		{
			
			Acm_AxClose(&(m_Axishand[AxisNum]));
		}
		//Close Device
		Acm_DevClose(&(m_hDev));
		m_bInit = FALSE;
	}
	return TRUE;
}
//**************Axis Emergency stop***********************
void CAdvMotion::AxStopEmg(UINT AxisNum)//AxisNum:0-全部，1-轴1,2-轴2……
{
	UINT i(0);
	USHORT usAxisState=0;
	
	if (AxisNum > (m_ulAxisCount + 2) || (m_hDev == NULL))
	{
		return ;
	}
	else if (AxisNum > 0)
	{
		i = AxisNum - 1;
		if (i > 3)
		{
			if (m_GpHand[i - 4])
				Acm_GpStopDec(m_GpHand[i - 4]);
			return ;
		}
	}

	//if axis is in error state , reset it first. then Stop Axes
	m_CriticalCard.Lock();
	do
	{ 
		if (i < 2 && (AxisNum == 0) && m_GpHand[i])
			Acm_GpStopDec(m_GpHand[i]);
		if (!m_Axishand[i])
			break;
		Acm_AxGetState(m_Axishand[i], &usAxisState);
		if (!(m_MotionStatus[AxisNum] >> 6 & 0x01) && usAxisState == STA_AX_ERROR_STOP)
		{
			Acm_AxResetError(m_Axishand[i]);
		}
		Acm_AxStopEmg (m_Axishand[i]);//AxisID
		i++;
	} while ((i<m_ulAxisCount) && (AxisNum == 0));
	m_CriticalCard.Unlock();

	return ;
}
//**************Get Specified Card's Axis Position***********************/
int CAdvMotion::GetCurPos(UINT AxisNum,double &dPosition)//AxisNum:1-轴1,2-轴2……
{
	UINT i(0);
	U32 lErrorID;
	int nChange(0);
	double Readpositon;
	if (AxisNum>m_ulAxisCount || (m_hDev == NULL))
	{
		return -1;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	m_CriticalCard.Lock();
	do
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){

			lErrorID = Acm_AxGetActualPosition(m_Axishand[i], &Readpositon);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				m_CriticalCard.Unlock();
				return -1;
			}
			float diff = fabs(m_dAxisCurPos[i] - Readpositon);
			if (diff*m_nAxisResolution[i] > 10)
			{
				m_dAxisCurPos[i] = Readpositon;
				nChange |= (1 << i);
			}
			if (AxisNum)
				dPosition = Readpositon;
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));//
	m_CriticalCard.Unlock();
	if (AxisNum == 0)
		return nChange;
	return 0;

}
//**************MoveMode()********************************
BOOL CAdvMotion::Move(UINT AxisNum, double dEnd, double dVel, double dAcc, BOOL bAbs)
{
	double CfgVel=dVel;
	double CfgAcVel=dAcc;
	double ProperJerk=0;//s曲线速度
	USHORT usAxisState=0;
	U32 lErrorID(SUCCESS);
	if (AxisNum>m_ulAxisCount||AxisNum==0||(m_hDev==NULL)||m_bEStop)
	{
		return FALSE;
	} 
	int i=AxisNum-1;
	m_CriticalCard.Lock();

	Acm_AxGetState(m_Axishand[i], &usAxisState);
	if(usAxisState==STA_AX_READY)
	{
		Acm_SetF64Property(m_Axishand[i], PAR_AxVelLow, CfgVel/5);
		lErrorID = Acm_SetF64Property(m_Axishand[i], PAR_AxVelHigh, CfgVel);
		lErrorID = Acm_SetF64Property(m_Axishand[i], PAR_AxAcc, CfgAcVel);
		lErrorID = Acm_SetF64Property(m_Axishand[i], PAR_AxDec, CfgAcVel);
		
		if (bAbs)
		{
			lErrorID = Acm_AxMoveAbs(m_Axishand[i], dEnd);
		}else{
			lErrorID = Acm_AxMoveRel(m_Axishand[i], dEnd);
		}
		
		if (lErrorID==SUCCESS)
		{
			ResetEvent(m_hMotionDone[i]);
			m_CriticalCard.Unlock();
			return TRUE;
		}
	
	}
	m_CriticalCard.Unlock();
	ErrorInfo(lErrorID);
	return FALSE;
}

/*********  MotionStatus位状态 
0：RDY1---- RDY 针脚输入；1：ALM ---- 报警信号输入；
2：LMT_P ---- 限位开关+ ；3：LMT_P ---- 限位开关- ；
4：ORG---- 原始开关；5：DIR ---- DIR 输出；6：EMG ---- 紧急信号输入；
7：PCS ---- PCS 信号输入（PCI-1245/1245E/1265 不支持）；8：ERC ---- 输出偏转计数器清除信号至伺服电机驱动（OUT7）
9：EZ ---- 索引信号；10：CLR ---- 外部输入至清除位置计数器（PCI-1245/1245E/1265 不支持）；
11：LTC ---- 锁存信号输入；12：SD ---- 减速信号输入（PCI-1245/1245E/1265 不支持）；
13：INP ---- 到位信号输入；14：SVON ---- 伺服开启（OUT6）；
15：ALRM ---- 报警复位输出状态；16：SLMT_P ---- 软件限位+ ；
17：SLMT_N ---- 软件限位- ；18：TRIG----- 比较信号（OUT5）；
19：CAMDO ---- 凸轮区间DO （OUT4）。*/
									
BOOL CAdvMotion::GetMotionStatus(ULONG &lChangeAxis, ULONG& lError, ULONG& lMotorSenser)
{
	UINT	i(0);
	ULONG	Status(0);
	USHORT  Asts(0);
	ULONG	lBit(0);
	USHORT	uBit(0);
	U32		lErrorID;
	U16		State(0);

	if (m_hDev == NULL)
	{
		return FALSE;
	} 

	m_CriticalCard.Lock();
	do
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_AxGetState(m_Axishand[i], &Asts);

			lErrorID = Acm_AxGetMotionIO(m_Axishand[i], &Status);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				m_CriticalCard.Unlock();
				return FALSE;
			}
			lBit = m_MotionStatus[i] ^ Status;
			uBit = m_AxisStatus[i] ^ Asts;

			if ((lBit & 0x02&Status) || (uBit&&Asts == STA_AX_ERROR_STOP))
			{
				lError |= (1 << i);
				lChangeAxis |= (1 << i);
			}
			if (lBit & 0x04)//limit+
				lMotorSenser |= (1 << i);
			if (lBit & 0x08)//limit-
				lMotorSenser |= (1 << (4 + i));
			if (lBit & 0x010)//ORG
				lMotorSenser |= (1 << (8 + i));
			if ((Status & 0x02000) && (Asts < STA_AX_HOMING) && !m_bAxisMotionDone[i])
			{
				m_bAxisMotionDone[i] = TRUE;
				SetEvent(m_hMotionDone[i]);
			}
			if (!(Status & 0x02000) && Asts < STA_AX_HOMING && m_bAxisMotionDone[i])
				m_bAxisMotionDone[i] = FALSE;
			if (lBit || uBit)
			{
				m_MotionStatus[i] = Status;
				m_AxisStatus[i] = Asts;
				lChangeAxis |= (1 << (4 + i));
			}
		}
		i++;
	}while (i<m_ulAxisCount);//
	m_CriticalCard.Unlock();

	return TRUE;	
}
/********AxisStatus值状态
0：STA_AxDisable ---- 轴被禁用，用户可打开并激活。1：STA_AxReady ---- 轴已准备就绪，等待新的命令。
2：STA_Stopping ---- 轴停止。3：STA_AxErrorStop ---- 出现错误，轴停止。4：STA_AxHoming ---- 轴正在执行返回原点运动。
5：STA_AxPtpMotion ---- 轴正在执行PTP 运动。6：STA_AxContiMotion ---- 轴正在执行连续运动。
7：STA_AxSyncMotion ---- 轴在一个群组中，群组正在执行插补运动；或轴是一个从轴，正在执行E-cam/E-gear/Gantry 运动。
8：STA_AX_EXT_JOG ---- 轴由外部信号控制。当外部信号激活时，轴将执行JOG 模式运动。9：STA_AX_EXT_MPG ---- 轴由外部信号控制。当外部信号激活时，轴将执行MPG 模式运动。*/
BOOL CAdvMotion::GetAxisStatus(UINT AxisNum)
{
	UINT i(0);
	U32 lErrorID;
	if (AxisNum>m_ulAxisCount || (m_hDev == NULL))
	{
		return FALSE;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	m_CriticalCard.Lock();
	do
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			lErrorID = Acm_AxGetState(m_Axishand[i], &m_AxisStatus[i]);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				m_CriticalCard.Unlock();
				return FALSE;
			}
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));//
	m_CriticalCard.Unlock();
	return TRUE;
}
BOOL CAdvMotion::StartMotionCardMonitor(CWnd* pWnd)
{
	if(pWnd==NULL)
		return FALSE;
	m_pWnd=pWnd;
	SetEvent(m_hEventStart);
	ResetEvent(m_hEventEnd);
	if (!m_bThreadAlive)
	{
		if (!(m_Thread = ::AfxBeginThread(MotionThread,this)))
		{
			return FALSE;
		}
			
	}
	return TRUE;	
}
BOOL CAdvMotion::StopThread()
{
	do
	{
		SetEvent(m_hEventEnd);
		ResetEvent(m_hEventStart);
		
	} while(m_bThreadAlive);
	
	TRACE0("ADvMotion Card Monitor Thread end successful\n");	
	return TRUE;
}

void CAdvMotion::SuspendThread()
{
	m_Thread->SuspendThread();
	TRACE0("ADvMotion Card Monitor Thread suspend successful\n");	
}

void CAdvMotion::ResumeThread()
{
	m_Thread->ResumeThread();
	TRACE0("ADvMotion Card Monitor Thread resumethread successful\n");	
}
UINT CAdvMotion::MotionThread(LPVOID pParam)
{
	U32 nResult;

	CAdvMotion* pAdv = (CAdvMotion*)pParam;
	if (pAdv==NULL)
	{
		return 0;
	}
	DWORD dwEvent=0;
	BOOL bAlive=TRUE;
	
	while(bAlive)
	{
		nResult = 0;
		dwEvent=WaitForMultipleObjects(2,pAdv->m_hEventArray,FALSE,INFINITE);  //notice position
		switch (dwEvent)
		{
		case 0:
			bAlive=FALSE;
			break;
		case 1:
			pAdv->m_bThreadAlive=TRUE;	
			double dChangePos=0;
			int lChangeStatus=0;
			ULONG lCardError=0;
			ULONG lMotorSenser=0;
			lChangeStatus = pAdv->GetCurPos(ALL_AXIS, dChangePos);
			if (lChangeStatus > 0 && pAdv->m_pWnd->m_hWnd != NULL)
				::PostMessage(pAdv->m_pWnd->m_hWnd, WM_USER_MOTIONPOS, (WPARAM)pAdv->m_nCardID, (LPARAM)lChangeStatus);
			Sleep(0);			
			pAdv->GetMotionStatus(nResult, lCardError, lMotorSenser);
			Sleep(0);
			pAdv->CheckMotionDone(1);
			if (nResult && (pAdv->m_pWnd->m_hWnd != NULL))
				::PostMessage(pAdv->m_pWnd->m_hWnd, WM_USER_MOTIONSTATUS, (WPARAM)pAdv->m_nCardID, (LPARAM)nResult);
			break;
		}
		Sleep(5);
	}
	pAdv->m_bThreadAlive=FALSE;
	TRACE("Motion Card MonitorThread end successful\n");
	return 0;
}
BOOL CAdvMotion::CheckMotionDone(UINT nWaitTime)
{	
	UINT  i(0);
	U32 AxEvtStatusArray[4] = { 0 };
	U32 GpEvtStatusArray[3] = { 0 };
	U32 lErrorID;

	lErrorID = Acm_CheckMotionEvent(m_hDev, AxEvtStatusArray, GpEvtStatusArray, m_ulAxisCount, 3, nWaitTime);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	for (i = 0; i < 2; i++)
	{
		if (GpEvtStatusArray[0] & (EVT_GP1_MOTION_DONE << m_GpID[i]))
		{
			SetEvent(m_hMotionDone[4 + m_GpID[i]]);

		}
	}
	for (i=0; i < m_ulAxisCount;++i)
	{
		if (m_bAxisMotionDone[i] && (AxEvtStatusArray[i] & EVT_AX_MOTION_DONE))
		{
			SetEvent(m_hMotionDone[i]);
		}
	}

	return TRUE;
}

BOOL CAdvMotion::CheckMotionDone(BYTE AxisNum, UINT nWaitTime /*= 5000*/)
{
	U32		lErrorID;
	ULONG	Status(0);
	UINT i(0), j(0);
	BYTE nInp(0);
	if (m_hDev == NULL )
	{
		return FALSE;
	}

	for (j = 0; j < nWaitTime/10; j++)
	{
		Sleep(10);
		for (int k=0; k < m_ulAxisCount;k++)
		{
			if ((1 << k)&m_nAxisUsed&AxisNum&(~nInp)){

				lErrorID = Acm_AxGetMotionIO(m_Axishand[k], &Status);
				if (lErrorID != SUCCESS)
				{
					ErrorInfo(lErrorID);
					return FALSE;
				}
				if (m_bEStop || (Status & 0x3004E))
					FALSE;
				if ((Status & 0x06001)==0x6001)
					nInp|=1<<k;
			}
		}
		if (nInp == AxisNum)
			return TRUE;
	}
	return FALSE;
}

BOOL CAdvMotion::ResetAxisErr(UINT AxisNum)
{
	UINT i(0);
	USHORT usAxisState[4] = {0};
	USHORT State(0);
	SetEStop(FALSE);
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	//if axis is in error state , reset it first.
	m_CriticalCard.Lock();
	for (int j = 0; j < 2; j++)
	{
		if (m_GpHand[j]){
			Acm_GpGetState(m_GpHand[j], &State);
			if (State == STA_GP_ERROR_STOP)
			{
				Acm_GpResetError(m_GpHand[j]);
			}
		}
	}
	do
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_AxGetState(m_Axishand[i], &usAxisState[i]);
			//if axis is in error state , reset it 
			if (!(m_MotionStatus[AxisNum] >> 6 & 0x01) && usAxisState[i] == STA_AX_ERROR_STOP)
			{
				Acm_AxResetError(m_Axishand[i]);
			}
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	m_CriticalCard.Unlock();
	return TRUE;
}
BOOL CAdvMotion::SetAxSvOn(ULONG OnOff, UINT AxisNum)
{
	UINT i(0);
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	//if axis is in error state , reset it first. then Stop Axes
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			if (Acm_AxSetSvOn(m_Axishand[i], OnOff) != SUCCESS)
				return FALSE;
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}

BOOL CAdvMotion::SetCurrentPos(double pos,UINT AxisNum)
{
	UINT i(0);	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	//if axis is in error state , reset it first. then Stop Axes
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_AxSetCmdPosition(m_Axishand[i], pos);
			Acm_AxSetActualPosition(m_Axishand[i], pos);
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}

BOOL CAdvMotion::HardLimitConfig(UINT AxisNum,ULONG OnOff,ULONG HighLow)
{

	UINT i(0);	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_SetU32Property(m_Axishand[i], CFG_AxElEnable, OnOff);
			Acm_SetU32Property(m_Axishand[i], CFG_AxElLogic, HighLow);
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}

BOOL CAdvMotion::SoftLimitConfig(UINT AxisNum,ULONG OnOff,double PosLimit,double NegLimit)
{
	UINT i(0);	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_SetU32Property(m_Axishand[i], CFG_AxSwMelEnable, OnOff);
			Acm_SetU32Property(m_Axishand[i], CFG_AxSwPelEnable, OnOff);
			Acm_SetF64Property(m_Axishand[i], CFG_AxSwMelValue, NegLimit);
			Acm_SetF64Property(m_Axishand[i], CFG_AxSwPelValue, PosLimit);
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}

BOOL CAdvMotion::HomeConfig(UINT AxisNum,ULONG HighLow)
{
	UINT i(0);
	U32 lErrorID;
	if (AxisNum>m_ulAxisCount || (m_hDev == NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			lErrorID = Acm_SetU32Property(m_Axishand[i], CFG_AxOrgReact, HLMT_IMMED_STOP);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
			lErrorID = Acm_SetU32Property(m_Axishand[i], PAR_AxHomeExSwitchMode, EdgeOn);//上升沿
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}

			lErrorID = Acm_SetF64Property(m_Axishand[i], PAR_AxHomeCrossDistance, 5);//5mm
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
			lErrorID = Acm_SetU32Property(m_Axishand[i], CFG_AxHomeResetEnable, IN_ENABLE);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
			lErrorID = Acm_SetU32Property(m_Axishand[i], CFG_AxOrgLogic, HighLow);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
			lErrorID = Acm_SetU32Property(m_Axishand[i], CFG_AxEzLogic, EZ_ACT_LOW);
			if (lErrorID != SUCCESS)
			{
				ErrorInfo(lErrorID);
				return FALSE;
			}
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}
BOOL CAdvMotion::InpConfig(UINT AxisNum,ULONG HighLow)
{
	UINT i(0);	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE ;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	do 
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_SetU32Property(m_Axishand[i], CFG_AxInpEnable, INP_EN);
			Acm_SetU32Property(m_Axishand[i], CFG_AxInpLogic, HighLow);
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	return TRUE;
}
BOOL CAdvMotion::GetAxisError(UINT AxisNum)
{
	USHORT usAxisState[32];

	UINT	  i(0);	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL))
	{
		return FALSE;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
	}
	do
	{
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			Acm_AxGetState(m_Axishand[i], &usAxisState[i]);
			if (usAxisState[i] == STA_AX_ERROR_STOP)
			{
				return TRUE;
			}
			if (m_MotionStatus[i] & 0x2)
			{
				return TRUE;
			}
		}
		i++;
	}while ((i<m_ulAxisCount)&&(AxisNum==0));
	
	return FALSE;
}

BOOL CAdvMotion::GetSensorState(UINT AxisNum,Sensor sensor)
{	
	if (AxisNum>m_ulAxisCount||(m_hDev==NULL)||AxisNum==0)
	{
		return FALSE;
	} 
	switch (sensor)
	{
	case Alarm:
		if (m_MotionStatus[AxisNum - 1] & 0x02 || (m_AxisStatus[AxisNum - 1] == STA_AX_ERROR_STOP))
			return TRUE;
		break;
	case HomeSensor:
		if(m_MotionStatus[AxisNum-1] & 0x10)
			return TRUE;
		break;
	case PosSensor:
		if(m_MotionStatus[AxisNum-1]&0x4)
			return TRUE;
		break;
	case NegSensor:
		if(m_MotionStatus[AxisNum-1]&0x8)
			return TRUE;
		break;
	default:
		break;
	}
	return FALSE;
}

BOOL CAdvMotion::StartVMove(UINT AxisNum,double vel,UINT dir)
{
	double CfgAcVel=10*vel;
	double ProperJerk=0;
	U32 lErrorID;

	BOOL  bResult(FALSE);
	USHORT usAxisState=0;	
	if (AxisNum>m_ulAxisCount||(AxisNum==0)||(m_hDev==NULL)||m_bEStop)
	{
		return bResult;
	} 
	m_CriticalCard.Lock();
	lErrorID = Acm_AxGetState(m_Axishand[AxisNum - 1], &usAxisState);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return bResult;
	}
	if(usAxisState==STA_AX_READY)
	{
		lErrorID = lErrorID ? lErrorID : Acm_SetF64Property(m_Axishand[AxisNum - 1], PAR_AxVelHigh, vel);
		lErrorID = lErrorID ? lErrorID : Acm_SetF64Property(m_Axishand[AxisNum - 1], PAR_AxAcc, CfgAcVel);
		lErrorID = lErrorID ? lErrorID : Acm_SetF64Property(m_Axishand[AxisNum - 1], PAR_AxDec, CfgAcVel);
		
		lErrorID = Acm_AxMoveVel(m_Axishand[AxisNum - 1], dir);
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			return bResult;
		}
		ResetEvent(m_hMotionDone[AxisNum - 1]);
		bResult = TRUE;
	}
	m_CriticalCard.Unlock();
	return bResult;
}
BOOL CAdvMotion::AxStopDec(UINT AxisNum)
{
	UINT		i(0);
	BOOL		bSuccess(TRUE);
	if (AxisNum>(m_ulAxisCount+2)||(m_hDev==NULL))
	{
		return FALSE;
	} 
	else if (AxisNum>0)
	{
		i=AxisNum-1;
		if (i > 3)
		{
			if (m_GpHand[i - 4])
				if (Acm_GpStopDec(m_GpHand[i - 4]) != SUCCESS)
					bSuccess= FALSE;
			return bSuccess;
		}
	}
	do
	{
		if (i<2&& (AxisNum == 0)&&m_GpHand[i])
			if (Acm_GpStopDec(m_GpHand[i]) != SUCCESS)
				bSuccess=FALSE;
		if (!m_Axishand[i])
			break;
		if ((1 << i)&m_nAxisUsed){
			if (Acm_AxStopDec(m_Axishand[i]) != SUCCESS)
				bSuccess = FALSE;
		}
		i++;
	} while ((i < m_ulAxisCount) && (AxisNum == 0));

	return bSuccess;
}

BOOL CAdvMotion::AxisNormalHome(UINT AxisNum, BOOL nDireFlag, UINT nHomeMode)
{
	CString strext;
	UINT i(0);
	UINT delaytime(100);
	U32 lErrorID;
	USHORT  AxisStatus[4] = { 0 };
	if (!AxisNum)
		return FALSE;
	nHomeMode = nHomeMode > 15 ? MODE16_LmtSearchReFind_Ref : nHomeMode;//Search ORG +Refind ORG+NegEZ, Move(Dir)->Search ORG->Stop->Move(-Dir)->Leave ORG(FL)->Stop->Move(-Dir)->Refind ORG(FL)->Stop->Move(-Dir)->touch EZ->Stop.
	for (i = 0; i < m_ulAxisCount; i++)
	{
		if (AxisNum & (1 << i))
		{
			double VelLow = m_dHomeVel[i] / 3;
			Acm_SetF64Property(m_Axishand[i], PAR_AxVelLow, VelLow);
			Acm_SetF64Property(m_Axishand[i], PAR_AxVelHigh, m_dHomeVel[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxAcc, m_dHomeAcc[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxDec, m_dHomeAcc[i]);


			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeVelLow, VelLow);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeVelHigh, m_dHomeVel[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeAcc, m_dHomeAcc[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeDec, m_dHomeAcc[i]);
			if (m_bEStop)
			{
				AxStopEmg();
				m_ErrMsg = _T("被强制停止\n");
				return FALSE;
			}
			lErrorID = Acm_AxHome(m_Axishand[i], nHomeMode, (~nDireFlag >> i) & 0x01);
			if (lErrorID != SUCCESS)
			{
				strext.Format(_T("%d轴标准回零模式失败"), i);
				ErrorInfo(lErrorID, strext);
				AxStopEmg();
				return FALSE;
			}
		}
	}

	BOOL bDone = FALSE;
	while (!bDone&&delaytime)
	{
		bDone = TRUE;
		for (i = 0; i < m_ulAxisCount; i++)
		{
			if (AxisNum & (1 << i))
			{
				Acm_AxGetState(m_Axishand[i], &AxisStatus[i]);

				if (AxisStatus[i] == STA_AX_ERROR_STOP)
				{
					m_ErrMsg = _T("停止,回零出错!\n");
					AxStopEmg();
					return FALSE;
				}
				if (bDone && AxisStatus[i] > STA_AX_ERROR_STOP)
					bDone = FALSE;
			}
		}
		Sleep(250);
		delaytime--;
	}
	Sleep(1000);
	return TRUE;
}

int CAdvMotion::AxisHSPHome(USHORT AxisNum, UINT nDireFlag, UINT nHomeMode)//
{
	UINT nFind(0);
	UINT nCurrentDir = nDireFlag;
	UINT num = _mm_popcnt_u32(AxisNum);
	num = num > m_ulAxisCount ? m_ulAxisCount : num;
	UINT i(0);
	U32 lErrorID;
	double VelL = 0.1;//relation with PPU
	ULONG	lStatus[4] = {0};
	USHORT  AxisStatus[4] = { 0 };
	CString strext;
	//***************set param and move************
	for (i = 0; i<m_ulAxisCount; i++)
	{
		if (AxisNum & (1 << i))
		{
			VelL = m_dHomeVel[i] / 10;
 			Acm_SetF64Property(m_Axishand[i], PAR_AxVelLow, VelL);
 			Acm_SetF64Property(m_Axishand[i], PAR_AxVelHigh, m_dHomeVel[i]);
 			Acm_SetF64Property(m_Axishand[i], PAR_AxAcc, m_dHomeAcc[i]);
 			Acm_SetF64Property(m_Axishand[i], PAR_AxDec, m_dHomeAcc[i]);

			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeVelLow, VelL);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeVelHigh, m_dHomeVel[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeAcc, m_dHomeAcc[i]);
			Acm_SetF64Property(m_Axishand[i], PAR_AxHomeDec, m_dHomeAcc[i]);
			if (m_bEStop)
			{
				AxStopEmg();
				m_ErrMsg = _T("被强制停止\n");
				return FALSE;
			}
			lErrorID = Acm_AxMoveVel(m_Axishand[i], (~nDireFlag>>i)&0x01);
			if (lErrorID != SUCCESS)
			{
				AxStopDec();
				strext.Format(_T("%d轴回零启动运行:"),i);
				ErrorInfo(lErrorID,strext);
				return FALSE;
			}
		}
	}
	//*****************check home and stop************
	BOOL bHomeSensor = FALSE;
	BOOL bPosLimit = FALSE;
	BOOL bNegLimit = FALSE;
	while (num)
	{
		for (i = 0; i<m_ulAxisCount; i++)
		{
			if (m_bEStop)
			{
				AxStopEmg();
				m_ErrMsg = _T("被强制停止\n");
				return FALSE;
			}
			if ((AxisNum & (1 << i)) && !(nFind & (1 << i)))
			{
				Acm_AxGetMotionIO(m_Axishand[i], &lStatus[i]);
				bHomeSensor = lStatus[i] & 0x10;
				bPosLimit = lStatus[i] & 0x4;
				bNegLimit = lStatus[i] & 0x8;
				//**************************HomeSensor:
				if (bHomeSensor)
				{
					Acm_AxStopEmg(m_Axishand[i]);
					nFind |= (1 << i);
					num--;
				}
				else{
					//**************************PosSensor or NegSensor then inverse:
					if (bPosLimit&&bNegLimit)
					{
						AxStopEmg();
						m_ErrMsg = _T("正负限位同时触发\n");
						return FALSE;
					}
					if (bPosLimit && ((nCurrentDir >> i) & 0x01))//PosSensor
					{
						Acm_AxStopEmg(m_Axishand[i]);
						Sleep(500);
						Acm_AxGetState(m_Axishand[i], &AxisStatus[i]);
						if (AxisStatus[i] > STA_AX_ERROR_STOP)
						{
							m_ErrMsg = _T("正限位触发却未停止\n");
							AxStopEmg();
							return FALSE;
						}
						Acm_AxResetError(m_Axishand[i]);

						lErrorID = Acm_AxMoveVel(m_Axishand[i], DIR_NEGATIVE);
						if (lErrorID != SUCCESS)
						{
							strext.Format(_T("%d轴回零移动 DIR_NEGATIVE:"), i);
							ErrorInfo(lErrorID, strext);
							AxStopEmg();
							return FALSE;
						}
						nCurrentDir &=~(1 << i);
					}
					if (bNegLimit && ((~nCurrentDir >> i) & 0x01))//NegSensor
					{
						Acm_AxStopEmg(m_Axishand[i]);
						Sleep(500);
						Acm_AxGetState(m_Axishand[i], &AxisStatus[i]);
						if (AxisStatus[i] > STA_AX_ERROR_STOP)
						{
							m_ErrMsg = _T("触发负限位未停止\n");
							AxStopEmg();
							return FALSE;
						}
						Acm_AxResetError(m_Axishand[i]);

						lErrorID = Acm_AxMoveVel(m_Axishand[i], DIR_POSITIVE);
						if (lErrorID != SUCCESS)
						{
							strext.Format(_T("%d轴回零移动 DIR_POSITIVE:"), i);
							ErrorInfo(lErrorID,strext);
							AxStopEmg();
							return FALSE;
						}
						nCurrentDir |= 1 << i;
					}
				}
			}
		}
	}
	Sleep(500);
	
	for (i = 0; i<m_ulAxisCount; i++)
	{
		if (AxisNum & (1 << i))
		{
			nHomeMode = nHomeMode > 15 ? MODE15_AbsSearchReFind_NegRef : nHomeMode;//Search ORG +Refind ORG+NegEZ, Move(Dir)->Search ORG->Stop->Move(-Dir)->Leave ORG(FL)->Stop->Move(-Dir)->Refind ORG(FL)->Stop->Move(-Dir)->touch EZ->Stop.
			lErrorID = Acm_AxHome(m_Axishand[i], nHomeMode, (~nDireFlag >> i) & 0x01);
			if (lErrorID != SUCCESS)
			{
				strext.Format(_T("%d轴回零标准模式失败:"), i);
				ErrorInfo(lErrorID,strext);
				AxStopEmg();
				return FALSE;
			}
		}
	}
	num = 600;
	BOOL bDone = FALSE;
	while (!bDone&&num)
	{
		bDone = TRUE;
		for (i = 0; i < m_ulAxisCount; i++)
		{
			if (AxisNum & (1 << i))
			{
				Acm_AxGetState(m_Axishand[i], &AxisStatus[i]);

				if (AxisStatus[i] == STA_AX_ERROR_STOP)
				{
					m_ErrMsg = _T("回零出错,停止\n");
					AxStopEmg();
					return FALSE;
				}
				if (bDone && AxisStatus[i] > STA_AX_ERROR_STOP)
					bDone = FALSE;
			}
		}
		Sleep(100);
		num--;
	}
	Sleep(150);
	if (!m_bEStop)
		ResetAxisErr();

	return TRUE;
}

BOOL CAdvMotion::ChangePos(UINT AxisNum,double pos)
{
	if (!AxisNum || AxisNum > m_ulAxisCount)
		return FALSE;
	if (Acm_AxChangePos (m_Axishand[AxisNum-1],pos)==SUCCESS)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CAdvMotion::ChangeVel(UINT AxisNum,double vel)
{
	if (!AxisNum || AxisNum > m_ulAxisCount)
		return FALSE;
	if (Acm_AxChangeVel(m_Axishand[AxisNum - 1], vel) == SUCCESS)
	{
		return TRUE;
	}
	return FALSE;
}

void CAdvMotion::ErrorInfo(LONG lErrorID, CString ext_info)
{
	char szErrorInfo[100];
	Acm_GetErrorMessage(lErrorID,szErrorInfo,99);
	m_ErrMsg = szErrorInfo;
	m_ErrMsg = ext_info + m_ErrMsg+_T("\n");

}

BOOL CAdvMotion::AddRuninTo(UINT nGp, TrackPoint& tPoint, BOOL bImd /*= FALSE*/)
{
	U32 lErrorID;
	U32 ArrayElementCnt = 2;
	U16 MoveCmd;
	if (m_GpHand[nGp])
		MoveCmd = Abs2DLine;
	else
		return FALSE;
	if (bImd){
		lErrorID = Acm_GpMoveLinearAbs(m_GpHand[nGp], tPoint.EndPoint, &ArrayElementCnt);
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			return FALSE;
		}
		ResetEvent(m_hMotionDone[4+nGp]);
	}
	else
	{
		lErrorID = Acm_GpAddPath(m_GpHand[nGp], MoveCmd, 0, tPoint.Vel / 10, tPoint.Vel, tPoint.EndPoint, NULL, &ArrayElementCnt);
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CAdvMotion::RuninConfig(UINT nGp, double dVel, double dAcc)
{
	double dVellow = dVel / 10;
	U32 lErrorID;
	if (!m_GpHand[nGp])
		return FALSE;
	lErrorID = Acm_SetF64Property(m_GpHand[nGp], PAR_GpVelLow, dVellow);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	lErrorID = Acm_SetF64Property(m_GpHand[nGp], PAR_GpVelHigh, dVel);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	lErrorID = Acm_SetF64Property(m_GpHand[nGp], PAR_GpAcc, dAcc);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	lErrorID = Acm_SetF64Property(m_GpHand[nGp], PAR_GpDec, dAcc);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	return TRUE;
}

BOOL CAdvMotion::RuninMove(UINT nGp, USHORT uType /*= TRUE*/)
{
	U16 State;
	U32 lErrorID;
	U32 ArrayElementCnt = 2;
	switch (uType)
	{
	case 0://恢复
		lErrorID = Acm_GpResumeMotion(m_GpHand[nGp]);
		break;
	case 1://开始
		lErrorID = Acm_GpAddPath(m_GpHand[nGp], EndPath, 0, 0, 0, 0, NULL, &ArrayElementCnt);
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			return FALSE;
		}
		lErrorID = Acm_GpMovePath(m_GpHand[nGp], NULL); //启动运动
		ResetEvent(m_hMotionDone[4+nGp]);

		Sleep(100);
		break;
	case 2://暂停
		lErrorID = Acm_GpPauseMotion(m_GpHand[nGp]);
		break;
	case 3://停止并清空
		Acm_GpGetState(m_GpHand[nGp], &State);
		if (State == STA_GP_ERROR_STOP)
		{
			Acm_GpResetError(m_GpHand[nGp]);
		}
		lErrorID = Acm_GpStopDec(m_GpHand[nGp]);
		lErrorID = Acm_GpResetPath(&m_GpHand[nGp]);
		break;
	}
	return TRUE;
}

void CAdvMotion::SetUnitPulse(UINT AxisNum, UINT nPulse)
{
	UINT i(0);
	U32 lErrorID;
	if (AxisNum>m_ulAxisCount || m_hDev == NULL)
	{
		return ;
	}
	else if (AxisNum > 0)
	{
		i = AxisNum - 1;
	}
	//设置单位脉冲数
	do
	{
		if (!m_Axishand[i])
			return;
		lErrorID = Acm_SetU32Property(m_Axishand[i], CFG_AxPPU,nPulse);
		if (lErrorID != SUCCESS)
		{
			ErrorInfo(lErrorID);
			break;
		}
		m_nAxisResolution[i] = nPulse;
		++i;
	} while (i < m_ulAxisCount && AxisNum == 0);
}

BOOL CAdvMotion::LoadConfig(CString CfgPath)
{
	U32 lErrorID;
	char* CfgFile;
	if (!m_hDev)
		return FALSE;
#ifdef UNICODE
	USES_CONVERSION;
	CfgFile = T2A(CfgPath);
#else
	CfgFile = CfgPath;
#endif
	lErrorID = Acm_DevLoadConfig(m_hDev, CfgFile);
	if (lErrorID != SUCCESS)
	{
		ErrorInfo(lErrorID);
		return FALSE;
	}
	return TRUE;
}




