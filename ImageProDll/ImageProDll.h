#pragma once
#include <vector>
class ImgProcess;
//class myCameraClass;
class AFX_EXT_CLASS ImageProDll
{
public:
	ImageProDll();
	~ImageProDll();
	//bool SetContinueShow(bool bCont);					  //设置相机连续显示
	
	bool HalconSaveParaFind(CString strPath); //保存匹配参数
	bool HalconLoadParaFind(CString strPath); //加载匹配参数
	bool IsHalconLoadModel();
	bool HalconLoadModel(CString pModelPath, CString pModelImagePath);
	void SetModelPathName(char* pathname);											//设置保存模板的路径，和模板的名称包含后缀
	bool HalconCreateModel(BYTE *pImage, int imW, int imH);							 //创建模板,参数：只接受单通道图像
	bool HalconCreateModel(BYTE *pImage, int imW, int imH, char* ModImgPathname);    //使用一张小图创建临时模板在大图中匹配得到区域，用此区域以及大图创建模板。   pImage: 用于创建模板的大图。 imW，imH：大图长宽。 ModImgPathname：事先截取的模板图像。  
	bool HalconCreateModel(char* pathname);											 //创建模板，从路径中加载图像
	bool HalconCreateModel(char* pathname, std::vector<CRect> &vecRoi);				 //创建模板 传入原始图像，和多个ROI
	bool HalconFind_A(BYTE *pImage, int imH, int imW, int &iNum,double& lfMinScore); //在pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore),参数：只接受单通道图像
	bool HalconFind_B(char* pPathName, int &iNum, double& lfMinScore); //从路径中加载图像，pImage中定位模板，得到个数iNum, 坐标（pX,pY,pAng), 匹配度(pScore),参数：只接受单通道图像
	bool SetHalconDispWnd(HWND wnd, CRect showRect);
	int GetTargetNum();  //获取查找到的目标数
	bool GetMaxScoreTarget(double &x, double&y, double&r, double&maxScoreOut);
	void GetModelPt(double&x, double&y);  //获取模板的中心坐标
	int GetIndex();						  //获取当前算法对象的序号也是相机的序号
	
	
	//V1.0 接口
	bool DrawDetectRoi();						//框选并设置检测模板区域
	bool DrawDetectCodebarRoi();				//框选并设置检测二维码区域
	void ShowDetectArea(bool bshow);
	bool FindBarCode(CString &strCode, bool bGetImage);			//检测二维码 bGetImage:是否从相机获取新图像
	bool CreateModel();							//创建模板
	bool CreateModelFromModel();				//通过识别图像，自动框选模板区域， 创建模板
	bool FindModel();							//查找模板
	bool GetTarget(int ptIdx, double&x, double&y, double&r); //获取第n个目标的 x, y, theta
	bool GetTargetRel(int ptIdx, double& x, double &y, double &r); //获取第n个目标相对于模板的偏移量pix
	bool GetTargetRelSp(int ptIdx, double& x, double &y, double &r); //获取第n个目标相对于模板的偏移量物理量mm
	void GrabOneImage();				  //采集一次图像并显示
	void LoadImageFromFile();			  //文件中加载图像并显示
	void SetManuMode(bool bMan);		  //设置为手动加载图像模式		
	void LoadCalibIni();
	bool Calibrate();//3点标定过程
	bool Calibrate(int ptIndex, double Spx, double Spy);//9点标定。平台每走一个点位调用一次，第9次时自动标定完成。返回TRUE;
	bool CheckCalibrate(double& Spx_out, double& Spy_out); //标定检验，参数：Spx_out：计算得到的y轴物理坐标。   函数包括了拍照框选得到图像坐标转化为物理坐标（如果标定板位置不变则物理坐标不变。 但标定板位置移动了也没关系，两个物理坐标的差值不变）。
	bool CalibrateRot(int ptIndex);	//旋转标定
	bool SetExposureTime(double fExpTime); //设置曝光
	bool SaveImage(char* path); //保存当前图像
	void SetFullScreen(bool bFull); //设置显示全部
	void CloseCamera(); //关闭相机

	//V2.0接口（对应着使用步骤）
	//框选转接板上特征模板及检索区域(只有当移动过相机和平台的相对位置，才需要调用)
	bool DrawBaseMark();
	//框选产品上特征模板及检索区域（只有当移动过相机和平台的相对位置，才需要调用）
	bool DrawDutMark();

	//拍照识别产品上模板（获得偏移量）
	bool FindDutMark(double& spx, double &spy, double &ang);
	//拍照识别转接板上模板 （获得偏移量）
	bool FindBaseMark(double& spx, double &spy, double &ang);

	//计算产品偏移量与转接板上偏移量总和减掉保存的补偿量，得到移动平台的移动量（这一步可以在外面程序做）


private:
	ImgProcess *m_pImgProcess;
};