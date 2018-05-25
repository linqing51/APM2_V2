
// APMDoc.cpp : CAPMDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "APM.h"
#endif
#include "MainFrm.h"

#include "APMDoc.h"
#include "StopWatch.h"
#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAPMDoc

IMPLEMENT_DYNCREATE(CAPMDoc, CDocument)

BEGIN_MESSAGE_MAP(CAPMDoc, CDocument)
	ON_COMMAND(ID_FILE_SAVE, &CAPMDoc::OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, &CAPMDoc::OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, &CAPMDoc::OnFileOpen)
END_MESSAGE_MAP()


// CAPMDoc 构造/析构

CAPMDoc::CAPMDoc()
{
	// TODO:  在此添加一次性构造代码

	GetCurrentPath();
	CreatePrjFolders(_T("Defaults"));
	m_cListPrjName.RemoveAll();
	FindPrjFolders();
}

CAPMDoc::~CAPMDoc()
{

}

BOOL CAPMDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO:  在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	CString str;
	int i(0);
	if (!LoadParameter())
		SaveParameter();
	m_cParam.DefaultProjectName.MakeUpper();
	if (m_cListPrjName.Find(m_cParam.DefaultProjectName) == NULL)
	{
		str.Format(_T("没找到上次工程文件夹[%s],加载 [DEFAULTS]"), m_cParam.DefaultProjectName);
		BCGPMessageBox(str);
		m_cParam.DefaultProjectName = _T("DEFAULTS");
		if (m_cListPrjName.Find(m_cParam.DefaultProjectName) == NULL)
			m_cListPrjName.AddHead(m_cParam.DefaultProjectName);
	}
	if (!LoadPrjData(m_cParam.DefaultProjectName))
	{
		LoadPrjData(m_cParam.DefaultProjectName, FALSE);
	}

	return TRUE;
}

// CAPMDoc 序列化

void CAPMDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CAPMDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CAPMDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CAPMDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CAPMDoc 诊断

#ifdef _DEBUG
void CAPMDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAPMDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CAPMDoc 命令

BOOL CAPMDoc::IsFileExisted(LPCTSTR lpszFileName)
{
	BOOL bFinded = FALSE;
	CFileFind filefind;
	bFinded = filefind.FindFile(lpszFileName);
	filefind.Close();
	return bFinded;
}

BOOL CAPMDoc::BackUpFile(LPCTSTR lpszFileName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath_s(lpszFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);
	CTime tm = CTime::GetCurrentTime();
	CString str = tm.Format("_bk_%M");

	CString strFolder, strNewFileName;
	strFolder.Format(_T("%s%s"), drive, dir);
	strNewFileName.Format(_T("%s%s%s"), drive, dir, fname);
	strNewFileName += str;
	if (GetFileAttributes(lpszFileName) == INVALID_FILE_ATTRIBUTES)
	{
		if (GetFileAttributes(strFolder) == INVALID_FILE_ATTRIBUTES)
		{
			if (!CreateDirectory(strFolder, NULL))
				return FALSE;
		}
	}
	if (!IsFileExisted(lpszFileName))
		return FALSE;
	CopyFile(lpszFileName, strNewFileName, FALSE);
	DeleteFile(lpszFileName);
	return TRUE;
}

BOOL CAPMDoc::GetCurrentPath(void)
{
	TCHAR path_buffer[_MAX_PATH];
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	if (GetModuleFileName(NULL, path_buffer, _MAX_PATH) == 0)
		return FALSE;
	_tsplitpath_s(path_buffer, drive, dir, fname, ext);
	m_szCurrentFolder.Format(_T("%s%s"), drive, dir);
	return TRUE;
}

