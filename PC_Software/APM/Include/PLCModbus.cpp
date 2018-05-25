#include "stdafx.h"
#include "PLCModbus.h"


CPLCModbus::CPLCModbus()
{
	m_nDataAddress[0] =16384;
	m_nDataAddress[1] = 000;
	m_nID = 0x01;
	m_mb = NULL;
	m_bInitOk = FALSE;
	m_bConnectOk = FALSE;
}


CPLCModbus::~CPLCModbus()
{
	Closeport();
}

BOOL CPLCModbus::Initport(UINT nPort, int baud, char parity, int data_bit)
{
	CStringA device;
	if (nPort < 10)
		device.Format("COM%d", nPort);
	else
		device.Format("\\\\.\\COM%d",nPort);
//	m_mb = modbus_new_ascii(device, baud, parity, data_bit, 1);   //相同的端口只能同时打开一个 
	m_mb = modbus_new_rtu(device, baud, parity, data_bit, 1);   //相同的端口只能同时打开一个 
	if (m_mb)
	{
		m_bInitOk = TRUE;
		struct timeval t;
		t.tv_sec = 0;
		t.tv_usec = 1000000;   //设置modbus超时时间为1000毫秒  
		modbus_set_response_timeout(m_mb, t.tv_sec, t.tv_usec);
		SetDeviceID();
	}
	return m_bInitOk;
}

int CPLCModbus::Openport()
{
	if (m_bInitOk&&!m_bConnectOk)
		m_bConnectOk = modbus_connect(m_mb)<0?0:1;
	return m_bConnectOk;
}

void CPLCModbus::Closeport()
{
	if (m_bConnectOk)
		modbus_close(m_mb);
	modbus_free(m_mb);
	m_mb = NULL;
	m_bConnectOk = FALSE;
	m_bInitOk = FALSE;
}

void CPLCModbus::SetDeviceID(UINT nId/*=1*/)
{
	if (m_mb&&m_bInitOk)
		modbus_set_slave(m_mb, nId);  //设置modbus从机地址  

}

void CPLCModbus::SetDataAddress(BYTE bType, LONG lAddress)
{
	m_nDataAddress[bType] = lAddress;
}

BOOL CPLCModbus::CoilRW(UINT nIndex, UCHAR* pSetvalue, UINT nCount, UINT nMode)
{
	int regs(0);
	if (!m_bConnectOk)
		return FALSE;
	m_CriticalObj.Lock();
	switch (nMode)
	{
	case 0:
		regs = modbus_read_bits(m_mb, nIndex, nCount, pSetvalue);
		break;
	case 1:
		regs = modbus_read_input_bits(m_mb, nIndex, nCount, pSetvalue);
		break;
	case 2:
		regs = modbus_write_bits(m_mb, nIndex, nCount, pSetvalue);
		break;
	}
	m_CriticalObj.Unlock();
	
	return regs<0 ? 0 : 1;
}

BOOL CPLCModbus::RegisterRW(UINT nIndex, USHORT* pSetvalue, UINT nCount /*= 1*/, UINT nMode /*= 0*/)
{
	int regs(0);
	if (!m_bConnectOk)
		return FALSE;
	m_CriticalObj.Lock();

	switch (nMode)
	{
	case 0:
		regs = modbus_read_registers(m_mb, nIndex, nCount, pSetvalue);
		break;
	case 1:
		regs = modbus_read_input_registers(m_mb, nIndex, nCount, pSetvalue);
		break;
	case 2:
		regs = modbus_write_registers(m_mb, nIndex, nCount, pSetvalue);
		break;
	}
	m_CriticalObj.Unlock();

	return regs < 0 ? 0 : 1;
}

BOOL CPLCModbus::ResetStatus()
{
	UCHAR arg (TRUE);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 42, &arg, 1, WRIT))//MR210
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::IsHoming()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 43, &arg))//MR211
		return FALSE;
	return arg;
}

BOOL CPLCModbus::IsHomed()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 44, &arg))//MR212
		return FALSE;
	return arg;
}

