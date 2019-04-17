//数据结构体，数据类型，等定义
#pragma once
#include <string>
#include "stdafx.h"
//////////////////////////////////////////////////////
//宏定义
/////////////////////////////////////////////////////
#define LOG_DEBUG


//任务
#define TXT_INIFILENAME_TASKROOT "Task\\"  //任务文件夹
#define TXT_TASKFILE_TYPE ".Task"
#define TXT_NODE_TASK "Task"
#define SUBNODE_TASKPATH "TaskPath" //任务的路径
#define TXT_NODE_LIGHT "Light"

//相机
#define CAM1_IDX 0 //相机1的序号
#define CAM2_IDX 1 //相机2的序号
#define CAM3_IDX 2 //相机3的序号
#define CAM4_IDX 3 //相机4的序号
#define MIN_CAM_EXP 5000//相机最大曝光范围 5毫秒
#define MAX_CAM_EXP 500000//相机最大曝光范围 500毫秒
#define TXT_INIFILENAME_CAM1 "CamConfig1.ini"        //第一个相机的配置文件完整路径
#define TXT_INIFILENAME_CAM2 "CamConfig2.ini"

#define SUBNODE_LIGHTCTRL_CH1 "lightch1"
#define SUBNODE_LIGHTCTRL_CH2 "lightch2"
#define LIGHT_DEFAUL1		  100		
#define LIGHT_DEFAUL2		  100	


//算法
#define TXT_INIFILENAME_IMG "ImgProcessConfig.ini"	    //配置路径和文件名 
#define TXT_INIFILENAME_IMG_FIX_A_1 "ImgProcessFix1Config.ini"  //治具定位算法的文件名
#define TXT_INIFILENAME_IMG_FIX_A_2 "ImgProcessFix2Config.ini"
#define TXT_INIFILENAME_IMG_FIX_A_3 "ImgProcessFix3Config.ini"
#define TXT_INIFILENAME_IMG_FIX_A_4 "ImgProcessFix4Config.ini"
#define TXT_MODELFILENAME		 "Model.shm"			//产品定位模板文件名
#define TXT_MODELFILENAME_FIX_A_1	 "ModelFix1.shm"		//治具1定位模板文件名
#define TXT_MODELFILENAME_FIX_A_2	 "ModelFix2.shm"		//治具2定位模板文件名
#define TXT_MODELFILENAME_FIX_A_3	 "ModelFix3.shm"		//治具3定位模板文件名
#define TXT_MODELFILENAME_FIX_A_4	 "ModelFix4.shm"		//治具4定位模板文件名


#define TXT_MODELFILENAME_IMG		 "Model.bmp"			//产品定位模板文件名
#define TXT_MODELFILENAME_FIX1_IMG_A	 "ModelFix1.jpg"		//治具1定位模板文件名
#define TXT_MODELFILENAME_FIX2_IMG_A	 "ModelFix2.jpg"		//治具2定位模板文件名
#define TXT_MODELFILENAME_FIX3_IMG_A	 "ModelFix3.jpg"		//治具3定位模板文件名
#define TXT_MODELFILENAME_FIX4_IMG_A	 "ModelFix4.jpg"		//治具4定位模板文件名



#define  PI 3.1415926535897932384626433832795
#define  RAD_0 0			//0度的弧度值
#define  RAD_90 1.570796	//90度的弧度值
#define	 RAD_90P (-1.570796)//-90度
#define  RAD_180 PI			//180
#define  RAD_180P (-PI)		//-180
#define  RAD_DELTA 0.174533	//允许的范围10度

//治具
//#define FIX_NUM 4  //治具的个数
//#define FIX1 0
//#define FIX2 1
//#define FIX3 2
//#define FIX4 3
enum FixEnum {
	FIX1=0,
	FIX2,
	FIX3,
	FIX4,
	FIX_NUM
};
#define TIME_WAIT_TESTPC 5000 //治具完成到测试PC也完成的超时时间
#define TIME_WAIT_STOP	10	  //查询停止信号的超时时间
#define TIME_WAIT_PLCWRITE 5 //查询plc写事件的等待超时时间
#define TIME_WAIT_PLCREAD 5  //查询plc读事件的等待超时时间
#define TIME_WAIT_ROBOTBACK 30000 //机械手复位超时时间

