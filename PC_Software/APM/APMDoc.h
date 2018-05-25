
// APMDoc.h : CAPMDoc ��Ľӿ�
//


#pragma once
#include "MachineParam.h"
#include "ReadWriteIni.h"


class CAPMDoc : public CDocument
{
protected: // �������л�����
	CAPMDoc();
	DECLARE_DYNCREATE(CAPMDoc)

// ����
public:
	CMachineParam								m_cParam;
	CReadWriteIni								m_nLogger;

	CString										m_szCurrentFolder;
	CString										m_szCurParamName;
	CArray<MotionPoint, MotionPoint>			m_UserPt;
	CList<CString, CString&>					m_cListPrjName;

// ����
public:
	BOOL InitializeLaserTrack(CPointCollection* pTrack);
	BOOL IsFileExisted(LPCTSTR lpszFileName);
	BOOL BackUpFile(LPCTSTR lpszFileName);
	BOOL GetCurrentPath(void);
	BOOL CheckFileFolder(LPCTSTR lpszFileName);
	BOOL CreateLocalDataStoragePath(CString strDir);
	BOOL OpenFileDlg(BOOL bOpen = TRUE);
	BOOL CheckData();
	BOOL SaveParameter();
	BOOL LoadParameter();
	BOOL CreatePrjFolders(LPCTSTR lpPrjName);
	void FindPrjFolders();
	BOOL CreateNewPrj(LPCTSTR lpPrjName);
	BOOL PrjRename(LPCTSTR lpPrjName, LPCTSTR lpNewPrjName);
	BOOL DeleteParameter();
	BOOL LoadPrjData(LPCTSTR lpszFileName, BOOL bIn_Out = TRUE);
	void Initlog();
	void Writelog(UINT nIndex, LPCTSTR info);

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileOpen();
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CAPMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