BOOL CPLCModbus::SetbondServoRDY(BOOL bEnable /*= TRUE*/)
{
	UCHAR arg(bEnable);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 45, &arg, 1, WRIT))//MR213
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::SetBallRDY(BOOL bOn)
{
	UCHAR arg(bOn);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 46, &arg, 1, WRIT))//MR214
		return FALSE;
	return TRUE;
}

BOOL CPLCModbus::GetLaserRDY()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 47, &arg))//MR215
		return FALSE;
	return arg;
}

BOOL CPLCModbus::SetLaserRDY(BOOL bEnable)
{
	UCHAR arg(bEnable);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 47, &arg, 1, WRIT))//MR215
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::SetTriggerCleanMode()
{
	UCHAR arg(TRUE);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 112, &arg, 1, WRIT))//MR700
		return FALSE;
	return TRUE;

}

//寄存器类型
UINT CPLCModbus::GetAlarm()//0:设备复位;1:无球;2:堵球;3:清洗;4:;5:;7:;
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 400, &arg))//DM400
		return FALSE;
// 	for (int i = 1; i < 8; i++)
// 		arg[0] |= arg[i] << i;
	return arg;

}

BOOL CPLCModbus::SetNGBallNum(BYTE nNum)
{
	USHORT arg(nNum);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 402, &arg, 1, WRIT))//DM402
		return FALSE;
	return TRUE;

}

BYTE CPLCModbus::GetNGBallNum()
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 402, &arg))//DM402
		return FALSE;
	return arg;
}

 BOOL CPLCModbus::GetCleanPower(FLOAT&pw)
{
	USHORT arg;
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 404 , &arg))//DM404
		return FALSE;
	pw = arg/10.0;
	return TRUE;
}

BOOL CPLCModbus::SetCleanPower(FLOAT nWidth)
{
	USHORT arg(0);
	arg = nWidth*10;
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 404, &arg, 1, WRIT))//DM404
		return FALSE;

	return TRUE;

}

FLOAT CPLCModbus::GetPower(UINT nIndex)
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 410 , &arg))//DM410
		return FALSE;

	return arg / 10.0;
}

BOOL CPLCModbus::SetPower(FLOAT nWidth, UINT nIndex)
{
	USHORT arg(0);
	arg = nWidth * 10;
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 410 , &arg, 1, WRIT))//DM410
		return FALSE;

	return TRUE;

}

UINT CPLCModbus::GetLaserPressure(UINT nIndex)
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 1000 , &arg))//DM1000
		return FALSE;
	return arg;

}

BOOL CPLCModbus::SetLaserPressure(UINT nNumber, UINT nIndex)
{
	USHORT arg(nNumber);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 411 , &arg, 1, WRIT))//DM411
		return FALSE;
	return TRUE;

}

UINT CPLCModbus::GetDetectPressure(UINT nIndex)
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 412 , &arg))//DM412
		return FALSE;
	return arg;

}

BOOL CPLCModbus::SetDetectPressure(UINT nNumber, UINT nIndex)
{
	USHORT arg(nNumber);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 412, &arg, 1, WRIT))//DM412
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::GetConfigList(FLOAT& fPower, UINT& nPressure, UINT& ndetect_up, UINT & ndetect_low)
{
	USHORT arg[4] = {0};
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 410, arg, 4))//DM410--DM413
		return FALSE;
	fPower = arg[0]/10.0;
	nPressure = arg[1];
	ndetect_up = arg[2] ;
	ndetect_low = arg[3];
	return TRUE;
}

BOOL CPLCModbus::SetConfigList(FLOAT fPower, UINT nPressure, UINT ndetect_up, UINT ndetect_low)
{
	USHORT arg[] = { fPower * 10, nPressure, ndetect_up, ndetect_low };

	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 410, arg, 4, WRIT))//DM410--DM413
		return FALSE;
	return TRUE;
}

