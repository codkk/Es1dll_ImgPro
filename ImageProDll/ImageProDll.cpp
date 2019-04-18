// ImageProDll.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ImgProcess.h"
#include "ImageProDll.h"
//#include "myCameraClass.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//ImgProcess m_ImgProcess;

ImageProDll::ImageProDll():
	m_pImgProcess(NULL)
{
	m_pImgProcess = new ImgProcess;

}

ImageProDll::~ImageProDll()
{
	if (m_pImgProcess != NULL)
	{
		delete m_pImgProcess;
		m_pImgProcess = NULL;
	}
}

//bool ImageProDll::SetContinueShow(bool bCont)
//{
//	return m_pImgProcess->SetContinueShow(bCont);
//}

bool ImageProDll::HalconSaveParaFind(CString strPath) //保存匹配参数
{

	return m_pImgProcess->SaveParaFind(strPath);

}

bool ImageProDll::HalconLoadParaFind(CString strPath) //加载匹配参数
{
	return m_pImgProcess->LoadParaFind(strPath);

}

bool ImageProDll::IsHalconLoadModel()
{
	return m_pImgProcess->IsLoadModel();

}

bool ImageProDll::SetHalconDispWnd(HWND wnd, CRect showRect)
{
	Hlong  wnd1 = (Hlong)wnd;
	return m_pImgProcess->SetDispWnd(wnd1, showRect);
}
bool ImageProDll::DrawDetectRoi()
{
	return m_pImgProcess->DrawModelDetectArea("DetectArea.ini");
}
bool ImageProDll::DrawDetectCodebarRoi()
{
	return m_pImgProcess->DrawBarcodeDetectArea("DetectArea.ini");
}
void ImageProDll::ShowDetectArea(bool bshow)
{
	m_pImgProcess->ShowDetectArea(bshow);
}
bool ImageProDll::FindBarCode(CString &strCode, bool bGetImage)
{
	return m_pImgProcess->FindBarCode(strCode, bGetImage);
}
bool ImageProDll::HalconLoadModel(CString pModelPath, CString pModelImagePath)
{
	return m_pImgProcess->LoadModel(pModelPath, pModelImagePath);

}

void ImageProDll::SetModelPathName(char * pathname)
{
	m_pImgProcess->SetModelPathName(pathname);
}

bool ImageProDll::HalconCreateModel(BYTE * pImage, int imW, int imH)
{
	return m_pImgProcess->CreateModel(pImage, imW, imH);
}

bool ImageProDll::HalconCreateModel(BYTE * pImage, int imW, int imH, char * ModImgPathname)
{
	return m_pImgProcess->CreateModel(pImage, imW, imH, ModImgPathname);
}

bool ImageProDll::HalconCreateModel(char * pathname)
{
	return m_pImgProcess->CreateModel(pathname);
}

bool ImageProDll::HalconCreateModel(char * pathname, std::vector<CRect>& vecRoi)
{
	return m_pImgProcess->CreateModel(pathname, vecRoi);
}

bool ImageProDll::HalconFind_A(BYTE *pImage, int imH, int imW, int &iNum, double& lfMinScore) //在pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore)
{
	return m_pImgProcess->Find(pImage, imH, imW, iNum,lfMinScore);
}

bool ImageProDll::HalconFind_B(char* pPathName, int &iNum, double& lfMinScore) //从路径中加载图像，pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore)
{
	return m_pImgProcess->Find(pPathName, iNum,lfMinScore);
}

bool ImageProDll::CreateModel()
{
	return m_pImgProcess->CreateModelFromCam();
}

bool ImageProDll::CreateModelFromModel()
{
	return m_pImgProcess->CreateModelFromModel();
}

bool ImageProDll::FindModel()
{
	return m_pImgProcess->FindFromCam();
}

int ImageProDll::GetTargetNum()
{
	return m_pImgProcess->m_targetNum;
}

