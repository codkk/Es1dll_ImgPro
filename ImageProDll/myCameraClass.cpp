#include "stdafx.h"
#include "myCameraClass.h"
#include <iostream>

myCameraClass::myCameraClass(int idex):
	m_idex(idex),
	m_cwndDisplay(NULL)
	// ch:自己定义的变量初始化 | en:User defined variable*
	, m_pcMyCamera(NULL)
	, m_bCreateDevice(0)                          // 是否创建设备 | en:Create
	, m_bOpenDevice(FALSE)                        // 是否打开 | en:Open
	, m_bStartGrabbing(FALSE)                     // 是否开始抓图 | en:Start grabbing
	, m_nTriggerMode(TRIGGER_NOT_SET)             // 触发模式 | en:Trigger mode
{
	m_nPayloadSize = 0;
	//设置显示窗口
	//m_hwndDisplay = m_cwndDisplay->GetSafeHwnd();
}


myCameraClass::~myCameraClass()
{
}


// ch:工作线程 | en:Working thread
unsigned int    __stdcall   WorkThread(void* pUser)
{
	if (NULL == pUser)
	{
		return -1;
	}
	
	myCameraClass* pCam = (myCameraClass*)pUser;
	if (NULL == pCam)
	{
		return -1;
	}

	pCam->ThreadFun();

	return 0;
}


int myCameraClass::CameraInit(CWnd* pWnd)
{
	if (pWnd)
	{
		m_cwndDisplay = pWnd;
		m_hwndDisplay = m_cwndDisplay->GetSafeHwnd();
	}
	//枚举相机个数
	int nRet = CMyCamera::EnumDevices(&m_stDevList);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}
	if (0 == m_stDevList.nDeviceNum)
	{
		return STATUS_ERROR;
	}

	//打开相机
	nRet = OpenDevice(m_idex);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}
	

	return nRet;
}
// ch:注册图像数据回调 | en:Register Image Data CallBack
int myCameraClass::RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo,
	void* pUser), void* pUser)
{
	 //注册图像回调函数
	return m_pcMyCamera->RegisterImageCallBack(cbOutput, pUser);
}

void myCameraClass::ShowErrorMsg(CString csMessage, int nErrorNum)
{
	CString errorMsg;
	if (nErrorNum == 0)
	{
		errorMsg.Format(_T("%s"), csMessage);
	}
	else
	{
		errorMsg.Format(_T("%s: Error = %x: "), csMessage, nErrorNum);
	}

	switch (nErrorNum)
	{
	case MV_E_HANDLE:           errorMsg += "Error or invalid handle ";                                         break;
	case MV_E_SUPPORT:          errorMsg += "Not supported function ";                                          break;
	case MV_E_BUFOVER:          errorMsg += "Cache is full ";                                                   break;
	case MV_E_CALLORDER:        errorMsg += "Function calling order error ";                                    break;
	case MV_E_PARAMETER:        errorMsg += "Incorrect parameter ";                                             break;
	case MV_E_RESOURCE:         errorMsg += "Applying resource failed ";                                        break;
	case MV_E_NODATA:           errorMsg += "No data ";                                                         break;
	case MV_E_PRECONDITION:     errorMsg += "Precondition error, or running environment changed ";              break;
	case MV_E_VERSION:          errorMsg += "Version mismatches ";                                              break;
	case MV_E_NOENOUGH_BUF:     errorMsg += "Insufficient memory ";                                             break;
	case MV_E_ABNORMAL_IMAGE:   errorMsg += "Abnormal image, maybe incomplete image because of lost packet ";   break;
	case MV_E_UNKNOW:           errorMsg += "Unknown error ";                                                   break;
	case MV_E_GC_GENERIC:       errorMsg += "General error ";                                                   break;
	case MV_E_GC_ACCESS:        errorMsg += "Node accessing condition error ";                                  break;
	case MV_E_ACCESS_DENIED:	errorMsg += "No permission ";                                                   break;
	case MV_E_BUSY:             errorMsg += "Device is busy, or network disconnected ";                         break;
	case MV_E_NETER:            errorMsg += "Network error ";                                                   break;
	}

	//MessageBox(errorMsg, TEXT("PROMPT"), MB_OK | MB_ICONWARNING);
}