FLOAT CPLCModbus::GetPowerTime(UINT nIndex)
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 300 + nIndex , &arg))//DM300
		return FALSE;
	return arg/10.0;
}

BOOL CPLCModbus::SetPowerTime(FLOAT ftime, UINT nIndex)
{
	USHORT arg(0);
	arg = ftime * 10;
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 300 + nIndex, &arg, 1, WRIT))//DM300
		return FALSE;
	return TRUE;

}
/*
//过时
BOOL CPLCModbus::SetTriggerLaser()
{
	UCHAR arg(TRUE);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 55, &arg, 1, WRIT))
		return FALSE;
	return TRUE;
}

UINT CPLCModbus::GetN2Num()
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 6, &arg))
		return FALSE;
	return arg;

}
 
BOOL CPLCModbus::GetControllerRDY()
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 150, &arg))
		return FALSE;
	return arg = TRUE;
}

BOOL CPLCModbus::SetControllerRDY(BOOL bEnable )
{
	BOOL bSuccess(FALSE);
	UCHAR arg = bEnable;
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 50, &arg, 1, WRIT))//伺服
		return bSuccess;
	return bSuccess;
}

BOOL CPLCModbus::IsHomingTriggerSts()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 53, &arg))
		return FALSE;
	return arg;

}

BOOL CPLCModbus::GetN2RDY()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 52, &arg))
		return FALSE;
	return arg;

}

BOOL CPLCModbus::SetN2RDY(BOOL bEnable )
{
	UCHAR arg(bEnable);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 52, &arg, 1, WRIT))
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::GetBondRDY()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 21, &arg))
		return FALSE;
	return arg;

}

BOOL CPLCModbus::GetControlMode()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 1, &arg))
		return FALSE;
	return arg ? 1 : 0;

}

BOOL CPLCModbus::SetControlMode(BOOL bInner)
{
	UCHAR arg(bInner);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 1, &arg, 1, WRIT))
		return FALSE;
	return TRUE;

}

UINT CPLCModbus::GetCurrentGp()
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 200, &arg))
		return FALSE;
	return arg;
}

BOOL CPLCModbus::SetCurrentGP(UINT nIndex)
{
	USHORT arg(nIndex);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 200, &arg, 1, WRIT))
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::GetTriggerSts()
{
	UCHAR arg(0);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 48, &arg))
		return FALSE;
	return arg;

}

BOOL CPLCModbus::SetHomingTrigger(BOOL bOn)
{
	UCHAR arg(bOn);
	if (!m_mb || !CoilRW(m_nDataAddress[0] + 53, &arg, 1, WRIT))
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::SetTriggerTestNum(UINT nCount)//出球测试
{
	UCHAR arg(0);
	USHORT argR(nCount);
	if (nCount)
	{
		arg = TRUE;
		if (!m_mb || !RegisterRW(m_nDataAddress[1] + 4, &argR, 1, WRIT))
			return FALSE;
	}

	if (!m_mb || !CoilRW(m_nDataAddress[0] + 68, &arg, 1, WRIT))
		return FALSE;
	return TRUE;
}

UINT CPLCModbus::GetDelayTime(UINT nIndex)
{
	USHORT arg(0);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 212 + nIndex * 10, &arg))
		return FALSE;
	return arg;

}

BOOL CPLCModbus::SetDelayTime(UINT nIndex, UINT nNumber)
{
	USHORT arg(nNumber);
	if (!m_mb || !RegisterRW(m_nDataAddress[1] + 212 + nIndex * 10, &arg, 1, WRIT))
		return FALSE;
	return TRUE;

}

BOOL CPLCModbus::GetTestModeSts()
{
UCHAR arg(0);
if (!m_mb || !CoilRW(m_nDataAddress[0] + 112, &arg))
return FALSE;
return arg;
}

BOOL CPLCModbus::SetTriggerTestMode(BOOL bEnable)
{
UCHAR arg(bEnable);
if (!m_mb || !CoilRW(m_nDataAddress[0] + 112, &arg, 1, WRIT))
return FALSE;
return TRUE;
}


*/