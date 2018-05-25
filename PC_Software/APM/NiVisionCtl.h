#pragma once
#include "stdafx.h"
#define MAX_CAMERAS			4
#define IMAGE_WINDOW		MAX_CAMERAS+1
#define TOOL_WINDOW			MAX_CAMERAS+2
#define ROI_WINDOW			MAX_CAMERAS+3
class CNiVisionCtl
{
	BOOL			m_bBestMatch;
	UINT			m_nInterval;
public:
	CNiVisionCtl();
	~CNiVisionCtl();
	UINT			m_nDisplayBinder[MAX_CAMERAS];
	UINT			m_nCameraNum;
	UINT			m_nConnect;
	UINT			m_nRotate;
	uInt32			m_nBufferNumber[MAX_CAMERAS];
	SESSION_ID		m_session[MAX_CAMERAS];
	Image*			m_pImage[MAX_CAMERAS];
	Image*			m_pOverlay[MAX_CAMERAS];
	Image*			m_pCalibration[MAX_CAMERAS];
	double			m_dCalibrationUnit[MAX_CAMERAS];
	std::vector <CStringA>	m_szCameraName;
	std::vector <CString>	m_szCameraAttributes;
	// 	HWND			m_hWndDispaly[MAX_CAMERAS];
// 	CWnd*			m_pWndDisplay[16];
	CWnd*			m_pWndParent[MAX_CAMERAS];
	BOOL            m_bCameraLive[MAX_CAMERAS];
	BOOL			m_bSameDisplayWnd;
	BOOL            m_bEnableZoomTool[MAX_CAMERAS];
	void(*ShowMessage)(CString str, BOOL bAutoClose, UINT nTime);

#ifdef ___niimaqdx_h___
	void		DisplayNIIMAQdxError(IMAQdxError error);
	BOOL		Camera_EnumerateAttributes(UINT nIndex);
	BOOL		Camera_GetAttributes(UINT nIndex, UINT nCount, CString& str);
	BOOL		Camera_SetAttributes(UINT nIndex, UINT nCount, CString str);
	int			Camera_GetExposure(UINT nIndex);
	BOOL		Camera_SetExposure(UINT nIndex,UINT nExposure);

	BOOL		PreInit(void* pWnd = NULL,UINT nNumber=1,BOOL bNiCamera=TRUE);
	BOOL		Camera_Connect(UINT nIndex);
	BOOL		Camera_Connect(CString szCam);
	BOOL		Camera_Close(UINT nIndex);
	BOOL		Camera_SnapShot(UINT nIndex);
	BOOL		Camera_Live(UINT nIndex,BOOL bLive = TRUE);
	BOOL		GetLastGrab(UINT nIndex);
	UINT		GrabThreadFuncInternal(UINT nIndex);

#else
	BOOL		PreInit(void* pWnd = NULL,int nNumber=1);
	BOOL		Camera_Close(UINT nIndex);
	BOOL		Camera_Live(UINT nIndex,BOOL bLive = TRUE);
	UINT		GrabThreadFuncInternal(UINT nIndex);

#endif 
	BOOL        NewDisplayBinder(CWnd* pOwner,int nCameraId,int nWndId);
	void		SetParent(UINT nIndex ,CWnd* pWndParent);
	void		SetHandleWnd(CWnd* pWndParent);
	void		SetSameDisplayWnd(UINT nCameraId);
	void		EnableZoomDisplay(UINT nIndex, BOOL bEnable);
	BOOL		Camera_CreateImage( UINT nIndex, Image* image);
	BOOL		Camera_Display(UINT nIndex, BOOL bShow = TRUE, Image* image = NULL);
	BOOL		CameraCalib(UINT nIndex, Image* source);

	UINT		GetAcquisitionInterval();
	void		SetAcquisitionInterval(UINT nInterval);
	BOOL		GetImage(UINT nIndex, Image* image, BYTE bCaliration = FALSE);
	BOOL		GetImageEx(UINT nIndex, Image* image, BYTE bCaliration = FALSE);
	BOOL		RotateImage(UINT nIndex, Image* image);
	BOOL		CreateImage(Image* image, void* pBits, UINT nWidth, UINT nHeight, UINT nChannels);
	Image* 		CreateImage(UINT nWidth, UINT nHeight, ImageType type = IMAQ_IMAGE_U8);
	BOOL		DisplayLocalImage(UINT nIndex, CString FilePath);
	void		ResizeDisplayWnd(UINT nIndex,CRect rc);
	BOOL		StartImageMonitor(UINT nIndex,BOOL bStart = TRUE);
	void		StopGrabThread(UINT nIndex);
	void		DisplayNIVisionError(int error);

