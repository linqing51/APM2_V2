// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS
// #include <vld.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes
#include <setupapi.h>
#include <devguid.h>
#include <math.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxcontrolbars.h>
#include <vector>
#include <BCGCBProInc.h>			// BCGPro Control Bar
#include "NIIMAQdx.h"
#include "nivision.h"
#include "nimachinevision.h"
#include "modbus.h"
#include "LogHelper.h"
#define TODO( x )  message( x )

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)		if(p){delete p; p = NULL;}
#endif

#ifndef SAFE_DELETEARRAY
#define SAFE_DELETEARRAY(p)		if(p){delete[] p; p = NULL;}
#endif

#ifndef SAFE_DELETEDLG
#define SAFE_DELETEDLG(p)		if(p){if(p->GetSafeHwnd())p->DestroyWindow();delete p; p = NULL;}
#endif

#ifndef ZERO_FL
#define ZERO_FL			0.000001
#endif

#define STR_DOUBLE(x) (_tstof((LPSTR)(LPCTSTR)(x)))
#define STR_INT(x)    (_tstoi((LPCTSTR)(x)))
static inline CString DOUBLE_STR07(double x){ CString str; str.Format(_T("%07.3f"), x); return str; }

typedef struct _ImageProcessBinder_Struct
{
	BYTE nSearchNum;
	BYTE nProcessNum;
// 	bool operator == (const ImageProcessBinder& A,const ImageProcessBinder& B){ return(true) }
}ImageProcessBinder, *pImageProcessBinder;

typedef struct _MotionPoint_Struct{
	double EndPoint[4];
}MotionPoint, *lpMoPoint;

typedef struct _TrackPoint_Struct :public MotionPoint
{
	BYTE uType;//
	BYTE nDir;
	double CenterPt[3];
	double Vel;
	double ACC;

}TrackPoint, *pTrackPoint;

typedef struct _LaserPoint_Struct :public TrackPoint
{
	BYTE uVisionIndex;//
	ImageProcessBinder uVisionType;//0нч
	BYTE uLaserType;//
	BYTE uWeldingTimes;
	USHORT DelayOffTime;

}LaserPoint, *pLaserPoint;

typedef struct _CPointCollection_Struct
{
	UINT nCount;
	CTypedPtrArray<CPtrArray, pLaserPoint> pKeyCollection;
}CPointCollection, *pCPointCollection;

typedef struct _LuckyBag_Struct
{
	LPVOID pOwner;
	LPVOID pData;
	UINT* nIndex;
}LuckyBag, *pLuckyBag;

typedef struct _Template_Data_Struct
{
	Rect* rcSearch;
	Rect rcTemp;
	int nContrast;
	int nScore;
	BYTE bType;
}TemplateData;

typedef struct _EdgeLine_Data_Struct
{
	ContourUnion SearchArea; //The information necessary to describe the contour in coordinate space.
	int     nDir;
	int     nEdgeStrength;
	int     nGap;
	int		nKernelSize;
	int		nWidth;
	BYTE	bType;

}EdgeData;

typedef struct Axis_Speed
{
	double			Run_MU_VEL[8];
	double			Run_AU_VEL[8];
	double			Run_ACC[8];
}AxSp;

typedef enum IVA_ResultType_Enum { IVA_NUMERIC, IVA_STRING } IVA_ResultType;

typedef union IVA_ResultValue_Struct    // A result in Vision Assistant can be of type double, BOOL or string.
{
	double numVal;
	int    boolVal;
	LPTSTR  strVal;
} IVA_ResultValue;

typedef struct IVA_Result_Struct
{
	BYTE  resultIndex;           // Result name
	IVA_ResultType  type;           // Result type
	IVA_ResultValue resultVal;      // Result value
} IVA_Result;

typedef struct IVA_StepResultsStruct
{
	BYTE		stepIndex;             // Step name
	int         numResults;         // number of results created by the step
	IVA_Result* results;            // array of results
} IVA_StepResults;