#define TXT_INIFILENAME_FIX1 "FixConfig1.ini"
#define TXT_INIFILENAME_FIX2 "FixConfig2.ini"
#define TXT_INIFILENAME_FIX3 "FixConfig3.ini"
#define TXT_INIFILENAME_FIX4 "FixConfig4.ini"
#define TXT_NODE_PARA_SERIAL "SerialPara"
#define SUBNODE_PORT		"portnr"
#define SUBNODE_BAUD		"baud"
#define SUBNODE_PARITY		"parity"
#define SUBNODE_DATABITS	"databits"
#define SUBNODE_STOPBITS	"stopbits"

#define TXT_FIX1 "FIX_1"
#define TXT_FIX2 "FIX_2"
#define TXT_FIX3 "FIX_3"
#define TXT_FIX4 "FIX_4"

//PLC 串口号 com1 = 0; com2 = 1
#define COMM_PLC 4
#define COMM_LIGHT 4
////机械手
#define TXT_INIFILENAME_ROBOT "RobotConfig.ini"
#define TXT_NODE_TCP "TCPconfig"
#define TXT_SUBNODE_TCP_NAME "Name"
#define TXT_SUBNODE_TCP_ADDR "IPAddr"
#define TXT_SUBNODE_TCP_PORT "Port"

enum RobotAxis {
	Axis_X = 0,
	Axis_Y,
	Axis_Z,
	Axis_R,
	ROBOT_AXIS_NUM
};
#define TXT_NODE_DUT_LOCATE_POS_ROBOT "RobotPos_DUT_LOCATE"  //相机定位产品的位置
#define TXT_NODE_FIX1_LOCATE_POS_ROBOT "RobotPos_FIX1_LOCATE"  //相机定位治具1的位置
#define TXT_NODE_FIX2_LOCATE_POS_ROBOT "RobotPos_FIX2_LOCATE"  
#define TXT_NODE_FIX3_LOCATE_POS_ROBOT "RobotPos_FIX3_LOCATE"  
#define TXT_NODE_FIX4_LOCATE_POS_ROBOT "RobotPos_FIX4_LOCATE"  

#define TXT_NODE_DUT_CAP_POS_ROBOT "RobotPos_DUT_CAP"  //相机抓取产品的位置
#define TXT_NODE_FIX1_CAP_POS_ROBOT "RobotPos_FIX1_CAP"  //相机抓取治具1的位置
#define TXT_NODE_FIX2_CAP_POS_ROBOT "RobotPos_FIX2_CAP"  //
#define TXT_NODE_FIX3_CAP_POS_ROBOT "RobotPos_FIX3_CAP"  
#define TXT_NODE_FIX4_CAP_POS_ROBOT "RobotPos_FIX4_CAP"  
//
#define TXT_NODE_OK_POS_ROBOT "RobotPos_OK"  //ok产品的放置位置
#define TXT_NODE_NG_POS_ROBOT "RobotPos_NG"  //ng产品的放置位置

#define TXT_SUBNODE_AXIS_X "X" 
#define TXT_SUBNODE_AXIS_Y "Y"
#define TXT_SUBNODE_AXIS_Z "Z"
#define TXT_SUBNODE_AXIS_U "U"

//标定
#define TXT_INIFILENAME_CABLI1 "CabliConfig1.ini"
#define TXT_INIFILENAME_CABLI2 "CabliConfig1.ini"
#define TXT_INIFILENAME_CABLI3 "CabliConfig1.ini"
#define TXT_INIFILENAME_CABLI4 "CabliConfig1.ini"
#define TXT_NODE_CABLI_PT1	"Image Pt1"
#define TXT_NODE_CABLI_PT2	"Image Pt2"
#define TXT_NODE_CABLI_PT1_MA	"Mach Pt1"
#define TXT_NODE_CABLI_PT2_MA	"Mach Pt2"


