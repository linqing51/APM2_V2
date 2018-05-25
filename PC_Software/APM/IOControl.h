// IOControl.h: interface for the CIOControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOCONTROL_H__9AC13676_0C01_4EAF_933F_C7FC143CC45E__INCLUDED_)
#define AFX_IOCONTROL_H__9AC13676_0C01_4EAF_933F_C7FC143CC45E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// #include <afxmt.h>
#include <bdaqctrl.h>
using namespace Automation::BDaq;

//#define WM_IOCHANGE WM_USER+50

#define MAX_INPUT_PORT 3
#define MAX_OUTPUT_PORT 3
#define MAX_INPUT_LINE (MAX_INPUT_PORT*32)
#define MAX_OUTPUT_LINE (MAX_OUTPUT_PORT*32)
#define UNSIGNED_LONG_LENGTH (8*sizeof(unsigned long))

class CIOControl  
{
private:
	int		m_ncard;	
	HANDLE	m_hShutDown;
	HANDLE	m_hScan;
	HANDLE	m_hEventArray[2];
	HANDLE	(*m_hEventInput)[2];
	BOOL	m_bThreadAlive;
	BOOL	m_bInit;
public:
public:
	CIOControl();
	virtual ~CIOControl();
	BOOL StartIOMonitoring(CWnd* pIOOwner);
	BOOL RestartIOMonitoring();
	BOOL StopIOMonitoring();

	int InitCard(int CardType,int cardnum = 0);
	int ReleaseCard();
	BOOL LoadConfig(CString CfgPath);
	void SetInputEvent(int nBit, int nValue);
	void InitEvent();
	BOOL ReadInputByBit(int chnnum);
	int  ReadInputByPort(int Portnum,UINT nrange = 1);//<4
	BOOL ReadOutputByBit(int chnnum);
	int  ReadOutputByPort(int Portnum, UINT nrange = 1);
	BOOL  WriteOutputByBit(int chnnum,int value);
	BOOL  WriteOutputByPort(int Portnum, int value, UINT nrange = 1);
	BOOL GetDIBit(int nBit);
	BOOL GetDOBit(int nBit);
	BOOL WaitSensorSignal(int nSensorNum, int nFlag = 1, DWORD dwMilliseconds = 2000);
	BOOL	m_bInput[32];
	CEvent	m_EventIOStop;
	CCriticalSection m_CriticalCard;
private:
	unsigned long *m_pInput;
	unsigned long *m_pOutput;
	static UINT	_cdecl IOThread(LPVOID pParam);
	static void  BDAQCALL OnDiSnapEvent(void * sender, DiSnapEventArgs * args, void * userParam);

	CWinThread*			m_Thread;
	CWnd*				m_pWnd;
private:
	InstantDiCtrl *      m_instantDiCtrl;
	InstantDoCtrl *      m_instantDoCtrl;

};

#endif // !defined(AFX_IOCONTROL_H__9AC13676_0C01_4EAF_933F_C7FC143CC45E__INCLUDED_)
