
// APM.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "APM.h"
#include "MainFrm.h"

#include "APMDoc.h"
#include "APMView.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAPMApp

BEGIN_MESSAGE_MAP(CAPMApp, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CAPMApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CBCGPWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CBCGPWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CAPMApp ����

CAPMApp::CAPMApp()
{
	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("APM.AppID.NoVersion"));
	m_nRangingSts = 0;
	m_bSortingDir = 0x01;
	m_bNeedSave = FALSE;
	m_bHasLocation = FALSE;
	m_dLocated[0] = 0;
	m_dLocated[1] = 0;
	m_bHigh_Risk[0] = FALSE;
	m_bHigh_Risk[1] = FALSE;
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CAPMApp ����

CAPMApp theApp;


// CAPMApp ��ʼ��

BOOL CAPMApp::InitInstance()
{
	HANDLE hMutex = CreateMutex(NULL, TRUE, m_pszAppName);
	if (GetLastError() == ERROR_ALREADY_EXISTS) { ::AfxMessageBox(_T("ֻ��������һ������,�����ظ�����")); return FALSE; }

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CBCGPWinApp::InitInstance();


	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// ʹ�� RichEdit �ؼ���Ҫ  AfxInitRichEdit2()	
	 AfxInitRichEdit2();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO:  Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("LYPE\\APM"));
	LoadStdProfileSettings(2);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)


	// ע��Ӧ�ó�����ĵ�ģ�塣  �ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAPMDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CAPMView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// ������׼ shell ���DDE�����ļ�������������
	m_nCmdShow = SW_HIDE;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// ��������������ָ�������  ���
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	if (((CMainFrame*)m_pMainWnd)->InitialDefine())
	{
 #ifndef _DEBUG
		m_pMainWnd->ShowWindow(SW_HIDE);

	CLoginDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDCANCEL)
		return FALSE;
 #endif
	if (!((CMainFrame*)m_pMainWnd)->InitHardware())
		return FALSE;
	}
	else
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
// 	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	::PostMessage(m_pMainWnd->m_hWnd, WM_USER_UPDATEUI, 2, 0);
	::PostMessage(m_pMainWnd->m_hWnd, WM_USER_UPDATEUI, 3, 0);

	return TRUE;
}

int CAPMApp::ExitInstance()
{
	//TODO:  �����������ӵĸ�����Դ
	AfxOleTerm(FALSE);
#ifdef _DEBUG
	CleanState();
#endif
	BCGCBProCleanUp();
	return CBCGPWinApp::ExitInstance();
}

// CAPMApp ��Ϣ�������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CBCGPDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CAPMApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CAPMApp::PreLoadState()
{
}

// CAPMApp ��Ϣ�������


BOOL CAPMApp::ShowWaiteProcessDlg(BOOL bShow /*= TRUE*/)
{
	::PostMessage(m_pMainWnd->m_hWnd, WM_USER_UPDATEUI, 7, bShow);
	return TRUE;
}


