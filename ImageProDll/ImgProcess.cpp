#include "stdafx.h"
#include <boost/property_tree/ptree.hpp> 
#include <boost/property_tree/ini_parser.hpp>
#include "Def.h"
#include "HalcnDefineInterface.h"
#include "ImgProcess.h"
#include <boost/filesystem.hpp>
#include "afxmt.h"

#define IMG_W 4024
#define IMG_H 3036

using namespace boost::property_tree;
namespace fs = boost::filesystem;
int ImgProcess::g_num = 0;
CCriticalSection g_criSection;  //锁
// Main procedure 
//图像回调函数
void __stdcall ImageCallBackEx(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	if (pFrameInfo)
	{
		std::cout << pFrameInfo->nWidth << std::endl;
		printf("Get One Frame: Width[%d], Height[%d], nFrameNum[%d]\n",
			pFrameInfo->nWidth, pFrameInfo->nHeight, pFrameInfo->nFrameNum);
		ImgProcess* pc = (ImgProcess*)pUser;
		HalconCpp::HObject Image;
		pc->ConvertoHalcon(pData, *pFrameInfo, Image);
		HalconCpp::DispObj(Image, pc->WindowHandle);
		//HalconCpp::WriteImage(Image, "bmp", 0, "test");
	}
}


int ImgProcess::ConvertoHalcon(unsigned char * pData, MV_FRAME_OUT_INFO_EX stImageInfo, HalconCpp::HObject& image)
{
	if (NULL == m_pImageBuffer)//Allocates the cache needed to transform the image，Mono8 and RGB are no need to allocate memory
	{
		if (PixelType_Gvsp_RGB8_Packed != stImageInfo.enPixelType && PixelType_Gvsp_Mono8 != stImageInfo.enPixelType)
		{
			m_pImageBuffer = (unsigned char *)malloc(sizeof(unsigned char) * stImageInfo.nWidth * stImageInfo.nHeight * 3);
		}
	}

	int nRet = 0;
	unsigned char* pTemp = NULL;
	if (IsColorPixelFormat(stImageInfo.enPixelType))//Color
	{
		if (NULL == m_pDataSeparate)
		{
			m_pDataSeparate = (unsigned char *)malloc(sizeof(unsigned char) * stImageInfo.nHeight * stImageInfo.nWidth * 3);
		}
		if (PixelType_Gvsp_RGB8_Packed == stImageInfo.enPixelType)
		{
			pTemp = pData;
		}
		else
		{
			nRet = ConvertToRGB(m_pCam->m_pcMyCamera->m_hDevHandle, pData, stImageInfo.nHeight, stImageInfo.nWidth, stImageInfo.enPixelType, m_pImageBuffer);
			if (MV_OK != nRet)
			{
				return -1;
			}
			pTemp = m_pImageBuffer;
		}

		nRet = ConvertRGBToHalcon(&image, stImageInfo.nHeight, stImageInfo.nWidth, stImageInfo.enPixelType, pTemp, m_pDataSeparate);
		if (MV_OK != nRet)
		{
			return -1;
		}
	}
	else if (IsMonoPixelFormat(stImageInfo.enPixelType))//Mono
	{
		if (PixelType_Gvsp_Mono8 == stImageInfo.enPixelType)
		{
			pTemp = pData;
		}
		else
		{
			nRet = ConvertToMono8(m_pCam->m_pcMyCamera->m_hDevHandle, pData, m_pImageBuffer, stImageInfo.nHeight, stImageInfo.nWidth, stImageInfo.enPixelType);
			if (MV_OK != nRet)
			{
				return -1;
			}
			pTemp = m_pImageBuffer;
		}

		nRet = ConvertMono8ToHalcon(&image, stImageInfo.nHeight, stImageInfo.nWidth, stImageInfo.enPixelType, pTemp);
		if (MV_OK != nRet)
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
	//HalconDisplay(hWindow, Hobj, stImageInfo.nHeight, stImageInfo.nWidth);
	return nRet;
}

//异常处理函数
void MyExcepHandle(const HException &except)
{
	throw except;
}
void ImgProcess::disp_message(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem,
	HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_GenParamName, hv_GenParamValue;

	//This procedure displays text in a graphics window.
	//
	//Input parameters:
	//WindowHandle: The WindowHandle of the graphics window, where
	//   the message should be displayed
	//String: A tuple of strings containing the text message to be displayed
	//CoordSystem: If set to 'window', the text position is given
	//   with respect to the window coordinate system.
	//   If set to 'image', image coordinates are used.
	//   (This may be useful in zoomed images.)
	//Row: The row coordinate of the desired text position
	//   A tuple of values is allowed to display text at different
	//   positions.
	//Column: The column coordinate of the desired text position
	//   A tuple of values is allowed to display text at different
	//   positions.
	//Color: defines the color of the text as string.
	//   If set to [], '' or 'auto' the currently set color is used.
	//   If a tuple of strings is passed, the colors are used cyclically...
	//   - if |Row| == |Column| == 1: for each new textline
	//   = else for each text position.
	//Box: If Box[0] is set to 'true', the text is written within an orange box.
	//     If set to' false', no box is displayed.
	//     If set to a color string (e.g. 'white', '#FF00CC', etc.),
	//       the text is written in a box of that color.
	//     An optional second value for Box (Box[1]) controls if a shadow is displayed:
	//       'true' -> display a shadow in a default color
	//       'false' -> display no shadow
	//       otherwise -> use given string as color string for the shadow color
	//
	//It is possible to display multiple text strings in a single call.
	//In this case, some restrictions apply:
	//- Multiple text positions can be defined by specifying a tuple
	//  with multiple Row and/or Column coordinates, i.e.:
	//  - |Row| == n, |Column| == n
	//  - |Row| == n, |Column| == 1
	//  - |Row| == 1, |Column| == n
	//- If |Row| == |Column| == 1,
	//  each element of String is display in a new textline.
	//- If multiple positions or specified, the number of Strings
	//  must match the number of positions, i.e.:
	//  - Either |String| == n (each string is displayed at the
	//                          corresponding position),
	//  - or     |String| == 1 (The string is displayed n times).
	//
	//
	//Convert the parameters for disp_text.
	if (0 != (HTuple(hv_Row == HTuple()).TupleOr(hv_Column == HTuple())))
	{
		return;
	}
	if (0 != (hv_Row == -1))
	{
		hv_Row = 12;
	}
	if (0 != (hv_Column == -1))
	{
		hv_Column = 12;
	}
	//
	//Convert the parameter Box to generic parameters.
	hv_GenParamName = HTuple();
	hv_GenParamValue = HTuple();
	if (0 != ((hv_Box.TupleLength())>0))
	{
		if (0 != (HTuple(hv_Box[0]) == HTuple("false")))
		{
			//Display no box
			hv_GenParamName = hv_GenParamName.TupleConcat("box");
			hv_GenParamValue = hv_GenParamValue.TupleConcat("false");
		}
		else if (0 != (HTuple(hv_Box[0]) != HTuple("true")))
		{
			//Set a color other than the default.
			hv_GenParamName = hv_GenParamName.TupleConcat("box_color");
			hv_GenParamValue = hv_GenParamValue.TupleConcat(HTuple(hv_Box[0]));
		}
	}
	if (0 != ((hv_Box.TupleLength())>1))
	{
		if (0 != (HTuple(hv_Box[1]) == HTuple("false")))
		{
			//Display no shadow.
			hv_GenParamName = hv_GenParamName.TupleConcat("shadow");
			hv_GenParamValue = hv_GenParamValue.TupleConcat("false");
		}
		else if (0 != (HTuple(hv_Box[1]) != HTuple("true")))
		{
			//Set a shadow color other than the default.
			hv_GenParamName = hv_GenParamName.TupleConcat("shadow_color");
			hv_GenParamValue = hv_GenParamValue.TupleConcat(HTuple(hv_Box[1]));
		}
	}
	//Restore default CoordSystem behavior.
	if (0 != (hv_CoordSystem != HTuple("window")))
	{
		hv_CoordSystem = "image";
	}
	//
	if (0 != (hv_Color == HTuple("")))
	{
		//disp_text does not accept an empty string for Color.
		hv_Color = HTuple();
	}
	//
	//DispText(hv_WindowHandle, hv_String, hv_CoordSystem, hv_Row, hv_Column, hv_Color,
	//	hv_GenParamName, hv_GenParamValue);
	return;
}

void ImgProcess::ShowException(HalconCpp::HException & except)
{
	CString str(except.ErrorMessage());
	str.Replace(_T("HALCON"), _T(""));
	MessageBoxW((HWND)hl_wnd, str, _T("Error"), MB_OK);
}

void ImgProcess::GrabOneImage()
{
	//m_pCam->StartGrabbing();  //开始视频流
	//m_pCam->DoSoftwareOnce();
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	g_criSection.Lock();
	m_pCam->GrabOneImage(&pData, stImageInfo);
	g_criSection.Unlock();
	if (pData)
	{
		try
		{
			HalconCpp::HObject Image;
			ConvertoHalcon(pData, stImageInfo, Image);
			//g_criSection.Unlock(); //解锁
			DispObj(Image, WindowHandle);
			ho_Image = Image.Clone();
			//是否显示检测区域
			if (m_bShowDetectArea)
			{
				if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
				{
					//
					DispObj(m_ho_ModelDetectArea, WindowHandle);
				}
				if (m_BarcodeDetectArea.bAvalid && m_ho_DetectBarcodeArea.IsInitialized())
				{
					//
					DispObj(m_ho_DetectBarcodeArea, WindowHandle);
				}
			}

		}
		catch (HalconCpp::HException & except)
		{
			//g_criSection.Unlock(); //解锁
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "GetImage Error:");
			return ;
		}

	}
	else
	{
		//g_criSection.Unlock(); //解锁
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "GetImage Nothing:获取不到图像，请检查相机");
		}
	}
}

bool ImgProcess::SetContinueShow(bool bCont)
{
	if (bCont)
	{
		//m_pCam->StopGrabbing(); 
		//m_pCam->SetAcquisitionMode(MV_ACQ_MODE_CONTINUOUS);
		//m_pCam->StartGrabbing();  //开始视频流
		m_pCam->SetTriggerMode(MV_TRIGGER_MODE_OFF);
	}
	else
	{
		//m_pCam->StopGrabbing();  
		//m_pCam->SetAcquisitionMode(MV_ACQ_MODE_SINGLE);
		//m_pCam->StartGrabbing();  
		m_pCam->SetTriggerMode(MV_TRIGGER_MODE_ON);
	}
	return false;
}

bool ImgProcess::SetExposureTime(double ExpTime)
{
	int ret = m_pCam->SetExposureTime(ExpTime);
	if (MV_OK != ret)
	{
		return false;
	}
	else
	{
		//保存曝光
		SaveDetectArea(PATH_DETECTAREA);
	}
	return true;
}

