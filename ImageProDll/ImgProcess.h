/*
purpose:			算法类封装
date:				2017-12-25
author:				吴汉权
last modification:	2017-12-25	吴汉权	新建
					2017-12-26  吴汉权  修改成加载模板，并识别。
					2019-02-23  吴汉权  增加相机，roi设置
*/
#pragma once
#include "stdafx.h"
# include "HalconCpp.h"
# include "HDevThread.h"
#include "myCameraClass.h"
# include "ConvertPixel.h"
#include <string>
#include <vector>
using namespace std;
using namespace HalconCpp;
#define CALIBRAT_PT_NUM 9

typedef struct xPara {
	double AngleStart;  //起始角度
	double AngleExtent; //角度范围
	double MinScore;	//最小分数 匹配度（0到1之间）
	int    NumMatches;  //匹配个数（0代表尽可能多的匹配)
	double MaxOverlap;  //最大重叠
	string SubPixel;    //亚像素
	int	   ModelLevels; //模板金字塔级
	int    NumLevels;   //金字塔级
	double Greediness;  //贪婪算法
	xPara() {
		AngleStart = -90;
		AngleExtent =180;
		MinScore = 0.5;
		NumMatches = 0;
		MaxOverlap = 0.5;
		SubPixel = "least_squares";
		ModelLevels = 8;
		NumLevels = 1;
		Greediness = 0.75;
	}
}XPARA;

//检测区域
typedef struct DetectArea{
	bool bAvalid;  //是否有效
	int row1;	   //左上角坐标
	int col1;
	int row2;	   //右下角坐标
	int col2;
	DetectArea() {
		bAvalid = false;
		row1	= -1;
		col1	= -1;
		row2	= -1;
		col2	= -1;
	}
}DetectArea;

typedef struct RectArea {
	bool bAvalid;  //是否有效
	int row1;	   //左上角坐标
	int col1;
	int row2;	   //右下角坐标
	int col2;
	RectArea() {
		bAvalid = false;
		row1 = -1;
		col1 = -1;
		row2 = -1;
		col2 = -1;
	}
}RectArea;

typedef struct MyPoint2f {
	double fx;
	double fy;
	MyPoint2f() {
		fx = 0.0;
		fy = 0.0;
	}
}MyPoint2f;

