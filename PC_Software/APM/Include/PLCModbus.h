#pragma once
#include "modbus.h"
class CPLCModbus
{
	modbus_t *m_mb;
	int m_nID;
	LONG m_nDataAddress[2];
	CCriticalSection m_CriticalObj;
	BOOL CoilRW(UINT nIndex, UCHAR* pSetvalue, UINT nCount = 1, UINT nMode = 0);
	BOOL RegisterRW(UINT nIndex, USHORT* pSetvalue, UINT nCount = 1, UINT nMode = 0);
	enum { READ = 0, ONLY_READ, WRIT };
public:
	CPLCModbus();
	~CPLCModbus();
	BOOL m_bInitOk;
	BOOL m_bConnectOk;
	BOOL Initport(UINT nPort, int baud, char parity,int data_bit);
	int Openport();
	void Closeport();
	void SetDeviceID(UINT nId = 1);
	void SetDataAddress(BYTE bType,LONG lAddress);
	//状态 线圈
	BOOL ResetStatus();
	BOOL SetBallRDY(BOOL bOn=TRUE);
	BOOL SetTriggerCleanMode();
	BOOL IsHoming();
	BOOL IsHomed();
	BOOL SetbondServoRDY(BOOL bEnable = TRUE);
	BOOL GetLaserRDY();
	BOOL GetPLCRDY();
	BOOL SetLaserRDY(BOOL bEnable = TRUE);
	BOOL IsEnable();
	BOOL IsErroy();
	BOOL IsPowering();
	BOOL IsLasering();
	BOOL IsWarning();

	//参数 寄存器
	UINT GetAlarm();
	BOOL SetNGBallNum(BYTE nNum);
	BYTE GetNGBallNum();
	BOOL GetCleanPower(FLOAT &fpower);
	BOOL SetCleanPower(FLOAT fpower);

	FLOAT GetPower(UINT nIndex=0);
	BOOL SetPower(FLOAT fpower, UINT nIndex=0);

	FLOAT GetPowerTime(UINT nIndex = 0);
	BOOL SetPowerTime(FLOAT ftime, UINT nIndex = 0);
	UINT GetDroppingTime(UINT nIndex = 0);
	BOOL SetDroppingTime(UINT ntime, UINT nIndex = 0);
	UINT GetLaserPressure(UINT nIndex = 0);
	BOOL SetLaserPressure(UINT nNumber, UINT nIndex=0);

	UINT GetDetectPressure(UINT nIndex);
	BOOL SetDetectPressure(UINT nNumber,UINT nIndex=0 );

	BOOL GetConfigList(FLOAT& nPower, FLOAT& Pressure, UINT& ndetect_up, UINT& ndetect_low);//410-418
	BOOL SetConfigList(FLOAT nPower, FLOAT Pressure, UINT ndetect_up, UINT ndetect_low);//410-418

	UINT GetTotalBalls(UINT nIndex = 0);
	UINT GetCurBalls(UINT nIndex = 0);
	/*
	//过时
	BOOL GetTestModeSts();
	BOOL SetTriggerTestMode(BOOL bEnable = TRUE);
	BOOL GetControllerRDY();
	BOOL SetControllerRDY(BOOL bEnable = TRUE);
	BOOL IsHomingTriggerSts();
	BOOL GetN2RDY();
	BOOL SetN2RDY(BOOL bEnable = TRUE);
	UINT GetN2Num();
	BOOL GetBondRDY();
	BOOL SetTriggerLaser();
	BOOL GetTriggerSts();
	BOOL SetHomingTrigger(BOOL bOn);

	BOOL GetControlMode();
	BOOL SetControlMode(BOOL bInner);
	UINT GetCurrentGp();
	BOOL SetCurrentGP(UINT nIndex);
	BOOL SetTriggerTestNum(UINT nCount = 1);
	UINT GetDelayTime(UINT nIndex);
	BOOL SetDelayTime(UINT nIndex, UINT ndtime);
	*/

protected:
};