BOOL CAPMDoc::CheckFileFolder(LPCTSTR lpszFileName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];
	_tsplitpath_s(lpszFileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname,
		_MAX_FNAME, ext, _MAX_EXT);

	CString strFolder;
	strFolder.Format(_T("%s%s"), drive, dir);
	if (GetFileAttributes(lpszFileName) == INVALID_FILE_ATTRIBUTES)
	{
		if (GetFileAttributes(strFolder) == INVALID_FILE_ATTRIBUTES)
		{
			if (!CreateDirectory(strFolder, NULL))
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CAPMDoc::CreateLocalDataStoragePath(CString strDir)
{
	BOOL bSuccess(FALSE);
	//remove ending / if exists
	if (strDir.Right(1) == "\\")
		strDir = strDir.Left(strDir.GetLength() - 1);
	// base case . . .if directory exists
	if (GetFileAttributes(strDir) != INVALID_FILE_ATTRIBUTES)
		return TRUE;
	// recursive call, one less directory
	int nFound = strDir.ReverseFind('\\');
	if (CreateLocalDataStoragePath(strDir.Left(nFound)))
	{// actual work
		if (CreateDirectory(strDir, NULL))
			return TRUE;
	}
	return bSuccess;
}

void CAPMDoc::FindPrjFolders()
{
	CFileFind tempFind;
	CString strTmp;   
	BOOL bFound; 
	bFound = tempFind.FindFile(m_szCurrentFolder + _T("Parameter\\*.*")); // 
	while (bFound)     
	{
		bFound = tempFind.FindNextFile(); 
		//到下一个文件
		if (tempFind.IsDots()) continue; 
		if (tempFind.IsSystem()) continue; 
		if (tempFind.IsDirectory())   
		{
			CString Foldername = tempFind.GetFileName();
			Foldername.MakeUpper();
			strTmp.Format(_T("%sParameter\\%s\\%s.prj"), m_szCurrentFolder, Foldername,Foldername);
			if (IsFileExisted(strTmp))
			{
				if (m_cListPrjName.Find(Foldername) == NULL){
					m_cListPrjName.AddTail(Foldername);
				}

			}
		}
	}
	tempFind.Close();
	return;
}

BOOL CAPMDoc::CreateNewPrj(LPCTSTR lpPrjName)
{
	BOOL bSuccess(FALSE);
	if (m_cListPrjName.Find(CString(lpPrjName)) == NULL)
	{
		if (!CreatePrjFolders(lpPrjName))
			return FALSE;
		m_cListPrjName.AddTail(CString(lpPrjName));
		LoadPrjData(lpPrjName, FALSE);
		m_cParam.DefaultProjectName = lpPrjName;
		m_szCurParamName = lpPrjName;
		bSuccess = TRUE;
	}
	return bSuccess;
}

BOOL CAPMDoc::PrjRename(LPCTSTR lpPrjName, LPCTSTR lpNewPrjName)
{
	BOOL bSuccess(FALSE);
	CString FolderPathOld;
	CString FolderPathNew;
	CString PathNew;

	FolderPathOld.Format(_T("%sParameter\\%s"), m_szCurrentFolder, lpPrjName);
	FolderPathNew.Format(_T("%sParameter\\%s"), m_szCurrentFolder, lpNewPrjName);

	if (IsFileExisted(FolderPathNew))
		return bSuccess;
	POSITION pos = m_cListPrjName.Find(CString(lpPrjName));
	if ( pos != NULL && m_cListPrjName.Find(CString(lpNewPrjName)) == NULL)
	{
		CFile::Rename(FolderPathOld, FolderPathNew);
		FolderPathOld.Format(_T("%s\\%s.prj"), FolderPathNew, lpPrjName);
		PathNew.Format(_T("%s\\%s.prj"), FolderPathNew, lpNewPrjName);
		CopyFile(FolderPathOld, PathNew, FALSE);

		m_cListPrjName.RemoveAt(pos);
		m_cListPrjName.AddTail(CString(lpNewPrjName));
		if (m_cParam.DefaultProjectName == lpPrjName)
		{
			m_cParam.DefaultProjectName = lpNewPrjName;
			m_szCurParamName = lpNewPrjName;
		}
		bSuccess = TRUE;
	}
	return bSuccess;
}

BOOL CAPMDoc::SaveParameter()
{
	BOOL bSuccess(FALSE);
	CString	pFileName = m_szCurrentFolder + _T("config.ini");//
//	BackUpFile(pFileName);
	bSuccess = m_cParam.LoadParameter(pFileName, FALSE);
	return bSuccess;
}

BOOL CAPMDoc::LoadParameter()
{
	BOOL bSuccess(FALSE);
	CString	pFileName = m_szCurrentFolder + _T("Config.ini");//

	//读取IO配置等基本参数
	BOOL bExisted = IsFileExisted(pFileName);
	if (bExisted)
		bSuccess = m_cParam.LoadParameter(pFileName, TRUE);
	return bSuccess;
}

BOOL CAPMDoc::DeleteParameter()
{
	return TRUE;
}

BOOL CAPMDoc::LoadPrjData(LPCTSTR lpszFileName, BOOL bIn_Out)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT i(0);
	CFile file;
	CFileException fileEx;
	CString FolderPathName;
	CString FilePathName;
	FolderPathName.Format(_T("%sParameter\\%s"), m_szCurrentFolder, lpszFileName);
	FilePathName.Format(_T("%s\\%s.prj"), FolderPathName, lpszFileName);
	if (bIn_Out)//
	{
		if (!file.Open(FilePathName, CFile::modeRead, &fileEx))
			return FALSE;
		file.Read(reinterpret_cast<TCHAR*>(&m_cParam.PrjCfg), sizeof(ProjectCfg));

		file.Close();
		m_szCurParamName = lpszFileName;
		CheckData();
		theApp.m_bSortingDir = m_cParam.PrjCfg.bSortingDir;
	}
	else
	{
//		BackUpFile(FilePathName);
		if (!file.Open(FilePathName, CFile::modeCreate | CFile::modeWrite, &fileEx))
			return FALSE;
		file.Write(reinterpret_cast<TCHAR*>(&m_cParam.PrjCfg), sizeof(ProjectCfg));
		file.Close();

	}
		
	return TRUE;
}

void CAPMDoc::OnFileSave()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT i(0);
	CString str;
	CStopWatch sw;
	USHORT& nSts = pFrame->m_pDoc->m_cParam.PrjCfg.uTemplateSts;
	if (nSts)
	{
		pFrame->ShowWaiteProcessDlg(TRUE);
		pFrame->UpdateWaiteProcessDlg(0, _mm_popcnt_u32(nSts));
		for (i = 0; i < 16; i++)
		{
			if (nSts & (1 << i))
			{
				if (!pFrame->LoadTemplate(i))
				{
					nSts &= ~(0x01 << i);
					str.Format(_T("第 %d 个模板保存失败"), i + 1);
					pFrame->ShowPopup(str);
				}
				pFrame->UpdateWaiteProcessDlg(1, 0);
			}
		}
		theApp.ShowWaiteProcessDlg(FALSE);
	}
// 	str.Format(_T("保存模板耗时：%.1f 微秒"),sw.GetTimeSpan());
// 	pFrame->ShowPopup(str,1,10);
	LoadPrjData(m_szCurParamName, FALSE);
	m_cParam.DefaultProjectName = m_szCurParamName;
// 	str.Format(_T("保存二进制文件耗时：%.1f 微秒"), sw.GetTimeSpan());
// 	pFrame->ShowPopup(str,1,5);
	SaveParameter();
// 	str.Format(_T("保存ini文件耗时：%.1f 微秒"), sw.GetTimeSpan());
// 	pFrame->ShowPopup(str);
	theApp.m_bNeedSave = FALSE;
}

