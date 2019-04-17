
#pragma once


#define INIFILE_PATH ("Task\\config.ini")
#define NODE_SR ("Serial")
#define SUBNODE_COMM ("Comm")
#define SUBNODE_BAUD ("Baud")
#define SUBNODE_CMD1 ("CMD1")
#define SUBNODE_CMD2 ("CMD2")
#define SUBNODE_CMD3 ("CMD3")
#define SUBNODE_CMD4 ("CMD4")

class CLightCtrl
{
public:
	CLightCtrl();
	~CLightCtrl();
	void SetWnd(HWND powner);
	bool ConnectLight(int comm); //com1 = 0,com2 = 1
	bool SetLight(int idx,int nLight);
	int  GetLight(int idx);
	bool SetTrigle(int idx, int mode = 1);  //触发模式，1，内触发，0，外触发
	bool SendCmd1();
	bool SendCmd2();
	bool Loadini();
	bool receiveCmd(CString& strRev);

	itas109::CSerialPort m_sp;
	int m_light[4];	
	int m_comm;   //comm号，0代表com1
	long m_lbaud; //波特率	

	char m_cmd1[1024];
	char m_cmd2[1024];
	CString m_strCmd1;
	CString m_strCmd2;
	CString m_strCmd3;
	CString m_strCmd4;
private:
	bool send(char * pCh, int len);

	HWND m_pOwner;
};

