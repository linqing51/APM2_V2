// IOControl.cpp: implementation of the CIOControl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IOControl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIOControl::CIOControl() :m_instantDiCtrl(nullptr), m_instantDoCtrl(nullptr)
{
	m_hShutDown=NULL;
	m_hScan=NULL;
	m_hEventInput=NULL;
	m_bThreadAlive=FALSE;
	m_bInit = FALSE;
	m_ncard=-1;
	m_pInput=new unsigned long[MAX_INPUT_PORT];
	m_pOutput=new unsigned long[MAX_OUTPUT_PORT];
	memset(m_pInput,0,sizeof(m_pInput)*MAX_INPUT_PORT);
	memset(m_pOutput, 0, sizeof(unsigned long)*MAX_OUTPUT_PORT);
	for(int i=0;i<32;i++)
		m_bInput[i]=FALSE;
}

CIOControl::~CIOControl()
{
	do
	{
		m_EventIOStop.SetEvent();
		SetEvent(m_hShutDown);
	} while (m_bThreadAlive);
	ReleaseCard();
	TRACE("------------->IO Monitoring Thread ended\n");
	SAFE_DELETEARRAY(m_pInput);
	SAFE_DELETEARRAY(m_pOutput);
	SAFE_DELETEARRAY(m_hEventInput);
}

BOOL CIOControl::RestartIOMonitoring()
{
	TRACE("------------->IO Thread resumed\n");
	m_Thread->ResumeThread();
	return TRUE;	
}

BOOL CIOControl::StartIOMonitoring(CWnd* pIOOwner)
{
	m_pWnd=pIOOwner;
	m_EventIOStop.ResetEvent();
	if (!(m_Thread = AfxBeginThread(IOThread,(LPVOID)this)))
		return FALSE;
	TRACE("------------->IO Monitoring Thread start\n");
	return TRUE;	
}
BOOL CIOControl::StopIOMonitoring()
{
	m_EventIOStop.SetEvent();
	TRACE("------------->IO Monitoring Thread suspended\n");
//	m_Thread->SuspendThread(); 
	return TRUE;	
}

UINT CIOControl::IOThread(LPVOID pParam)
{
	CIOControl *IO=(CIOControl *)pParam;
	unsigned long iValue;
	unsigned long temp;
	DWORD Event = 0;
	IO->m_bThreadAlive=TRUE;
	
	while(::WaitForSingleObject(IO->m_EventIOStop,10)==WAIT_TIMEOUT)
	{
		Event=::WaitForMultipleObjects(2,IO->m_hEventArray,FALSE,INFINITE);
		switch(Event)
		{
		case 0:
			{
				IO->m_bThreadAlive = FALSE;			
				AfxEndThread(100);
				break;
			}
		case 1:
			{
				iValue=IO->ReadInputByPort(0,4);
				Sleep(1);
				if(iValue!=*(IO->m_pInput+0)) 
				{
					temp=(*(IO->m_pInput+0)) ^ iValue;
					*(IO->m_pInput+0)=iValue;
					for(int bit=0;bit<32;++bit)
					{
						if((temp>>bit) & 0x01)
						{
							iValue =(*(IO->m_pInput) >> bit) & 0x1;
							IO->m_bInput[bit]=iValue;
							IO->SetInputEvent(bit,iValue);
						}
						Sleep(0);
					}
					::PostMessage(IO->m_pWnd->m_hWnd, WM_USER_IOCHANGE, 0, 0);
				}
				Sleep(1);
				iValue = IO->ReadOutputByPort(0, 4);
				if (iValue ^ *(IO->m_pOutput))
					*(IO->m_pOutput) = iValue;
				break;
			}
		}
		Sleep(5);
	}
	IO->m_bThreadAlive = FALSE;
	TRACE("---------------->IO Monitoring Thread Stop\n");
	return 0;
}