bool ImgProcess::Calibrate9(double * Imx, double * Imy, double * Spx, double * Spy)
{
	HTuple hv_Imy, hv_Imx, hv_Spx, hv_Spy;
	try
	{
		for (int i = 0; i < CALIBRAT_PT_NUM; i++)
		{
			hv_Imx[i] = Imx[i];
			hv_Imy[i] = Imy[i];
			hv_Spx[i] = Spx[i];
			hv_Spy[i] = Spy[i];
			//m_fImx[i] = Imx[i];
			//m_fImy[i] = Imy[i];
			//m_fSpx[i] = Spx[i];
			//m_fSpy[i] = Spy[i];
		}
		VectorToHomMat2d(hv_Imx, hv_Imy, hv_Spx, hv_Spy, &m_hv_HomMat2D);
		m_bIsCalied = true;
	}
	catch (HalconCpp::HException & except)
	{
		m_bIsCalied = false;
		ShowException(except);
		AfxMessageBox(_T("标定执行失败"));
		return false;
	}
	/*catch (...)
	{
		m_bIsCalied = false;
		
		return false;
	}*/

	return true;
}
bool ImgProcess::CalibrateRot(double * Imx, double * Imy, int ptNum)
{
	//
	if (ptNum == 0) return false;
	if (ptNum >= CALIBRAT_PT_NUM) return false;

	HObject ho_Contour, ho_Circle, ho_Cross, ho_Cross2;
	HTuple  hv_rx, hv_ry;
	HTuple  hv_RowRound, hv_ColumnRound, hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
	hv_rx = HTuple();
	hv_ry = HTuple();

	for (int i = 0; i < ptNum; i++)
	{
		hv_rx[i] = Imx[i];
		hv_ry[i] = Imy[i];

		m_lfRotX[i] = Imx[i];
		m_lfRotY[i] = Imy[i];
	}

	//
	if (!hv_ry.Length()) return false;
	GenContourPolygonXld(&ho_Contour, hv_ry, hv_rx);
	FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_RowRound, &hv_ColumnRound,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	GenCircle(&ho_Circle, hv_RowRound, hv_ColumnRound, hv_Radius);
	GenCrossContourXld(&ho_Cross, hv_RowRound, hv_ColumnRound, 6, 0.785398);
	GenCrossContourXld(&ho_Cross2, hv_ry, hv_rx, 6, 0.785398);
	
	DispObj(ho_Cross, WindowHandle);
	DispObj(ho_Cross2, WindowHandle);
	DispObj(ho_Circle, WindowHandle);
	SetTposition(WindowHandle, HTuple(m_TextRow+150), HTuple(m_TextCol+150));
	WriteString(WindowHandle, "cent(x,y):" + HTuple(hv_RowRound).TupleString("3.2f") + "," + HTuple(hv_ColumnRound).TupleString("3.2f"));

	//圆心
	m_lfCentX = hv_ColumnRound.D();
	m_lfCentY = hv_RowRound.D();

	//半径
	m_lfRadiu = hv_Radius.D();

	m_bIsCaliedRot = true;
	return true;
}
//保存9点位置和仿射矩阵
bool ImgProcess::SaveCalibrate9(char * pPath)
{
	ptree m_pt, tag_setting;
	tag_setting.clear();					//不清除的话新的标签默认会继承上一个标签中的内容

	tag_setting.put<bool>(SUBNODE_VALID, m_bIsCalied);
	//图像坐标
	tag_setting.put<double >(SUBNODE_PT_X1, m_fImx[0]);
	tag_setting.put<double >(SUBNODE_PT_X2, m_fImx[1]);
	tag_setting.put<double >(SUBNODE_PT_X3, m_fImx[2]);
	tag_setting.put<double >(SUBNODE_PT_X4, m_fImx[3]);
	tag_setting.put<double >(SUBNODE_PT_X5, m_fImx[4]);
	tag_setting.put<double >(SUBNODE_PT_X6, m_fImx[5]);
	tag_setting.put<double >(SUBNODE_PT_X7, m_fImx[6]);
	tag_setting.put<double >(SUBNODE_PT_X8, m_fImx[7]);
	tag_setting.put<double >(SUBNODE_PT_X9, m_fImx[8]);

	tag_setting.put<double >(SUBNODE_PT_Y1, m_fImy[0]);
	tag_setting.put<double >(SUBNODE_PT_Y2, m_fImy[1]);
	tag_setting.put<double >(SUBNODE_PT_Y3, m_fImy[2]);
	tag_setting.put<double >(SUBNODE_PT_Y4, m_fImy[3]);
	tag_setting.put<double >(SUBNODE_PT_Y5, m_fImy[4]);
	tag_setting.put<double >(SUBNODE_PT_Y6, m_fImy[5]);
	tag_setting.put<double >(SUBNODE_PT_Y7, m_fImy[6]);
	tag_setting.put<double >(SUBNODE_PT_Y8, m_fImy[7]);
	tag_setting.put<double >(SUBNODE_PT_Y9, m_fImy[8]);
	m_pt.put_child(NODE_IMAGE_PT, tag_setting); //节点//写入新的标签，如果标签名一样，会修改你前一个的内容，如果不一样就会在下面新加一个 
	
	//物理坐标
	tag_setting.clear();
	tag_setting.put<double >(SUBNODE_PT_X1, m_fSpx[0]);
	tag_setting.put<double >(SUBNODE_PT_X2, m_fSpx[1]);
	tag_setting.put<double >(SUBNODE_PT_X3, m_fSpx[2]);
	tag_setting.put<double >(SUBNODE_PT_X4, m_fSpx[3]);
	tag_setting.put<double >(SUBNODE_PT_X5, m_fSpx[4]);
	tag_setting.put<double >(SUBNODE_PT_X6, m_fSpx[5]);
	tag_setting.put<double >(SUBNODE_PT_X7, m_fSpx[6]);
	tag_setting.put<double >(SUBNODE_PT_X8, m_fSpx[7]);
	tag_setting.put<double >(SUBNODE_PT_X9, m_fSpx[8]);

	tag_setting.put<double >(SUBNODE_PT_Y1, m_fSpy[0]);
	tag_setting.put<double >(SUBNODE_PT_Y2, m_fSpy[1]);
	tag_setting.put<double >(SUBNODE_PT_Y3, m_fSpy[2]);
	tag_setting.put<double >(SUBNODE_PT_Y4, m_fSpy[3]);
	tag_setting.put<double >(SUBNODE_PT_Y5, m_fSpy[4]);
	tag_setting.put<double >(SUBNODE_PT_Y6, m_fSpy[5]);
	tag_setting.put<double >(SUBNODE_PT_Y7, m_fSpy[6]);
	tag_setting.put<double >(SUBNODE_PT_Y8, m_fSpy[7]);
	tag_setting.put<double >(SUBNODE_PT_Y9, m_fSpy[8]);
	m_pt.put_child(NODE_WORLD_PT, tag_setting);

	//仿射变换矩阵
	tag_setting.clear();
	int num = m_hv_HomMat2D.Length();
	if (6 == num)
	{
		tag_setting.put<double >(SUBNODE_AFFINE_P1, m_hv_HomMat2D[0]);
		tag_setting.put<double >(SUBNODE_AFFINE_P2, m_hv_HomMat2D[1]);
		tag_setting.put<double >(SUBNODE_AFFINE_P3, m_hv_HomMat2D[2]);
		tag_setting.put<double >(SUBNODE_AFFINE_P4, m_hv_HomMat2D[3]);
		tag_setting.put<double >(SUBNODE_AFFINE_P5, m_hv_HomMat2D[4]);
		tag_setting.put<double >(SUBNODE_AFFINE_P6, m_hv_HomMat2D[5]);
	}
	else
	{
		tag_setting.put<double >(SUBNODE_AFFINE_P1, 0.0);
		tag_setting.put<double >(SUBNODE_AFFINE_P2, 0.0);
		tag_setting.put<double >(SUBNODE_AFFINE_P3, 0.0);
		tag_setting.put<double >(SUBNODE_AFFINE_P4, 0.0);
		tag_setting.put<double >(SUBNODE_AFFINE_P5, 0.0);
		tag_setting.put<double >(SUBNODE_AFFINE_P6, 0.0);
	}
	m_pt.put_child(NODE_AFFINE, tag_setting);
	
	//旋转标定
	tag_setting.clear();
	tag_setting.put<bool>(SUBNODE_VALID, m_bIsCaliedRot);
	//图像坐标
	tag_setting.put<double >(SUBNODE_PT_X1, m_lfRotX[0]);
	tag_setting.put<double >(SUBNODE_PT_X2, m_lfRotX[1]);
	tag_setting.put<double >(SUBNODE_PT_X3, m_lfRotX[2]);
	tag_setting.put<double >(SUBNODE_PT_X4, m_lfRotX[3]);
	tag_setting.put<double >(SUBNODE_PT_X5, m_lfRotX[4]);
	tag_setting.put<double >(SUBNODE_PT_X6, m_lfRotX[5]);
	tag_setting.put<double >(SUBNODE_PT_X7, m_lfRotX[6]);
	tag_setting.put<double >(SUBNODE_PT_X8, m_lfRotX[7]);
	tag_setting.put<double >(SUBNODE_PT_X9, m_lfRotX[8]);

	tag_setting.put<double >(SUBNODE_PT_Y1, m_lfRotY[0]);
	tag_setting.put<double >(SUBNODE_PT_Y2, m_lfRotY[1]);
	tag_setting.put<double >(SUBNODE_PT_Y3, m_lfRotY[2]);
	tag_setting.put<double >(SUBNODE_PT_Y4, m_lfRotY[3]);
	tag_setting.put<double >(SUBNODE_PT_Y5, m_lfRotY[4]);
	tag_setting.put<double >(SUBNODE_PT_Y6, m_lfRotY[5]);
	tag_setting.put<double >(SUBNODE_PT_Y7, m_lfRotY[6]);
	tag_setting.put<double >(SUBNODE_PT_Y8, m_lfRotY[7]);
	tag_setting.put<double >(SUBNODE_PT_Y9, m_lfRotY[8]);
	//半径，圆心
	tag_setting.put<double >(SUBNODE_ROT_CENT_X, m_lfCentX);
	tag_setting.put<double >(SUBNODE_ROT_CENT_Y, m_lfCentY);
	tag_setting.put<double >(SUBNODE_ROT_RADIUS, m_lfRadiu);

	m_pt.put_child(NODE_ROT, tag_setting);

	write_ini(pPath, m_pt);

	return true;
}
//加载9点位置和仿射矩阵和旋转标定
bool ImgProcess::LoadCalibrate9(char * pPath)
{
	ptree m_pt, tag_setting;
	read_ini(pPath, m_pt);
	
	try
	{
	//图像坐标
		tag_setting = m_pt.get_child(NODE_IMAGE_PT);	
		if (tag_setting.size())
		{
			m_bIsCalied = tag_setting.get<bool>(SUBNODE_VALID, false);
			if (!m_bIsCalied) return true;

			m_fImx[0] = tag_setting.get<double>(SUBNODE_PT_X1, 0.0);
			m_fImx[1] = tag_setting.get<double>(SUBNODE_PT_X2, 0.0);
			m_fImx[2] = tag_setting.get<double>(SUBNODE_PT_X3, 0.0);
			m_fImx[3] = tag_setting.get<double>(SUBNODE_PT_X4, 0.0);
			m_fImx[4] = tag_setting.get<double>(SUBNODE_PT_X5, 0.0);
			m_fImx[5] = tag_setting.get<double>(SUBNODE_PT_X6, 0.0);
			m_fImx[6] = tag_setting.get<double>(SUBNODE_PT_X7, 0.0);
			m_fImx[7] = tag_setting.get<double>(SUBNODE_PT_X8, 0.0);
			m_fImx[8] = tag_setting.get<double>(SUBNODE_PT_X9, 0.0);

			m_fImy[0] = tag_setting.get<double>(SUBNODE_PT_Y1, 0.0);
			m_fImy[1] = tag_setting.get<double>(SUBNODE_PT_Y2, 0.0);
			m_fImy[2] = tag_setting.get<double>(SUBNODE_PT_Y3, 0.0);
			m_fImy[3] = tag_setting.get<double>(SUBNODE_PT_Y4, 0.0);
			m_fImy[4] = tag_setting.get<double>(SUBNODE_PT_Y5, 0.0);
			m_fImy[5] = tag_setting.get<double>(SUBNODE_PT_Y6, 0.0);
			m_fImy[6] = tag_setting.get<double>(SUBNODE_PT_Y7, 0.0);
			m_fImy[7] = tag_setting.get<double>(SUBNODE_PT_Y8, 0.0);
			m_fImy[8] = tag_setting.get<double>(SUBNODE_PT_Y9, 0.0);
		}
	
		//物理坐标
		tag_setting = m_pt.get_child(NODE_WORLD_PT);
		if (tag_setting.size())
		{
			m_fSpx[0] = tag_setting.get<double>(SUBNODE_PT_X1, 0.0);
			m_fSpx[1] = tag_setting.get<double>(SUBNODE_PT_X2, 0.0);
			m_fSpx[2] = tag_setting.get<double>(SUBNODE_PT_X3, 0.0);
			m_fSpx[3] = tag_setting.get<double>(SUBNODE_PT_X4, 0.0);
			m_fSpx[4] = tag_setting.get<double>(SUBNODE_PT_X5, 0.0);
			m_fSpx[5] = tag_setting.get<double>(SUBNODE_PT_X6, 0.0);
			m_fSpx[6] = tag_setting.get<double>(SUBNODE_PT_X7, 0.0);
			m_fSpx[7] = tag_setting.get<double>(SUBNODE_PT_X8, 0.0);
			m_fSpx[8] = tag_setting.get<double>(SUBNODE_PT_X9, 0.0);

			m_fSpy[0] = tag_setting.get<double>(SUBNODE_PT_Y1, 0.0);
			m_fSpy[1] = tag_setting.get<double>(SUBNODE_PT_Y2, 0.0);
			m_fSpy[2] = tag_setting.get<double>(SUBNODE_PT_Y3, 0.0);
			m_fSpy[3] = tag_setting.get<double>(SUBNODE_PT_Y4, 0.0);
			m_fSpy[4] = tag_setting.get<double>(SUBNODE_PT_Y5, 0.0);
			m_fSpy[5] = tag_setting.get<double>(SUBNODE_PT_Y6, 0.0);
			m_fSpy[6] = tag_setting.get<double>(SUBNODE_PT_Y7, 0.0);
			m_fSpy[7] = tag_setting.get<double>(SUBNODE_PT_Y8, 0.0);
			m_fSpy[8] = tag_setting.get<double>(SUBNODE_PT_Y9, 0.0);
		}

		//仿射变换
		tag_setting = m_pt.get_child(NODE_AFFINE);

		m_hv_HomMat2D.Clear();
		if (tag_setting.size())
		{
			m_hv_HomMat2D[0] = tag_setting.get<double>(SUBNODE_AFFINE_P1, 0.0);
			m_hv_HomMat2D[1] = tag_setting.get<double>(SUBNODE_AFFINE_P2, 0.0);
			m_hv_HomMat2D[2] = tag_setting.get<double>(SUBNODE_AFFINE_P3, 0.0);
			m_hv_HomMat2D[3] = tag_setting.get<double>(SUBNODE_AFFINE_P4, 0.0);
			m_hv_HomMat2D[4] = tag_setting.get<double>(SUBNODE_AFFINE_P5, 0.0);
			m_hv_HomMat2D[5] = tag_setting.get<double>(SUBNODE_AFFINE_P6, 0.0);
		}

		//旋转标定
		tag_setting.clear();
		tag_setting = m_pt.get_child(NODE_ROT);
		if (tag_setting.size())
		{
			m_bIsCaliedRot = tag_setting.get<bool>(SUBNODE_VALID, false);
			if (!m_bIsCaliedRot) return true;

			m_lfRotX[0] = tag_setting.get<double>(SUBNODE_PT_X1, 0.0);
			m_lfRotX[1] = tag_setting.get<double>(SUBNODE_PT_X2, 0.0);
			m_lfRotX[2] = tag_setting.get<double>(SUBNODE_PT_X3, 0.0);
			m_lfRotX[3] = tag_setting.get<double>(SUBNODE_PT_X4, 0.0);
			m_lfRotX[4] = tag_setting.get<double>(SUBNODE_PT_X5, 0.0);
			m_lfRotX[5] = tag_setting.get<double>(SUBNODE_PT_X6, 0.0);
			m_lfRotX[6] = tag_setting.get<double>(SUBNODE_PT_X7, 0.0);
			m_lfRotX[7] = tag_setting.get<double>(SUBNODE_PT_X8, 0.0);
			m_lfRotX[8] = tag_setting.get<double>(SUBNODE_PT_X9, 0.0);

			m_lfRotY[0] = tag_setting.get<double>(SUBNODE_PT_Y1, 0.0);
			m_lfRotY[1] = tag_setting.get<double>(SUBNODE_PT_Y2, 0.0);
			m_lfRotY[2] = tag_setting.get<double>(SUBNODE_PT_Y3, 0.0);
			m_lfRotY[3] = tag_setting.get<double>(SUBNODE_PT_Y4, 0.0);
			m_lfRotY[4] = tag_setting.get<double>(SUBNODE_PT_Y5, 0.0);
			m_lfRotY[5] = tag_setting.get<double>(SUBNODE_PT_Y6, 0.0);
			m_lfRotY[6] = tag_setting.get<double>(SUBNODE_PT_Y7, 0.0);
			m_lfRotY[7] = tag_setting.get<double>(SUBNODE_PT_Y8, 0.0);
			m_lfRotY[8] = tag_setting.get<double>(SUBNODE_PT_Y9, 0.0);

			//半径，圆心
			m_lfCentX = tag_setting.get<double>(SUBNODE_ROT_CENT_X, 0.0);
			m_lfCentY = tag_setting.get<double>(SUBNODE_ROT_CENT_Y, 0.0);
			m_lfRadiu = tag_setting.get<double>(SUBNODE_ROT_RADIUS, 0.0);
		}
	}
	catch (...)
	{
		AfxMessageBox(_T("加载标定文件失败"));
		return false;
	}

	return true;
}
//框选
bool ImgProcess::DrawMyRectangle(HalconCpp::HTuple HWindowID, HalconCpp::HTuple & RowTmp, HalconCpp::HTuple & ColumnTmp, HalconCpp::HTuple & PhiTmp, HalconCpp::HTuple & Length1Tmp, HalconCpp::HTuple & Length2Tmp)
{
	HalconCpp::HObject ho_RoiTmp;
	DrawRectangle2(HWindowID, &RowTmp, &ColumnTmp, &PhiTmp, &Length1Tmp, &Length2Tmp);
	if (Length1Tmp < 10 || Length2Tmp < 10)
		return false;
	HalconCpp::GenRectangle2(&ho_RoiTmp, RowTmp, ColumnTmp, PhiTmp, Length1Tmp, Length2Tmp);
	HalconCpp::DispObj(ho_RoiTmp, HWindowID);
	return true;
}
bool ImgProcess::DrawMyRectangle1(HalconCpp::HTuple HWindowID, HalconCpp::HTuple & RowTmp1, HalconCpp::HTuple & ColumnTmp1, HalconCpp::HTuple & RowTmp2, HalconCpp::HTuple & ColumnTmp2)
{
	HalconCpp::HObject ho_RoiTmp;
	HalconCpp::DrawRectangle1(HWindowID, &RowTmp1, &ColumnTmp1, &RowTmp2, &ColumnTmp2);
	HTuple w = ColumnTmp2 - ColumnTmp1;
	HTuple h = RowTmp2 - RowTmp1;
	if (w < 10 || h < 10)
		return false;
	HalconCpp::GenRectangle1(&ho_RoiTmp, RowTmp1, ColumnTmp1, RowTmp2, ColumnTmp2);
	HalconCpp::DispObj(ho_RoiTmp, HWindowID);
	return true;
}
bool ImgProcess::Calibration()
{
	//将圆点标定板大致摆成如下的方向（倒三角）（不需要绝对平行）， 
	//理想状态下，3点坐标如下。
	/*
		(圆1(0,0))------(圆2(6,0))
		  \				/
		   \		   /
			\         /
			 \       /
			  \     /
			  (圆3（3, -5.19615））
	*/
	if (IDOK != AfxMessageBox(_T("确定开始标定过程?（请做好标定数据的备份）"), MB_OKCANCEL))
	{
		SetColor(WindowHandle, "red");
		WriteString(WindowHandle, "Abort");
		return false;
	}

	//1.拍照
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	g_criSection.Lock();
	m_pCam->GrabOneImage(&pData, stImageInfo);
	g_criSection.Unlock();
	if (!pData)
	{
		//g_criSection.Unlock(); //解锁
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Camera Error");
		}
		return false;
	}

	try
	{
		ho_Image.Clear();
		ConvertoHalcon(pData, stImageInfo, ho_Image);

		//g_criSection.Unlock(); //解锁
		if (!WindowHandle.Length())
			return false;
		//ReadImage(&ho_Image, "barcode.bmp");
		DispObj(ho_Image, WindowHandle);
	
		HObject ho_ROI_1, ho_ROI_2, ho_ROI_3;
		HTuple Cir1_RowY, Cir2_RowY, Cir3_RowY, Cir1_ColX, Cir2_ColX, Cir3_ColX;
		HTuple Radius;
		//框选圆1并计算圆心
		HTuple Roi1_Row, Roi1_Column, Roi1_Phi, Roi1_Length1, Roi1_Length2;
	
		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw a region1");

		if (!DrawMyRectangle(WindowHandle, Roi1_Row, Roi1_Column, Roi1_Phi, Roi1_Length1, Roi1_Length2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle2(&ho_ROI_1, Roi1_Row, Roi1_Column, Roi1_Phi, Roi1_Length1, Roi1_Length2);
		if (FindMyCircle(ho_Image, ho_ROI_1, Cir1_RowY, Cir1_ColX, Radius))
		{
			//显示
			HObject CircleTmp;
			GenCircle(&CircleTmp, Cir1_RowY, Cir1_ColX, Radius);
			DispObj(CircleTmp, WindowHandle);
			WriteString(WindowHandle, " Successed");
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
		}


		//框选圆2并计算圆心
		HTuple Roi2_Row, Roi2_Column, Roi2_Phi, Roi2_Length1, Roi2_Length2;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow+150), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw a region2");

		if (!DrawMyRectangle(WindowHandle, Roi2_Row, Roi2_Column, Roi2_Phi, Roi2_Length1, Roi2_Length2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle2(&ho_ROI_2, Roi2_Row, Roi2_Column, Roi2_Phi, Roi2_Length1, Roi2_Length2);
		if (FindMyCircle(ho_Image, ho_ROI_2, Cir2_RowY, Cir2_ColX, Radius))
		{
			//显示
			HObject CircleTmp;
			GenCircle(&CircleTmp, Cir2_RowY, Cir2_ColX, Radius);
			DispObj(CircleTmp, WindowHandle);
			WriteString(WindowHandle, " Successed");
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
			return false;
		}

		//框选圆3并计算圆心
		HTuple Roi3_Row, Roi3_Column, Roi3_Phi, Roi3_Length1, Roi3_Length2;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow+300), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw a region3");

		if (!DrawMyRectangle(WindowHandle, Roi3_Row, Roi3_Column, Roi3_Phi, Roi3_Length1, Roi3_Length2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle2(&ho_ROI_3, Roi3_Row, Roi3_Column, Roi3_Phi, Roi3_Length1, Roi3_Length2);
		if (FindMyCircle(ho_Image, ho_ROI_3, Cir3_RowY, Cir3_ColX, Radius))
		{
			//显示
			HObject CircleTmp;
			GenCircle(&CircleTmp, Cir3_RowY, Cir3_ColX, Radius);
			DispObj(CircleTmp, WindowHandle);
			WriteString(WindowHandle, " Successed");
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
			return false;
		}
		if (IDOK != AfxMessageBox(_T("确定是否执行标定过程?（请做好标定数据的备份）"), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}

		//计算圆1圆2的连线的角度
		HTuple lineAngle = 0.0;
		HTuple lineAngle2 = 0.0;
		AngleLx(Cir1_RowY, Cir1_ColX, Cir2_RowY, Cir2_ColX, &lineAngle);
		lineAngle = -lineAngle; //得出的角度是到x轴所需的角度， 故为-angle
		AngleLx(Cir1_RowY, Cir1_ColX, Cir3_RowY, Cir3_ColX, &lineAngle2);
		lineAngle2 = -lineAngle2; //得出的角度是到x轴所需的角度， 故为-angle2

		//设圆1的物理坐标为（0，0）,其他圆的起始坐标如下， 根据上述角度计算出其他两个圆的物理坐标
		HTuple hv_HomMat2DIdentity, hv_HomMat2DRotate;
		double x1 = 0.0;
		double y1 = 0.0;

		double x2 = 6.0;
		double y2 = 0.0;

		double x3 = 3.0;
		double y3 = -5.19615;

		HTuple res_x = 0.0;
		HTuple res_y = 0.0;
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dRotateLocal(hv_HomMat2DIdentity, lineAngle, &hv_HomMat2DRotate);
		AffineTransPoint2d(hv_HomMat2DRotate, x2-x1, y2-y1, &res_x, &res_y);
		x2 = res_x.D();
		y2 = res_y.D();

		hv_HomMat2DRotate.Clear();
		HomMat2dRotateLocal(hv_HomMat2DIdentity, lineAngle2, &hv_HomMat2DRotate);
		AffineTransPoint2d(hv_HomMat2DRotate, x3 - x1, y3 - y1, &res_x, &res_y);
		x3 = res_x.D();
		y3 = res_y.D();

		//将像素坐标3个， 对应物理3个传入标定函数
		double fImx[CALIBRAT_PT_NUM];
		double fImy[CALIBRAT_PT_NUM];
		double fSpx[CALIBRAT_PT_NUM];
		double fSpy[CALIBRAT_PT_NUM];

		fImx[0] = Cir1_ColX.D();
		fImx[1] = Cir2_ColX.D();
		fImx[2] = Cir3_ColX.D();
		fImy[0] = Cir1_RowY.D();
		fImy[1] = Cir2_RowY.D();
		fImy[2] = Cir3_RowY.D();
		for (int i = 3; i < CALIBRAT_PT_NUM; i++)
		{
			fImx[i] = Cir3_ColX.D();
			fImy[i] = Cir3_RowY.D();
		}
		fSpx[0] = x1;
		fSpx[1] = x2;
		fSpx[2] = x3;
		fSpy[0] = y1;
		fSpy[1] = y2;
		fSpy[2] = y3;
		for (int i = 3; i < CALIBRAT_PT_NUM; i++)
		{
			fSpx[i] = x3;
			fSpy[i] = y3;
		}

		if (Calibrate9(fImx, fImy, fSpx, fSpy))
		{
			//保存标定
			SaveCalibrate9(PATH_CALIBRATE);
			SetTposition(WindowHandle, HTuple(m_TextRow + 450), HTuple(m_TextCol));
			WriteString(WindowHandle, "Calibrate Successed. Done");
		}
		else
		{
			SetColor(WindowHandle, "red");
			SetTposition(WindowHandle, HTuple(m_TextRow + 450), HTuple(m_TextCol));
			WriteString(WindowHandle, "Calibrate Failed. Try again");
			return false;
		}
	}
	catch (HalconCpp::HException & except)
	{
		//g_criSection.Unlock(); //解锁
		ShowException(except);
		SetColor(WindowHandle, "red");
		WriteString(WindowHandle, "Cablirate Error");
		return false;
	}
	
	return true;
}

bool ImgProcess::CalibrationStep(int ptIndex, double Spx, double Spy)
{
	if (ptIndex < 0) return false;
	if (ptIndex >= CALIBRAT_PT_NUM) return false;

	if (0 == ptIndex)
	{
		m_ho_Circles.Clear();
		GenEmptyRegion(&m_ho_Circles);
	}
	//拍照框选，并提取圆心
	//1.拍照
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	g_criSection.Lock();
	m_pCam->GrabOneImage(&pData, stImageInfo);
	g_criSection.Unlock();
	if (!pData)
	{
		//g_criSection.Unlock(); //解锁
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Camera Error");
		}
		return false;
	}
	try
	{
		ho_Image.Clear();
		ConvertoHalcon(pData, stImageInfo, ho_Image);
		//g_criSection.Unlock(); //解锁
		if (!WindowHandle.Length())
			return false;
		//ReadImage(&ho_Image, "barcode.bmp");
		HalconCpp::DispObj(ho_Image, WindowHandle);
		HTuple num;
		if (m_ho_Circles.IsInitialized())
		{
			CountObj(m_ho_Circles, &num);
			if (num.I())  //显示识别到的圆
			{
				DispObj(m_ho_Circles, WindowHandle);
			}
		}
		else
		{
			GenEmptyRegion(&m_ho_Circles);
		}

		HObject ho_ROI_1;
		HTuple Cir1_RowY, Cir1_ColX;
		HTuple Radius;
		//框选圆1并计算圆心
		HTuple Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		char p[1024];
		sprintf_s(p, "Please Draw a region %d", ptIndex + 1);
		WriteString(WindowHandle, p);
		if (IDOK != AfxMessageBox(_T("请框选目标圆点,右键完成框选。点击取消则中断标定 "), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}
		if (!DrawMyRectangle1(WindowHandle, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle1(&ho_ROI_1, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2);
		if (FindMyCircle(ho_Image, ho_ROI_1, Cir1_RowY, Cir1_ColX, Radius))
		{
			//显示
			HalconCpp::HObject CircleTmp, CrossTmp;
			HalconCpp::GenCircle(&CircleTmp, Cir1_RowY, Cir1_ColX, Radius);
			HalconCpp::ConcatObj(m_ho_Circles, CircleTmp, &m_ho_Circles);
			HalconCpp::GenCrossContourXld(&CrossTmp, Cir1_RowY, Cir1_ColX, 40, 0);
			HalconCpp::DispObj(CrossTmp, WindowHandle);
			HalconCpp::DispObj(CircleTmp, WindowHandle);
			HalconCpp::WriteString(WindowHandle, " Successed");
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, " Failed, try again.");
			return false;
		}

		if (!Cir1_RowY.Length())
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
			return false;
		}

		m_fImxTmp[ptIndex] = Cir1_ColX.D();
		m_fImyTmp[ptIndex] = Cir1_RowY.D();
		m_fSpxTmp[ptIndex] = Spx;
		m_fSpyTmp[ptIndex] = Spy;
		if (ptIndex < CALIBRAT_PT_NUM - 1)
			return true;

		if (IDOK != AfxMessageBox(_T("确定是否执行标定过程?（请做好标定数据的备份）"), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}
		//如果已经达到9个点，开始标定
		if (Calibrate9(m_fImxTmp, m_fImyTmp, m_fSpxTmp, m_fSpyTmp))
		{
			//临时变量转移到真实变量
			for (int i = 0; i < CALIBRAT_PT_NUM; i++)
			{
				m_fImx[i] = m_fImxTmp[i];
				m_fImy[i] = m_fImyTmp[i]; 
				m_fSpx[i] = m_fSpxTmp[i]; 
				m_fSpy[i] = m_fSpyTmp[i]; 
			}
			//保存标定
			SaveCalibrate9(PATH_CALIBRATE);
			SetTposition(WindowHandle, HTuple(m_TextRow + 100), HTuple(m_TextCol));
			WriteString(WindowHandle, "Calibrate Successed. Done");
		}
		else
		{
			SetColor(WindowHandle, "red");
			SetTposition(WindowHandle, HTuple(m_TextRow + 100), HTuple(m_TextCol));
			HalconCpp::WriteString(WindowHandle, "Calibrate Failed. Try again");
			return false;
		}
	}
	catch (HalconCpp::HException & except)
	{
		//g_criSection.Unlock(); //解锁
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::CheckCalibrate(double & Spx, double & Spy)
{
	//if (!m_bIsCalied) //未完成标定则返回false;
	//	return false;
	//拍照
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	g_criSection.Lock();
	m_pCam->GrabOneImage(&pData, stImageInfo);
	g_criSection.Unlock();
	if (!pData)
	{
		//g_criSection.Unlock(); //解锁
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Camera Error");
		}
		return false;
	}
	//框选
	try
	{
		ho_Image.Clear();
		ConvertoHalcon(pData, stImageInfo, ho_Image);
		//g_criSection.Unlock(); //解锁
		if (!WindowHandle.Length())
			return false;
		//ReadImage(&ho_Image, "barcode.bmp");
		HalconCpp::DispObj(ho_Image, WindowHandle);
		HTuple num;
		if (m_ho_Circles.IsInitialized())
		{
			CountObj(m_ho_Circles, &num);
			if (num.I())  //显示识别到的圆
			{
				DispObj(m_ho_Circles, WindowHandle);
			}
		}
		else
		{
			GenEmptyRegion(&m_ho_Circles);
		}

		HObject ho_ROI_1;
		HTuple Cir1_RowY, Cir1_ColX;
		HTuple Radius;
		//框选圆1并计算圆心
		HTuple Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please draw a retangle.");
		if (IDOK != AfxMessageBox(_T("请框选目标圆点,右键完成框选。点击取消则中断标定 "), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}
		if (!DrawMyRectangle1(WindowHandle, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle1(&ho_ROI_1, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2);
		if (FindMyCircle(ho_Image, ho_ROI_1, Cir1_RowY, Cir1_ColX, Radius))
		{
			//显示
			HalconCpp::HObject CircleTmp, CrossTmp;
			HalconCpp::GenCircle(&CircleTmp, Cir1_RowY, Cir1_ColX, Radius);
			HalconCpp::ConcatObj(m_ho_Circles, CircleTmp, &m_ho_Circles);
			HalconCpp::GenCrossContourXld(&CrossTmp, Cir1_RowY, Cir1_ColX, 40, 0);
			HalconCpp::DispObj(CrossTmp, WindowHandle);
			HalconCpp::DispObj(CircleTmp, WindowHandle);
			HalconCpp::WriteString(WindowHandle, " Successed");
			SetTposition(WindowHandle, HTuple(m_TextRow + 150), HTuple(m_TextCol + 150));
			WriteString(WindowHandle, "cent(x,y):" + HTuple(Cir1_RowY).TupleString("3.2f") + "," + HTuple(Cir1_ColX).TupleString("3.2f"));
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, " Failed, try again.");
			return false;
		}

		if (!Cir1_RowY.Length())
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
			return false;
		}

		//转为物理坐标
		double lfx = Cir1_ColX.D();
		double lfy = Cir1_RowY.D();
		Image2World(&lfx, &lfy, &Spx, &Spy);

		return true;

	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}


	return true;
}

bool ImgProcess::CalibrateRotStep(int ptIndex)
{
	if (ptIndex < 0) return false;
	if (ptIndex >= CALIBRAT_PT_NUM) return false;

	if (0 == ptIndex)
	{
		m_ho_Circles.Clear();
		GenEmptyRegion(&m_ho_Circles);
	}
	//拍照框选，并提取圆心
	//1.拍照
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	g_criSection.Lock();
	m_pCam->GrabOneImage(&pData, stImageInfo);
	g_criSection.Unlock();
	if (!pData)
	{
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Camera Error");
		}
		return false;
	}
	try
	{
		ho_Image.Clear();
		ConvertoHalcon(pData, stImageInfo, ho_Image);
		if (!WindowHandle.Length())
			return false;
		//ReadImage(&ho_Image, "barcode.bmp");
		HalconCpp::DispObj(ho_Image, WindowHandle);
		HTuple num;
		if (m_ho_Circles.IsInitialized())
		{
			CountObj(m_ho_Circles, &num);
			if (num.I())  //显示识别到的圆
			{
				DispObj(m_ho_Circles, WindowHandle);
			}
		}
		else
		{
			GenEmptyRegion(&m_ho_Circles);
		}

		HObject ho_ROI_1;
		HTuple Cir1_RowY, Cir1_ColX;
		HTuple Radius;
		//框选圆1并计算圆心
		HTuple Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		char p[1024];
		sprintf_s(p, "Please Draw a region %d", ptIndex + 1);
		WriteString(WindowHandle, p);
		if (IDOK != AfxMessageBox(_T("请框选目标圆点,右键完成框选。点击取消则中断标定 "), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}
		if (!DrawMyRectangle1(WindowHandle, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, too small.");
			return false;
		}
		GenRectangle1(&ho_ROI_1, Roi1_Row1, Roi1_Column1, Roi1_Row2, Roi1_Column2);
		if (FindMyCircle(ho_Image, ho_ROI_1, Cir1_RowY, Cir1_ColX, Radius))
		{
			//显示
			HalconCpp::HObject CircleTmp, CrossTmp;
			HalconCpp::GenCircle(&CircleTmp, Cir1_RowY, Cir1_ColX, Radius);
			HalconCpp::ConcatObj(m_ho_Circles, CircleTmp, &m_ho_Circles);
			HalconCpp::GenCrossContourXld(&CrossTmp, Cir1_RowY, Cir1_ColX, 40, 0);
			HalconCpp::DispObj(CrossTmp, WindowHandle);
			HalconCpp::DispObj(CircleTmp, WindowHandle);
			HalconCpp::WriteString(WindowHandle, " Successed");
		}
		else
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, " Failed, try again.");
			return false;
		}

		if (!Cir1_RowY.Length())
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Failed, try again.");
			return false;
		}

		m_lfRotX[ptIndex] = Cir1_ColX.D();
		m_lfRotY[ptIndex] = Cir1_RowY.D();
		if (ptIndex < CALIBRAT_PT_NUM - 1)
			return true;

		if (IDOK != AfxMessageBox(_T("确定是否执行旋转标定过程?（请做好标定数据的备份）"), MB_OKCANCEL))
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Abort");
			return false;
		}
		//如果已经达到9个点，开始标定
		if (CalibrateRot(m_fImx, m_fImy, CALIBRAT_PT_NUM))
		{
			//保存标定
			SaveCalibrate9(PATH_CALIBRATE);
			SetTposition(WindowHandle, HTuple(m_TextRow + 100), HTuple(m_TextCol));
			WriteString(WindowHandle, "Calibrate Successed. Done");
		}
		else
		{
			SetColor(WindowHandle, "red");
			SetTposition(WindowHandle, HTuple(m_TextRow + 100), HTuple(m_TextCol));
			HalconCpp::WriteString(WindowHandle, "Calibrate Failed. Try again");
			return false;
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

//返回圆心半径
bool ImgProcess::FindMyCircle(HObject srcImg, HObject regionROI, HTuple & RowY, HTuple & ColX, HTuple & Radius)
{
	//
	HTuple hv_Radius1, hv_StartPhi1, hv_EndPhi1, hv_PointOrder1, hv_Number;
	HObject ho_ImageReduced1, ho_Region, ho_RegionFillUp, ho_Contours1, ho_Selected;
	HObject ho_ConnectedRegions, ho_SelectedRegions, ho_RegionTrans;
	ReduceDomain(srcImg, regionROI, &ho_ImageReduced1);
	//Threshold(ho_ImageReduced1, &ho_Region, 0, 117);
	HTuple hvThr;
	BinaryThreshold(ho_ImageReduced1, &ho_Region, "max_separability", "dark", &hvThr);
	FillUp(ho_Region, &ho_RegionFillUp);
	Connection(ho_RegionFillUp, &ho_ConnectedRegions);
	SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", 200000, 500000);
	CountObj(ho_SelectedRegions, &hv_Number);
	if (0 != (hv_Number == 0))
	{
		return false;
	}
	else if (0 == (hv_Number == 1))
	{	
		return false;
	}
	ShapeTrans(ho_SelectedRegions, &ho_RegionTrans, "convex");
	GenContourRegionXld(ho_RegionTrans, &ho_Contours1, "center");
	DispObj(ho_Contours1, WindowHandle);
	SelectObj(ho_Contours1, &ho_Selected, 1);
	FitCircleContourXld(ho_Selected, "algebraic", -1, 0, 0, 3, 2, &RowY, &ColX,
		&Radius, &hv_StartPhi1, &hv_EndPhi1, &hv_PointOrder1);
	if (!hv_StartPhi1.Length())
		return false;
	return true;
}

void ImgProcess::Image2World(double * Imx, double * Imy, double * Spx, double * Spy)
{
	*Spx = 0.0;
	*Spy = 0.0;
	if (!m_bIsCalied)
	{
		return;
	}

	//仿射变换
	HTuple hv_Qx, hv_Qy;
	if (!m_hv_HomMat2D.Length()) return;
	AffineTransPoint2d(m_hv_HomMat2D, *Imx, *Imy, &hv_Qx, &hv_Qy);
	*Spx = hv_Qx.D();
	*Spy = hv_Qy.D();

	return;
}

bool ImgProcess::SavePicture(char * path)
{
	if (!ho_Image.IsInitialized()) return false;
	try
	{
		WriteImage(ho_Image, "bmp", 0, path);
	}
	catch (HalconCpp::HException & except)
	{
		//ShowException(except);
		return false;
	}
	return true;
}

void ImgProcess::SetFullScreen(bool bFull)
{
	if (!WindowHandle.Length()) return;
	if (bFull)
	{
		SetPart(WindowHandle, 0, 0, IMG_H - 1, IMG_W - 1);
		m_TextRow = 10;
		m_TextCol = 10;
		m_TextRowBarcode = IMG_H - 150;
		m_TextColBarcode = 10;
	}
	else
	{
		if (m_ModelDetectArea.bAvalid)
		{
			//
			SetPart(WindowHandle,
				HTuple(m_ModelDetectArea.row1),
				HTuple(m_ModelDetectArea.col1),
				HTuple(m_ModelDetectArea.row2),
				HTuple(m_ModelDetectArea.col2));
			m_TextRow = m_ModelDetectArea.row1 + 10;
			m_TextCol = m_ModelDetectArea.col1 + 10;
			m_TextRowBarcode = m_ModelDetectArea.row2 - 150;
			m_TextColBarcode = m_ModelDetectArea.col1 + 10;
		}
	}


}

void ImgProcess::CloseCamera()
{
	m_pCam->CloseDevice();
}

void ImgProcess::LoadImageFromFile()
{
	char* strTemp = NULL;
	//打开文件选择窗口
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("位图文件(*.BMP)|*.BMP|jpg文件(*.jpg)|*.jpg||"));
	if (IDOK == dlg.DoModal())
	{
		CString imgpath;
		imgpath.Format(_T("%s"), dlg.GetPathName());
		int len = WideCharToMultiByte(CP_ACP, 0, imgpath, -1, NULL, 0, NULL, NULL);//宽字符转
		strTemp = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, imgpath, -1, strTemp, len, NULL, NULL);
		try
		{
			HalconCpp::ReadImage(&ho_Image, strTemp);
			DispObj(ho_Image, WindowHandle);
		}
		catch (HalconCpp::HException & except)
		{
			if (strTemp)
			{
				delete []strTemp;
				strTemp = NULL;
			}
			ShowException(except);
			return ;
		}
		if (strTemp)
		{
			delete []strTemp;
			strTemp = NULL;
		}
	}
	else
	{
		return;
	}

	return;
}

void ImgProcess::SetManuMode(bool bMan)
{
	m_bManu = bMan;
}

bool ImgProcess::DrawBaseMark()
{
	//
	try
	{
		//显示图像
		if (!WindowHandle.Length()) return false;
		ClearWindow(WindowHandle);
		if (ho_Image.IsInitialized())
			DispObj(ho_Image, WindowHandle);

		//框选检测范围区域
		int idex = 0;
		int idex2 = 0;
		for (; idex < 2; idex++)
		{
			CString strshow;
			strshow.Format(_T("请框选第%d个检测范围,[确定]：开始框选，右键结束。 [取消]：则中断框选流程。"), idex + 1);
			if (IDOK != AfxMessageBox(strshow, MB_OKCANCEL))
			{
				return false;
			}
			HalconCpp::HTuple Row1, Col1, Row2, Col2;
			SetColor(WindowHandle, "green");
			SetDraw(WindowHandle, "margin");
			SetTposition(WindowHandle, HTuple(m_TextRow+150*(idex+ idex2)), HTuple(m_TextCol));
			WriteString(WindowHandle, "请框选检测范围区域(尽量覆盖可能出现模板的区域)");

			HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
			RectArea areaTmp;
			if (!Row1.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol+10));
				WriteString(WindowHandle, "框选太小，无法处理");
				return false;
			}
			areaTmp.row1 = (double)Row1.D();
			areaTmp.col1 = (double)Col1.D();
			areaTmp.row2 = (double)Row2.D();
			areaTmp.col2 = (double)Col2.D();
			int w = areaTmp.col2 - areaTmp.col1;
			int h = areaTmp.row2 - areaTmp.row1;
			if (w < 10 || h < 10) //框选范围太小
			{
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol+10));
				WriteString(WindowHandle, "框选太小，无法处理");
				return false;
			}

			GenRectangle1(&m_ho_BaseROI[idex], Row1, Col1, Row2, Col2);
			DispObj(m_ho_BaseROI[idex], WindowHandle);
			areaTmp.bAvalid = true;
			m_BaseROI[idex] = areaTmp;

			//SaveDetectArea(PATH_DETECTAREA);
			SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol+10));
			WriteString(WindowHandle, "成功");

			//框选第n个模板
			for (; idex2 < 2; idex2++)
			{
				strshow.Format(_T("请框选第%d个模板,[确定]：开始框选，右键结束。 [取消]：则中断框选流程。"), idex2 + 1);
				if (IDOK != AfxMessageBox(strshow, MB_OKCANCEL))
				{
					return false;
				}
				SetColor(WindowHandle, "green");
				SetDraw(WindowHandle, "margin");
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol));
				WriteString(WindowHandle, "请框选模板(mark点)");

				HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
				RectArea areaTmp;
				if (!Row1.Length())
				{
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2+1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "框选太小，无法处理");
					return false;
				}
				areaTmp.row1 = (double)Row1.D();
				areaTmp.col1 = (double)Col1.D();
				areaTmp.row2 = (double)Row2.D();
				areaTmp.col2 = (double)Col2.D();
				int w = areaTmp.col2 - areaTmp.col1;
				int h = areaTmp.row2 - areaTmp.row1;
				if (w < 10 || h < 10) //框选范围太小
				{
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2 + 1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "框选太小，无法处理");
					return false;
				}
				
				//保存和显示
				GenRectangle1(&m_ho_BaseModel[idex2], Row1, Col1, Row2, Col2);
				DispObj(m_ho_BaseModel[idex2], WindowHandle);
				areaTmp.bAvalid = true;
				m_BaseModel[idex2] = areaTmp; 

				//制作模板
				if (!CreateMarkModel(m_ho_BaseModel[idex2], m_hv_BaseModelId[idex2]))
				{
					AfxMessageBox(_T("创建Mark模板失败"));
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2 + 1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "创建Mark模板失败");
					return false;
				}

				//SaveDetectArea(PATH_DETECTAREA);
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol + 10));
				WriteString(WindowHandle, "成功");
			}
		}

	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::DrawDutMark()
{
	try
	{
		//显示图像
		if (!WindowHandle.Length()) return false;
		ClearWindow(WindowHandle);
		if (ho_Image.IsInitialized())
			DispObj(ho_Image, WindowHandle);

		//框选检测范围区域
		int idex = 0;
		int idex2 = 0;
		for (; idex < 2; idex++)
		{
			CString strshow;
			strshow.Format(_T("请框选第%d个检测范围,[确定]：开始框选，右键结束。 [取消]：则中断框选流程。"), idex + 1);
			if (IDOK != AfxMessageBox(strshow, MB_OKCANCEL))
			{
				return false;
			}
			HalconCpp::HTuple Row1, Col1, Row2, Col2;
			SetColor(WindowHandle, "green");
			SetDraw(WindowHandle, "margin");
			SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol));
			WriteString(WindowHandle, "请框选检测范围区域(尽量覆盖可能出现模板的区域)");

			HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
			RectArea areaTmp;
			if (!Row1.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol + 10));
				WriteString(WindowHandle, "框选太小，无法处理");
				return false;
			}
			areaTmp.row1 = (double)Row1.D();
			areaTmp.col1 = (double)Col1.D();
			areaTmp.row2 = (double)Row2.D();
			areaTmp.col2 = (double)Col2.D();
			int w = areaTmp.col2 - areaTmp.col1;
			int h = areaTmp.row2 - areaTmp.row1;
			if (w < 10 || h < 10) //框选范围太小
			{
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol + 10));
				WriteString(WindowHandle, "框选太小，无法处理");
				return false;
			}

			GenRectangle1(&m_ho_DutROI[idex], Row1, Col1, Row2, Col2);
			DispObj(m_ho_DutROI[idex], WindowHandle);
			areaTmp.bAvalid = true;
			m_DutROI[idex] = areaTmp;

			//SaveDetectArea(PATH_DETECTAREA);
			SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol + 10));
			WriteString(WindowHandle, "成功");

			//框选第n个模板
			for (; idex2 < 2; idex2++)
			{
				strshow.Format(_T("请框选第%d个模板,[确定]：开始框选，右键结束。 [取消]：则中断框选流程。"), idex2 + 1);
				if (IDOK != AfxMessageBox(strshow, MB_OKCANCEL))
				{
					return false;
				}
				SetColor(WindowHandle, "green");
				SetDraw(WindowHandle, "margin");
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol));
				WriteString(WindowHandle, "请框选模板(mark点)");

				HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
				RectArea areaTmp;
				if (!Row1.Length())
				{
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2 + 1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "框选太小，无法处理");
					return false;
				}
				areaTmp.row1 = (double)Row1.D();
				areaTmp.col1 = (double)Col1.D();
				areaTmp.row2 = (double)Row2.D();
				areaTmp.col2 = (double)Col2.D();
				int w = areaTmp.col2 - areaTmp.col1;
				int h = areaTmp.row2 - areaTmp.row1;
				if (w < 20 || h < 20) //框选范围太小
				{
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2 + 1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "框选太小，无法处理");
					return false;
				}

				//保存和显示
				GenRectangle1(&m_ho_DutModel[idex2], Row1, Col1, Row2, Col2);
				DispObj(m_ho_DutModel[idex2], WindowHandle);
				areaTmp.bAvalid = true;
				m_DutModel[idex2] = areaTmp;
				//制作模板
				if (!CreateMarkModel(m_ho_DutModel[idex2], m_hv_DutModelId[idex2]))
				{
					AfxMessageBox(_T("创建模板失败"));
					SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2 + 1)), HTuple(m_TextCol + 10));
					WriteString(WindowHandle, "创建模板失败");
					return false;
				}

				//SaveDetectArea(PATH_DETECTAREA);
				SetTposition(WindowHandle, HTuple(m_TextRow + 150 * (idex + idex2)), HTuple(m_TextCol + 10));
				WriteString(WindowHandle, "成功");
			}
		}

		


	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::CreateMarkModel(HObject Roi, HTuple &hvModel)
{
	try
	{
		//裁剪模板
		HObject tmpImage;
		ReduceDomain(ho_Image, Roi, &tmpImage);

		if (hvModel.Length())
		{
			ClearShapeModel(hvModel);
		}
		//创建模板
		HalconCpp::CreateShapeModel(tmpImage, "auto", (HTuple(-20).TupleRad()), HTuple(40).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hvModel);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::FindDutMark(double & spx, double & spy, double & ang)
{
	return false;
}

bool ImgProcess::FindBaseMark(double & spx, double & spy, double & ang)
{
	return false;
}

ImgProcess::ImgProcess()
{
	m_bShowDetectArea = true;
	m_PosX = NULL;      //输出点阵X坐标
	m_PosY = NULL;      //输出点阵Y坐标
	m_RotDeg = NULL;    //输出点阵旋转角度
	m_Score = NULL;     //输出点匹配度
	m_bLoadModel = false;
	m_TextRow = 10;
	m_TextCol = 10;
	m_TextRowBarcode = IMG_H-150;
	m_TextColBarcode = 10;
	m_fExporTime = 50000;
	m_bIsCalied = false;
	m_bIsCaliedRot = false;
	for (int i = 0; i < CALIBRAT_PT_NUM; i++)
	{
		m_fImx[i] = 0.0;
		m_fImy[i] = 0.0;
		m_fSpx[i] = 0.0;
		m_fSpy[i] = 0.0;
		m_fImxTmp[i] = 0.0;
		m_fImyTmp[i] = 0.0;
		m_fSpxTmp[i] = 0.0;
		m_fSpyTmp[i] = 0.0;

		m_lfRotX[i] = 0.0;
		m_lfRotY[i] = 0.0;
	}
	m_lfCentX = 0.0;
	m_lfCentY = 0.0;
	m_lfRadiu = 0.0;
	m_fModelX = 0.0;
	m_fModelY = 0.0;
	m_fModelAngle = 0.0;
	m_fModelScore = 0.0;
	m_bLoadParaFind = false;
	HalconCpp::HException::InstallHHandler(MyExcepHandle);  //关联异常处理函数 
	strcpy_s(m_ModelPathAndName, PATH_MODELFILE);
	m_bManu = false;
	//加载模板文件
	if (!LoadModel(m_ModelPathAndName))
	{
		m_bLoadModel = false;
		AfxMessageBox(_T("加载模板文件失败，请先制作模板"));
	}
	else
	{
		m_bLoadModel = true;
	}
	if (!LoadParaFind(PATH_FIND_PARA))
	{
		m_bLoadParaFind = false;
		AfxMessageBox(_T("加载模板参数FindPara.ini失败"));
	}
	else
	{
		m_bLoadParaFind = true;
	}
	if (!LoadDetectArea(PATH_DETECTAREA))
	{
		AfxMessageBox(_T("加载检测区域文件失败，默认使用图像大小"));
	}
	if (!LoadCalibrate9(PATH_CALIBRATE))
	{
		AfxMessageBox(_T("加载标定文件Calib.ini失败，请检查或重新标定"));
	}

	m_iIndex = g_num++;
	m_pImageBuffer = NULL;
	m_pDataSeparate = NULL;
	m_pCam = NULL;
	m_pCam = new myCameraClass(m_iIndex);
	if (MV_OK == m_pCam->CameraInit(NULL/*GetDlgItem(IDC_STATIC_SHOW)*/))  //不传显示窗口句柄
	{
		m_pCam->SetExposureTime(m_fExporTime);
		m_pCam->SetAcquisitionMode(MV_ACQ_MODE_CONTINUOUS);
		//m_pCam->RegisterImageCallBack(ImageCallBackEx, this);
		m_pCam->SetTriggerMode(MV_TRIGGER_MODE_OFF);
		//m_pCam->SetTriggerSource(MV_TRIGGER_SOURCE_SOFTWARE);
		m_pCam->StartGrabbing();  //开始视频流
	}
	else
	{
		AfxMessageBox(_T("连接相机失败."));
	}

	ReadImage(&ho_Image, "barcode.bmp");
	//DispObj(m_ho_DetectBarcodeArea, WindowHandle);
	//二维码
	CreateDataCode2dModel("Data Matrix ECC 200", HTuple(), HTuple(), &m_hv_DataCodeHandle);
	SetDataCode2dParam(m_hv_DataCodeHandle, "default_parameters", "enhanced_recognition");
}

ImgProcess::~ImgProcess()
{
	if (m_PosX)
	{
		delete []m_PosX;
		m_PosX = NULL;
	}
	if (m_PosY)
	{
		delete[]m_PosY;
		m_PosY = NULL;
	}
	if (m_RotDeg)
	{
		delete[]m_RotDeg;
		m_RotDeg = NULL;
	}
	if (m_Score)
	{
		delete[]m_Score;
		m_Score = NULL;
	}
	//如果MODELID存在,则释放
	int len = hv_ModelID.TupleLength();
	if (len)
	{
		int idModel = hv_ModelID[0].I();
		if (idModel >-1)
			HalconCpp::ClearShapeModel(hv_ModelID);
	}

	if (m_pCam)
	{
		m_pCam->StopGrabbing();
		m_pCam->CloseDevice();
		m_pCam->DestroyDevice();
		delete m_pCam;
		m_pCam = NULL;
	}
	if (m_pImageBuffer)
	{
		free(m_pImageBuffer);
		m_pImageBuffer = NULL;
	}
	if (m_pDataSeparate)
	{
		free(m_pDataSeparate);
		m_pDataSeparate = NULL;
	}
	if (WindowHandle.Length())
	{
		HalconCpp::CloseWindow(WindowHandle);
	}
	if (m_hv_DataCodeHandle.Length())
	{
		ClearDataCode2dModel(m_hv_DataCodeHandle);
	}
}

bool ImgProcess::SaveParaFind(char* pPath)
{
	ptree m_pt, tag_setting;
	tag_setting.clear();					//不清除的话新的标签默认会继承上一个标签中的内容
	try
	{
		tag_setting.put<double>(SUBNODE_ANGSTART,	m_paraFind.AngleStart);  //子节点
		tag_setting.put<double>(SUBNODE_ANGEXTENT,	m_paraFind.AngleExtent);
		tag_setting.put<double>(SUBNODE_MINSCORE,	m_paraFind.MinScore);
		tag_setting.put<int>(SUBNODE_NUMMATCHES,	m_paraFind.NumMatches);
		tag_setting.put<double>(SUBNODE_MAXOVERLAP, m_paraFind.MaxOverlap);
		tag_setting.put<string>(SUBNODE_SUBPIXEL,   m_paraFind.SubPixel);
		tag_setting.put<int>(SUBNODE_MODELLEVELS,	m_paraFind.ModelLevels);
		tag_setting.put<int>(SUBNODE_NUMLEVELS,		m_paraFind.NumLevels);
		tag_setting.put<double>(SUBNODE_GREEDINESS, m_paraFind.Greediness);
		//模板中心
		tag_setting.put<double>(SUBNODE_MODEL_X, m_fModelX);
		tag_setting.put<double>(SUBNODE_MODEL_Y, m_fModelY);
		tag_setting.put<double>(SUBNODE_MODEL_A, m_fModelAngle);
		//模板矩形
		tag_setting.put<double>(SUBNODE_MODELRECT_ROW, m_MRow);
		tag_setting.put<double>(SUBNODE_MODELRECT_COL, m_MCol);
		tag_setting.put<double>(SUBNODE_MODELRECT_PHI, m_MPhi);
		tag_setting.put<double>(SUBNODE_MODELRECT_LEN1, m_MLen1);
		tag_setting.put<double>(SUBNODE_MODELRECT_LEN2, m_MLen2);

		m_pt.put_child(TXT_NODE_PARAFIND, tag_setting); //节点//写入新的标签，如果标签名一样，会修改你前一个的内容，如果不一样就会在下面新加一个 
		write_ini(pPath, m_pt);
	}
	catch (...)
	{
		AfxMessageBox(_T("加载ParaFind文件失败"));
		return false;
	}
	return true;
}

bool ImgProcess::SaveParaFind(CString strPath)
{
	int len = WideCharToMultiByte(CP_ACP, 0, strPath, -1, NULL, 0, NULL, NULL);//宽字符转
	char* strTemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, strTemp, len, NULL, NULL);
	bool ret = SaveParaFind(strTemp);
	delete[]strTemp; strTemp = NULL;
	return ret;
}

bool ImgProcess::LoadParaFind(char* pPath)
{
	try
	{
		ptree m_pt, tag_setting;
		read_ini(pPath, m_pt);
		tag_setting = m_pt.get_child(TXT_NODE_PARAFIND);	//匹配参数

		m_paraFind.AngleStart	= tag_setting.get<double>(SUBNODE_ANGSTART,	-90);  //子节点
		m_paraFind.AngleExtent	= tag_setting.get<double>(SUBNODE_ANGEXTENT,180);
		m_paraFind.MinScore		= tag_setting.get<double>(SUBNODE_MINSCORE,	0.5);
		m_paraFind.NumMatches	= tag_setting.get<int>(SUBNODE_NUMMATCHES,	0);
		m_paraFind.MaxOverlap	= tag_setting.get<double>(SUBNODE_MAXOVERLAP,0.5);
		m_paraFind.SubPixel		= tag_setting.get<string>(SUBNODE_SUBPIXEL,	"least_squares");
		m_paraFind.ModelLevels	= tag_setting.get<int>(SUBNODE_MODELLEVELS,	8);
		m_paraFind.NumLevels	= tag_setting.get<int>(SUBNODE_NUMLEVELS,	1);
		m_paraFind.Greediness	= tag_setting.get<double>(SUBNODE_GREEDINESS,0.75);

		m_fModelX = tag_setting.get<double>(SUBNODE_MODEL_X, 0.0);
		m_fModelY = tag_setting.get<double>(SUBNODE_MODEL_Y, 0.0);
		m_fModelAngle = tag_setting.get<double>(SUBNODE_MODEL_A, 0.0);

		m_MRow = tag_setting.get<double>(SUBNODE_MODELRECT_ROW, 0.0);
		m_MCol = tag_setting.get<double>(SUBNODE_MODELRECT_COL, 0.0);
		m_MPhi = tag_setting.get<double>(SUBNODE_MODELRECT_PHI, 0.0);
		m_MLen1 = tag_setting.get<double>(SUBNODE_MODELRECT_LEN1, 0.0);
		m_MLen2 = tag_setting.get<double>(SUBNODE_MODELRECT_LEN2, 0.0);

	}
	catch (...)
	{
		AfxMessageBox(_T("加载ParaFind文件失败"));
		return false;
	}

	return true;
}

bool ImgProcess::LoadParaFind(CString strPath)
{
	int len = WideCharToMultiByte(CP_ACP, 0, strPath, -1, NULL, 0, NULL, NULL);//宽字符转
	char* strTemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, strPath, -1, strTemp, len, NULL, NULL);
	bool ret = LoadParaFind(strTemp);
	delete[]strTemp; strTemp = NULL;
	return ret;
}

