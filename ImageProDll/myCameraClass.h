#pragma once
#include "afxwin.h"
#include <stdio.h>
#include "MultipleCameraDefine.h"
#include "MyCamera.h"

class myCameraClass
{
public:
	myCameraClass(int idex);
	~myCameraClass();
private:
	// ch:状态 | en:
	BOOL  m_bCreateDevice;                          // ch:是否创建设备 | en:Create
	BOOL  m_bOpenDevice;                            // ch:是否打开 | en:Open
	BOOL  m_bStartGrabbing;                         // ch:是否开始抓图 | en:Start grabbing
	int   m_nTriggerMode;                           // ch:触发模式 | en:Trigger mode
	int   m_nTriggerSource;                         // ch:触发源 | en:Trigger source
	
public:
	int				m_idex; //相机序号
	CMyCamera*		m_pcMyCamera;
	void*           m_hGetOneFrameHandle;
	unsigned int    m_nPayloadSize;


	// ch:标志设备 | en:
	HWND  m_hwndDisplay;            // ch:显示句柄 | en:Display window
	CWnd* m_cwndDisplay;

	MV_CC_DEVICE_INFO_LIST m_stDevList;             // ch:设备信息列表结构体变量，用来存储设备列表
													// en:Device information list structure variable, to save device list
	// ch:最开始时的窗口初始化 | en:Window initialization
	int CameraInit(CWnd* pWnd);               // ch:获取窗口句柄 | en:Get window handle 
	int RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo,
		void* pUser), void* pUser);
											  
											  // ch:按钮的亮暗使能 | en:Button enable
	//int EnableControls(BOOL bIsCameraReady);
	//int EnableWindowWhenInitCamera(void);    // ch:按下初始化按钮时的按钮颜色 | en:Button color after press initialization
	void   ShowErrorMsg(CString csMessage, int nErrorNum);
	int OpenDevice(int idx);                     // ch:打开设备 | en:Open Device

	// ch:设置参数操作 | en:Set parameter
	int SetTriggerMode(int nTriggerMode);       // ch:设置触发模式 | en:
	int SetExposureTime(int dExpouse);			// ch:设置曝光时间 | en:
	int SetGain(int dGain);                     // ch:设置增益 | en:
	int SetTriggerSource(int nTriggerSource);   // ch:设置触发源 | en:
	int DoSoftwareOnce(void);                    // ch:软触发一次 | en:
	int SetAcquisitionMode(int dMode);
	// ch:相机基本功能操作 | en:Basic function
	int StartGrabbing(void);                     // ch:开始流 | en:
	int StopGrabbing(void);                      // ch:结束流 | en:
	int GrabOneImage(unsigned char**pData, MV_FRAME_OUT_INFO_EX &info);                      // ch:截取一张图
	// ch:关闭销毁操作 | en:Close and destroy
	int CloseDevice(void);                       // ch:关闭设备 | en:
	int DestroyDevice(void);                     // ch:销毁设备 | en:
public:
	int ThreadFun();
};