int CIOControl::InitCard(int CardType,int cardnum)
{
	ErrorCode	errorCode;
	CString Cardstring;
	Cardstring.Format(_T("PCI-%d,BID#%d"),CardType,cardnum);

	m_instantDiCtrl = InstantDiCtrl::Create();
	m_instantDoCtrl = InstantDoCtrl::Create();

	Array<DeviceTreeNode>* sptedDevices = m_instantDiCtrl->getSupportedDevices();

	if (sptedDevices->getCount() == 0)
	{
		AfxMessageBox(_T("’“≤ªµΩIO ‰»Îø®!"));
		return FALSE;
	}else{
		for (int i = 0; i < sptedDevices->getCount(); ++i)
		{
			DeviceTreeNode const & node = sptedDevices->getItem(i);
			TRACE(_T("%d, %s\n"), node.DeviceNumber, node.Description);
			CString Description(node.Description);
			if (Description.Compare(Cardstring) == 0){
				m_ncard = node.DeviceNumber;
				break;
			}
		}
		sptedDevices->Dispose();
	}
	if (m_ncard < 0)
		return FALSE;
	DeviceInformation devInfo(m_ncard,ModeWriteShared);
	errorCode = m_instantDiCtrl->setSelectedDevice(devInfo);
	errorCode = m_instantDoCtrl->setSelectedDevice(devInfo);
	m_instantDiCtrl->addChangeOfStateHandler(OnDiSnapEvent,this);
	if (m_instantDiCtrl->getFeatures()->getDiCosintSupported()){
		Array<DiCosintPort>* diCosintPorts = m_instantDiCtrl->getDiCosintPorts();
		int cosintPorts = diCosintPorts->getCount();
		for (int i = 0; i < cosintPorts; ++i)
		{
			diCosintPorts->getItem(i).setMask(0xff);
		}
	}

	if (m_hShutDown != NULL)
		ResetEvent(m_hShutDown);
	m_hShutDown = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	if (m_hScan != NULL)
		ResetEvent(m_hScan);
	m_hScan = CreateEvent(NULL, TRUE, FALSE, NULL);
	SetEvent(m_hScan);

	m_hEventArray[0]=m_hShutDown;
	m_hEventArray[1]=m_hScan;
	InitEvent();
	m_bInit = TRUE;
	WriteOutputByPort(0, 0, m_instantDoCtrl->getPortCount());
	return m_bInit;
}