void CAPMDoc::OnFileSaveAs()
{
	OpenFileDlg(FALSE);
}

void CAPMDoc::OnFileOpen()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	if (OpenFileDlg()){
// 		::PostMessage(pFrame->m_pManualDlg->GetSafeHwnd(), WM_USER_UPDATEUI, 2, 0);

	}
}

BOOL CAPMDoc::OpenFileDlg(BOOL bOpen)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bSuccess(FALSE);
	CString Tip;
	CString FileName;
	CString PathName;
	CString Filetype;
	CFileDialog dlg(bOpen, _T("*.*"), NULL, NULL, _T("(Image File)*.PNG,JPEG,TIFF,BMP|*.png;*.jpg;*.jpeg;*.tif;*.bmp||"));
	dlg.m_ofn.lpstrInitialDir = m_szCurrentFolder;
	if (IDOK == dlg.DoModal())
	{
		FileName = dlg.GetFileName();
		Filetype = dlg.GetFileExt();
		PathName = dlg.GetPathName();
		if (Filetype == _T("prj"))
		{
// 			if (bOpen){
// 				Tip = _T("Will Loading a new project data \n Comfirm？\n");
// 				if (IDYES == BCGPMessageBox(Tip, MB_YESNO)){
// 					if (LoadPrjData(FileName)){
// 						bSuccess = TRUE;
// 						pFrame->LoadTemplate(0);
// 					}
// 					else
// 						BCGPMessageBox(_T("File not found."));
// 				}
// 			}
// 			else{
// 				m_szCurParamName = FileName.Left(FileName.Find(_T(".")));
// 				LoadPrjData(m_szCurParamName, FALSE);
// 				m_cParam.DefaultProjectName = m_szCurParamName;
// 				SaveParameter();
// 				SetTitle(m_szCurParamName);
// 			}
		}
		else
		{
			if (bOpen){
				Tip = _T("即将读取新图像文件\n确认吗？\n");
				if (IDYES == BCGPMessageBox(Tip, MB_YESNO))
					pFrame->m_NiVision.DisplayLocalImage(0, PathName);
			}
			else{
				Image* CurrentImg = imaqCreateImage(IMAQ_IMAGE_U8, 2);
				pFrame->m_NiVision.GetImage(pFrame->m_nCameraAlive>>1, CurrentImg);
				pFrame->m_NiVision.RWVisionFile(CurrentImg, PathName);
				imaqDispose(CurrentImg);
			}
		}

	}
	return bSuccess;
}

