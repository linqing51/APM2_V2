// CMachineParam.cpp: implementation of the CProjectParam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineParam.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMachineParam::CMachineParam()
{
	int i(0);
	SecureZeroMemory(&PrjCfg, sizeof(PrjCfg));
	ZeroMemory(dCCD_Nozzle, sizeof(dCCD_Nozzle));
	ZeroMemory(nNozzleLife, sizeof(nNozzleLife));
	//-Input
	In_StartLBtn = 0;//1
	In_StartRBtn = 1;//1
	In_StopBtn = 2;//2
	In_GS_AUX = 3;//安全光栅
	In_GS_FSD1 = 4;//安全光栅
	In_GS_FSD2 = 5;//安全光栅

	In_Gate_Open = 6;//前，后，右，左
	for (i = 0; i < 4; i++)
	{
		In_JL_Status[i] = 7 + i;//距离传感器0:高判; 1:低判; 2:判定输出; 3:报警;
		In_Cyder[i] = 11 + i;//气缸传感器
		In_PLC_Input[i] = 16 + i;//PLC 信号
		In_PLC_Input[4 + i] = 20 + i;//PLC 信号
	}

	//-Output
	Ou_ForceEnable = 0;
	for (i = 0; i < 4; i++)
	{
		Ou_Tower[i]=i;//灯塔：0-红；1-黄；2-绿；3-鸣
		Ou_DCF[i] = 4+i;//电磁阀
		Ou_BCD[i] = 8+i;//参数位标识
		if (i < 3)
		{
			Ou_Panel_lamp[i] = 18+i;//左绿，右绿，红灯
			Ou_Driver_power[i] =21+ i;//驱动器电源
		}
		Ou_Welding[i] = 27+i;//bond头控制命令：0：手自动；1：回零；2：出激光；3：红光；
	}
	Ou_BCD[4] = 12;//参数位标识
	Ou_Trigger = 13;//出球打光
	Ou_EMGPower = 24;//光栅复位
	Ou_JL_CMD[0] = 25;//距离传感器，0零漂；1复位；
	Ou_JL_CMD[1] = 26;//距离传感器，0零漂；1复位；
	Ou_Welding[4] = 31;//bond头控制命令：0：手自动；1：回零；2：出激光；3：红光；
	for (i = 0; i < 8; ++i)
	{
		HOME_VEL[i] = 25.010;
		HOME_ACC[i] = 100.040;
		if (i < 2)
		{
			PrjCfg.nExposure[i] = 100;
			PrjCfg.bProductNum[i] = 1;
		}
		PrjCfg.SpeedCfg.Run_AU_VEL[i] = 100;
		PrjCfg.SpeedCfg.Run_MU_VEL[i] = 50;
		PrjCfg.SpeedCfg.Run_ACC[i] = 1000;
	}
	nEnabeSafetyDev = 3;
	PrjCfg.uInspection = 1;
	PrjCfg.nPinNum = 1;
	//X_Y_Axis	
	nLightLimit[0] = 255;
	nLightLimit[1] = 6;
	nLightLimit[2] = 6;
	BallAlarmNum = 5;
	nComPort[0] = 1;
	nComPort[1] = 3;
	DefaultProjectName = _T("DEFAULTS");
	PrjCfg.Magic[0] = 12345678;
	PrjCfg.Magic[1] = 87654321;
	nBondHomeTime = 60;
	nDroppingTime[0] = 1000;
	nDroppingTime[1] = 100;

	nLaserSpan = 500;
}

CMachineParam::~CMachineParam()
{

}

