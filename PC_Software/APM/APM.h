
// APM.h : APM Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

#include "stopwatch.h"

// CAPMApp:
// �йش����ʵ�֣������ APM.cpp
//

class CAPMApp : public CBCGPWinApp
{
public:
	CAPMApp();
	BYTE						m_nRangingSts;
	BYTE						m_bSortingDir;
	BYTE						m_bNeedSave;
	BYTE						m_bHigh_Risk[2];//��
	BOOL						m_bHasLocation;
	double						m_dLocated[2];
	CStopWatch					m_StopWatch;
// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
public:

	virtual void PreLoadState();
	BOOL	ShowWaiteProcessDlg(BOOL bShow = TRUE);

};

extern CAPMApp theApp;