	BOOL		CreateToolWnd();
	BOOL		CreateROI(const Image* image, ROI* pRoi,int nSel,UINT nTool);
	BOOL		CreateROI(const Image* image,Rect* rc);
	void		ShowToolWnd(BOOL bShow = TRUE);
	BOOL		SetEventCallBack(EventCallback callback);
	BOOL		RWVisionFile(Image* image,CString szName,BOOL bSave = TRUE);
	BOOL		AdjustDynamic(Image* image, ImageType imageType);
	void		SelectRect(Rect rect);
	BOOL		CreateMaskImage(Image* dest, const Image* source, Rect rc);
	BOOL		CreateMaskFromROI(Image* image, ROI* roi, int invertMask=FALSE, int extractRegion=FALSE);
	BOOL		CreateTemplate(Image* dest, const Image* source, Rect rc, Image* mask = NULL,float offsetx=0,float offsety=0);
	BOOL		CreateGeomTemplate(Image* dest, const Image* source, Rect rc, BYTE nContrast, Image* mask = NULL, float offsetx = 0, float offsety = 0);
	BOOL		CreateTemplate(Image* itemplate, const Image* source, Rect* rc, BYTE nMode = 0);
	BOOL		CreateBinary(Image* dest, const Image* source, UINT nMode);

	HistogramReport*		 Histogram_GrayMask(Image* image, ROI* roi, BYTE min_gray = 0, BYTE max_gray = 0, UINT numClass = 256);
	PatternMatchReport*		 TemplatemSearch(const Image* source, Image* itemplate, Rect searchRect, UINT* nCount, UINT nScore = 800,BOOL bBest=FALSE);
	GeometricPatternMatch3*		 GTemplatemSearch(const Image* source, Image* itemplate, Rect searchRect,BYTE nContrast, UINT* nCount, UINT nScore = 800);
	BestCircle2*			 FitCircle(PointFloat* circlePoints, UINT numPoints);
	FindEdgeReport*			 FindEdgeLine(Image *image, EdgeData straightdata,UINT nGp);
	FindCircularEdgeReport*  DetectCircle(Image* image, EdgeData circldata);
	BestCircle2*			 DetectCircle2(Image* image, EdgeData circldata);
	CircleMatch*			 FindShapeCircle(Image* image, Rect searchRect, int curveThreshold, double minRadius, double maxRadius, UINT score, int &numMatchesFound);
	RectangleMatch*			 FindShapeRect(Image* image, Rect searchRect, int curveThreshold, double maxminRang[], UINT score, int &numMatchesFound);
	EdgeReport2*			 EdgeDetector(Image* image, ROI* roi, int pProcessType, int pPolarity, int pKernelSize, int pWidth, float pMinThreshold, int bDir=FALSE ,int pInterpolationType = IMAQ_BILINEAR, int pColumnProcessingMode = IMAQ_AVERAGE_COLUMNS);
	BOOL		ClampMax(Image* image, RotatedRect searchRect, RakeDirection direction, float* distance, const FindEdgeOptions* options, const CoordinateTransform2* transform, PointFloat* firstEdge, PointFloat* lastEdge);

	void		UpdateOverlay(Image* dest, Image* source, UINT nDstGp = NULL, BOOL bErase = TRUE);
	void		UpdateOverlay(UINT nIndex, Image* source, UINT nDstGp = NULL, BOOL bErase = TRUE);
	void		ClearOverlay(Image* dest, UINT nGp = NULL);//0 for all
	void		ClearOverlay(UINT nIndex, UINT nGp = NULL);//0 for all
	void		CopyOvelay(Image* dest, const Image* source, UINT nGp = NULL);

	void		DrawContour(Image* dest, Image* source, ROI* pRoi, BYTE nDir,CurveParameters* curveParams, UINT nGp);
	void		DrawRoi(Image* dest, ROI* pRoi, UINT nIndex = 0, const RGBValue*  crText = NULL);
	void		DrawCross(Image* dest, Point pt, UINT nIndex = 0, const RGBValue*  crText = NULL);
	void		DrawLine(Image* dest, Line line, UINT nIndex = 0, const  RGBValue*  crText = NULL);
	void		DrawRect(Image* dest, RotatedRect roRect, UINT nIndex = 0, const  RGBValue*  crText = NULL);
	void		DrawOval(Image* dest, RotatedRect roRect, UINT nIndex = 0, const  RGBValue*  crText = NULL);
	void		DrawTextInfo(Image* dest, CString info, Point pt, UINT nIndex = 0, BYTE Font_szie = 20, const  RGBValue*  crText = NULL);
	void		DrawTextInfoDirect(UINT nIndex, CString info, Point pt, UINT nGp = 0, BYTE Font_szie = 20, const  RGBValue*  crText = NULL);
	void		DisplayImage(Image* image);
	BOOL		ImageCalib(Image* dest, Image* source);
protected:
	static UINT _cdecl GrabThreadFunc(LPVOID lpParam);
	static void UserDrawnProc(WindowEventType, int, Tool, Rect);
	static CNiVisionCtl* m_pThis;
	CCriticalSection  m_CriticalCard[MAX_CAMERAS];
	CWinThread* grabThread[MAX_CAMERAS];
	CWnd*       m_pHandleWnd;
	HANDLE		m_hgrabThreadEvent[2][MAX_CAMERAS];//
	HANDLE      m_hGetImageEvent[MAX_CAMERAS];
};