BOOL CMachineParam::LoadParameter(CString strFileName,BOOL bLoadSave)
{
	CReadWriteIni m_nFileIni;
	m_nFileIni.m_szFileName = strFileName;
	int i(0);
	CString str;
	static CString	IOString[2][32] = { _T("左启动按钮"), _T("右启动按钮"), _T("停止按钮"), _T("AUX"), _T("FSD1"), _T("FSD2"), _T("安全门"), _T("距离高判"),
		_T("距离低判"), _T("距离判定中"), _T("距离报警"), _T("气缸1"), _T("气缸2"), _T("气缸3"), _T("气缸4"), _T("PLC输入1"),
		_T("PLC输入2"), _T("PLC输入3"), _T("PLC输入4"), _T("PLC输入5"), _T("PLC输入6"), _T("PLC输入7"), _T("PLC输入8"), _T("ReserveI23"), _T("ReserveI24"), _T("ReserveI25"),
		_T("ReserveI26"), _T("ReserveI27"), _T("ReserveI28"), _T("ReserveI29"), _T("ReserveI30"), _T("ReserveI31"), 
		
		_T("灯塔红"), _T("灯塔黄"), _T("灯塔绿"), _T("蜂鸣"), _T("电磁阀1"), _T("电磁阀2"), _T("电磁阀3"),_T("电磁阀4"),
		_T("焊接触发"), _T("BCD1"), _T("BCD2"), _T("BCD3"), _T("BCD4"), _T("BCD5"), _T("左绿灯"), _T("右绿灯"),
		_T("停止灯"), _T("伺服电源1"), _T("伺服电源2"), _T("伺服电源3"), _T("光栅复位"), _T("距离零漂"), _T("距离复位"), _T("手自动模式"),
		_T("碟片回零"), _T("出光"),_T("红光"), _T("保留命令"), _T("屏蔽光栅"), _T("ReserveO29"), _T("ReserveO30"), _T("ReserveO31")
	};

	if (bLoadSave)//Load
	{
		m_nFileIni.m_szTitle = _T("------Main------");
		DefaultProjectName = m_nFileIni.GetString(_T("Defaults Project"));
		nComPort[0] = m_nFileIni.GetInt(_T("LaserCtrlPort"));
		nComPort[1] = m_nFileIni.GetInt(_T("LightCtrlPort"));
		nEnabeSafetyDev = m_nFileIni.GetInt(_T("Safety Device"));
		nNozzleLife[0] = m_nFileIni.GetInt(_T("Nozzle Current Life"));
		nNozzleLife[1] = m_nFileIni.GetInt(_T("Nozzle Total Life"));
		BallAlarmNum = m_nFileIni.GetInt(_T("Ball Supply Alarm"));
		nLaserSpan = m_nFileIni.GetInt(_T("打球间隔时间"));
		nBondHomeTime = m_nFileIni.GetInt(_T("碟片复位等待时间"));
		nDroppingTime[0] = m_nFileIni.GetInt(_T("落球等待时间"));
		nDroppingTime[1] = m_nFileIni.GetInt(_T("降压等待时间"));
		if (nLaserSpan > 5000 || !nLaserSpan)
			nLaserSpan = 500;
		if (nBondHomeTime > 300 || !nBondHomeTime)
			nBondHomeTime = 60;
		if (nDroppingTime[0] > 5000 || !nDroppingTime[0])
			nDroppingTime[0] = 500;
		if (nDroppingTime[1] > 1000 || !nDroppingTime[1])
			nDroppingTime[1] = 100;
		for (i = 0; i < 8; ++i)
		{
			str.Format(_T("Home_Vel_%c"), 65 + i);
			HOME_VEL[i] = m_nFileIni.GetDouble(str);
			str.Format(_T("Home_Acc_%c"), 65 + i);
			HOME_ACC[i] = m_nFileIni.GetDouble(str);
		}
		for (i = 0; i < 3; ++i)
		{
			str.Format(_T("CCD-Nozzle_Offset%c"), 88 + i);
			dCCD_Nozzle[i] = m_nFileIni.GetDouble(str);
			str.Format(_T("LightLimit<%d>"), i);
			nLightLimit[i] = m_nFileIni.GetInt(str);
		}

		//-Input
		m_nFileIni.m_szTitle = _T("------InputIO------");

		In_StartLBtn = m_nFileIni.GetInt(IOString[0][0]);
		In_StartRBtn = m_nFileIni.GetInt(IOString[0][1]);
		In_StopBtn = m_nFileIni.GetInt(IOString[0][2]);
		In_GS_AUX = m_nFileIni.GetInt(IOString[0][3]);
		In_GS_FSD1 = m_nFileIni.GetInt(IOString[0][4]);
		In_GS_FSD2 = m_nFileIni.GetInt(IOString[0][5]);
		In_Gate_Open = m_nFileIni.GetInt(IOString[0][6]);
		for (i = 0; i < 4; ++i)
		{
			In_JL_Status[i] = m_nFileIni.GetInt(IOString[0][7+i]);
			In_Cyder[i] = m_nFileIni.GetInt(IOString[0][11+i]);
			In_PLC_Input[i] = m_nFileIni.GetInt(IOString[0][15 + i]);
			In_PLC_Input[4+i] = m_nFileIni.GetInt(IOString[0][19 + i]);
		}
		//-Output
		m_nFileIni.m_szTitle = _T("------OutputIO------");
		Ou_ForceEnable = m_nFileIni.GetInt(IOString[1][28]);
		for (i = 0; i < 4; i++)
		{
			Ou_Tower[i] = m_nFileIni.GetInt(IOString[1][i]);//灯塔：0-红；1-黄；2-绿；3-鸣
			Ou_DCF[i] = m_nFileIni.GetInt(IOString[1][i+4]);//电磁阀
			Ou_BCD[i] = m_nFileIni.GetInt(IOString[1][i+9]);//参数位标识
			if (i < 3)
			{
				Ou_Panel_lamp[i] = m_nFileIni.GetInt(IOString[1][i+14]);//左绿，右绿，红灯
				Ou_Driver_power[i] = m_nFileIni.GetInt(IOString[1][i+17]);//驱动器电源
			}
			Ou_Welding[i] = m_nFileIni.GetInt(IOString[1][i+23]);//bond头控制命令：0：手自动；1：回零；2：出激光；3：红光；
		}
		Ou_BCD[4] = m_nFileIni.GetInt(IOString[1][13]);//参数位标识

		Ou_Trigger = m_nFileIni.GetInt(IOString[1][8]);//出球打光
		Ou_EMGPower = m_nFileIni.GetInt(IOString[1][20]);//光栅复位
		Ou_JL_CMD[0] = m_nFileIni.GetInt(IOString[1][21]);//距离传感器，0零漂；1复位；
		Ou_JL_CMD[1] = m_nFileIni.GetInt(IOString[1][22]);//距离传感器，0零漂；1复位；
		Ou_Welding[4] = m_nFileIni.GetInt(IOString[1][27]);//bond头控制命令：0：手自动；1：回零；2：出激光；3：红光；
	}
	else{//Save
		m_nFileIni.m_szTitle = _T("------Main------");
		m_nFileIni.SetValue(DefaultProjectName, _T("Defaults Project"));
		m_nFileIni.SetValue(nComPort[0], _T("LaserCtrlPort"));
		m_nFileIni.SetValue(nComPort[1], _T("LightCtrlPort"));
		m_nFileIni.SetValue(nEnabeSafetyDev, _T("Safety Device"));
		m_nFileIni.SetValue(nNozzleLife[0],_T("Nozzle Current Life"));
		m_nFileIni.SetValue(nNozzleLife[1],_T("Nozzle Total Life"));
		m_nFileIni.SetValue(BallAlarmNum, _T("Ball Supply Alarm"));
		m_nFileIni.SetValue(nLaserSpan, _T("打球间隔时间"));
		m_nFileIni.SetValue(nBondHomeTime, _T("碟片复位等待时间"));
		m_nFileIni.SetValue(nDroppingTime[0], _T("落球等待时间"));
		m_nFileIni.SetValue(nDroppingTime[1], _T("降压等待时间"));

		for (i = 0; i < 8; i++)
		{
			str.Format(_T("Home_Vel_%c"), 65 + i);
			m_nFileIni.SetValue(HOME_VEL[i], str);
		}
		for (i = 0; i < 8; i++)
		{
			str.Format(_T("Home_Acc_%c"), 65 + i);
			m_nFileIni.SetValue(HOME_ACC[i], str);
		}
		for (i = 0; i < 3; ++i)
		{
			str.Format(_T("CCD-Nozzle_Offset%c"), 88 + i);
			m_nFileIni.SetValue(dCCD_Nozzle[i], str);
		}
		for (i = 0; i < 3; ++i)
		{
			str.Format(_T("LightLimit<%d>"), i);
			m_nFileIni.SetValue(nLightLimit[i], str);
		}

		m_nFileIni.m_szTitle = _T("------InputIO------");
		m_nFileIni.SetValue(In_StartLBtn, IOString[0][0]);
		m_nFileIni.SetValue(In_StartRBtn, IOString[0][1]);
		m_nFileIni.SetValue(In_StopBtn, IOString[0][2]);
		m_nFileIni.SetValue(In_GS_AUX, IOString[0][3]);
		m_nFileIni.SetValue(In_GS_FSD1, IOString[0][4]);
		m_nFileIni.SetValue(In_GS_FSD2, IOString[0][5]);
		m_nFileIni.SetValue(In_Gate_Open, IOString[0][6]);
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(In_JL_Status[i], IOString[0][7 + i]);
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(In_Cyder[i], IOString[0][11 + i]);
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(In_PLC_Input[i], IOString[0][15 + i]);
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(In_PLC_Input[4 + i], IOString[0][19 + i]);
		m_nFileIni.m_szTitle = _T("------OutputIO------");
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(Ou_Tower[i], IOString[1][i]);
		for (i = 0; i < 4; ++i)
			m_nFileIni.SetValue(Ou_DCF[i], IOString[1][i + 4]);
		for (i = 0; i < 5; ++i)
			m_nFileIni.SetValue(Ou_BCD[i], IOString[1][i + 9]);
		for (i = 0; i < 3; ++i)
			m_nFileIni.SetValue(Ou_Panel_lamp[i], IOString[1][14 + i]);
		for (i = 0; i < 3; ++i)
			m_nFileIni.SetValue(Ou_Driver_power[i], IOString[1][17 + i]);

		m_nFileIni.SetValue(Ou_Trigger, IOString[1][8]);
		m_nFileIni.SetValue(Ou_EMGPower, IOString[1][20]);
		m_nFileIni.SetValue(Ou_JL_CMD[0], IOString[1][21]);
		m_nFileIni.SetValue(Ou_JL_CMD[1], IOString[1][22]);
		for (i = 0; i < 5; ++i)
			m_nFileIni.SetValue(Ou_Welding[i], IOString[1][23+i]);
		m_nFileIni.SetValue(Ou_ForceEnable, IOString[1][28]);
	}
	return TRUE;
}

