#include "..\stdafx.h"
#include "LightCtrl.h"

//CCriticalSection g_mycs;
CLightCtrl::CLightCtrl()
{
	m_light[0] = 0;
	m_light[1] = 0;
	m_comm = 0;   //comm号，0代表com1
	m_lbaud = 115200; //波特率

	memset(m_cmd1, 0, 1024);
	memset(m_cmd2, 0, 1024);
	sprintf_s(m_cmd1, "OPEN");
	sprintf_s(m_cmd2, "CLOSE");
	m_strCmd1 = "@up_ok$";
	m_strCmd2 = "@dn_ok$";
}

CLightCtrl::~CLightCtrl()
{
	if (m_sp.IsOpened())
		m_sp.ClosePort();
}

void CLightCtrl::SetWnd(HWND powner)
{
	m_pOwner = powner;
}

bool CLightCtrl::ConnectLight(int comm)
{
	m_sp.InitPort(m_pOwner, comm+1, m_lbaud);
	if (!m_sp.IsOpened())
	{
		//AfxMessageBox(_T("open filed"));
		return false;
	}
	else
	{
		//AfxMessageBox(_T("open comm%s successed"), 1);
	}
	m_sp.StartMonitoring();
	//m_sp.SuspendMonitoring();
	
	return true;
}

//为苏州需求
bool CLightCtrl::SendCmd1()
{
	if (!m_sp.IsOpened())
		return false;
	//添加换行符
	CString str = m_strCmd1;
	char* cmd1 = (char*)str.GetBuffer(0);
	//send(cmd1, 6);
	
	m_sp.Send(cmd1);
	return true;
}

bool CLightCtrl::SendCmd2()
{
	if (!m_sp.IsOpened())
		return false;
	CString str = m_strCmd2 ;
	char* cmd = (char*)str.GetBuffer(0);
	//send(cmd, 7);
	m_sp.Send(cmd);
	return true;
}

bool CLightCtrl::Loadini()
{
	CIni iniFile;
	CString strIniFileName;
	bool ret = iniFile.Read(INIFILE_PATH);
	iniFile.GetValue(NODE_SR, SUBNODE_COMM, m_comm);
	iniFile.GetValue(NODE_SR, SUBNODE_BAUD, m_lbaud);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD1, m_strCmd1);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD2, m_strCmd2);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD3, m_strCmd3);
	iniFile.GetValue(NODE_SR, SUBNODE_CMD4, m_strCmd4);
	return ret;
}

bool CLightCtrl::receiveCmd(CString& strRev)
{
	int ret = m_sp.Receive(strRev, 10);
	if(WAIT_OBJECT_0 == ret)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CLightCtrl::send(char * pCh,int len)
{
	m_sp.Send(pCh, len);
	return true;
}