bool ImgProcess::LoadDetectArea(char * pPath)
{
	CString csFile = _T("DetectArea.ini");
	DWORD dwAttrib = GetFileAttributes(csFile);
	bool ret = INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);

	try {
		ptree m_pt, tag_setting;
		
		read_ini(pPath, m_pt);
		tag_setting = m_pt.get_child(TXT_NODE_AREA_MODEL);	//匹配区域参数
		if (tag_setting.size())
		{
			m_ModelDetectArea.bAvalid	= tag_setting.get<bool>(SUBNODE_VALID, false);
			m_ModelDetectArea.row1		= tag_setting.get<int >(SUBNODE_ROW1, -1);
			m_ModelDetectArea.col1		= tag_setting.get<int >(SUBNODE_COL1, -1);
			m_ModelDetectArea.row2		= tag_setting.get<int >(SUBNODE_ROW2, -1);
			m_ModelDetectArea.col2		= tag_setting.get<int >(SUBNODE_COL2, -1);
		
			if ((m_ModelDetectArea.row1 < 0) || (m_ModelDetectArea.col1 < 0) || (m_ModelDetectArea.row2 < 0) || (m_ModelDetectArea.col2 < 0))
			{
				m_ModelDetectArea.bAvalid = false;
			}
			else
			{
				try
				{
					//HalconCpp::HTuple Row1, Col1, Row2, Col2;
					//Row1 = HTuple((double)m_ModelDetectArea.row1);
					//Col1 = HTuple((double)m_ModelDetectArea.col1);
					//Row2 = HTuple((double)m_ModelDetectArea.row2);
					//Col2 = HTuple((double)m_ModelDetectArea.col2);
					//GenRectangle1(&m_ho_ModelDetectArea,0, 0, 100, 100/* Row1, Col1, Row2, Col2*/);
					m_ModelDetectArea.bAvalid = true;
				}
				catch (HalconCpp::HException & except)
				{
					ShowException(except);
					//return false;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("加载检测区域文件失败"));
		}
		tag_setting = m_pt.get_child(TXT_NODE_AREA_BARCODE);	//二维码检测区域参数
		if (tag_setting.size())
		{
			m_BarcodeDetectArea.bAvalid = tag_setting.get<bool>(SUBNODE_VALID, false);
			m_BarcodeDetectArea.row1 = tag_setting.get<int >(SUBNODE_ROW1, -1);
			m_BarcodeDetectArea.col1 = tag_setting.get<int >(SUBNODE_COL1, -1);
			m_BarcodeDetectArea.row2 = tag_setting.get<int >(SUBNODE_ROW2, -1);
			m_BarcodeDetectArea.col2 = tag_setting.get<int >(SUBNODE_COL2, -1);

			if ((m_BarcodeDetectArea.row1 < 0) || (m_BarcodeDetectArea.col1 < 0) || (m_BarcodeDetectArea.row2 < 0) || (m_BarcodeDetectArea.col2 < 0))
			{
				m_BarcodeDetectArea.bAvalid = false;
			}
			else
			{
				try
				{
					/*HalconCpp::HTuple Row1, Col1, Row2, Col2;
					Row1 = m_BarcodeDetectArea.row1;
					Col1 = m_BarcodeDetectArea.col1;
					Row2 = m_BarcodeDetectArea.row2;
					Col2 = m_BarcodeDetectArea.col2;
					GenRectangle1(&m_ho_DetectBarcodeArea, 100, 100, 200, 200);
					m_BarcodeDetectArea.bAvalid = true;
					DispObj(m_ho_DetectBarcodeArea, WindowHandle);*/
				}
				catch (HalconCpp::HException & except)
				{
					ShowException(except);
					//return false;
				}
			}
		}
		else
		{
			AfxMessageBox(_T("加载检测区域文件失败"));
		}

		//相机
		tag_setting.clear();
		tag_setting = m_pt.get_child(TXT_NODE_CAM);	//相机节点
		if (tag_setting.size())
		{
			m_fExporTime = tag_setting.get<double>(SUBNODE_EXP_TIME, 50000);
		}
	}
	catch (...)
	{
		AfxMessageBox(_T("加载检测区域文件失败"));
	}
	return true;
}
//框选模板检测区域
bool ImgProcess::DrawModelDetectArea(char * pSavePath)
{
	//
	try
	{
		if (!WindowHandle.Length()) return false;
		ClearWindow(WindowHandle);
		if (ho_Image.IsInitialized())
			DispObj(ho_Image, WindowHandle);

		HalconCpp::HTuple Row1, Col1, Row2, Col2;
		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw A Model Detect Area");

		HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
		DetectArea areaTmp;
		if (!Row1.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow+200), HTuple(m_TextCol));
			WriteString(WindowHandle, "Model Detect Area Too Small");
			return false;
		}
		areaTmp.row1 = (int)Row1.D();
		areaTmp.col1 = (int)Col1.D();
		areaTmp.row2 = (int)Row2.D();
		areaTmp.col2 = (int)Col2.D();
		int w = areaTmp.col2 - areaTmp.col1;
		int h = areaTmp.row2 - areaTmp.row1;
		if (w < 10 || h < 10) //框选范围太小
		{
			SetTposition(WindowHandle, HTuple(m_TextRow+200), HTuple(m_TextCol));
			WriteString(WindowHandle, "Model Detect Area Too Small");
			return false;
		}

		GenRectangle1(&m_ho_ModelDetectArea, Row1, Col1, Row2, Col2);
		DispObj(m_ho_ModelDetectArea, WindowHandle);
		areaTmp.bAvalid = true;
		m_ModelDetectArea = areaTmp;
		SaveDetectArea(PATH_DETECTAREA);
		SetTposition(WindowHandle, HTuple(m_TextRow + 200), HTuple(m_TextCol));
		WriteString(WindowHandle, "Model Detect Area Save Successed");
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

//框选二维码检测区域
bool ImgProcess::DrawBarcodeDetectArea(char * pSavePath)
{
	//
	try
	{
		if (!WindowHandle.Length()) return false;
		ClearWindow(WindowHandle);
		if (ho_Image.IsInitialized())
			DispObj(ho_Image, WindowHandle);

		HalconCpp::HTuple Row1, Col1, Row2, Col2;
		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw A Barcode Detect Area");

		HalconCpp::DrawRectangle1(WindowHandle, &Row1, &Col1, &Row2, &Col2);
		DetectArea areaTmp;
		if (!Row1.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow+200), HTuple(m_TextCol));
			WriteString(WindowHandle, "Barcode Detect Area Too Small");
			return false;
		}
		areaTmp.row1 = (int)Row1.D();
		areaTmp.col1 = (int)Col1.D();
		areaTmp.row2 = (int)Row2.D();
		areaTmp.col2 = (int)Col2.D();
		int w = areaTmp.col2 - areaTmp.col1;
		int h = areaTmp.row2 - areaTmp.row1;
		if (w < 10 || h < 10) //框选范围太小
		{
			SetTposition(WindowHandle, HTuple(m_TextRow+200), HTuple(m_TextCol));
			WriteString(WindowHandle, "Barcode Detect Area Too Small");
			return false;
		}

		GenRectangle1(&m_ho_DetectBarcodeArea, Row1, Col1, Row2, Col2);
		DispObj(m_ho_DetectBarcodeArea, WindowHandle);
		areaTmp.bAvalid = true;
		m_BarcodeDetectArea = areaTmp;
		SaveDetectArea(PATH_DETECTAREA); //保存文件
		SetTposition(WindowHandle, HTuple(m_TextRow+200), HTuple(m_TextCol));
		WriteString(WindowHandle, "Barcode Detect Area Save Successed");
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::SaveDetectArea(char * pPath)
{
	ptree m_pt, tag_setting;
	tag_setting.clear();					//不清除的话新的标签默认会继承上一个标签中的内容

	tag_setting.put<bool>(SUBNODE_VALID, m_ModelDetectArea.bAvalid);
	tag_setting.put<int >(SUBNODE_ROW1, m_ModelDetectArea.row1);
	tag_setting.put<int >(SUBNODE_COL1, m_ModelDetectArea.col1);
	tag_setting.put<int >(SUBNODE_ROW2, m_ModelDetectArea.row2);
	tag_setting.put<int >(SUBNODE_COL2, m_ModelDetectArea.col2);
	m_pt.put_child(TXT_NODE_AREA_MODEL, tag_setting); //节点//写入新的标签，如果标签名一样，会修改你前一个的内容，如果不一样就会在下面新加一个 
	
	tag_setting.clear();
	tag_setting.put<bool>(SUBNODE_VALID,m_BarcodeDetectArea.bAvalid);
	tag_setting.put<int >(SUBNODE_ROW1, m_BarcodeDetectArea.row1);
	tag_setting.put<int >(SUBNODE_COL1, m_BarcodeDetectArea.col1);
	tag_setting.put<int >(SUBNODE_ROW2, m_BarcodeDetectArea.row2);
	tag_setting.put<int >(SUBNODE_COL2, m_BarcodeDetectArea.col2);
	m_pt.put_child(TXT_NODE_AREA_BARCODE, tag_setting);

	//相机
	tag_setting.clear();
	tag_setting.put<double >(SUBNODE_EXP_TIME, m_fExporTime);
	m_pt.put_child(TXT_NODE_CAM, tag_setting);

	write_ini(pPath, m_pt);

	return true;
}

void ImgProcess::ShowDetectArea(bool bShow)
{
	m_bShowDetectArea = bShow;
}

bool ImgProcess::IsLoadModel()
{
	return m_bLoadModel;
}

bool ImgProcess::SetDispWnd(Hlong wnd, CRect showRect)
{
	try
	{
		hl_wnd = wnd;
		m_rect = showRect;
		if (WindowHandle.Length())
		{
			CloseWindow(WindowHandle);
		}
		OpenWindow(m_rect.top, m_rect.left, m_rect.Width(), m_rect.Height(), hl_wnd, "", "", &WindowHandle);
		HalconCpp::SetSystem("tsp_width", IMG_W);
		HalconCpp::SetSystem("tsp_height", IMG_H);
		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		//测试
		//ReadImage(&ho_Image, "barcode.bmp");
		//DispObj(ho_Image, WindowHandle);
		//如果框选了ROI则显示roi部分
		if (m_ModelDetectArea.bAvalid)
		{
			//SetPart(WindowHandle, 0, 0, IMG_H - 1, IMG_W - 1);
			SetPart(WindowHandle, 
				HTuple(m_ModelDetectArea.row1), 
				HTuple(m_ModelDetectArea.col1), 
				HTuple(m_ModelDetectArea.row2),
				HTuple(m_ModelDetectArea.col2));
			m_TextRow = m_ModelDetectArea.row1+10;
			m_TextCol = m_ModelDetectArea.col1+10;
			m_TextRowBarcode = m_ModelDetectArea.row2 - 150;
			m_TextColBarcode = m_ModelDetectArea.col1 + 10;


			HalconCpp::HTuple Row1, Col1, Row2, Col2;
			Row1 = HTuple((double)m_ModelDetectArea.row1);
			Col1 = HTuple((double)m_ModelDetectArea.col1);
			Row2 = HTuple((double)m_ModelDetectArea.row2);
			Col2 = HTuple((double)m_ModelDetectArea.col2);
			GenRectangle1(&m_ho_ModelDetectArea, Row1, Col1, Row2, Col2);
			DispObj(m_ho_ModelDetectArea, WindowHandle);
		}
		else
		{
			SetPart(WindowHandle, 0, 0, IMG_H - 1, IMG_W - 1);
			m_TextRow = 10;
			m_TextCol = 10;
			m_TextRowBarcode = IMG_H - 150;
			m_TextColBarcode = 10;
		}
		if (m_BarcodeDetectArea.bAvalid)
		{
			HalconCpp::HTuple Row1, Col1, Row2, Col2;
			Row1 = HTuple((double)m_BarcodeDetectArea.row1);
			Col1 = HTuple((double)m_BarcodeDetectArea.col1);
			Row2 = HTuple((double)m_BarcodeDetectArea.row2);
			Col2 = HTuple((double)m_BarcodeDetectArea.col2);
			GenRectangle1(&m_ho_DetectBarcodeArea, Row1, Col1, Row2, Col2);
			m_BarcodeDetectArea.bAvalid = true;
			DispObj(m_ho_DetectBarcodeArea, WindowHandle);
		}
		
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::LoadModel(char * pModelPath, char* pModelImagePath)
{
	try 
	{
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel >-1)
				HalconCpp::ClearShapeModel(hv_ModelID);
		}
		//Matching 02: Read the shape model from file
		ReadShapeModel(pModelPath, &hv_ModelID);

		//Matching 02: Get the model contour for transforming it later into the image
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
		//
		//Matching 02: Set a reference position to show the model
		SmallestRectangle1Xld(ho_ModelContours, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		hv_RefRow = ((hv_Row2.TupleMax()) - (hv_Row1.TupleMin())) / 2;
		hv_RefColumn = ((hv_Column2.TupleMax()) - (hv_Column1.TupleMin())) / 2;
		VectorAngleToRigid(0, 0, 0, hv_RefRow, hv_RefColumn, 0, &hv_HomMat2D);
		HalconCpp::AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);

		//Matching 02: Display the model contours
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return LoadModelImage(pModelImagePath);
}

bool ImgProcess::LoadModel(CString pModelPath, CString pModelImagePath)
{
	int len = WideCharToMultiByte(CP_ACP, 0, pModelPath, -1, NULL, 0, NULL, NULL);//宽字符转
	char* strTemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, pModelPath, -1, strTemp, len, NULL, NULL);

	len = WideCharToMultiByte(CP_ACP, 0, pModelImagePath, -1, NULL, 0, NULL, NULL);//宽字符转
	char* strTemp2 = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, pModelImagePath, -1, strTemp2, len, NULL, NULL);

	fs::path path(pModelPath);
	if (!fs::exists(path))
	{
		return  false;
	}

	fs::path path2(pModelImagePath);
	if (!fs::exists(path2))
	{
		return  false;
	}
	bool ret = LoadModel(strTemp,strTemp2);

	delete[]strTemp; strTemp = NULL;
	delete[]strTemp2; strTemp2 = NULL;
	return ret;
}

