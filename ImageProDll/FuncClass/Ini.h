// Copyright 1997-98 (c) Iuri Apollonio
// given freely to www.codeguru.com


// Ini.h: interface for the CIni class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INI_H__4A8E7C07_902F_11D1_8B70_0000B43382FE__INCLUDED_)
#define AFX_INI_H__4A8E7C07_902F_11D1_8B70_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <afxcoll.h>

// *****************************************************************************
// Explanation of file structure:
// [A Section]
// A double Field = 0.23123
// A int Field = 132432
// A string Field = This is a string!
// A multicolumn string Field = This is a 1st line stringç|This is a 2nd line stringç|This is a 3rd line string
// [New Section]


class CIni  
{
public:
	void RemoveMultiLineItem(const int idx);
	CString csLineEnd;
	int FindItem(const int iSection, const char * cItem, CString &csVal);
	int FindMultiItem(const int iSection, const char * cItem, CString &csVal);
	bool RemoveSection(const char * cSection);
	bool IsSection(const int iSection);
	int InsertSection(const char * cSection);
	int FindSection(const char * cSection);

	bool SetMultiValue(const char * cSection, const char * cItem, const char * cVal);

	bool SetValue(const char * cSection, const char * cItem, const COLORREF crVal);
	bool SetValue(const char * cSection, const char * cItem, const bool bVal);
	bool SetValue(const char * cSection, const char * cItem, const char * cVal);
	bool SetValue(const char * cSection, const char * cItem, const double dbVal);
	bool SetValue(const char * cSection, const char * cItem, const float fVal);
	bool SetValue(const char * cSection, const char * cItem, const long lVal);
	bool SetValue(const char * cSection, const char * cItem, const int iVal);
	bool SetValue(const char * cSection, const char * cItem, const CRect rcVal);
	bool SetValue(const char * cSection, const char * cItem, const CPoint ptVal);

	bool GetMultiValue(const char * cSection, const char * cItem, CString &cVal);

	bool GetValue(const char * cSection, const char * cItem, COLORREF &crVal);
	bool GetValue(const char * cSection, const char * cItem, bool &bVal);
	bool GetValue(const char * cSection, const char * cItem, CString &cVal);
	bool GetValue(const char * cSection, const char * cItem, double &dbVal);
	bool GetValue(const char * cSection, const char * cItem, float &fVal);
	bool GetValue(const char * cSection, const char * cItem, long &lVal);
	bool GetValue(const char * cSection, const char * cItem, int &iVal);
	bool GetValue(const char * cSection, const char * cItem, CRect &rcVal);
	bool GetValue(const char * cSection, const char * cItem, CPoint &ptVal);

	void Clear();
	bool Write(const char * cFileName);
	bool Read(const char * cFileName);
	CIni();
	virtual ~CIni();
	CStringArray	csList;

};

#endif // !defined(AFX_INI_H__4A8E7C07_902F_11D1_8B70_0000B43382FE__INCLUDED_)