class ImgProcess {
public:
	ImgProcess();
	~ImgProcess();
	bool SaveParaFind(char* pPath);		//保存匹配参数
	bool SaveParaFind(CString strPath); //保存匹配参数
	bool LoadParaFind(char* pPath);		//加载匹配参数
	bool LoadParaFind(CString strPath); //加载匹配参数
	bool LoadDetectArea(char* pPath);	//加载检测区域参数
	bool DrawModelDetectArea(char* pSavePath); //框选模板区域并保存参数
	bool DrawBarcodeDetectArea(char* pSavePath); //框选二维码检测区域并保存参数
	bool SaveDetectArea(char* pPath);	//保存检测区域参数
	void ShowDetectArea(bool bShow);    //是否显示检测区域参数
	bool IsLoadModel();
	bool SetDispWnd(Hlong wnd, CRect showRect);
	bool LoadModel(char* pModelPath, char* pModelImagePath); //从其他地方加载模板文件； pModelPath：模板的路径和名称例如："C:/Users/wuhq/Desktop/Matching 01.shm"
	bool LoadModel(CString pModelPath, CString pModelImagePath);
	bool LoadModel(char* pModelPath);
	void SetModelPathName(char* pModelPath);						//设置模板路径 + 名称 + 无后缀
	bool CreateModel(BYTE *pImage, int imW, int imH);				//创建模板，对整一张传入的图片做模板
	bool CreateModel(BYTE *pImage, int imW, int imH, char* ModImgPathname);    //使用一张小图创建临时模板在大图中匹配得到区域，用此区域以及大图创建模板。   pImage: 用于创建模板的大图。 imW，imH：大图长宽。 ModImgPathname：事先截取的模板图像。
	bool CreateModel(char* pModePath);								//创建模板 根据路径创建模板
	bool CreateModel(char* pModePath, std::vector<CRect> &vctRoi);	//创建模板 传入多个ROI
	bool CreateModelFromCam();										//从相机采集图像并制作模板
	bool CreateModelFromModel();									//使用预设的图像粗略匹配并框选模板，制作模板。
	bool SaveModel(char* pModelPath);								//保存模板
	bool Find(BYTE *pImage,int imW,int imH, int &iNum,double &lfMinScore); //在pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore)
	bool Find(char* pPathName, int &iNum, double &lfMinScore);		//从路径中加载图像，pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore)
	bool FindFromCam();				//从相机采集图像并识别
	bool FindBarCode(CString & codestr, bool bgetimage=true); //查找二维码
	void disp_message(HTuple hv_WindowHandle, HTuple hv_String, HTuple hv_CoordSystem,HTuple hv_Row, HTuple hv_Column, HTuple hv_Color, HTuple hv_Box);
	void ShowException(HalconCpp::HException & except);
	bool LoadModelImage(char* pModelImagePath); //
	int	 ConvertoHalcon(unsigned char * pData, MV_FRAME_OUT_INFO_EX stImageInfo, HalconCpp::HObject& image);
	void GrabOneImage();
	bool SetContinueShow(bool bCont);
	bool SetExposureTime(double ExpTime);
	bool Calibrate9(double* Imx, double * Imy, double* Spx, double* Spy); //九点标定
	bool CalibrateRot(double* Imx, double* Imy, int ptNum);  //旋转标定
	bool SaveCalibrate9(char* pPath);	//保存9点坐标
	bool LoadCalibrate9(char* pPath);   //加载9点坐标并标定
	bool Calibration();  //整个标定过程（非9点标定）
	bool CalibrationStep(int ptIndex, double Spx, double Spy); //9点标定 的每个点圆的提取过程, 传入物理坐标spx, spy
	bool CheckCalibrate(double &Spx, double &Spy); //检验标定结果。 拍照框选目标得到图像坐标输出对应的物理坐标Spx Spy;
	bool CalibrateRotStep(int ptIndex);	//旋转标定
	bool DrawMyRectangle(HalconCpp::HTuple HWindowID, HalconCpp::HTuple & RowTmp, HalconCpp::HTuple & ColumnTmp, HalconCpp::HTuple & PhiTmp, HalconCpp::HTuple & Length1Tmp, HalconCpp::HTuple & Length2Tmp);
	bool DrawMyRectangle1(HalconCpp::HTuple HWindowID, HalconCpp::HTuple & RowTmp1, HalconCpp::HTuple & ColumnTmp1, HalconCpp::HTuple & RowTmp2, HalconCpp::HTuple & ColumnTmp2);
	bool FindMyCircle(HObject srcImg, HObject regionROI, HTuple &RowY, HTuple &ColX, HTuple &Radius); //regionROI: 圆所在区域， RowY： 圆心行坐标
	void Image2World(double* Imx, double * Imy, double* Spx, double* Spy);  //图像坐标转换为物理坐标
	bool SavePicture(char* path); //保存当前图像
	void SetFullScreen(bool bFull); //设置显示全部
	void CloseCamera();           //关闭相机
	void LoadImageFromFile();			  //文件中加载图像并显示
	void SetManuMode(bool bMan);		  //设置为手动加载图像模式	

	//v2.0接口
	//框选转接板上特征模板及检索区域(只有当移动过相机和平台的相对位置，才需要调用)
	bool DrawBaseMark();
	//框选产品上特征模板及检索区域（只有当移动过相机和平台的相对位置，才需要调用）
	bool DrawDutMark();
	//制作模板
	bool CreateMarkModel(HObject Roi, HTuple &hvModel);

	//拍照识别产品上模板（获得偏移量）
	bool FindDutMark(double& spx, double &spy, double &ang);
	//拍照识别转接板上模板 （获得偏移量）
	bool FindBaseMark(double& spx, double &spy, double &ang);


public:
	
	static int g_num;
	int m_iIndex;		 //相机序号
	HTuple	WindowHandle;//窗口句柄，这里用于显示
	bool  m_bManu;		//本地加载图像，不通过相机采集
	//模板坐标
	double m_fModelX;
	double m_fModelY;
	double m_fModelAngle;
	double m_fModelScore;