BOOL CAPMDoc::CheckData()
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	BOOL bSuccess(TRUE);
	m_cParam.CheckData();
	return bSuccess;
}

void CAPMDoc::Writelog(UINT nIndex, LPCTSTR info)
{
	CString sztemp;
	m_nLogger.m_szFileName;
	if (nIndex > 0x01)
	{
		m_nLogger.m_szTitle = _T("------Details------");
		if (nIndex & 0xff)
		{
			sztemp.Format(_T("<%d>Pin%d"), nIndex >> 8, nIndex & 0x0ff);
			m_nLogger.SetValue(info, sztemp);

		}
		else
		{
			sztemp.Format(_T("<Cluster%d>"), nIndex >> 8);
			m_nLogger.SetValue(info, sztemp);

		}
	}
	else{
		m_nLogger.m_szTitle = _T("------Summary------");
		if (nIndex)
			m_nLogger.SetValue(info, _T("Detect_OK_Number"));
		else
			m_nLogger.SetValue(info, _T("Location_OK_Number"));

	}
}

void CAPMDoc::Initlog()
{
	CString sztemp;
	CString szFile;
	int ntm;
	static int nNumber(0);
	BOOL bDele(FALSE);
	CTime tm = CTime::GetCurrentTime();
	szFile = tm.Format("\\%H%M%S.log");
	sztemp = tm.Format("%H%M%S");
	ntm = STR_INT(sztemp);
	sztemp = m_szCurrentFolder + _T("Parameter\\") + m_szCurParamName.Left(m_szCurParamName.Find(_T(".")));
	CFileFind tempFind;
	BOOL bFound;
	bFound = tempFind.FindFile(sztemp + "\\*.log");
	if (nNumber >= 4)
	{
		nNumber = 0;
		bDele = TRUE;
	}
	while (bFound)
	{
		bFound = tempFind.FindNextFile();
		if (tempFind.IsDots())
			continue;
		if (!tempFind.IsDirectory())
		{
			CString strtemp = tempFind.GetFileTitle();
			int n = STR_INT(strtemp);
			if (n >= ntm || bDele)
				DeleteFile(tempFind.GetFilePath());
		}
	}
	tempFind.Close();

	m_nLogger.m_szFileName = sztemp + szFile;
	m_nLogger.m_szTitle = _T("------Summary------");
	m_nLogger.SetValue(m_cParam.PrjCfg.nPinNum, _T("Total Pin"));
	m_nLogger.SetValue(m_cParam.PrjCfg.fTolerance, _T("Tolerance"));

	m_nLogger.SetValue(_T(""), _T("Location_OK_Number"));
	m_nLogger.SetValue(_T(""), _T("Detect_OK_Number"));
	m_nLogger.m_szTitle = _T("------Details------");
// 	for (i = 0; i < m_cParam.PrjCfg.nCluster; i++)
// 	{
// 		sztemp.Format(_T("<Cluster%d>"), i + 1);
// 		m_nLogger.SetValue(_T(""), sztemp);
// 		for (j = 0; j < m_cParam.PrjCfg.nPinNum; j++)
// 		{
// 			sztemp.Format(_T("<%d>Pin%d"), i + 1, j + 1);
// 			m_nLogger.SetValue(_T(""), sztemp);
// 
// 		}
// 
// 	}
	nNumber++;

}

BOOL CAPMDoc::InitializeLaserTrack(CPointCollection* pTrack)
{
	int i(0);
	int m;
	m = pTrack->pKeyCollection.GetCount();
	for (i = 0; i < m; i++)
	{
		pLaserPoint pPt = pTrack->pKeyCollection[i];
		SAFE_DELETE(pPt);
	}
	pTrack->pKeyCollection.RemoveAll();
	pTrack->nCount = 0;
	return TRUE;
}

BOOL CAPMDoc::CreatePrjFolders(LPCTSTR lpPrjName)
{
	CString FolderPathName;
	FolderPathName.Format(_T("%sParameter\\%s"), m_szCurrentFolder, lpPrjName);
	if (CreateLocalDataStoragePath(FolderPathName + _T("\\Images\\")))
		CreateLocalDataStoragePath(FolderPathName + _T("\\Templates\\"));
	else
		return FALSE;
	return TRUE;
}
