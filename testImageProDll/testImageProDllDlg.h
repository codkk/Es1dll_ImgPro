
// testImageProDllDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CtestImageProDllDlg 对话框
class CtestImageProDllDlg : public CDialogEx
{
// 构造
public:
	CtestImageProDllDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTIMAGEPRODLL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	ImageProDll m_mydll;
	//ImageProDll m_mydll2;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CStatic m_show;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedButton10();
	// 是否显示检测区域
	BOOL m_bShowArea;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton12();
};
