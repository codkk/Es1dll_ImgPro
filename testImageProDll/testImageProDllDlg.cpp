
// testImageProDllDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "testImageProDll.h"
#include "testImageProDllDlg.h"
#include "afxdialogex.h"
#include "opencv2\opencv.hpp"
using namespace cv;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestImageProDllDlg 对话框



CtestImageProDllDlg::CtestImageProDllDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TESTIMAGEPRODLL_DIALOG, pParent)
	, m_bShowArea(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CtestImageProDllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHOW, m_show);
	DDX_Check(pDX, IDC_CHECK1, m_bShowArea);
}

BEGIN_MESSAGE_MAP(CtestImageProDllDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CtestImageProDllDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CtestImageProDllDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CtestImageProDllDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CtestImageProDllDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CtestImageProDllDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CtestImageProDllDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CtestImageProDllDlg::OnBnClickedButton7)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON8, &CtestImageProDllDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CtestImageProDllDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CtestImageProDllDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_CHECK1, &CtestImageProDllDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON11, &CtestImageProDllDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CtestImageProDllDlg::OnBnClickedButton12)
END_MESSAGE_MAP()


// CtestImageProDllDlg 消息处理程序

BOOL CtestImageProDllDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	CRect rect;
	m_show.GetClientRect(&rect);
	m_mydll.SetHalconDispWnd(m_show.GetSafeHwnd(), rect);

	SetTimer(1, 500, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CtestImageProDllDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CtestImageProDllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CtestImageProDllDlg::OnBnClickedButton1()
{
	double spx1, spy1;
	double spx2, spy2;

	//平台移动到原点或者某一点

	//第一个点
	m_mydll.CheckCalibrate(spx1, spy1);

	//平台移动 delX,delY;

	//第二个点
	m_mydll.CheckCalibrate(spx2, spy2);

	//求两个点的差值，这2个差值与平台真正的移动量delX, delY进行比较。
	double delta_x, delta_y;
	delta_x = spx2 - spx1;
	delta_y = spy2 - spy1;

	CString str;
	str.Format(_T("x,y的差值：%lf, %lf"), delta_x, delta_y);
	AfxMessageBox(str);
}


void CtestImageProDllDlg::OnBnClickedButton2()
{
	m_mydll.Calibrate();
	//for (int i = 0; i < 9; i++)
	//{
	//	if (!m_mydll.Calibrate(i, 0.0, 0.0))
	//		break;
	//}
}

//创建模板
void CtestImageProDllDlg::OnBnClickedButton3()
{
	m_mydll.CreateModel();
	// TODO: 在此添加控件通知处理程序代码
	//Mat img;
	//img = imread("test.bmp",CV_LOAD_IMAGE_GRAYSCALE);
	//imshow("test", img);
	////m_mydll.HalconCreateModel(img.data, img.cols, img.rows);
	//m_mydll.HalconCreateModel("test.bmp");
	//cvReleaseMat(&img);
}


void CtestImageProDllDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_mydll.SetModelPathName("d:\\model2.shm");
	m_mydll.SetFullScreen(true);
}


void CtestImageProDllDlg::OnBnClickedButton5()
{
	//测试获取显示一张图像
	//m_mydll.GrabOneImage();
	long t1 = GetTickCount();
	m_mydll.FindModel();
	long t2 = GetTickCount();
	CString str;
	str.Format(_T("time: %d ms"), t2 - t1);
	AfxMessageBox(str);
}


void CtestImageProDllDlg::OnBnClickedButton6()
{
	//
	//m_mydll.LoadCalibIni();
	if (m_mydll.SaveImage("saveimg.bmp"))
	{
		AfxMessageBox(_T("ok"));
	}
	else
	{
		AfxMessageBox(_T("ng"));
	}
}


void CtestImageProDllDlg::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_mydll.SetContinueShow(false);
}


void CtestImageProDllDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//if (1 == nIDEvent)
	//{
	//	m_mydll.GrabOneImage();
	//}
	CDialogEx::OnTimer(nIDEvent);
}


void CtestImageProDllDlg::OnBnClickedButton8()
{
	long t1 = GetTickCount();
	m_mydll.GrabOneImage();
	long t2 = GetTickCount();
	CString str;
	str.Format(_T("time: %d ms"), t2 - t1);
	AfxMessageBox(str);
}


void CtestImageProDllDlg::OnBnClickedButton9()
{
	m_mydll.DrawDetectRoi();
}


void CtestImageProDllDlg::OnBnClickedButton10()
{
	m_mydll.DrawDetectCodebarRoi();
}


void CtestImageProDllDlg::OnBnClickedCheck1()
{
	UpdateData();
	m_mydll.ShowDetectArea(m_bShowArea);
}


void CtestImageProDllDlg::OnBnClickedButton11()
{
	CString str;
	m_mydll.FindBarCode(str, false);
	//AfxMessageBox(str);
}


void CtestImageProDllDlg::OnBnClickedButton12()
{
	// TODO: 在此添加控件通知处理程序代码
	m_mydll.LoadImageFromFile();
	m_mydll.SetManuMode(true);
}
