// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "APM.h"
#include "LoginDlg.h"
#include "MainFrm.h"

// CLoginDlg 对话框

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
	CString Str_Wrong = _T("密码错误! ");
	if (bOld)
		GetDlgItem(IDC_EDIT_OLDPASSWORD)->GetWindowText(str);
	else
		GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(str);
	UINT m = STR_INT(str);
	switch (i)
	{
	case 1://技术员
		if (m == pFrame->m_pDoc->m_cParam.PrjCfg.Magic[0] || m == 20176666)
			nSuccess = 1;
		else
			pFrame->ShowPopup(Str_Wrong);
		break;
	case 2://工程师
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


// CLoginDlg 消息处理程序


BOOL CLoginDlg::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	((CBCGPComboBox*)GetDlgItem(IDC_COMBO_USERTYPE))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CLoginDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码

}


void CLoginDlg::OnBnClickedButtonLogin()
{
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加控件通知处理程序代码
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CString str;
	CString Str_Wrong = _T("密码错误! ");
	GetDlgItem(IDC_EDIT_PASSWORD)->GetWindowText(str);
	UINT m = STR_INT(str);
	nType = CheckPassword(TRUE);
	switch (nType)
	{
	case 1:
	case 2:
		pFrame->m_pDoc->m_cParam.PrjCfg.Magic[nType - 1] = m;
		pFrame->ShowPopup(_T("修改成功！"));
		break;
	case 0:
		pFrame->ShowPopup(_T("当前用户不支持该操作"));
		break;
	}
}


void CLoginDlg::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CBCGPDialog::OnClose();
}


void CLoginDlg::OnDestroy()
{
	CBCGPDialog::OnDestroy();

	// TODO:  在此处添加消息处理程序代码
}


void CLoginDlg::OnCancel()
{
	// TODO:  在此添加专用代码和/或调用基类

	CBCGPDialog::OnCancel();
}
