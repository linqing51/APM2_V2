// LaserSettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "APM.h"
#include "LaserSettingDlg.h"
#include "MainFrm.h"

// CLaserSettingDlg �Ի���

IMPLEMENT_DYNAMIC(CLaserSettingDlg, CBCGPDialog)

CLaserSettingDlg::CLaserSettingDlg(CWnd* pParent /*=NULL*/)
: CBCGPDialog(CLaserSettingDlg::IDD, pParent)
, m_bN2Ready(TRUE)
, m_bDiscReady(TRUE)
, m_nTestBall(5)
{

}

CLaserSettingDlg::~CLaserSettingDlg()
{
}

void CLaserSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_POWER, fPower);
	DDX_Text(pDX, IDC_EDIT_PRESSURE, fLaserPressure);
	DDX_Text(pDX, IDC_EDIT_BALL_PRESSURE_UP, nBallPressure[0]);

	DDX_Text(pDX, IDC_EDIT_BALL_PRESSURE_LOW, nBallPressure[1]);

	DDX_Text(pDX, IDC_EDIT_ClEAN_POWER, fCleanPower);

	DDX_Check(pDX, IDC_CHECK_N2_READY, m_bN2Ready);
	DDX_Check(pDX, IDC_CHECK_BOND_READY, m_bDiscReady);
	DDX_Text(pDX, IDC_EDIT_TESTMODE_BALLNUM, m_nTestBall);
	DDV_MinMaxFloat(pDX, fPower, 0.1, 100);
	DDV_MinMaxFloat(pDX, fLaserPressure, 0.1, 100);
}


BEGIN_MESSAGE_MAP(CLaserSettingDlg, CBCGPDialog)
	ON_MESSAGE(WM_USER_UPDATEUI, &CLaserSettingDlg::OnUpdateDlg)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_EDIT_POWER, IDC_EDIT_TESTMODE_BALLNUM, OnKillFocusLaserCfgEdit)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_N2_READY, IDC_CHECK_BOND_READY, OnBnClickedCheckLaserReady)

	ON_BN_CLICKED(IDC_BUTTON_BOUND_HOME, &CLaserSettingDlg::OnBnClickedButtonBoundHome)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CLaserSettingDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CLaserSettingDlg::OnBnClickedButtonCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CLaserSettingDlg::OnKillFocusEditVel(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

}

void CLaserSettingDlg::OnKillFocusLaserCfgEdit(UINT idCtl)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UpdateData();
}


// CLaserSettingDlg ��Ϣ�������


BOOL CLaserSettingDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	PostMessage(WM_USER_UPDATEUI, 0x02, FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

LRESULT CLaserSettingDlg::OnUpdateDlg(WPARAM wParam /*= 0*/, LPARAM lParam /*= 0*/)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT i(0);
	UINT nIndex = wParam;

	switch (nIndex)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		fPower = pFrame->m_pDoc->m_cParam.PrjCfg.fLaserPower;
		fLaserPressure = pFrame->m_pDoc->m_cParam.PrjCfg.uLaserPressure/10.0;
		nBallPressure[0] = pFrame->m_pDoc->m_cParam.PrjCfg.uDetectPressure[0];
		nBallPressure[1] = pFrame->m_pDoc->m_cParam.PrjCfg.uDetectPressure[1];

		fCleanPower = pFrame->m_pDoc->m_cParam.PrjCfg.fCleanPower;
		m_nTestBall = pFrame->m_pDoc->m_cParam.PrjCfg.nTestBallNumber;
		UpdateData(FALSE);
		break;
	}


	return 0;
}

void CLaserSettingDlg::OnCbnSelchangeCombo(UINT idCtl)
{
}

BOOL CLaserSettingDlg::OnBnClickedTransfer(BOOL bDownload)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT tem(0);
	int i;
	if (bDownload)
	{
		UpdateData(TRUE);
		if (pFrame->m_LaserCtrl.SetConfigList(fPower, fLaserPressure, nBallPressure[0], nBallPressure[1]) && pFrame->m_LaserCtrl.SetCleanPower(fCleanPower))
		{
			GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(FALSE);
			UINT n=SetTimer(3, 1000,NULL);
			if (n ^ 3)
				AfxMessageBox(_T("��������ض�:��ʱ��3�����쳣"));
		}
		else
			pFrame->AddedErrorMsg(_T("�������д��PLCʧ��\r\n"));

	}
	else
	{
		if (!pFrame->m_LaserCtrl.GetConfigList(fPower, fLaserPressure,
			nBallPressure[0], nBallPressure[1]) || !pFrame->m_LaserCtrl.GetCleanPower(fCleanPower))
		{
			pFrame->AddedErrorMsg(_T("��PLC��ȡ�������ʧ��\r\n"));
		}
		UpdateData(FALSE);
	}
	return TRUE;
}


