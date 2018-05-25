// AdvMotion.h: interface for the CAdvMotion class.
//研华运动控制卡
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADVMOTION_H__30A442D1_47C8_4FE6_BA2D_99B3C43D19AE__INCLUDED_)
#define AFX_ADVMOTION_H__30A442D1_47C8_4FE6_BA2D_99B3C43D19AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AdvMotApi.h"
#include "stdafx.h"


#define 		ALL_AXIS  						0
#define 		X__AXIS  						1
#define 		XY_AXIS  						3
#define 		XYZ_AXIS  						7
#define 		XYZU_AXIS  						15
#define			MAX_AXIS						4

enum Sensor
{
	Alarm,
	HomeSensor = 1,
	PosSensor,
	NegSensor,
};

class CAdvMotion
{
	DEVLIST		m_AvaDevs[4];	// 可用卡信息：编号，名称
	DWORD		m_dwDevNum;	//设备卡数量
	HAND		m_hDev;				// 设备卡句柄
	HAND		m_Axishand[MAX_AXIS];			// 轴句柄
	HAND		m_GpHand[2];
public:

	UINT		m_nAxisUsed;
	CWnd*		m_pWnd;
	HANDLE		m_hEventArray[2];
	HANDLE		m_hMotionDone[6];//[4]--Gp1；[5]--Gp2
	HANDLE		m_hEventStart;
	HANDLE		m_hEventEnd;
	BOOL		m_bInit;
	BOOL		m_bThreadAlive;
	CCriticalSection	m_CriticalCard;
public:
	bool		m_bEStop;
	UINT		m_ulAxisCount;			//轴数
	UINT		m_nAxisResolution[MAX_AXIS];	//PPU  :mm or μ单位脉冲数>=1
	double		m_dAxisCurPos[MAX_AXIS];		//各轴位置存储
	double		m_dHomeVel[MAX_AXIS];
	double      m_dHomeAcc[MAX_AXIS];
	ULONG		m_MotionStatus[MAX_AXIS];
	USHORT		m_AxisStatus[MAX_AXIS];
	BOOL		m_bAxisMotionDone[MAX_AXIS];

	UINT		m_nCardID;
	ULONG		m_GpID[2];
 	CString		m_ErrMsg;
	CWinThread* m_Thread;
public:
	CAdvMotion(int nID);
	virtual ~CAdvMotion();
	BOOL InitBoard(ULONG DeviceNumber, UINT AxisUsed=4);//<4
	void SetEStop(bool Estop=true);
	void SetUnitPulse(UINT AxisNum, UINT nPulse);
	BOOL CreateGroup(UINT AxisNum, UINT nGp);
	BOOL ModifyGroup(BYTE nGp, BYTE AxisNum, BYTE bAdd);
	BOOL CloseCard();

	BOOL StartVMove(UINT AxisNum,double vel,UINT dir);
	BOOL Move(UINT AxisNum, double dEnd, double dVel, double dAcc, BOOL bAbs = TRUE);//1:x;2:y…………
	BOOL AddRuninTo(UINT nGp, TrackPoint& tPoint, BOOL bImd = FALSE);//添加直线插补段
	BOOL RuninMove(UINT nGp, USHORT uType = TRUE);
	BOOL AxStopDec(UINT AxisNum = ALL_AXIS);
	void AxStopEmg(UINT AxisNum=ALL_AXIS);////AxisNum is zero for all Axis
	int	 GetCurPos(UINT AxisNum,double &dPosition);
	BOOL CheckMotionDone(UINT nWaitTime = 5000);
	BOOL CheckMotionDone(BYTE AxisNum ,UINT nWaitTime);

	BOOL RuninConfig(UINT nGp, double dVel, double dAcc);//插补速度配置
	BOOL ResetAxisErr(UINT AxisNum = ALL_AXIS);
	BOOL SetAxSvOn(ULONG OnOff, UINT AxisNum = ALL_AXIS);
	BOOL SetCurrentPos(double pos,UINT AxisNum=0);
	BOOL LoadConfig(CString CfgPath);
	BOOL HardLimitConfig(UINT AxisNum = 0, ULONG OnOff = 1, ULONG HighLow = 1);
	BOOL SoftLimitConfig(UINT AxisNum,ULONG OnOff,double PosLimit,double NegLimit);
	BOOL HomeConfig(UINT AxisNum,ULONG HighLow);
	BOOL InpConfig(UINT AxisNum,ULONG HighLow);
	BOOL GetAxisError(UINT AxisNum);
	BOOL GetSensorState(UINT AxisNum,Sensor sensor);

	int  AxisHSPHome(USHORT AxisNum, UINT nDireFlag, UINT nHomeMode = MODE15_AbsSearchReFind_NegRef);//Save time
	BOOL AxisNormalHome(UINT AxisNum, BOOL bDireFlag, UINT nHomeMode = MODE16_LmtSearchReFind_Ref);
	BOOL StartMotionCardMonitor(CWnd* pWnd);
	BOOL StopThread();
	void SuspendThread();
	void ResumeThread();
protected:
	static UINT _cdecl MotionThread(LPVOID pParam);
	BOOL GetMotionStatus(ULONG &lChangeAxis, ULONG& lError, ULONG& lMotorSenser);
	BOOL GetAxisStatus(UINT AxisNum);
	BOOL ChangeVel(UINT AxisNum,double vel);//运动过程中改变速度。
	BOOL ChangePos(UINT AxisNum,double pos);//运动过程中改变p-p点。
	void ErrorInfo(LONG lErrorID,CString ext_info=_T(""));				//错误信息

};

#endif // !defined(AFX_ADVMOTION_H__30A442D1_47C8_4FE6_BA2D_99B3C43D19AE__INCLUDED_)
