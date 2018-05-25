#pragma once

// CImageView ������ͼ

class CImageView : public CBCGPFormView 
{
	DECLARE_DYNCREATE(CImageView)

public:
	CImageView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CImageView();

	FLOAT	m_nFPS;
public:
	enum { IDD = IDD_DIALOG_MONITOR };
	float			m_fRate;
	FLOAT			m_fZoom;
	CString			szImgInfo;
	CList<CString,CString&> m_cListMsg;
	void DrawCross(CPoint CnterPoint,CDC* pDC, FLOAT fZoom=1);

public:

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);

};


