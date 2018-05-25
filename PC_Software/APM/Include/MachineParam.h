// CMachineParam.h: interface for the CProjectParam class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ProjectPARAM_H__07C03036_ADA0_4F1E_984D_4A218AC9FCDF__INCLUDED_)
#define AFX_ProjectPARAM_H__07C03036_ADA0_4F1E_984D_4A218AC9FCDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include "ReadWriteIni.h"

#ifndef MAX_LASERPARAM
#define MAX_LASERPARAM			16
#endif

#ifndef MAX_ProductNumber
#define MAX_ProductNumber		100
#endif

#ifndef MAX_PinNumber
#define MAX_PinNumber			16
#endif


typedef struct _ProjectCfg{
	BYTE			bUseFixer;//ʹ�ù̶�����
	BYTE			bUseRanging;//ʹ�ò��
	BYTE			bRangingMode;//��෽ʽ
	BYTE			bAutoClean;
	BYTE			bIntensity[2];//��Դ����
	BYTE			bProductNum[2];
	BYTE			bSortingDir;
	BYTE			bReverse[79];//
	//X_Y_Z_Axis	
	AxSp			SpeedCfg;

//product 
	USHORT			uTemplateSts;
	USHORT			uInspection;//�������
	UINT			nPinNum;//
	UINT64			nSelPin;
	UINT			nSelCluster;
	UINT			nTestBallNumber;
	float			fCleanPower;
	float			fClimbHeight[3];//0����䣻1����Ʒ�䣻2���м䣻
	float			fProductSpace[2];

	double			dXPosition[8];//0-n check;1-n left clean;2-right clean;3-cali check;4-cali laser;
	double			dYPosition[2][8];//0-load;1-cali check;2-cali laser;3-clean
	double			dZPosition[8];//0-safe;1--n check;2-n clean;3-cali check 4-cali laser;
	double			dUPosition[2][2];
	double			dRangingFixed[2][3][3];//���ҹ̶����λ��
	double			dRangingRel[2][40][3];//���ұ仯���λ��:���20��

//Laser Param
	float			fLaserPower;
	USHORT			uLaserPressure;
	USHORT			uDetectPressure[2];
	//Image procee

	UINT			nExposure[2];//����ع�ֵ
	Rect			rcSearch[MAX_PinNumber];
	TemplateData	tTemplateArry[MAX_PinNumber];
	Annulus			arcSearch[MAX_PinNumber];
	EdgeData		cCircleArry[MAX_PinNumber];
	RectangleDescriptor rcMinMax[MAX_PinNumber];
	LaserPoint		PinArray[16];
	MotionPoint		ProductArray[2][MAX_ProductNumber][MAX_PinNumber];
	float           fTolerance;
	BYTE			bUseBinary;
	UINT			Magic[2];
}ProjectCfg,*lpProjectCfg;


class CMachineParam  
{
public:
	CMachineParam();
	virtual ~CMachineParam();
	BOOL LoadParameter(CString strFileName,BOOL bLoadSave);
	BOOL CheckData();
	//IO Port
	
	BYTE			In_StartLBtn;//0
	BYTE			In_StartRBtn;//1
	BYTE			In_StopBtn;//2

	BYTE			In_GS_AUX;//��ȫ��դ
	BYTE			In_GS_FSD1;//��ȫ��դ
	BYTE			In_GS_FSD2;//��ȫ��դ
	BYTE			In_Gate_Open;//ǰ�����ң���

	BYTE			In_JL_Status[4];//���봫����0:����; 1:����; 2:�ж����; 3:����;
	BYTE			In_Cyder[4];//���״�����
	BYTE			In_PLC_Input[8];//PLC �ź�0:���;1����

	//0--Output
	BYTE			Ou_ForceEnable;//���ι�դ
	BYTE			Ou_Tower[4];//������0-�죻1-�ƣ�2-�̣�3-��
	BYTE			Ou_DCF[4];//��ŷ�
	BYTE			Ou_Trigger;//������
	BYTE			Ou_BCD[5];//����λ��ʶ
	BYTE			Ou_Panel_lamp[3];//���̣����̣����
	BYTE			Ou_Driver_power[3];//��������Դ
	BYTE			Ou_EMGPower;//��դ��λ
	BYTE			Ou_JL_CMD[2];//���봫������0��Ư��1��λ��
	BYTE			Ou_Welding[5];//bondͷ�������0�����Զ�ģʽ�л���1�����㣻2�������⣻3����⣻4��
	USHORT			nLaserSpan;
	double			dCCD_Nozzle[3];
	UINT			nNozzleLife[2];
	UINT			BallAlarmNum;
	UINT			nEnabeSafetyDev;
	BYTE			nLightLimit[3];
	//
	BYTE			nComPort[2];//0��PLC��1:��Դ;
	double			HOME_VEL[8];
	double			HOME_ACC[8];
	ProjectCfg		PrjCfg;
	CString			DefaultProjectName;
};

#endif // !defined(AFX_MOTIONPARAM_H__07C03036_ADA0_4F1E_984D_4A218AC9FCDF__INCLUDED_)