bool ImgProcess::LoadModel(char * pModelPath)
{
	try
	{
		int len = hv_ModelID.Length();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel > -1)
				HalconCpp::ClearShapeModel(hv_ModelID);
		}
		//Matching 02: Read the shape model from file
		ReadShapeModel(pModelPath, &hv_ModelID);
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}

	return true;
}

void ImgProcess::SetModelPathName(char * pModelPath)
{
	int len = strlen(pModelPath);
	if (len > sizeof(m_ModelPathAndName))
		return;
	strcpy_s(m_ModelPathAndName, pModelPath);
}

bool ImgProcess::CreateModel(BYTE * pImage, int imW, int imH)
{
	//
	HObject h0_Imagerow;
	bool ret = true;
	//
	try {
		//如果MODELID存在,则释放
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel >-1)
				HalconCpp::ClearShapeModel(hv_ModelID);
		}

		ho_Image.Clear();
		GenImage1(&ho_Image, "byte", imW, imH, (Hlong)(pImage));
		//MirrorImage(h0_Imagerow, &ho_Image, "row"); //上下翻转	
		//创建模板，参数自动
		HalconCpp::CreateShapeModel(ho_Image, "auto", -(HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
		SaveModel(m_ModelPathAndName);
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::CreateModel(BYTE * pImage, int imW, int imH, char * ModImgPathname)
{
	HObject h0_Imagerow, h0_Imagerow2;
	HTuple  hv_ModelIDtmp, hv_RowTmp, hv_ColumnTmp, hv_AngleTmp, hv_ScoreTmp;
	HTuple  hv_w, hv_h;
	
	bool ret = true;
	//使用从文件中加载的小图创建临时模板
	ReadImage(&h0_Imagerow, (HTuple)ModImgPathname);
	GetImageSize(h0_Imagerow, &hv_w, &hv_h);
	HalconCpp::CreateShapeModel(h0_Imagerow, "auto", -(HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
		"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelIDtmp);


	//在大图里查找小图，得到区域。
	GenImage1(&h0_Imagerow2, "byte", imW, imH, (Hlong)(pImage));
	MirrorImage(h0_Imagerow2, &ho_Image, "row"); //上下翻转
	HalconCpp::FindShapeModel(ho_Image, hv_ModelID,
		HTuple(m_paraFind.AngleStart).TupleRad(),
		HTuple(m_paraFind.AngleExtent).TupleRad(),
		m_paraFind.MinScore,
		1/*m_paraFind.NumMatches*/,
		m_paraFind.MaxOverlap,
		m_paraFind.SubPixel.c_str(),
		(HTuple(m_paraFind.ModelLevels).Append(m_paraFind.NumLevels)),
		m_paraFind.Greediness,
		&hv_RowTmp, &hv_ColumnTmp, &hv_AngleTmp, &hv_ScoreTmp);
	if (hv_ScoreTmp.Length() == 0)
	{
		return false;
	}
	//利用此区域ROI创建模板
	HObject hv_rect, hv_rectTran, ho_TemplateImage;
	HTuple hv_centRow, hv_centCol, hv_halfW, hv_halfH, hv_HomMat2Dtmp;
	hv_centRow = hv_h / 2.0;
	hv_centCol = hv_w / 2.0;
	hv_halfH = hv_h / 2.0;
	hv_halfW = hv_w / 2.0;
	GenRectangle2(&hv_rect, hv_centRow, hv_centCol, 0, hv_halfW, hv_halfH);
	//放射变换,得到roi区域
	HomMat2dIdentity(&hv_HomMat2Dtmp);
	HalconCpp::HomMat2dRotate(hv_HomMat2Dtmp, HTuple(hv_AngleTmp[0]), 0, 0, &hv_HomMat2Dtmp);
	HalconCpp::HomMat2dTranslate(hv_HomMat2Dtmp, HTuple(hv_RowTmp[0]), HTuple(hv_ColumnTmp[0]),
		&hv_HomMat2Dtmp);
	HalconCpp::AffineTransContourXld(hv_rect, &hv_rectTran, hv_HomMat2Dtmp);

	//截图图像
	ReduceDomain(ho_Image, hv_rectTran, &ho_TemplateImage);
	HalconCpp::WriteImage(ho_TemplateImage, "bmp", 0, "templateImage");
	//创建模板，参数自动
	HalconCpp::CreateShapeModel(ho_TemplateImage, "auto", -(HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
		"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
	SaveModel(m_ModelPathAndName);
	HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);

	
	return ret;
}

bool ImgProcess::CreateModel(char * pModePath)
{
	//
	HObject h0_Imagerow;
	bool ret = true;
	//
	try {
		//如果MODELID存在,则释放
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel >-1)
				HalconCpp::ClearShapeModel(hv_ModelID);
		}


		ho_Image.Clear();
		ReadImage(&ho_Image, (HTuple)pModePath);
		//GenImage1(&ho_Image, "byte", imW, imH, (Hlong)(pImage));
		//MirrorImage(h0_Imagerow, &ho_Image, "row"); //上下翻转	
		//创建模板，参数自动
		HalconCpp::CreateShapeModel(ho_Image, "auto", -(HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
		SaveModel(m_ModelPathAndName);
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::CreateModel(char * pModePath, std::vector<CRect> &vctRoi)
{
	//
	HObject h0_Imagerow, ho_ModelRegion, ho__TmpRegion, ho_TemplateImage;
	bool ret = true;
	if (vctRoi.size() == 0)
		return false;
	//
	try {
		//如果MODELID存在,则释放
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel >-1)
				HalconCpp::ClearShapeModel(hv_ModelID);
		}

		ho_Image.Clear();
		ReadImage(&ho_Image, (HTuple)pModePath);
		//计算roi区域
		for (int sz = 0; sz < vctRoi.size(); sz++)
		{
			CRect rc = vctRoi[sz];
			if (0 == sz)
			{
				GenRectangle1(&ho_ModelRegion, rc.top, rc.left, rc.bottom, rc.right);
			}
			else
			{
				GenRectangle1(&ho__TmpRegion, rc.top, rc.left, rc.bottom, rc.right);
				Union2(ho_ModelRegion, ho__TmpRegion, &ho_ModelRegion);
			}
		}

		//截图图像
		ReduceDomain(ho_Image, ho_ModelRegion, &ho_TemplateImage);
		HalconCpp::WriteImage(ho_TemplateImage, "bmp",0,"templateImage");
		//创建模板，参数自动
		HalconCpp::CreateShapeModel(ho_TemplateImage, "auto", -(HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
		SaveModel(m_ModelPathAndName);
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	return true;
}

bool ImgProcess::CreateModelFromCam()
{	
	//采集图像
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	if (!m_bManu)
	{
		g_criSection.Lock();
		m_pCam->GrabOneImage(&pData, stImageInfo);
		g_criSection.Unlock();
		if (!pData)
		{
			if (WindowHandle.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				SetColor(WindowHandle, "red");
				WriteString(WindowHandle, "Camera Error");
			}
			return false;
		}
		ho_Image.Clear();
		ConvertoHalcon(pData, stImageInfo, ho_Image);
	}

	if (!WindowHandle.Length())
		return false;

	DispObj(ho_Image, WindowHandle);
	//是否显示检测区域
	if (m_bShowDetectArea)
	{
		if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
		{
			//
			DispObj(m_ho_ModelDetectArea, WindowHandle);
		}
	}
	//框选模板
	try
	{
		HTuple Row, Col, Phi, Len1, Len2;
		HObject  ho_TemplateImage;

		SetColor(WindowHandle, "green");
		SetDraw(WindowHandle, "margin");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Please Draw a region");
		
		DrawRectangle2(WindowHandle, &Row, &Col, &Phi, &Len1, &Len2);
		GenRectangle2(&m_ho_ModelRegion, Row, Col, Phi, Len1, Len2);

		DispObj(ho_Image, WindowHandle);
		DispObj(m_ho_ModelRegion, WindowHandle);
		//是否显示检测区域
		if (m_bShowDetectArea)
		{
			if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
			{
				//
				DispObj(m_ho_ModelDetectArea, WindowHandle);
			}
		}
		ReduceDomain(ho_Image, m_ho_ModelRegion, &ho_TemplateImage);
		HalconCpp::WriteImage(ho_TemplateImage, "bmp", 0, "templateImage");

		if (hv_ModelID.Length())
		{
			ClearShapeModel(hv_ModelID);
		}
		//创建模板
		HalconCpp::CreateShapeModel(ho_TemplateImage, "auto", (HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
		SaveModel(m_ModelPathAndName);
		HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
		
		//保存模板中心坐标
		//AfxMessageBox(_T("test: col"));
		m_fModelX = (double)Col.D();
		m_fModelY = (double)Row.D();
		m_fModelAngle = 0.0;
		
		m_MRow = (double)Row.D();
		m_MCol = (double)Col.D();
		m_MPhi = (double)Phi.D();
		m_MLen1 = (double)Len1.D();
		m_MLen2 = (double)Len2.D();
		
		//AfxMessageBox(_T("test: save"));
		if (SaveParaFind(PATH_FIND_PARA))
		{
			m_bLoadParaFind = true;
		}
		else
		{
			m_bLoadParaFind = false;
		}

		m_bLoadModel = true;
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "green");
			WriteString(WindowHandle, "Create Sucessed");
		}
	}
	catch (HOperatorException	exception)
	{
		m_bLoadModel = false;
		ShowException(exception);
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		SetColor(WindowHandle, "red");
		WriteString(WindowHandle, "Create Failed");
		return false;
	}
	return true;
}

bool ImgProcess::CreateModelFromModel()
{
	//判断是否之前有加载模板和加载模板矩形
	if (!m_bLoadModel || !m_bLoadParaFind)//如果之前没有模板，则通过手动框选矩形制作模板
	{
		return CreateModelFromCam();
	}
	else//如果之前有模板和矩形，则通过识别得到矩形再制作新模板
	{
		//采集图像
		unsigned char * pData = NULL;
		MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
		
		if (!m_bManu)
		{
			g_criSection.Lock();
			m_pCam->GrabOneImage(&pData, stImageInfo);
			g_criSection.Unlock();
			if (!pData)
			{
				if (WindowHandle.Length())
				{
					SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
					SetColor(WindowHandle, "red");
					WriteString(WindowHandle, "Camera Error");
				}
				return false;
			}
			ho_Image.Clear();
			ConvertoHalcon(pData, stImageInfo, ho_Image);
		}
		
		if (!WindowHandle.Length())
			return false;

		DispObj(ho_Image, WindowHandle);
		//是否显示检测区域
		if (m_bShowDetectArea)
		{
			if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
			{
				//
				DispObj(m_ho_ModelDetectArea, WindowHandle);
			}
		}
		//使用自动识别框选模板
		try
		{
			HTuple Row, Col, Phi, Len1, Len2;
			HObject  ho_TemplateImage;
			HObject  ho_ImageReduce;  //检测区域图像
			/////////////////////////////////////////
			SetColor(WindowHandle, "green");
			SetDraw(WindowHandle, "margin");

			//裁剪图像
			if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
			{
				ReduceDomain(ho_Image, m_ho_ModelDetectArea, &ho_ImageReduce);
				if (m_bShowDetectArea)//是否显示检测区域
				{
					DispObj(m_ho_ModelDetectArea, WindowHandle);
				}
			}
			else
			{
				ho_ImageReduce = ho_Image.Clone();
			}
			//识别
			HalconCpp::FindShapeModel(ho_ImageReduce, hv_ModelID,
				HTuple(m_paraFind.AngleStart).TupleRad(),
				HTuple(m_paraFind.AngleExtent).TupleRad(),
				m_paraFind.MinScore,
				m_paraFind.NumMatches,
				m_paraFind.MaxOverlap,
				m_paraFind.SubPixel.c_str()/*SubPixel*/,
				(HTuple(m_paraFind.ModelLevels).Append(m_paraFind.NumLevels)),
				m_paraFind.Greediness,
				&hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			//画出识别到的模板框
			int iNum = hv_Score.TupleLength();
			if (iNum == 0)
			{
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				SetColor(WindowHandle, "red");
				WriteString(WindowHandle, "Detect Failed:找不到模板，请检查图像或光源");
				WriteImage(ho_Image, "bmp", 0, "DetectFailed"); //保存图像
				return false;
				//if (IDOK != AfxMessageBox(_T("自动识别找不到目标，请手动框选模板或者点击取消按钮中断操作")))
				//{
				//	return false;
				//}
				//return CreateModelFromCam();
			}
			int len = (hv_Score.TupleLength());
			int idx = 1;
			if (WindowHandle.Length())
				SetColor(WindowHandle, "green");
			HObject ho_ModelRegion_Trans;
			for (idx = 0; idx < len; idx++)
			{
				HomMat2dIdentity(&hv_HomMat2D);
				HalconCpp::HomMat2dRotate(hv_HomMat2D, HTuple(hv_Angle[idx]), 0, 0, &hv_HomMat2D);
				HalconCpp::HomMat2dTranslate(hv_HomMat2D, HTuple(hv_Row[idx]), HTuple(hv_Column[idx]),
					&hv_HomMat2D);
				HalconCpp::AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);
				
				//模板框仿射变换
				HTuple HomMat2D;
				HomMat2dIdentity(&HomMat2D);
				HalconCpp::HomMat2dRotate(HomMat2D, HTuple(hv_Angle[idx]), HTuple(m_MRow), HTuple(m_MCol), &HomMat2D);
				HalconCpp::HomMat2dTranslate(HomMat2D, 
					HTuple(hv_Row[idx].D() - m_MRow), 
					HTuple(hv_Column[idx].D() - m_MCol),
					&HomMat2D);
				//构造模板区域
				HObject ModelRegion;
				GenRectangle2(&ModelRegion, HTuple(m_MRow), HTuple(m_MCol), HTuple(m_MPhi), HTuple(m_MLen1), HTuple(m_MLen2));
				AffineTransRegion(ModelRegion, &ho_ModelRegion_Trans, HomMat2D, "nearest_neighbor");
			}
			m_ho_ModelRegion = ho_ModelRegion_Trans;
			SmallestRectangle2(ho_ModelRegion_Trans, &Row, &Col, &Phi, &Len1, &Len2);
			/////////////////////////////////////////
			DispObj(ho_Image, WindowHandle);
			DispObj(m_ho_ModelRegion, WindowHandle);
			//是否显示检测区域
			if (m_bShowDetectArea)
			{
				if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
				{
					//
					DispObj(m_ho_ModelDetectArea, WindowHandle);
				}
			}
			ReduceDomain(ho_Image, m_ho_ModelRegion, &ho_TemplateImage);
			HalconCpp::WriteImage(ho_TemplateImage, "bmp", 0, "templateImage");

			if (hv_ModelID.Length())
			{
				ClearShapeModel(hv_ModelID);
				hv_ModelID.Clear();
			}
			//创建模板
			HalconCpp::CreateShapeModel(ho_TemplateImage, "auto", (HTuple(m_paraFind.AngleStart).TupleRad()), HTuple(m_paraFind.AngleExtent).TupleRad(),
				"auto", "auto", "use_polarity", "auto", "auto", &hv_ModelID);
			SaveModel(m_ModelPathAndName);
			HalconCpp::GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);

			//保存模板中心坐标
			//AfxMessageBox(_T("test: col"));
			m_fModelX = (double)Col.D();
			m_fModelY = (double)Row.D();
			m_fModelAngle = 0.0;

			m_MRow = (double)Row.D();
			m_MCol = (double)Col.D();
			m_MPhi = (double)Phi.D();
			m_MLen1 = (double)Len1.D();
			m_MLen2 = (double)Len2.D();

			//AfxMessageBox(_T("test: save"));
			if (SaveParaFind(PATH_FIND_PARA))
			{
				m_bLoadParaFind = true;
			}
			else
			{
				m_bLoadParaFind = false;
			}

			m_bLoadModel = true;
			if (WindowHandle.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				SetColor(WindowHandle, "green");
				WriteString(WindowHandle, "Create Sucessed");
			}
		}
		catch (HOperatorException	exception)
		{
			m_bLoadModel = false;
			ShowException(exception);
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Create Failed");
			return false;
		}
		return true;
	}


	


	return false;
}

bool ImgProcess::SaveModel(char * pModelPath)
{
	WriteShapeModel(hv_ModelID,pModelPath);
	return true;
}

bool ImgProcess::LoadModelImage(char * pModelImagePath)
{
	//加载模板图像并识别，得到的是模板中心。
	ho_ModelImage.Clear();
	int num = 0;
	double minScore = 0.0;
	try
	{
		ReadImage(&ho_ModelImage, (HTuple)pModelImagePath);
		Find(pModelImagePath, num, minScore);
	}
	catch (HOperatorException	exception)
	{
		AfxMessageBox(_T("模板图像加载失败"));
		return false;
	}
	if(num == 0)
	{
		AfxMessageBox(_T("模板检查加载失败"));
		return false;
	}
	m_fModelX = m_PosX[0];
	m_fModelY = m_PosY[0];
	m_fModelAngle = m_RotDeg[0];
	m_fModelScore = m_Score[0];
	m_bLoadModel = true;
	return true;
}

bool ImgProcess::Find(BYTE *pImage, int imW, int imH, int &iNum, double &lfMinScore)
{
	//
	HObject h0_Imagerow;
	bool ret = true;
	try {

		//如果MODELID不存在
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel < 0)
				return false;
		}
		else
		{
			return false;
		}

		ho_Image.Clear();
		GenImage1(&h0_Imagerow, "byte", imW, imH, (Hlong)(pImage));
		//WriteImage(h0_Imagerow, "bmp", 0, "halconimg.bmp");
		MirrorImage(h0_Imagerow, &ho_Image, "row"); //上下翻转
		//WriteImage(ho_Image, "bmp", 0, "halconimg_mirro.bmp");
		//FIND
		HalconCpp::FindShapeModel(ho_Image, hv_ModelID,
			HTuple(m_paraFind.AngleStart).TupleRad(),
			HTuple(m_paraFind.AngleExtent).TupleRad(),
			m_paraFind.MinScore,
			m_paraFind.NumMatches,
			m_paraFind.MaxOverlap,
			m_paraFind.SubPixel.c_str()/*SubPixel*/,
			(HTuple(m_paraFind.ModelLevels).Append(m_paraFind.NumLevels)),
			m_paraFind.Greediness,
			&hv_Row, &hv_Column, &hv_Angle, &hv_Score);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}

	//输出
	iNum = hv_Score.TupleLength();
	if (iNum == 0)
	{
		ret = false;
	}
	if (m_PosX)
	{
		delete[]m_PosX; m_PosX = NULL;
	}
	if (m_PosY)
	{
		delete[]m_PosY; m_PosY = NULL;
	}
	if (m_RotDeg)
	{
		delete[]m_RotDeg; m_RotDeg = NULL;
	}
	if (m_Score)
	{
		delete[]m_Score; m_Score = NULL;
	}
	m_targetNum = iNum;
	m_PosX = new double[iNum];
	m_PosY = new double[iNum];
	m_RotDeg = new double[iNum];
	m_Score = new double[iNum];
	for (int i = 0; i<iNum; i++)
	{
		m_PosX[i] = hv_Column[i].D();
		m_PosY[i] = hv_Row[i].D();
		m_RotDeg[i] = hv_Angle[i].D();
		m_Score[i] = hv_Score[i].D();
	}
	//Matching 02: Transform the model contours into the detected positions
	if (lfMinScore > 0.0)
	{
		int m = 0;
		double maxscore = 0;
		for (int i = 0; i<iNum; i++)//求匹配分数最大值
		{
			if (maxscore <= m_Score[i])
				maxscore = m_Score[i];
		}
		//如果匹配分数最大值大于传入的比较参数，则显示出来返回真，
		if (lfMinScore > maxscore)
			//ret = false;
			return false;
	}
	try {
		if (hl_wnd)
			{
				HTuple hv_Width, hv_Height;
				GetImageSize(ho_Image, &hv_Width, &hv_Height);
				HalconCpp::SetSystem("tsp_width", hv_Width);			//0316 whq
				HalconCpp::SetSystem("tsp_height", hv_Height);
				if (HDevWindowStack::IsOpen())
					HalconCpp::CloseWindow(HDevWindowStack::Pop());
				SetWindowAttr("border_width", 0); //设置窗口属性
				SetWindowAttr("background_color", "white");
				OpenWindow(m_rect.top, m_rect.left, m_rect.Width(), m_rect.Height(), hl_wnd, "", "", &WindowHandle);
				HDevWindowStack::Push(WindowHandle);
				HDevWindowStack::SetActive(WindowHandle);
			}
			if (HDevWindowStack::IsOpen())
			{
				DispObj(ho_Image, WindowHandle/*HDevWindowStack::GetActive()*/);
				HTuple end_val41 = (hv_Score.TupleLength()) - 1;
				HTuple step_val41 = 1;
				for (hv_I = 0; hv_I.Continue(end_val41, step_val41); hv_I += step_val41)
				{
					HomMat2dIdentity(&hv_HomMat2D);
					HalconCpp::HomMat2dRotate(hv_HomMat2D, HTuple(hv_Angle[hv_I]), 0, 0, &hv_HomMat2D);
					HalconCpp::HomMat2dTranslate(hv_HomMat2D, HTuple(hv_Row[hv_I]), HTuple(hv_Column[hv_I]),
						&hv_HomMat2D);
					HalconCpp::AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);
					if (HDevWindowStack::IsOpen())
						SetColor(WindowHandle/*HDevWindowStack::GetActive()*/, "green");
					if (HDevWindowStack::IsOpen())
						DispObj(ho_TransContours, WindowHandle/*HDevWindowStack::GetActive()*/);
					if (HDevWindowStack::IsOpen())
					{
						DispCross(WindowHandle/*HDevWindowStack::GetActive()*//*WindowHandle*/, HTuple(hv_Row[hv_I]), HTuple(hv_Column[hv_I]), 100, HTuple(hv_Angle[hv_I]));
						WriteString(WindowHandle/*HDevWindowStack::GetActive()*/, "(" + (HTuple(hv_Row[hv_I]).TupleString("3.1f") + "," + HTuple(hv_Column[hv_I]).TupleString("3.1f") + "," + HTuple(hv_Angle[hv_I].D() * 180 / PI).TupleString("3.3f") + ")"));
					}
					// stop(...); only in hdevelop
				}
			}
			if (!ret)
			{
				SetColor(WindowHandle, "red");
				WriteString(WindowHandle, "detect failed");
			}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}
	
	return ret;
}

bool ImgProcess::Find(char * pPathName, int & iNum, double &lfMinScore)
{
	bool ret = true;
	HObject hregion;
	HObject h0_Imagerow;
	ho_Image.Clear();
	try
	{
		//如果MODELID不存在
		int len = hv_ModelID.TupleLength();
		if (len)
		{
			int idModel = hv_ModelID[0].I();
			if (idModel < 0)
				return false;
		}
		else
		{
			return false;
		}

		ReadImage(&ho_Image, (HTuple)pPathName);
	
		//FIND
		HalconCpp::FindShapeModel(ho_Image, hv_ModelID,
			HTuple(m_paraFind.AngleStart).TupleRad(),
			HTuple(m_paraFind.AngleExtent).TupleRad(),
			m_paraFind.MinScore,
			m_paraFind.NumMatches,
			m_paraFind.MaxOverlap,
			m_paraFind.SubPixel.c_str()/*SubPixel*/,
			(HTuple(m_paraFind.ModelLevels).Append(m_paraFind.NumLevels)),
			m_paraFind.Greediness,
			&hv_Row, &hv_Column,&hv_Angle, &hv_Score);
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		return false;
	}

	//输出
	iNum = hv_Score.TupleLength();
	if (iNum == 0)
	{
		ret = false;
	}
	if (m_PosX)
	{
		delete[]m_PosX; m_PosX = NULL;
	}
	if (m_PosY)
	{
		delete[]m_PosY; m_PosY = NULL;
	}
	if (m_RotDeg)
	{
		delete[]m_RotDeg; m_RotDeg = NULL;
	}
	if (m_Score)
	{
		delete[]m_Score; m_Score = NULL;
	}
	m_targetNum = iNum;
	m_PosX = new double[iNum];
	m_PosY = new double[iNum];
	m_RotDeg = new double[iNum];
	m_Score = new double[iNum];
	for (int i = 0; i<iNum; i++)
	{
		m_PosX[i] = hv_Column[i].D();
		m_PosY[i] = hv_Row[i].D();
		m_RotDeg[i] = hv_Angle[i].D();
		m_Score[i] = hv_Score[i].D();
	}
	
	if (lfMinScore > 0.0)
	{
		int m = 0;
		double maxscore = 0;
		for (int i = 0; i<iNum; i++)//求匹配分数最大值
		{
			if (maxscore <= m_Score[i])
				maxscore = m_Score[i];
		}
		//如果匹配分数最大值大于传入的比较参数，则显示出来返回真，
		if (lfMinScore > maxscore)
			//ret = false;
			return false;
	}
	//Matching 02: Transform the model contours into the detected positions
	if (hl_wnd)
	{
		try {
			HTuple hv_Width, hv_Height;
			GetImageSize(ho_Image, &hv_Width, &hv_Height);
			int w = hv_Width.D();
			int h = hv_Height.D();
			HalconCpp::SetSystem("tsp_width", hv_Width);
			HalconCpp::SetSystem("tsp_height", hv_Height);
			if (HDevWindowStack::IsOpen())
				HalconCpp::CloseWindow(HDevWindowStack::Pop());
			SetWindowAttr("border_width", 0); //设置窗口属性
			SetWindowAttr("background_color", "white");
			OpenWindow(m_rect.top, m_rect.left, m_rect.Width(), m_rect.Height(), hl_wnd, "", "", &WindowHandle);
			HDevWindowStack::Push(WindowHandle);
			HDevWindowStack::SetActive(WindowHandle);
		}
		catch (HalconCpp::HException & except)
		{
			ShowException(except);
			return false;
		}
	}
	if (HDevWindowStack::IsOpen())	
	{
		DispObj(ho_Image, WindowHandle/*HDevWindowStack::GetActive()*/);
		HTuple end_val41 = (hv_Score.TupleLength()) - 1;
		HTuple step_val41 = 1;
		for (hv_I = 0; hv_I.Continue(end_val41, step_val41); hv_I += step_val41)
		{
			HomMat2dIdentity(&hv_HomMat2D);
			HalconCpp::HomMat2dRotate(hv_HomMat2D, HTuple(hv_Angle[hv_I]), 0, 0, &hv_HomMat2D);
			HalconCpp::HomMat2dTranslate(hv_HomMat2D, HTuple(hv_Row[hv_I]), HTuple(hv_Column[hv_I]),
				&hv_HomMat2D);
			HalconCpp::AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);
			if (HDevWindowStack::IsOpen())
				SetColor(WindowHandle/*HDevWindowStack::GetActive()*/, "green");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_TransContours, WindowHandle/*HDevWindowStack::GetActive()*/);
			if (HDevWindowStack::IsOpen())
			{
				DispCross(WindowHandle/*HDevWindowStack::GetActive()*//*WindowHandle*/, HTuple(hv_Row[hv_I]), HTuple(hv_Column[hv_I]), 100, HTuple(hv_Angle[hv_I]));
				WriteString(WindowHandle/*HDevWindowStack::GetActive()*/, "("+(HTuple(hv_Row[hv_I]).TupleString("3.1f") + "," + HTuple(hv_Column[hv_I]).TupleString("3.1f") + "," + HTuple(hv_Angle[hv_I].D() * 180 / PI).TupleString("3.3f") + ")"));
			}
		}
		if (!ret)
		{
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "detect failed");
		}
	}
	return ret;
}

bool ImgProcess::FindFromCam()
{
	//采集图像
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	if (!m_bManu)
	{
		g_criSection.Lock();
		m_pCam->GrabOneImage(&pData, stImageInfo);
		g_criSection.Unlock();
		if (!pData)
		{
			if (WindowHandle.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				SetColor(WindowHandle, "red");
				WriteString(WindowHandle, "Camera Error:相机错误");
			}
			return false;
		}
	}


	try
	{
		HalconCpp::HObject ho_ImageReduce;
		if (!m_bManu)
		{
			ho_Image.Clear();
			ConvertoHalcon(pData, stImageInfo, ho_Image);
		}

		if (WindowHandle.Length())
			DispObj(ho_Image, WindowHandle);
		
		//裁剪图像
		if (m_ModelDetectArea.bAvalid && m_ho_ModelDetectArea.IsInitialized())
		{	
			ReduceDomain(ho_Image, m_ho_ModelDetectArea, &ho_ImageReduce);
			if (m_bShowDetectArea)//是否显示检测区域
			{
				DispObj(m_ho_ModelDetectArea, WindowHandle);
			}
		}
		else
		{
			ho_ImageReduce = ho_Image.Clone();
		}
		//WriteImage(ho_Image, "bmp", 0, "image");
		//WriteImage(ho_ImageReduce, "bmp", 0, "reduceImage");
		//识别图像
		HalconCpp::FindShapeModel(ho_ImageReduce, hv_ModelID,
			HTuple(m_paraFind.AngleStart).TupleRad(),
			HTuple(m_paraFind.AngleExtent).TupleRad(),
			m_paraFind.MinScore,
			m_paraFind.NumMatches,
			m_paraFind.MaxOverlap,
			m_paraFind.SubPixel.c_str()/*SubPixel*/,
			(HTuple(m_paraFind.ModelLevels).Append(m_paraFind.NumLevels)),
			m_paraFind.Greediness,
			&hv_Row, &hv_Column, &hv_Angle, &hv_Score);
	}
	catch (HalconCpp::HException & except)
	{
		if (WindowHandle.Length())
		{
			SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "Find Error:算法错误，请检查图像");
			WriteImage(ho_Image, "bmp", 0, "FindError"); //保存图像
		}

		ShowException(except);
		return false;
	}
	//显示结果并返回
	int iNum = hv_Score.TupleLength();
	if (iNum == 0)
	{
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		SetColor(WindowHandle, "red");
		WriteString(WindowHandle, "Detect Failed:找不到模板，请检查图像或光源");
		WriteImage(ho_Image, "bmp", 0, "DetectFailed"); //保存图像
		return false;
	}
	if (m_PosX)
	{
		delete[]m_PosX; m_PosX = NULL;
	}
	if (m_PosY)
	{
		delete[]m_PosY; m_PosY = NULL;
	}
	if (m_RotDeg)
	{
		delete[]m_RotDeg; m_RotDeg = NULL;
	}
	if (m_Score)
	{
		delete[]m_Score; m_Score = NULL;
	}
	m_targetNum = iNum;
	m_PosX = new double[iNum];
	m_PosY = new double[iNum];
	m_RotDeg = new double[iNum];
	m_Score = new double[iNum];
	for (int i = 0; i < iNum; i++)
	{
		m_PosX[i] = hv_Column[i].D();
		m_PosY[i] = hv_Row[i].D();
		m_RotDeg[i] = hv_Angle[i].D();
		m_Score[i] = hv_Score[i].D();
	}
	try
	{
		int len = (hv_Score.TupleLength());
		int idx = 1;
		if (WindowHandle.Length())
			SetColor(WindowHandle, "green");
		for (idx = 0; idx < len; idx++)
		{
			HomMat2dIdentity(&hv_HomMat2D);
			HalconCpp::HomMat2dRotate(hv_HomMat2D, HTuple(hv_Angle[idx]), 0, 0, &hv_HomMat2D);
			HalconCpp::HomMat2dTranslate(hv_HomMat2D, HTuple(hv_Row[idx]), HTuple(hv_Column[idx]),
				&hv_HomMat2D);
			HalconCpp::AffineTransContourXld(ho_ModelContours, &ho_TransContours, hv_HomMat2D);
			HObject ho_ModelRegion_Trans;
			//HalconCpp::AffineTransContourXld(m_ho_ModelRegion, &ho_ModelRegion_Trans, hv_HomMat2D);

			if (WindowHandle.Length())
			{
				DispObj(ho_TransContours, WindowHandle);
				//DispObj(ho_ModelRegion_Trans, WindowHandle);
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				WriteString(WindowHandle, "ABS:(" + (HTuple(hv_Row[idx]).TupleString("3.1f") + "," + HTuple(hv_Column[idx]).TupleString("3.1f") + "," + HTuple(hv_Angle[idx].D() * 180 / PI).TupleString("3.3f") + ")"));
				SetTposition(WindowHandle, HTuple(m_TextRow+60), HTuple(m_TextCol));
				WriteString(WindowHandle, "REL:(" + (HTuple(hv_Row[idx]-m_fModelY).TupleString("3.1f") + "," + HTuple(hv_Column[idx] - m_fModelX).TupleString("3.1f") + "," + HTuple(hv_Angle[idx].D() * 180 / PI).TupleString("3.3f") + ")"));
			}
		}
	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		SetColor(WindowHandle, "red");
		SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
		WriteString(WindowHandle, "Find Error:算法错误，请检查图像");
		WriteImage(ho_Image, "bmp", 0, "FindError"); //保存图像
		return false;
	}
	return true;
}

