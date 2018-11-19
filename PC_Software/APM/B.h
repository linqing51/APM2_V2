#pragma once
#include "PLCModbus.h"
#include "resource.h"

// CB 对话框

class CB : public CDialogEx
{
	DECLARE_DYNAMIC(CB)

public:
	CB(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CB();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_total;
	UINT m_this;
	CPLCModbus* st;
	virtual BOOL OnInitDialog();
};