bool ImageProDll::GetTarget(int ptIdx, double & x, double & y, double & r)
{
	if ((ptIdx < 0) || (ptIdx >= m_pImgProcess->m_targetNum))
		return false;
	x = m_pImgProcess->m_PosX[ptIdx];  //取第一个图像坐标
	y = m_pImgProcess->m_PosY[ptIdx];
	r = m_pImgProcess->m_RotDeg[ptIdx];
	return true;
}
bool ImageProDll::GetTargetRel(int ptIdx, double & x, double & y, double & r)
{
	if ((ptIdx < 0) || (ptIdx >= m_pImgProcess->m_targetNum))
		return false;
	x = m_pImgProcess->m_PosX[ptIdx] - m_pImgProcess->m_fModelX;  //取第一个图像坐标
	y = m_pImgProcess->m_PosY[ptIdx] - m_pImgProcess->m_fModelY;
	r = m_pImgProcess->m_RotDeg[ptIdx];
	return true;
}
bool ImageProDll::GetTargetRelSp(int ptIdx, double & x, double & y, double & r)
{
	if ((ptIdx < 0) || (ptIdx >= m_pImgProcess->m_targetNum))
		return false;
	double imx1, imy1, imx2, imy2;
	double spx1, spy1, spx2, spy2;
	imx1 = m_pImgProcess->m_PosX[ptIdx];  //取第一个图像坐标
	imy1 = m_pImgProcess->m_PosY[ptIdx];
	imx2 = m_pImgProcess->m_fModelX;
	imy2 = m_pImgProcess->m_fModelY;
	r = m_pImgProcess->m_RotDeg[ptIdx];

	//通过仿射变换将像素坐标转换为物理坐标
	m_pImgProcess->Image2World(&imx1, &imy1, &spx1, &spy1);
	m_pImgProcess->Image2World(&imx2, &imy2, &spx2, &spy2);
	x = spx1 - spx2;
	y = spy1 - spy2;

	return true;
}
//取最大分数值的坐标
bool ImageProDll::GetMaxScoreTarget(double & x, double & y, double & r, double & maxScoreOut)
{
	int ptIdx = 0;
	int MaxIndex = -1;
	double maxScore = 0.0;
	for (ptIdx = 0;ptIdx < m_pImgProcess->m_targetNum;ptIdx++)
	{
		if (maxScore <= m_pImgProcess->m_Score[ptIdx])
		{
			maxScore = m_pImgProcess->m_Score[ptIdx];
			MaxIndex = ptIdx;
		}
	}
	if (-1 != MaxIndex)
	{
		x = m_pImgProcess->m_PosX[MaxIndex];  //取第一个图像坐标
		y = m_pImgProcess->m_PosY[MaxIndex];
		r = m_pImgProcess->m_RotDeg[MaxIndex];
		maxScoreOut = maxScore;
	}
	else
	{
		return false;
	}

	return true;
}

void ImageProDll::GetModelPt(double & x, double & y)
{
	x = m_pImgProcess->m_fModelX;
	y = m_pImgProcess->m_fModelY;
}

int ImageProDll::GetIndex()
{
	return m_pImgProcess->m_iIndex;
	//return 0;
}

void ImageProDll::GrabOneImage()
{
	m_pImgProcess->GrabOneImage();
}

void ImageProDll::LoadImageFromFile()
{
	m_pImgProcess->LoadImageFromFile();
}

void ImageProDll::SetManuMode(bool bMan)
{
	m_pImgProcess->SetManuMode(bMan);
}

void ImageProDll::LoadCalibIni()
{
	m_pImgProcess->SaveCalibrate9("Calib.ini");
	m_pImgProcess->LoadCalibrate9("Calib.ini");
}

bool ImageProDll::Calibrate()
{
	//double Imx[9];
	//double Imy[9];
	//double Spx[9];
	//double Spy[9];

	//Imx[0] = 200;
	//Imy[0] = 200;
	//Spx[0] = 0;
	//Spy[0] = 0;

	//Imx[1] = 100;
	//Imy[1] = 200;
	//Spx[1] = -0.25;
	//Spy[1] = 0;

	//Imx[2] = 100;
	//Imy[2] = 100;
	//Spx[2] = -0.25;
	//Spy[2] =  0.25;

	//Imx[3] = 200;
	//Imy[3] = 100;
	//Spx[3] = 0;
	//Spy[3] = 0.25;

	//Imx[4] = 300;
	//Imy[4] = 100;
	//Spx[4] = 0.25;
	//Spy[4] = 0.25;

	//Imx[5] = 300;
	//Imy[5] = 200;
	//Spx[5] = 0.25;
	//Spy[5] = 0;

	//Imx[6] = 300;
	//Imy[6] = 300;
	//Spx[6] = 0.25;
	//Spy[6] = -0.25;

	//Imx[7] = 200;
	//Imy[7] = 300;
	//Spx[7] = 0;
	//Spy[7] = -0.25;

	//Imx[8] = 100;
	//Imy[8] = 300;
	//Spx[8] = -0.25;
	//Spy[8] = -0.25;

	//return m_pImgProcess->Calibrate9(Imx, Imy, Spx, Spy);
	return m_pImgProcess->Calibration();
}

bool ImageProDll::Calibrate(int ptIndex, double Spx, double Spy)
{
	return m_pImgProcess->CalibrationStep(ptIndex, Spx, Spy);
}

bool ImageProDll::CheckCalibrate(double & Spx_out, double & Spy_out)
{
	return m_pImgProcess->CheckCalibrate(Spx_out, Spy_out);
}

bool ImageProDll::CalibrateRot(int ptIndex)
{
	return m_pImgProcess->CalibrateRotStep(ptIndex);
}

bool ImageProDll::SetExposureTime(double fExpTime)
{
	return m_pImgProcess->SetExposureTime(fExpTime);
}

bool ImageProDll::SaveImage(char * path)
{
	return m_pImgProcess->SavePicture(path);
}

void ImageProDll::SetFullScreen(bool bFull)
{
	m_pImgProcess->SetFullScreen(bFull);
	return;
}

void ImageProDll::CloseCamera()
{
	m_pImgProcess->CloseCamera();
}

bool ImageProDll::DrawBaseMark()
{
	return false;
}

bool ImageProDll::DrawDutMark()
{
	return false;
}

bool ImageProDll::FindDutMark(double & spx, double & spy, double & ang)
{
	return false;
}

bool ImageProDll::FindBaseMark(double & spx, double & spy, double & ang)
{
	return false;
}