bool ImgProcess::FindBarCode(CString & codestr, bool bgetimage)
{
	//采集图像
	
	unsigned char * pData = NULL;
	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	if (bgetimage)
	{
		g_criSection.Lock();
		m_pCam->GrabOneImage(&pData, stImageInfo);
		g_criSection.Unlock();
		if (!pData)
		{
			if (WindowHandle.Length())
			{
				SetTposition(WindowHandle, HTuple(m_TextRow), HTuple(m_TextCol));
				SetColor(WindowHandle, "red");
				WriteString(WindowHandle, "Camera Error");
			}
			return false;
		}
		ho_Image.Clear();
	}
	
	try
	{
		HalconCpp::HObject ho_ImageReduce;
		if (bgetimage)
		{
			ConvertoHalcon(pData, stImageInfo, ho_Image);
		}
		if (WindowHandle.Length())
			DispObj(ho_Image, WindowHandle);

		//裁剪图像
		if (m_BarcodeDetectArea.bAvalid && m_ho_DetectBarcodeArea.IsInitialized())
		{
			ReduceDomain(ho_Image, m_ho_DetectBarcodeArea, &ho_ImageReduce);
			if (m_bShowDetectArea)//是否显示检测区域
			{
				DispObj(m_ho_DetectBarcodeArea, WindowHandle);
			}
		}
		else
		{
			ho_ImageReduce = ho_Image.Clone();
		}
		//查找二维码
		HObject ho_SymbolXLDs;
		HTuple  hv_Area, hv_Row, hv_Col, hv_Order;
		m_hv_DecodedDataStrings.Clear();
		FindDataCode2d(ho_ImageReduce, &ho_SymbolXLDs, m_hv_DataCodeHandle, HTuple(), HTuple(),
			&m_hv_ResultHandles, &m_hv_DecodedDataStrings);
		if (m_hv_DecodedDataStrings.Length())
		{
			codestr = m_hv_DecodedDataStrings.S();
			//显示
			SetColor(WindowHandle, "green");
			DispObj(ho_SymbolXLDs, WindowHandle);
			//AreaCenterXld(ho_SymbolXLDs, &hv_Area, &hv_Row, &hv_Col, &hv_Order);
			SetTposition(WindowHandle, m_TextRowBarcode, m_TextColBarcode);
			WriteString(WindowHandle, m_hv_DecodedDataStrings);
		}
		else
		{
			SetTposition(WindowHandle, HTuple(m_TextRowBarcode), HTuple(m_TextColBarcode));
			SetColor(WindowHandle, "red");
			WriteString(WindowHandle, "barCode failed");
			return false;
		}


	}
	catch (HalconCpp::HException & except)
	{
		ShowException(except);
		SetColor(WindowHandle, "red");
		SetTposition(WindowHandle, HTuple(m_TextRowBarcode), HTuple(m_TextColBarcode));
		WriteString(WindowHandle, "barCodeFind Error");
		return false;
	}
	return true;
}