int myCameraClass::OpenDevice(int idx)
{
	//判断idx是否在相机个数范围内
	if ((m_idex >= m_stDevList.nDeviceNum) || (m_idex < 0))
	{
		ShowErrorMsg(TEXT("Error:Idex out of range!"), MV_E_GC_RANGE);
		return MV_E_GC_RANGE;
	}
	m_pcMyCamera = new CMyCamera;
	m_pcMyCamera->m_pBufForDriver = NULL;
	m_pcMyCamera->m_pBufForSaveImage = NULL;
	m_pcMyCamera->m_nBufSizeForDriver = 0;
	m_pcMyCamera->m_nBufSizeForSaveImage = 0;
	m_pcMyCamera->m_nTLayerType = m_stDevList.pDeviceInfo[m_idex]->nTLayerType;

	int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[m_idex]);
	if (MV_OK != nRet)
	{
		delete(m_pcMyCamera);
		m_pcMyCamera = NULL;
	}
	else
	{
		// ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE camera)
		if (m_stDevList.pDeviceInfo[m_idex]->nTLayerType == MV_GIGE_DEVICE)
		{
			int nPacketSize = m_pcMyCamera->GetOptimalPacketSize();
			if (nPacketSize > 0)
			{
				nRet = m_pcMyCamera->SetIntValue("GevSCPSPacketSize", nPacketSize);
				if (nRet != MV_OK)
				{
					ShowErrorMsg(TEXT("Warning: Set Packet Size fail!"), nRet);
				}
			}
			else
			{
				ShowErrorMsg(TEXT("Warning: Get Packet Size fail!"), nPacketSize);
			}
		}


		// ch:获取数据包大小 | en:Get payload size
		MVCC_INTVALUE stParam;
		memset(&stParam, 0, sizeof(MVCC_INTVALUE));
		nRet = MV_CC_GetIntValue(m_pcMyCamera->m_hDevHandle, "PayloadSize", &stParam);
		if (MV_OK != nRet)
		{
			//printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);
			ShowErrorMsg(TEXT("Get PayloadSize fail!"), nRet);
			return MV_E_NODATA;
		}
		else
		{
			m_nPayloadSize = stParam.nCurValue;
		}

		m_bCreateDevice = TRUE;
		m_bOpenDevice = TRUE;
	}

	return MV_OK;
}

int myCameraClass::SetTriggerMode(int nTriggerMode)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	nRet = m_pcMyCamera->SetEnumValue("TriggerMode", nTriggerMode);

	return MV_OK;
}

int myCameraClass::SetExposureTime(int dExpouseTime)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;

	nRet = m_pcMyCamera->SetEnumValue("ExposureMode", 0);

	nRet = m_pcMyCamera->SetFloatValue("ExposureTime", dExpouseTime);
	

	return MV_OK;
}

int myCameraClass::SetGain(int dGain)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	nRet = m_pcMyCamera->SetEnumValue("GainAuto", 0);
	nRet = m_pcMyCamera->SetFloatValue("Gain", dGain);
	
	return nRet;
}

int myCameraClass::SetTriggerSource(int nTriggerSource)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;

	nRet = m_pcMyCamera->SetEnumValue("TriggerSource", nTriggerSource);
	
	return nRet;
}

int myCameraClass::DoSoftwareOnce(void)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	int i = 0;

	nRet = m_pcMyCamera->CommandExecute("TriggerSoftware");
	
	return nRet;
}

int myCameraClass::SetAcquisitionMode(int dMode)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	int i = 0;

	nRet = m_pcMyCamera->SetEnumValue("AcquisitionMode", dMode);

	return nRet;
	return 0;
}

int myCameraClass::StartGrabbing(void)
{
	if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	int i = 0;

	nRet = m_pcMyCamera->StartGrabbing();

	
	m_bStartGrabbing = TRUE;

	nRet = m_pcMyCamera->Display(m_hwndDisplay);
	/*
	// ch:开始采集之后才创建workthread线程 | en:Create workthread after start grabbing
	unsigned int nThreadID = 0;
	m_hGetOneFrameHandle = (void*)_beginthreadex(NULL, 0, WorkThread, this, 0, &nThreadID);
	if (NULL == m_hGetOneFrameHandle)
	{
		ShowErrorMsg(TEXT("Create thread fail"), 0);
	}
	
	*/
	return MV_OK;
}

