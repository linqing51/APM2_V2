#pragma once
#include "PLCModbus.h"
#include "resource.h"

// CB �Ի���

class CB : public CDialogEx
{
	DECLARE_DYNAMIC(CB)

public:
	CB(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CB();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	UINT m_total;
	UINT m_this;
	CPLCModbus* st;
	virtual BOOL OnInitDialog();
};