/*
//数据结构体
class CFloatPt
{
public:
	double x, y;

public:
	CFloatPt() { x = 0.0F; y = 0.0F; };
	CFloatPt(double x1, double y1) { x = x1; y = y1; };
	CFloatPt(const CFloatPt &pt) { x = pt.x; y = pt.y; };
	CFloatPt(const CPoint &pt) { x = (double)pt.x; y = (double)pt.y; };
	CFloatPt(const POINT & pt) { x = (double)pt.x; y = (double)pt.y; };

	CFloatPt* operator= (CFloatPt*      pPt)
	{
		x = pPt->x; y = pPt->y; return  this;
	};
	CFloatPt& operator= (const CFloatPt& pt)
	{
		x = pt.x; y = pt.y; return *this;
	};
	CFloatPt& operator= (const CPoint&   pt)
	{
		x = (double)pt.x; y = (double)pt.y; return *this;
	};
	CFloatPt& operator= (const POINT&    pt)
	{
		x = (double)pt.x; y = (double)pt.y; return *this;
	};
	CFloatPt& operator- (CFloatPt& pt)
	{
		x -= pt.x; y -= pt.y; return *this;
	};

	double Distance2Point(CFloatPt p = CFloatPt(0, 0))
	{
		return sqrt((x - p.x)* (x - p.x) + (y - p.y)* (y - p.y));
	}
	double Distance2Point(double fX, double fY)
	{
		return sqrt((x - fX)* (x - fX) + (y - fY)* (y - fY));
	}
};
*/
//串口通信参数结构体
typedef struct serialConfig{
	unsigned int	portnr;
	unsigned int	baud;
	unsigned int	parity;
	unsigned int	databits;
	unsigned int	stopbits;
	serialConfig(){
		portnr	= 2;
		baud = 9600;
		parity = 0; //0:None, 1:one, 2:two
		databits = 8;
		stopbits = 0;
	}
} serialConfig;

//TCP通信参数
typedef struct TCPConfig {
	std::string name;
	std::string/*char**/address;
	int			port;
	TCPConfig() {
		name = "CLIENT";
		address = "192.168.0.62";
		port = 10000;
	}
}TCPConfig;

//位置
typedef struct MYPOS {
	//物理坐标
	double PosX;
	double PosY;
	double PosZ;
	double RotU;
	//图像坐标
	double PosImX;
	double PosImY;
	double RotImU;
	//与模板的偏移量(mm)
	double deltaX;
	double deltaY;
	double deltaU;

	MYPOS() {
		PosX = 0.0;
		PosY = 0.0;
		PosZ = 0.0;
		RotU = 0.0;

		PosImX = 0.0;
		PosImY = 0.0;
		RotImU = 0.0;

		deltaX = 0.0;
		deltaY = 0.0;
		deltaU = 0.0;
	}
}MyPos;

//DUT(产品）
typedef struct DUT {
	int nFixIdx;		//工站号
	bool bIsOK;			//检测结果
	MyPos pos;			//位置

	DUT(){
		nFixIdx = -1;
		bIsOK = false;
	}
}DUT;

//治具的状态
enum StateOfFix {
	ST_DOING = 0,//未完成，正在进行中
	ST_DONE_OK,//完成待取
	ST_DONE_NG,//完成待取
	ST_READY//等待放料
};

//旋转矩阵参数
/*
a,-b,c
b, a,d

a = cos(theta)
b = sin(theta)
c = x0
d = y0
*/
/*
typedef struct RotT 
{
	double a;
	double b;
	double c;
	double d;
	RotT()
	{
		a = 0.0;
		b = 0.0;
		c = 0.0;
		d = 0.0;
	}
}RotT;
*/