int myCameraClass::StopGrabbing(void)
{
	int nRet = MV_OK;
	if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
	{
		return STATUS_ERROR;
	}

	nRet = m_pcMyCamera->StopGrabbing();
	
	m_bStartGrabbing = FALSE;

	if (m_hGetOneFrameHandle)
	{
		CloseHandle(m_hGetOneFrameHandle);
		m_hGetOneFrameHandle = NULL;
	}
	
	return nRet;
}

int myCameraClass::GrabOneImage(unsigned char**pData, MV_FRAME_OUT_INFO_EX &info)
{
	if (FALSE == m_bStartGrabbing)
	{
		return STATUS_ERROR;
	}
	// ch:获取1张图 | en:Get one frame
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	unsigned int nDataLen = 0;
	int nRet = MV_OK;

	// ch:仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
	// en:Request buffer first time save image, release after CloseDevice
	if (NULL == m_pcMyCamera->m_pBufForDriver)
	{
		unsigned int nRecvBufSize = 0;
		unsigned int nRet = m_pcMyCamera->GetIntValue("PayloadSize", &nRecvBufSize);
		if (nRet != MV_OK)
		{
			ShowErrorMsg(TEXT("failed in get PayloadSize"), nRet);
			return STATUS_ERROR;
		}

		m_pcMyCamera->m_nBufSizeForDriver = nRecvBufSize;  // 一帧数据大小

		m_pcMyCamera->m_pBufForDriver = (unsigned char *)malloc(m_pcMyCamera->m_nBufSizeForDriver);
		if (NULL == m_pcMyCamera->m_pBufForDriver)
		{
			ShowErrorMsg(TEXT("malloc failed"), 0);
			return STATUS_ERROR;
		}
	}

	nRet = m_pcMyCamera->GetOneFrameTimeout(m_pcMyCamera->m_pBufForDriver, &nDataLen, m_pcMyCamera->m_nBufSizeForDriver, &stImageInfo, 1000);
	if (MV_OK == nRet)
	{
		//
		*pData = m_pcMyCamera->m_pBufForDriver;
		info = stImageInfo;
		return nRet;	
	}
	else
	{
		*pData = NULL;
		ShowErrorMsg(TEXT("No data, can not save image"), nRet);
		return STATUS_ERROR;
	}
	
	return nRet;
}

int myCameraClass::CloseDevice(void)
{
	if (!m_bCreateDevice) return 0;//尝试解决有时候关闭后打开失败， 故注释
	if (!m_bOpenDevice) return 0;
	int nRet = MV_OK;
	if(m_pcMyCamera)
		nRet = m_pcMyCamera->Close();
	m_bStartGrabbing = FALSE;
	m_bOpenDevice = FALSE;
	return nRet;
}

int myCameraClass::DestroyDevice(void)
{
	//if (!m_bCreateDevice) return 0;
	//if (!m_bOpenDevice) return 0;
	if (m_pcMyCamera && m_pcMyCamera->m_pBufForDriver)
	{
		free(m_pcMyCamera->m_pBufForDriver);
		m_pcMyCamera->m_pBufForDriver = NULL;
	}

	if (m_pcMyCamera && m_pcMyCamera->m_pBufForSaveImage)
	{
		free(m_pcMyCamera->m_pBufForSaveImage);
		m_pcMyCamera->m_pBufForSaveImage = NULL;
	}

	if (m_pcMyCamera)
	{
		delete(m_pcMyCamera);
		m_pcMyCamera = NULL;
	}
	m_bCreateDevice = FALSE;
	return MV_OK;
}

int myCameraClass::ThreadFun()
{
	int nRet = 0;
	CString pos;
	int nPos = 0;
	pos.Format(_T("%d"), nPos);

	unsigned int nLostFrame = 0;
	unsigned int nFrameCount = 0;
	
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	unsigned char * pData = (unsigned char *)malloc(sizeof(unsigned char) * (m_nPayloadSize));
	unsigned int nDataSize = m_nPayloadSize;

	while (m_bStartGrabbing)
	{
		nRet = MV_CC_GetOneFrameTimeout(this, pData, nDataSize, &stImageInfo, 1000);
		if (MV_OK == nRet)
		{
			
		}
	}

	if (pData)
	{
		delete pData;
		pData = NULL;
	}
	return MV_OK;
}