BOOL CMachineParam::CheckData()
{
	BOOL bSuccess(TRUE);

	if (!PrjCfg.nPinNum)
	{
		PrjCfg.nPinNum = 1;
		bSuccess = FALSE;
	}
	if (!PrjCfg.uInspection)
	{
		PrjCfg.uInspection = 1;
		bSuccess = FALSE;
	}
		if (PrjCfg.fLaserPower>100)
		{
			PrjCfg.fLaserPower = 10;
			bSuccess = FALSE;
		}
	for (int i = 0; i < 2; i++)
	{
		if (PrjCfg.bIntensity[i] > nLightLimit[i])
			PrjCfg.bIntensity[i] = nLightLimit[i];
	}
	if (PrjCfg.bProductNum[0] == 0 || PrjCfg.bProductNum[0] > 100)
		PrjCfg.bProductNum[0] = 1;
	if (PrjCfg.bProductNum[1] == 0 || PrjCfg.bProductNum[1] > 100)
		PrjCfg.bProductNum[1] = 1;
	if (PrjCfg.bProductNum[0] * PrjCfg.bProductNum[1] > MAX_ProductNumber)
		PrjCfg.bProductNum[0] = MAX_ProductNumber / PrjCfg.bProductNum[1];
	if (!PrjCfg.bSortingDir)
		PrjCfg.bSortingDir = 0x01;
	return bSuccess;
}