int CIOControl::ReleaseCard()
{
	if (m_bInit&&m_ncard >= 0)
	{
		WriteOutputByPort(0, 0, m_instantDoCtrl->getPortCount());
		m_instantDiCtrl->Cleanup();
		m_instantDoCtrl->Cleanup();
		m_instantDiCtrl->Dispose();
		m_instantDoCtrl->Dispose();
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CIOControl::ReadInputByBit(int chnnum)
{
	BYTE state(0);
	unsigned short sPort = chnnum/8;
	if (!m_bInit)
		return FALSE;
	m_CriticalCard.Lock();
	m_instantDiCtrl->ReadBit(sPort,chnnum%8,&state);
	m_CriticalCard.Unlock();	

	return state;
}

BOOL CIOControl::ReadOutputByBit(int chnnum)
{
	BYTE state(0);
	unsigned short sPort = chnnum / 8;
	if (!m_bInit)
		return FALSE;
	m_CriticalCard.Lock();
	m_instantDoCtrl->ReadBit(sPort, chnnum % 8, &state);
	m_CriticalCard.Unlock();

	return state;
}

int CIOControl::ReadInputByPort(int Portnum, UINT nrange)
{
	int state(0);
	BYTE st[4] = { 0 };
	if (!m_bInit)
		return FALSE;
	m_CriticalCard.Lock();
	m_instantDiCtrl->Read(Portnum,nrange, st);
	m_CriticalCard.Unlock();
	for (UINT i = 0; i < nrange; i++)
		state|= st[i]<<(8*i);
	return state;	
}

int CIOControl::ReadOutputByPort(int Portnum, UINT nrange)
{
	int state(0);
	BYTE st[4] = { 0 };
	if (!m_bInit)
		return FALSE;
	m_CriticalCard.Lock();
	m_instantDoCtrl->Read(Portnum, nrange, st);
	m_CriticalCard.Unlock();
	for (UINT i = 0; i < nrange; i++)
		state |= st[i] << (8 * i);
	return state;
}

BOOL CIOControl::WriteOutputByBit(int chnnum,int value)
{
	ErrorCode	errorCode;
	unsigned short iPort, iLine;
	if (!m_bInit)
		return FALSE;
	iPort=chnnum/8;
	iLine=chnnum%8;	
	m_CriticalCard.Lock();
	errorCode = m_instantDoCtrl->WriteBit(iPort, iLine, value);
	m_CriticalCard.Unlock();
	return !errorCode;
}

BOOL CIOControl::WriteOutputByPort(int Portnum, int value, UINT nrange)
{
	ErrorCode	errorCode;
	if (!m_bInit)
		return FALSE;
	BYTE st[4] = { 0 };
	for (UINT i = 0; i < nrange; i++)
		st[i] = (value >> i) & 0xff;
	m_CriticalCard.Lock();
	errorCode = m_instantDoCtrl->Write(Portnum, nrange, st);
	m_CriticalCard.Unlock();

	return !errorCode;
}

BOOL CIOControl::GetDIBit(int nBit)
{
	if ((*m_pInput) & (1<<nBit))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CIOControl::GetDOBit(int nBit)
{
	if ((*m_pOutput) & (1<<nBit))
	{
		return TRUE;
	}
	return FALSE;
}

void CIOControl::InitEvent()
{
	m_hEventInput = new HANDLE[32][2];
	for(int i=0;i<2;++i)
	{
		for(int j=0;j<32;++j)
		{
			m_hEventInput[j][i]=CreateEvent(NULL,TRUE,FALSE,NULL);
			if(i)
				ResetEvent(m_hEventInput[j][i]);
			else
				SetEvent(m_hEventInput[j][i]);
		}
	}
}

void CIOControl::SetInputEvent(int nBit, int nValue)
{
	if(nValue)
	{
		ResetEvent(m_hEventInput[nBit][0]);
		SetEvent(m_hEventInput[nBit][1]);
	}
	else
	{
		ResetEvent(m_hEventInput[nBit][1]);
		SetEvent(m_hEventInput[nBit][0]);
	}
}
BOOL CIOControl::WaitSensorSignal(int nSensorNum, int nFlag, DWORD dwMilliseconds)
{
	DWORD dwResult;
	if(nSensorNum >= 32 || nSensorNum < 0 ||!m_bInit)
		return FALSE;
	if (m_bInput[nSensorNum] == nFlag)
		return TRUE;
	if(nFlag)
		dwResult=::WaitForSingleObject(m_hEventInput[nSensorNum][1],dwMilliseconds);
	else
		dwResult=::WaitForSingleObject(m_hEventInput[nSensorNum][0],dwMilliseconds);
	switch(dwResult)
	{
	case WAIT_OBJECT_0:
		return TRUE;
		break;
	case WAIT_TIMEOUT:
	default:
		return FALSE;
		break;
	}
	return FALSE;	
}

void BDAQCALL CIOControl::OnDiSnapEvent(void * sender, DiSnapEventArgs * args, void * userParam)
{
	InstantDiCtrl* pDi = (InstantDiCtrl*)sender;
	CIOControl* pIO = (CIOControl*)userParam;
	if (args->Id == EvtDiintChannel000){
		BOOL bStats = (args->PortData[args->SrcNum / 8] >> args->SrcNum) & 0x01;
		SetEvent(pIO->m_hEventInput[args->SrcNum][bStats]);
		ResetEvent(pIO->m_hEventInput[args->SrcNum][!bStats]);
	}
}

BOOL CIOControl::LoadConfig(CString CfgPath)
{
	ErrorCode	errorCode;

	WCHAR* CfgFile;
	if (!m_bInit)
		return FALSE;
	errorCode = m_instantDiCtrl->LoadProfile(CfgPath);
	if (errorCode != 0)
		return FALSE;
	return TRUE;
}
