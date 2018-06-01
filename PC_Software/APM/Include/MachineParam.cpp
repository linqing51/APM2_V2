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
	In_GS_AUX = 3;//��ȫ��դ
	In_GS_FSD1 = 4;//��ȫ��դ
	In_GS_FSD2 = 5;//��ȫ��դ

	In_Gate_Open = 6;//ǰ�����ң���
	for (i = 0; i < 4; i++)
	{
		In_JL_Status[i] = 7 + i;//���봫����0:����; 1:����; 2:�ж����; 3:����;
		In_Cyder[i] = 11 + i;//���״�����
		In_PLC_Input[i] = 16 + i;//PLC �ź�
		In_PLC_Input[4 + i] = 20 + i;//PLC �ź�
	}

	//-Output
	Ou_ForceEnable = 0;
	for (i = 0; i < 4; i++)
	{
		Ou_Tower[i]=i;//������0-�죻1-�ƣ�2-�̣�3-��
		Ou_DCF[i] = 4+i;//��ŷ�
		Ou_BCD[i] = 8+i;//����λ��ʶ
		if (i < 3)
		{
			Ou_Panel_lamp[i] = 18+i;//���̣����̣����
			Ou_Driver_power[i] =21+ i;//��������Դ
		}
		Ou_Welding[i] = 27+i;//bondͷ�������0�����Զ���1�����㣻2�������⣻3����⣻
	}
	Ou_BCD[4] = 12;//����λ��ʶ
	Ou_Trigger = 13;//������
	Ou_EMGPower = 24;//��դ��λ
	Ou_JL_CMD[0] = 25;//���봫������0��Ư��1��λ��
	Ou_JL_CMD[1] = 26;//���봫������0��Ư��1��λ��
	Ou_Welding[4] = 31;//bondͷ�������0�����Զ���1�����㣻2�������⣻3����⣻
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
	static CString	IOString[2][32] = { _T("��������ť"), _T("��������ť"), _T("ֹͣ��ť"), _T("AUX"), _T("FSD1"), _T("FSD2"), _T("��ȫ��"), _T("�������"),
		_T("�������"), _T("�����ж���"), _T("���뱨��"), _T("����1"), _T("����2"), _T("����3"), _T("����4"), _T("PLC����1"),
		_T("PLC����2"), _T("PLC����3"), _T("PLC����4"), _T("PLC����5"), _T("PLC����6"), _T("PLC����7"), _T("PLC����8"), _T("ReserveI23"), _T("ReserveI24"), _T("ReserveI25"),
		_T("ReserveI26"), _T("ReserveI27"), _T("ReserveI28"), _T("ReserveI29"), _T("ReserveI30"), _T("ReserveI31"), 
		
		_T("������"), _T("������"), _T("������"), _T("����"), _T("��ŷ�1"), _T("��ŷ�2"), _T("��ŷ�3"),_T("��ŷ�4"),
		_T("���Ӵ���"), _T("BCD1"), _T("BCD2"), _T("BCD3"), _T("BCD4"), _T("BCD5"), _T("���̵�"), _T("���̵�"),
		_T("ֹͣ��"), _T("�ŷ���Դ1"), _T("�ŷ���Դ2"), _T("�ŷ���Դ3"), _T("��դ��λ"), _T("������Ư"), _T("���븴λ"), _T("���Զ�ģʽ"),
		_T("��Ƭ����"), _T("����"),_T("���"), _T("��������"), _T("���ι�դ"), _T("ReserveO29"), _T("ReserveO30"), _T("ReserveO31")
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
		nLaserSpan = m_nFileIni.GetInt(_T("������ʱ��"));
		nBondHomeTime = m_nFileIni.GetInt(_T("��Ƭ��λ�ȴ�ʱ��"));
		nDroppingTime[0] = m_nFileIni.GetInt(_T("����ȴ�ʱ��"));
		nDroppingTime[1] = m_nFileIni.GetInt(_T("��ѹ�ȴ�ʱ��"));
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
			Ou_Tower[i] = m_nFileIni.GetInt(IOString[1][i]);//������0-�죻1-�ƣ�2-�̣�3-��
			Ou_DCF[i] = m_nFileIni.GetInt(IOString[1][i+4]);//��ŷ�
			Ou_BCD[i] = m_nFileIni.GetInt(IOString[1][i+9]);//����λ��ʶ
			if (i < 3)
			{
				Ou_Panel_lamp[i] = m_nFileIni.GetInt(IOString[1][i+14]);//���̣����̣����
				Ou_Driver_power[i] = m_nFileIni.GetInt(IOString[1][i+17]);//��������Դ
			}
			Ou_Welding[i] = m_nFileIni.GetInt(IOString[1][i+23]);//bondͷ�������0�����Զ���1�����㣻2�������⣻3����⣻
		}
		Ou_BCD[4] = m_nFileIni.GetInt(IOString[1][13]);//����λ��ʶ

		Ou_Trigger = m_nFileIni.GetInt(IOString[1][8]);//������
		Ou_EMGPower = m_nFileIni.GetInt(IOString[1][20]);//��դ��λ
		Ou_JL_CMD[0] = m_nFileIni.GetInt(IOString[1][21]);//���봫������0��Ư��1��λ��
		Ou_JL_CMD[1] = m_nFileIni.GetInt(IOString[1][22]);//���봫������0��Ư��1��λ��
		Ou_Welding[4] = m_nFileIni.GetInt(IOString[1][27]);//bondͷ�������0�����Զ���1�����㣻2�������⣻3����⣻
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
		m_nFileIni.SetValue(nLaserSpan, _T("������ʱ��"));
		m_nFileIni.SetValue(nBondHomeTime, _T("��Ƭ��λ�ȴ�ʱ��"));
		m_nFileIni.SetValue(nDroppingTime[0], _T("����ȴ�ʱ��"));
		m_nFileIni.SetValue(nDroppingTime[1], _T("��ѹ�ȴ�ʱ��"));

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

