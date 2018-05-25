// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "APM.h"
#include "LoginDlg.h"
#include "MainFrm.h"

// CLoginDlg �Ի���

IMPLEMENT_DYNAMIC(CLoginDlg, CBCGPDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CBCGPDialog(CLoginDlg::IDD, pParent)
{
	nType = 0;
}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CBCGPDialog)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CLoginDlg::OnBnClickedButtonLogin)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, &CLoginDlg::OnBnClickedButtonModify)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

UINT CLoginDlg::CheckPassword(BOOL bOld)
{
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT nSuccess(0xff);
	int i = ((CBCGPComboBox*)GetDlgItem(IDC_COMBO_USERTYPE))->GetCurSel();
	CString str;
	CString Str_Wrong = _T("�������! ");
	if (bOld)
		GetDlgItem(IDC_EDIT_OLDPASSWORD)->GetWindowText(str);
	else
		GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(str);
	UINT m = STR_INT(str);
	switch (i)
	{
	case 1://����Ա
		if (m == pFrame->m_pDoc->m_cParam.PrjCfg.Magic[0] || m == 20176666)
			nSuccess = 1;
		else
			pFrame->ShowPopup(Str_Wrong);
		break;
	case 2://����ʦ
		if (m == pFrame->m_pDoc->m_cParam.PrjCfg.Magic[1] || m == 20188888)
			nSuccess = 2;
		else
			pFrame->ShowPopup(Str_Wrong);

		break;
	case 0:
	default:
		nSuccess = 0;
		break;
	}

	return nSuccess;
}


// CLoginDlg ��Ϣ�������


BOOL CLoginDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	((CBCGPComboBox*)GetDlgItem(IDC_COMBO_USERTYPE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CLoginDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CLoginDlg::OnBnClickedButtonLogin()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	UINT nSuccess = CheckPassword();
	if (nSuccess != 0xff)
	{
		pFrame->SetUserType(nSuccess);
		CBCGPDialog::OnOK();
	}
}


void CLoginDlg::OnBnClickedButtonModify()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	CString Str_Wrong = _T("�������! ");
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(str);
	UINT m = STR_INT(str);
	nType = CheckPassword(TRUE);
	switch (nType)
	{
	case 1:
	case 2:
		pFrame->m_pDoc->m_cParam.PrjCfg.Magic[nType - 1] = m;
		pFrame->ShowPopup(_T("�޸ĳɹ���"));
		break;
	case 0:
		pFrame->ShowPopup(_T("��ǰ�û���֧�ָò���"));
		break;
	}
}


void CLoginDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CBCGPDialog::OnClose();
}


void CLoginDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CLoginDlg::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���

	CBCGPDialog::OnCancel();
}
