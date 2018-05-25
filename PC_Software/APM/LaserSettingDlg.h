#pragma once


// CLaserSettingDlg �Ի���

class CLaserSettingDlg : public CBCGPDialog
{
	DECLARE_DYNAMIC(CLaserSettingDlg)

public:
	CLaserSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLaserSettingDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_LASERSETTING };
	float   fPower;
	float   fCleanPower;
	UINT	nDelayTime;
	UINT	nLaserPressure;
	UINT	nBallPressure[2];
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	afx_msg void OnKillFocusEditVel(UINT idCtl);
	afx_msg void OnKillFocusLaserCfgEdit(UINT idCtl);
	afx_msg void OnBnClickedCheckLaserReady(UINT idCtl);
	afx_msg void OnCbnSelchangeCombo(UINT idCtl);
	afx_msg LRESULT OnUpdateDlg(WPARAM wParam = 0, LPARAM lParam = 0);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonBoundHome();
	BOOL m_bN2Ready;
	BOOL m_bDiscReady;
	UINT m_nTestBall;
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	BOOL OnBnClickedTransfer(BOOL bDownload);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
