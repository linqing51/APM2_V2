
// APM.h : APM 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

#include "stopwatch.h"

// CAPMApp:
// 有关此类的实现，请参阅 APM.cpp
//

class CAPMApp : public CBCGPWinApp
{
public:
	CAPMApp();
	BYTE						m_nRangingSts;
	BYTE						m_bSortingDir;
	BYTE						m_bNeedSave;
	BYTE						m_bHigh_Risk[2];//；
	BOOL						m_bHasLocation;
	double						m_dLocated[2];
	CStopWatch					m_StopWatch;
// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:

	virtual void PreLoadState();
	BOOL	ShowWaiteProcessDlg(BOOL bShow = TRUE);

};

extern CAPMApp theApp;
