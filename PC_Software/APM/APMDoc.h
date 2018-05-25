
// APMDoc.h : CAPMDoc 类的接口
//


#pragma once
#include "MachineParam.h"
#include "ReadWriteIni.h"


class CAPMDoc : public CDocument
{
protected: // 仅从序列化创建
	CAPMDoc();
	DECLARE_DYNCREATE(CAPMDoc)

// 特性
public:
	CMachineParam								m_cParam;
	CReadWriteIni								m_nLogger;

	CString										m_szCurrentFolder;
	CString										m_szCurParamName;
	CArray<MotionPoint, MotionPoint>			m_UserPt;
	CList<CString, CString&>					m_cListPrjName;

// 操作
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

// 重写
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

// 实现
public:
	virtual ~CAPMDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
