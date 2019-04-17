// --------------------------------------------------------
//┏━━━━━━━━━━常用程序库说明━━━━━━━━━━┓
//┃													  ┃
//┃　 该库中收集各种常用的程序，其中有处理文件名、测量程 ┃
//┃                                                      ┃
//┃序运行时间、判断操作系统...等等一些比较有用的函数。   ┃
//┃                                                      ┃
//┃希望得到大家的支持，如果有什么好的建议可以直接向我来  ┃
//┃                                                      ┃
//┃提。但是为了该库的进一步完善，请大家尽量不要对该库进  ┃
//┃                                                      ┃
//┃行直接修改。										  ┃
//┃                                                      ┃
//┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
//---------------------------------------------------------
#ifndef _FUN_COM
#define _FUN_COM

#ifndef PI
#define PI			3.1415926535897932384626433832795028841971693993751
#endif

#define R2D				57.295779513082323
#define Deg2Rad( Deg )	(Deg)/ R2D		// 角度到弧度
#define Rad2Deg( Rad )	(Rad)* R2D		// 弧度到角度

#include "math.h"

// 二维点的坐标类型定义
struct LFloatPt2D
{
	double	x;		// x坐标
	double	y;		// y坐标
	
	LFloatPt2D( ) { x= y= 0; };
	LFloatPt2D( double Ix, double Iy ) { x= Ix; y= Iy; };
	
	LFloatPt2D operator - ()
	{
		return LFloatPt2D( -x, -y );
	};
	friend LFloatPt2D operator + (const LFloatPt2D &p1, const LFloatPt2D &p2)
	{
		return LFloatPt2D( p1.x+ p2.x, p1.y+ p2.y );
	};
	friend LFloatPt2D operator - (const LFloatPt2D &p1, const LFloatPt2D &p2)
	{
		return LFloatPt2D( p1.x- p2.x, p1.y- p2.y );
	};
	double Distance2P( LFloatPt2D p = LFloatPt2D(0,0)) // Added by QZQ 
	{
		return sqrt( (x-p.x)* (x-p.x) + (y-p.y)* (y-p.y) );
	}
};

// 显示提取的图像特征的变量和函数
#include <afxtempl.h>
typedef	CList< LFloatPt2D, LFloatPt2D >	LFPtList;

//------------------------------------------文件名操作处理的函数
//--获得下一幅图像的文件名
CString	GetNextSerialFilename(CString Filename);

//--获得上一幅图像的文件名
CString	GetPrevSerialFilename(CString Filename);

//--获得下一幅图像的文件名,不一定是序列图像
BOOL    FindNextFile(CString &Filename);

//--获得上一幅图像的文件名,不一定是序列图像
BOOL    FindPrevFile(CString &Filename);

//--获得当前所在的目录
CString GetCurrentDirectory();

//--从图像文件全名中获得所在的目录名
CString GetDir_Form_FullFileName(CString Filename);

//--从图像文件全名中获得所在的文件名
CString GetFileName_Form_FullFileName(CString FullFileName);

//--判断是否存在某个目录或文件
BOOL    IsExistFile(CString Filename);

//--将当前目录中的符合所需要求的文件名输出到一个CStringList类对象中
void	GetCurrrentDirFilename(CString Dir, CString Ext, CStringList &FileList);

//--获得序列文件中数字的个数的函数
int		Get_Numbers_InFileName		  ( CString FileName );

//--获得序列文件中数字的函数
long	Get_IDNo_InFileName			  ( CString FileName );

//--设置序列文件中数字的函数
//void	ResetCurrentFileNameFromNumber( CString & FileName , int FileNum );
//--获得指定目录下第一个指定扩展名文件
CString Get_FirstFilename_From_Dir( CString Dir, CString FileExt);
//------------------------------------------文件操作处理的函数声明结束

//---检测windows 版本的函数
int		CheckOSVersion( CString &m_stInfo, CString &m_stServVersion);

long	getHardDriveComputerID ();

//--获得当前应用程序的全名
CString GetAppExeFileFullName();



// 保存工程设置类
class CIni  
{
public:
	CString csLineEnd;

	void RemoveMultiLineItem(const int idx);
	int	 FindItem		(const int iSection, const char * cItem, CString &csVal);
	int	 FindMultiItem	(const int iSection, const char * cItem, CString &csVal);
	bool RemoveSection	(const char *cSection);
	bool IsSection		(const int	 iSection);
	int	 InsertSection	(const char *cSection);
	int	 FindSection	(const char *cSection);

	bool SetMultiValue(const char * cSection, const char * cItem, const char * cVal);
	bool GetMultiValue(const char * cSection, const char * cItem, CString &cVal);

	bool SetValue(const char *cSection, const char *cItem, const COLORREF	crVal);
	bool SetValue(const char *cSection, const char *cItem, const bool		bVal);
	bool SetValue(const char *cSection, const char *cItem, const char		*cVal);
	bool SetValue(const char *cSection, const char *cItem, const double		dbVal);
	bool SetValue(const char *cSection, const char *cItem, const float		fVal);
	bool SetValue(const char *cSection, const char *cItem, const long		lVal);
	bool SetValue(const char *cSection, const char *cItem, const int		iVal);
	bool SetValue(const char *cSection, const char *cItem, const CRect		rcVal);
	bool SetValue(const char *cSection, const char *cItem, const CPoint		ptVal);

	bool GetValue(const char *cSection, const char *cItem, COLORREF &crVal);
	bool GetValue(const char *cSection, const char *cItem, bool		&bVal);
	bool GetValue(const char *cSection, const char *cItem, CString	&cVal);
	bool GetValue(const char *cSection, const char *cItem, double	&dbVal);
	bool GetValue(const char *cSection, const char *cItem, float	&fVal);
	bool GetValue(const char *cSection, const char *cItem, long		&lVal);
	bool GetValue(const char *cSection, const char *cItem, int		&iVal);
	bool GetValue(const char *cSection, const char *cItem, CRect	&rcVal);
	bool GetValue(const char *cSection, const char *cItem, CPoint	&ptVal);

	void Clear();
	bool Write(const char * cFileName);
	bool Read (const char * cFileName);
	CIni();
	virtual ~CIni();
	CStringArray	csList;
};

// 精确获得算法处理时间的类(毫秒量级)
class LTimeCount
{
private:	
	double			UseTime;				// 算法处理时间(单位:秒)
	LARGE_INTEGER	Time, Frequency, old;	// 计数值
	
public:	
	void Start() // 计时开始
	{
		QueryPerformanceFrequency( &Frequency );
		QueryPerformanceCounter  ( &old );
		UseTime = 0.0;
	}
	void End() // 计时结束
	{
		QueryPerformanceCounter( &Time );
		UseTime = (double) ( Time.QuadPart - old.QuadPart) / (double)Frequency.QuadPart;
		//		TRACE("Use time = %20.10f(ms)\n", UseTime );
	}
	double GetUseTime() // 获得算法处理时间(单位:秒)
	{		
		return UseTime;
	}
};
//------------------------------------------精确获得算法处理时间的类定义结束

#endif