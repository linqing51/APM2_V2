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
	BYTE			bUseFixer;//使用固定气缸
	BYTE			bUseRanging;//使用测距
	BYTE			bRangingMode;//测距方式
	BYTE			bAutoClean;
	BYTE			bIntensity[2];//光源亮度
	BYTE			bProductNum[2];
	BYTE			bSortingDir;
	BYTE			bReverse[79];//
	//X_Y_Z_Axis	
	AxSp			SpeedCfg;

//product 
	USHORT			uTemplateSts;
	USHORT			uInspection;//照相次数
	UINT			nPinNum;//
	UINT64			nSelPin;
	UINT			nSelCluster;
	UINT			nTestBallNumber;
	float			fCleanPower;
	float			fClimbHeight[3];//0：点间；1：产品间；2：行间；
	float			fProductSpace[2];

	double			dXPosition[8];//0-n check;1-n left clean;2-right clean;3-cali check;4-cali laser;
	double			dYPosition[2][8];//0-load;1-cali check;2-cali laser;3-clean
	double			dZPosition[8];//0-safe;1--n check;2-n clean;3-cali check 4-cali laser;
	double			dUPosition[2][2];
	double			dRangingFixed[2][3][3];//左右固定测距位置
	double			dRangingRel[2][40][3];//左右变化测距位置:最大20行

//Laser Param
	float			fLaserPower;
	USHORT			uLaserPressure;
	USHORT			uDetectPressure[2];
	//Image procee

	UINT			nExposure[2];//相机曝光值
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

	BYTE			In_GS_AUX;//安全光栅
	BYTE			In_GS_FSD1;//安全光栅
	BYTE			In_GS_FSD2;//安全光栅
	BYTE			In_Gate_Open;//前，后，右，左

	BYTE			In_JL_Status[4];//距离传感器0:高判; 1:低判; 2:判定输出; 3:报警;
	BYTE			In_Cyder[4];//气缸传感器
	BYTE			In_PLC_Input[8];//PLC 信号0:完成;1报警

	//0--Output
	BYTE			Ou_ForceEnable;//屏蔽光栅
	BYTE			Ou_Tower[4];//灯塔：0-红；1-黄；2-绿；3-鸣
	BYTE			Ou_DCF[4];//电磁阀
	BYTE			Ou_Trigger;//出球打光
	BYTE			Ou_BCD[5];//参数位标识
	BYTE			Ou_Panel_lamp[3];//左绿，右绿，红灯
	BYTE			Ou_Driver_power[3];//驱动器电源
	BYTE			Ou_EMGPower;//光栅复位
	BYTE			Ou_JL_CMD[2];//距离传感器，0零漂；1复位；
	BYTE			Ou_Welding[5];//bond头控制命令：0：手自动模式切换；1：回零；2：出激光；3：红光；4：
	USHORT			nLaserSpan;
	double			dCCD_Nozzle[3];
	UINT			nNozzleLife[2];
	UINT			BallAlarmNum;
	UINT			nEnabeSafetyDev;
	BYTE			nLightLimit[3];
	//
	BYTE			nComPort[2];//0：PLC；1:光源;
	double			HOME_VEL[8];
	double			HOME_ACC[8];
	ProjectCfg		PrjCfg;
	CString			DefaultProjectName;
};

#endif // !defined(AFX_MOTIONPARAM_H__07C03036_ADA0_4F1E_984D_4A218AC9FCDF__INCLUDED_)