	//识别结果
	int  m_targetNum;    //找到多少个目标
	double *m_PosX;      //输出点阵X坐标
	double *m_PosY;      //输出点阵Y坐标
	double *m_RotDeg;    //输出点阵旋转角度
	double *m_Score;     //输出点匹配度

private:
	//相机
	myCameraClass *m_pCam;
	unsigned char *m_pImageBuffer;
	unsigned char *m_pDataSeparate;
	double       m_fExporTime;
	
	//界面显示
	Hlong hl_wnd;//控件句柄
	CRect m_rect;//控件矩形
	bool m_bShowDetectArea;
	int  m_TextRow;
	int  m_TextCol;
	int  m_TextRowBarcode;
	int  m_TextColBarcode;

	//模板
	bool m_bLoadModel;		//成功加载模板文件.shm
	bool m_bLoadParaFind;	//成功加载参数
	XPARA m_paraFind;      //查找的参数
	char m_ModelPathAndName[128];
	HObject ho_ModelImage; //保存的模板图像。
	HObject  ho_ModelContours, ho_TransContours, ho_Image;
	HTuple  hv_ModelID, hv_Row1, hv_Column1, hv_Row2;
	HTuple  hv_Column2, hv_RefRow, hv_RefColumn, hv_HomMat2D;
	HTuple  hv_TestImages, hv_T, hv_Row, hv_Column, hv_Angle;
	HTuple  hv_Score, hv_I;

	double m_MRow, m_MCol, m_MPhi, m_MLen1, m_MLen2;
	HObject m_ho_ModelRegion;		//模板矩形
	HObject m_ho_ModelDetectArea;   //模板检测区域（ROI）
	DetectArea m_ModelDetectArea;   //模板检测区域坐标
	vector<MyPoint2f> m_vectMark;	//转接板mark点

	//二维码
	HObject m_ho_DetectBarcodeArea;//二维码检测区域（ROI）
	DetectArea m_BarcodeDetectArea;
	HTuple  m_hv_DataCodeHandle; //二维码句柄
	HTuple  m_hv_ResultHandles, m_hv_DecodedDataStrings;

	//标定
	HObject m_ho_Circles;   //框选识别得到的圆，用于显示
	//九点标定
	HTuple m_hv_HomMat2D;	//仿射矩阵
	bool   m_bIsCalied;		//是否成功标定
	double m_fImx[CALIBRAT_PT_NUM];			//图像坐标x 即 col
	double m_fImy[CALIBRAT_PT_NUM];			//图像坐标y 即 row
	double m_fSpx[CALIBRAT_PT_NUM];			//物理坐标x  即平台坐标x
	double m_fSpy[CALIBRAT_PT_NUM];			//物理坐标y  即平台坐标Y

	double m_fImxTmp[CALIBRAT_PT_NUM];			//图像坐标x 即 col
	double m_fImyTmp[CALIBRAT_PT_NUM];			//图像坐标y 即 row
	double m_fSpxTmp[CALIBRAT_PT_NUM];			//物理坐标x  即平台坐标x
	double m_fSpyTmp[CALIBRAT_PT_NUM];			//物理坐标y  即平台坐标Y
	//旋转标定
	double m_lfCentX; //中心点图像坐标
	double m_lfCentY;
	double m_lfRadiu; //半径像素长度
	double m_lfRotX[CALIBRAT_PT_NUM]; //用于标定的图像坐标x
	double m_lfRotY[CALIBRAT_PT_NUM];
	bool m_bIsCaliedRot;   //是否成功标定旋转

	//V2.0  
	//[转接板]两个模板和检索区域
	RectArea m_BaseROI[2]; //两个检索区域
	RectArea m_BaseModel[2]; //两个模板区域
	HObject m_ho_BaseROI[2]; //halcon类型的检索区域
	HObject m_ho_BaseModel[2]; //halcon类型的模板区域
	HTuple m_hv_BaseModelId[2]; //模板

	//[产品]的两个模板和检索区域
	RectArea m_DutROI[2]; //两个检索区域
	RectArea m_DutModel[2]; //两个模板区域
	HObject m_ho_DutROI[2]; //halcon类型的检索区域
	HObject m_ho_DutModel[2]; //halcon类型的模板区域
	HTuple m_hv_DutModelId[2]; //模板


};