// ReadWriteIni.h: interface for the CReadWriteIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_READWRITEINI_H__B863075F_2EA4_4DB1_9A5B_CAD72E9E1A48__INCLUDED_)
#define AFX_READWRITEINI_H__B863075F_2EA4_4DB1_9A5B_CAD72E9E1A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReadWriteIni  
{
public:
	CReadWriteIni();
	virtual ~CReadWriteIni();
	CString m_szTitle;
	CString m_szFileName;
	BOOL SetValue(CString str,CString sr);
	BOOL SetValue(double value,CString sr);
	BOOL SetValue(float value,CString sr);
	BOOL SetValue(int value,CString sr);
	BOOL SetValue(UINT value,CString sr);
	CString GetString(CString str);
	double GetDouble(CString str);
	float GetFloat(CString str);
	int GetInt(CString str);


};

#endif // !defined(AFX_READWRITEINI_H__B863075F_2EA4_4DB1_9A5B_CAD72E9E1A48__INCLUDED_)
