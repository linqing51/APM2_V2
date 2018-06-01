// HomeProcessDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "APM.h"
#include "HomeProcessDlg.h"
#include "MainFrm.h"


// CHomeProcessDlg �Ի���

IMPLEMENT_DYNAMIC(CHomeProcessDlg, CBCGPDialog)

CHomeProcessDlg::CHomeProcessDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CHomeProcessDlg::IDD, pParent)
{
	EnableVisualManagerStyle();
}

CHomeProcessDlg::~CHomeProcessDlg()
{
}

void CHomeProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ANIMATE, m_wndAnimate);
	DDX_Control(pDX, IDC_PROGRESS, m_ctlProgress);
	DDX_Control(pDX, IDC_STATIC_MESSAGE, m_wndTips);
}


BEGIN_MESSAGE_MAP(CHomeProcessDlg, CBCGPDialog)
	ON_MESSAGE(WM_USER_HOMING, &CHomeProcessDlg::OnHoming)
END_MESSAGE_MAP()


// CHomeProcessDlg ��Ϣ�������


void CHomeProcessDlg::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���
	::PostMessage(GetParent()->m_hWnd, WM_USER_HOMED, 0, 0);

	CBCGPDialog::OnCancel();
}


BOOL CHomeProcessDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	// 	::SetWindowPos(this->m_hWnd, HWND_TOP, 0, 0, 640, 480, SWP_NOZORDER);
	CenterWindow();
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
// 	m_wndAnimate.Open(IDR_ANIMATION);
	
	m_wndAnimate.Open(pFrame->m_pDoc->m_szCurrentFolder+_T("APM.Bin"));
	m_wndAnimate.Play(0, -1, -1);
	m_wndTips.m_clrText = 0xff11ad;
	m_wndTips.SetText(_T("ƽ̨�ȴ���λ��"));
	m_ctlProgress.SetPos(0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

LRESULT CHomeProcessDlg::OnHoming(WPARAM wParam, LPARAM lParam)
{
	static UINT nProcess=0;
	int nSts = lParam;
	switch ((UINT)wParam)
	{
	case 0:
		m_wndTips.SetText(_T("ƽ̨���ڸ�λ�����Ժ󡭡�����\n"));
		m_ctlProgress.SetPos(1);
		break;
	case 1:
		if (nSts == 1){
			m_ctlProgress.SetPos(99);
			m_wndTips.SetText(_T("X�Ḵλ������������"));
		}
		else
			m_wndTips.SetText(_T("X�Ḵλʧ�ܡ�������"));

		break;
	case 2:

		if (nSts == 1){
			m_ctlProgress.SetPos(66);
			m_wndTips.SetText(_T("LY�Ḵλ������������"));
		}
		else
			m_wndTips.SetText(_T("LY�Ḵλʧ�ܡ�������"));

		break;
	case 3:

		if (nSts == 1){
			m_ctlProgress.SetPos(66);
			m_wndTips.SetText(_T("XY�Ḵλ������������"));
		}
		else
			m_wndTips.SetText(_T("XY�Ḵλʧ�ܡ�������"));

		break;
	case 4:

		if (nSts == 1){
			m_ctlProgress.SetPos(33);
			m_wndTips.SetText(_T("RY�Ḵλ������������"));
		}
		else
			m_wndTips.SetText(_T("RY�Ḵλʧ�ܡ�������"));

		break;
	case 7:
		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("X��LY��RY�Ḵλ�������������ȴ�Bondͷ��Ƭ��λ"));
		}
		else
			m_wndTips.SetText(_T("X��LY��RY�Ḵλʧ�ܡ�������"));

		break;
	case 8:

		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("Z�Ḵλ�������������ȴ�X LY RY��λ"));
		}
		else
			m_wndTips.SetText(_T("Z�Ḵλʧ�ܡ�������"));

		break;
	case 0x30:

		if (nSts == 1){
			nProcess += 33;
			m_ctlProgress.SetPos(nProcess);
			m_wndTips.SetText(_T("CX��VZ�Ḵλ�������������ȴ�Bondͷ��Ƭ��λ"));
		}
		else
			m_wndTips.SetText(_T("CX��VZ�Ḵλʧ�ܡ�������"));

		break;

	case 0xff:
		break;
	}
	return 0;
}