void CLaserSettingDlg::OnBnClickedButtonConnect()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CLaserSettingDlg::OnBnClickedButtonBoundHome()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	if (!pFrame->m_LaserCtrl.GetPLCRDY())
	{
		pFrame->AddedErrorMsg(_T("PLC δ׼����\r\n"));
		return ;
	}
	pFrame->m_LaserCtrl.SetbondServoRDY();
	if (pFrame->m_LaserCtrl.IsHomed())
	{
		if (IDNO == MessageBox(_T("��Ƭ��ǰ״̬���������,��Ҫ���»�����?"), _T("��ʾ"), MB_YESNO))
			return;
	}
	if (!pFrame->m_LaserCtrl.IsHoming())
	{
		pFrame->m_nCurrentRunMode = 0x01;
		pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[0], FALSE);
		pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[1], TRUE);
		GetDlgItem(IDC_BUTTON_BOUND_HOME)->EnableWindow(FALSE);
		UINT n=SetTimer(1, 200, NULL);
		if (n ^ 1)
			AfxMessageBox(_T("��Ƭ����:��ʱ��1�����쳣"));
		n=SetTimer(2, 800, NULL);
		if (n ^ 2)
			AfxMessageBox(_T("��Ƭ����:��ʱ��2�����쳣"));
	}
}


void CLaserSettingDlg::OnBnClickedCheckLaserReady(UINT idCtl)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str[2] = { _T("���"), _T("ʹ��") };
	CString str_msg;
	switch (idCtl)
	{
	case IDC_CHECK_N2_READY:
		str_msg.Format(_T("ȷ��%s��������?"), str[!m_bN2Ready]);
		if (IDYES == MessageBox(str_msg, _T("��Ҫ��ʾ"), MB_YESNO))
		{
			UpdateData();
			pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_DCF[2], m_bN2Ready);
		}else
			UpdateData(FALSE);

		break;
	case IDC_CHECK_BOND_READY:
		if (!pFrame->m_LaserCtrl.GetPLCRDY())
		{
			pFrame->AddedErrorMsg(_T("PLC δ׼����\r\n"));
			return ;
		}
		str_msg.Format(_T("ȷ��%s��Ƭ���?"), str[!m_bDiscReady]);
		if (IDYES == MessageBox(str_msg, _T("��Ҫ��ʾ"), MB_YESNO))
		{
			UpdateData();
			pFrame->m_LaserCtrl.SetbondServoRDY(m_bDiscReady);
		}
		else
			UpdateData(FALSE);
		break;
	}
}


void CLaserSettingDlg::OnBnClickedButtonApply()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	int i(0);
	if (IDYES == MessageBox(_T("ȷ�����º��Ӳ�����������PLC��?"), _T("��Ҫ��ʾ"), MB_YESNO))
	{
		theApp.m_bNeedSave = TRUE;
		pFrame->m_pDoc->m_cParam.PrjCfg.nTestBallNumber = m_nTestBall;
		pFrame->m_pDoc->m_cParam.PrjCfg.fLaserPower = fPower;
		pFrame->m_pDoc->m_cParam.PrjCfg.fCleanPower = fCleanPower;
		pFrame->m_pDoc->m_cParam.PrjCfg.uLaserPressure = fLaserPressure*10;
		pFrame->m_pDoc->m_cParam.PrjCfg.uDetectPressure[0] = nBallPressure[0];
		pFrame->m_pDoc->m_cParam.PrjCfg.uDetectPressure[1] = nBallPressure[1];
		if (!pFrame->m_LaserCtrl.GetPLCRDY())
		{
			pFrame->AddedErrorMsg(_T("PLC δ׼����\r\n"));
			return ;
		}
		OnBnClickedTransfer(TRUE);
	}
}

void CLaserSettingDlg::OnBnClickedButtonCancel()
{
	PostMessage(WM_USER_UPDATEUI, 0x02, FALSE);
	CBCGPDialog::OnCancel();
}

void CLaserSettingDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	static int num(100);
	switch (nIDEvent)
	{
	case 1:
		pFrame->m_IoCtrller.WriteOutputByBit(pFrame->m_pDoc->m_cParam.Ou_Welding[1], FALSE);
		KillTimer(nIDEvent);
		break;
	case 2:
		if (!num || ::WaitForSingleObject(pFrame->m_hBondHomeEnd, 0) == WAIT_OBJECT_0)
		{
			GetDlgItem(IDC_BUTTON_BOUND_HOME)->EnableWindow(TRUE);
			if (::WaitForSingleObject(pFrame->m_hBondHomeEnd, 0) == WAIT_OBJECT_0)
				pFrame->AddedErrorMsg(_T("��Ƭ�ֶ����㶯����ʱ\r\n"));

			pFrame->m_nCurrentRunMode = 0x0;
			num = 100;
			KillTimer(nIDEvent);
		}
		else
			num--;

		break;
	case 3:
		OnBnClickedTransfer(FALSE);
		GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(TRUE);
		KillTimer(nIDEvent);
		break;
	}
	CBCGPDialog::OnTimer(nIDEvent);
}