typedef struct IVA_Data_Struct
{
	Image* buffers[10];            // Vision Assistant Image Buffers
	int numSteps;                               // Number of steps allocated in the stepResults array
	IVA_StepResults* stepResults;              // Array of step results
	CoordinateSystem *baseCoordinateSystems;    // Base Coordinate Systems
	CoordinateSystem *MeasurementSystems;       // Measurement Coordinate Systems
	int numCoordSys;                            // Number of coordinate systems
} IVA_Data;

static IVA_Data* IVA_InitData(int numSteps, int numCoordSys)
{
	int success = 1;
	IVA_Data* ivaData = NULL;
	int i;


	// Allocate the data structure.
	ivaData = new IVA_Data;

	// Initializes the image pointers to NULL.
	for (i = 0; i < 10; i++)
		ivaData->buffers[i] = NULL;

	// Initializes the steo results array to numSteps elements.
	ivaData->numSteps = numSteps;

	ivaData->stepResults = new IVA_StepResults[ivaData->numSteps ];
	for (i = 0; i < numSteps; i++)
	{
		ivaData->stepResults[i].stepIndex = i;
		ivaData->stepResults[i].numResults = 0;
		ivaData->stepResults[i].results = NULL;
	}

	// Create the coordinate systems
	ivaData->baseCoordinateSystems = NULL;
	ivaData->MeasurementSystems = NULL;
	if (numCoordSys)
	{
		ivaData->baseCoordinateSystems = new CoordinateSystem[numCoordSys];
		ivaData->MeasurementSystems = new CoordinateSystem[numCoordSys];
	}

	ivaData->numCoordSys = numCoordSys;
	return ivaData;
}

static int IVA_DisposeStepResults(IVA_Data* ivaData, int stepIndex)
{

	SAFE_DELETEARRAY(ivaData->stepResults[stepIndex].results);

	return TRUE;
}

static int IVA_DisposeData(IVA_Data* ivaData)
{
	int i;
	// Releases the memory allocated for the image buffers.
	for (i = 0; i < 10; i++)
		imaqDispose(ivaData->buffers[i]);

	// Releases the memory allocated for the array of measurements.
	for (i = 0; i < ivaData->numSteps; i++)
		IVA_DisposeStepResults(ivaData, i);

	SAFE_DELETEARRAY(ivaData->stepResults);

	// Dispose of coordinate systems
	if (ivaData->numCoordSys)
	{
		SAFE_DELETEARRAY(ivaData->baseCoordinateSystems);
		SAFE_DELETEARRAY(ivaData->MeasurementSystems);
	}

	SAFE_DELETE(ivaData);

	return TRUE;
}



template <class LockObject>
class ScopedLocker
{
public:
	ScopedLocker(LockObject& lockObject) : m_lockObject(lockObject) { m_lockObject.Lock(); }
	~ScopedLocker() { m_lockObject.Unlock(); }
protected:
	LockObject& m_lockObject;
};

static inline int   FindCommPort(CComboBox   *pComboBox, BYTE nIndex)
{
	HDEVINFO   hDevInfo;
	SP_DEVINFO_DATA   DeviceInfoData;
	DWORD   i;
	int j(0);
	int nResult(-1);
	hDevInfo = SetupDiGetClassDevs((LPGUID)& GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		//   Insert   error   handling   here. 
		return   nResult;
	}

	pComboBox->ResetContent();
	//   Enumerate   through   all   devices   in   Set. 

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i,
		&DeviceInfoData); i++)
	{
		DWORD   DataT;
		//LPTSTR   buffer   =   NULL; 
		TCHAR   buffer[2048] = _T("hello");
		DWORD   buffersize = sizeof(buffer);
		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			SPDRP_FRIENDLYNAME,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			break;
		}
		CString str = buffer;
		if (!str.IsEmpty()){
			str = str.Right(str.GetLength() - str.Find(_T("COM")));
			str = str.Mid(str.Find(_T("COM")) + 3, str.Find(_T(")")) - str.Find(_T("COM")) - 3);
			pComboBox->AddString(str);
			++j;
			if (nIndex == _tstoi(str))
				nResult = j;
		}
	}
	if (GetLastError() != NO_ERROR   && GetLastError() != ERROR_NO_MORE_ITEMS)
	{
		return   nResult;
	}
	//     Cleanup 
	SetupDiDestroyDeviceInfoList(hDevInfo);

	if (pComboBox->GetCount() > 0)
	{
		pComboBox->SetCurSel(nResult > 0 ? nResult-1 : 0);

		nResult = nResult > 0 ? nResult : 0;
	}
	return nResult;
}

