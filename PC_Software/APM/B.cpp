// B.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "B.h"
#include "afxdialogex.h"


// CB �Ի���

IMPLEMENT_DYNAMIC(CB, CDialogEx)

CB::CB(CWnd* pParent /*=NULL*/)
	: CDialogEx(CB::IDD, pParent)
	, m_total(0)
	, m_this(0)
{

}

CB::~CB()
{
}

void CB::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_total);
	DDX_Text(pDX, IDC_EDIT2, m_this);
}


BEGIN_MESSAGE_MAP(CB, CDialogEx)
END_MESSAGE_MAP()


// CB ��Ϣ�������


BOOL CB::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}
