// ReadWriteIni.cpp: implementation of the CReadWriteIni class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReadWriteIni.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReadWriteIni::CReadWriteIni()
{

}

CReadWriteIni::~CReadWriteIni()
{

}
int CReadWriteIni::GetInt(CString str)
{
	return(::GetPrivateProfileInt(m_szTitle,str,0,m_szFileName));

}
    //从INI文件中取出Float型数据
float CReadWriteIni::GetFloat(CString str)
{
	CString strret;
    ::GetPrivateProfileString(m_szTitle,str,_T("error"),strret.GetBuffer(10),10,m_szFileName);
	return(float(_tstof(strret)));
    
}
    //从INI文件中取出Double型数据
double CReadWriteIni::GetDouble(CString str)
{
	CString strret;
    ::GetPrivateProfileString(m_szTitle,str,_T("error"),strret.GetBuffer(10),10,m_szFileName);
	return(double(_tstof(strret)));
    
}
   //Load string data from ini file
CString CReadWriteIni::GetString(CString str)
{
	CString strret;
	TCHAR sr[512];
    ::GetPrivateProfileString(m_szTitle,str,_T("error"),sr,sizeof(TCHAR)*256,m_szFileName);
	strret.Format(_T("%s"),sr);
	return(strret);
}
	//Set int value to ini file
BOOL CReadWriteIni::SetValue(int value,CString sr)
{
	
	CString str;
	str.Format(_T("%d"),value);
	return(WritePrivateProfileString(m_szTitle,sr,str,m_szFileName));
}

BOOL CReadWriteIni::SetValue(UINT value,CString sr)
{
	
	CString str;
	str.Format(_T("%d"),value);
	return(WritePrivateProfileString(m_szTitle,sr,str,m_szFileName));
}

	//Set float value to ini file
BOOL CReadWriteIni::SetValue(float value,CString sr)
{

	CString str;
	str.Format(_T("%f"),value);
	return(WritePrivateProfileString(m_szTitle,sr,str,m_szFileName));
}
	//Set double value to ini file
BOOL CReadWriteIni::SetValue(double value,CString sr)
{

	CString str;
	str.Format(_T("%f"),value);
	return(WritePrivateProfileString(m_szTitle,sr,str,m_szFileName));
}
	//Set CString to ini file
BOOL CReadWriteIni::SetValue(CString str,CString sr)
{
	return(WritePrivateProfileString(m_szTitle,sr,str,m_szFileName));
}