static inline BOOL RecreateComboBox(CComboBox* pCombo, LPVOID lpParam/*=0*/)
{
	if (pCombo == NULL)
		return FALSE;
	if (pCombo->GetSafeHwnd() == NULL)
		return FALSE;

	CWnd* pParent = pCombo->GetParent();
	if (pParent == NULL)
		return FALSE;

	// get current attributes  
	DWORD dwStyle = pCombo->GetStyle();
	DWORD dwStyleEx = pCombo->GetExStyle();
	CRect rc;
	pCombo->GetDroppedControlRect(&rc);
	pParent->ScreenToClient(&rc);    // map to client co-ords  
	UINT nID = pCombo->GetDlgCtrlID();
	CFont* pFont = pCombo->GetFont();
	CWnd* pWndAfter = pCombo->GetNextWindow(GW_HWNDPREV);

	// get the currently selected text (and whether it is a valid list selection)  
	CString sCurText;
	int nCurSel = pCombo->GetCurSel();
	BOOL bItemSelValid = nCurSel != -1;
	if (bItemSelValid)
		pCombo->GetLBText(nCurSel, sCurText);
	else
		pCombo->GetWindowText(sCurText);

	// copy the combo box items into a temp combobox (not sorted)  
	// along with each item's data  
	CComboBox comboNew;
	if (!comboNew.CreateEx(dwStyleEx, _T("COMBOBOX"), _T(""),
		dwStyle, rc, pParent, nID, lpParam))
		return FALSE;
	comboNew.SetFont(pFont);
	int nNumItems = pCombo->GetCount();
	for (int n = 0; n < nNumItems; n++)
	{
		CString sText;
		pCombo->GetLBText(n, sText);
		int nNewIndex = comboNew.AddString(sText);
		comboNew.SetItemData(nNewIndex, pCombo->GetItemData(n));
	}
	// re-set selected text  
	if (bItemSelValid)
		comboNew.SetCurSel(comboNew.FindStringExact(-1, sCurText));
	else
		comboNew.SetWindowText(sCurText);

	// destroy the existing window, then attach the new one  
	pCombo->DestroyWindow();
	HWND hwnd = comboNew.Detach();
	pCombo->Attach(hwnd);

	// position correctly in z-order  
	pCombo->SetWindowPos(pWndAfter == NULL ?
		&CWnd::wndBottom :
		pWndAfter, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	return TRUE;
}


#define WM_USER_SHOWTRACKER						WM_USER + 0x100
#define WM_USER_INVALIDATE						WM_USER + 0x101
#define WM_USER_SHOWIMAGE						WM_USER + 0x102
#define WM_USER_PROCESS_MESSAGE					WM_USER + 0x103
#define WM_USER_IOCHANGE						WM_USER + 0x104
#define WM_USER_MOTIONPOS						WM_USER + 0x105
#define WM_USER_HOMING							WM_USER + 0x106
#define WM_USER_HOMED							WM_USER + 0x107
#define WM_USER_MOTIONSTATUS					WM_USER + 0x108
#define WM_USER_SHOWERRORINFO					WM_USER + 0x109
#define WM_USER_UPDATEUI						WM_USER + 0x10A
#define WM_USER_ROI								WM_USER + 0x10B
#define WM_USER_TIPS							WM_USER + 0x10C
#define WM_USER_SHOWPROCESS 					WM_USER + 0x10D

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
