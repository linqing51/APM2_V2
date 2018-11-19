// B.cpp : 实现文件
//

#include "stdafx.h"
#include "B.h"
#include "afxdialogex.h"


// CB 对话框

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


// CB 消息处理程序


BOOL CB::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}
