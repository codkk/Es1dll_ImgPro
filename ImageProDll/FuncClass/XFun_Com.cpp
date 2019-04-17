
#include "stdafx.h"

#include <io.h>
#include <direct.h>
#include "XFun_Com.h"

//***********************************************************
//函数类别： 序列图像文件名操作处理
//函数名称： GetNextSerialFilename
//函数用途： 获得下一幅图像的文件名
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString	GetNextSerialFilename(CString Filename)
{
	CString ReturnString = Filename;
	int StartPos = Filename.GetLength() - 4;

	if( Filename.GetAt(StartPos) != '.' )
	{
// 		AfxMessageBox("wrong in filename!");
 		return "haha";
	}
	BOOL OK = false;
	BYTE tChar;

    while( !OK )
	{
		StartPos--;
		if( StartPos <= 0 ) OK = true;
		tChar = Filename.GetAt(StartPos) - '0';

		if( tChar >= 0 && tChar < 9 )
		{
			tChar = tChar + 1 + '0';
			ReturnString.SetAt(StartPos, tChar);
			OK = true;
		}
		else
		{
			if( tChar == 9 )
				ReturnString.SetAt(StartPos, '0');
			else
				OK = true;
		}
	}	
	return ReturnString;
}
//***********************************************************
//函数类别： 序列图像文件名操作处理
//函数名称： GetPrevSerialFilename
//函数用途： 获得上一幅图像的文件名
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString	GetPrevSerialFilename(CString Filename)
{
	CString ReturnString = Filename;
	int StartPos = Filename.GetLength() - 4;

	if( Filename.GetAt(StartPos) != '.' )
	{
		AfxMessageBox("wrong in filename!");
		return "haha";
	}
	BOOL OK = false;
	BYTE tChar;

    while( !OK )
	{
		StartPos--;
		if( StartPos <= 0) OK = true;
		tChar = Filename.GetAt(StartPos) - '0';
		if( tChar > 0 && tChar <= 9)
		{
			tChar = tChar - 1 + '0';
			ReturnString.SetAt(StartPos, tChar);
			OK = true;
		}
		else
		{
			if( tChar == 0 )
				ReturnString.SetAt(StartPos,'9');
			else
				OK = true;
		}
	}	
	return ReturnString;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： FindNextFile
//函数用途： 获得下一幅图像的文件名,不一定是序列图像
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
BOOL FindNextFile(CString &Filename)
{
	int count1 = Filename.ReverseFind('\\');
	CString Path = Filename.Left(count1+1);
	CString FileExtname = Path + "*" + Filename.Right(4);
	CString LastFilename ;

	struct _finddata_t c_file;    
	long hFile;
	BOOL OK = false;
	 
	if( (hFile = _findfirst(FileExtname, &c_file )) != -1L )
	{
		LastFilename = Path + c_file.name;	
		while( _findnext( hFile, &c_file ) == 0 )            
		{
			if( LastFilename == Filename ) OK = true;
			if( OK )
			{
				Filename = Path + c_file.name;
				_findclose( hFile );
				return true;
			}
			LastFilename = Path + c_file.name;			
		}		 
	}
	_findclose( hFile );
	return false;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： FindPrevFile
//函数用途： 获得上一幅图像的文件名,不一定是序列图像
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
BOOL FindPrevFile(CString &Filename)
{
	int count1 = Filename.ReverseFind('\\');
	CString Path = Filename.Left(count1+1);
	CString FileExtname = Path + "*" + Filename.Right(4);
	CString LastFilename ;

	struct _finddata_t c_file;    
	long hFile;
	BOOL OK = false;
	 
	if( (hFile = _findfirst(FileExtname, &c_file )) != -1L )
	{
		LastFilename = Path + c_file.name;	
		while( _findnext( hFile, &c_file ) == 0 )            
		{
			CString Nowfile = Path + c_file.name;
			if( Nowfile == Filename ) OK = true;
			if( OK )
			{
				Filename = LastFilename;
				_findclose( hFile );
				return true;
			}
			LastFilename = Nowfile;			
		}
	}
	_findclose( hFile );
	return false;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： GetCurrentDirectory
//函数用途： 获得当前所在的目录
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString GetCurrentDirectory()
{
	//int	drive = _getdrive();
	char path[256];
	//_getdcwd(drive,path,256);
	GetCurrentDirectory(256, path);
	return path;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： IsExistFile
//函数用途： 判断是否存在某个目录或文件
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
BOOL IsExistFile(CString Filename)
{
	struct _finddata_t c_file;    
	long hFile;
	 
	if( (hFile = _findfirst(Filename, &c_file )) != -1L )
		return true;
	else
		return false;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： GetFileName_Form_FullFileName
//函数用途： 从图像文件全名中获得所在的文件名
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString GetFileName_Form_FullFileName(CString FullFileName)
{
	int	count = FullFileName.ReverseFind('\\');
	return FullFileName.Right(FullFileName.GetLength()- count- 1);
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： GetDir_Form_FullFileName
//函数用途： 从图像文件全名中获得所在的目录名
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString GetDir_Form_FullFileName(CString Filename)
{
	int count = Filename.ReverseFind('\\');
	return Filename.Left( count );
}
//***********************************************************
//函数类别： 序列图像文件名操作处理
//函数名称： GetCurrrentDirFilename
//函数用途： 将当前目录中的符合所需要求的文件名输出到
//			 一个CStringList类对象中
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
void GetCurrrentDirFilename(CString Dir, CString Ext, CStringList &FileList)
{
	CString FileExtname = Dir + "\\*." + Ext;
	
	struct _finddata_t c_file;    
	long hFile;

	FileList.RemoveAll();
	if( (hFile = _findfirst(FileExtname, &c_file )) != -1L )
	{
		CString temp( c_file.name );
		temp. MakeLower();
		FileList.AddTail(temp);
		while( _findnext( hFile, &c_file ) == 0 )            
		{
			CString temp( c_file.name );
			temp. MakeLower();
			FileList.AddTail(temp);
		}		 
	}
	_findclose( hFile );
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： Get_Numbers_InFileName
//函数用途： 获得序列文件中数字的个数的函数
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
int Get_Numbers_InFileName(CString FileName)
{
	int nRet     = 0;
	int count    = FileName.GetLength();
	int StartPos = count - 4;
	if( FileName.GetAt( StartPos )!='.' )
	{
		AfxMessageBox("文件名格式有误\n非*.*形式!");
		return -1;
	}

	BOOL OK = false;
	BYTE tChar;
    while( !OK )
	{
		StartPos--;
		if( StartPos <= 0 ) OK = true;
		tChar = FileName.GetAt(StartPos) - '0';
		if( tChar >= 0 && tChar <= 9 )
		{
			nRet ++;
		}
		else
		{
			OK = true;
		}
	}	
	return nRet;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： Get_IDNo_InFileName
//函数用途： 获得序列文件中数字的函数
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
long Get_IDNo_InFileName(CString FileName)
{
	long nRet = 0;
	int count = FileName.GetLength();
	int StartPos = count-4;
	if(FileName.GetAt(StartPos)!='.')
	{
		AfxMessageBox("文件名格式有误\n非*.*形式!");
		return -1;
	}

	BOOL OK = false;
	BYTE tChar;
	long nPow = 1;
	int  LimCount = 0;
    while( !OK )
	{
		StartPos--;
		if( StartPos <= 0 ) OK = true;
		tChar = FileName.GetAt(StartPos) - '0';
		if( tChar >= 0 && tChar <= 9 && LimCount < 4 )
		{
			nRet = nRet + tChar * nPow;
			nPow *= 10;
			LimCount++;
		}
		else
		{
			OK = true;
		}
	}	
	return nRet;
}
//***********************************************************
//函数类别： 文件名、目录名操作处理
//函数名称： ResetCurrentFileNameFromNumber
//函数用途： 设置序列文件中数字的函数
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
/*void ResetCurrentFileNameFromNumber( CString & FileName , int FileNum )
{
	int count = FileName.GetLength();
	int StartPos = count - 4, LimCount = 0;;
	if(FileName.GetAt(StartPos)!='.')
	{
		AfxMessageBox("文件名格式有误\n非*.*形式!");
		return;
	}

	BOOL OK = false;
	BYTE tChar, n;
    while( !OK )
	{
		StartPos--;
		if( StartPos <= 0 ) OK = true;
		tChar = FileName.GetAt(StartPos) - '0';
		if( tChar >= 0 && tChar<= 9 && LimCount < 4 )
		{
			n = FileNum % 10;
			FileNum /= 10;
			FileName.SetAt(StartPos, n + '0');
			LimCount++;
		}
		else
		{
			OK = true;
		}
	}	
}*/
// 获得指定目录下第一个指定扩展名文件
CString Get_FirstFilename_From_Dir( CString Dir, CString FileExt)
{
	CString RV = "", Filter = Dir + "\\*." + FileExt;
	struct _finddata_t c_file;
	long hFile;
	 
	if( (hFile = _findfirst(Filter, &c_file )) != -1L )
	{
		RV = Dir + "\\" + c_file.name;
	}
	return RV;
}
//***********************************************************
//函数类别： 系统操作
//函数名称： CheckOSVersion
//函数用途： 检测windows的版本
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
int CheckOSVersion( CString &m_stInfo, CString &m_stServVersion)
{
	int RV = 1;
	TCHAR			data [64];
	DWORD			dataSize;
	HKEY			hKey;
	LONG			result;
	OSVERSIONINFO	versionInfo;

	struct WinVerDword
	{
		union
		{
			DWORD dwVer;
			struct VER
			{
				WORD ver3;
				BYTE ver2;
				BYTE ver1;
			} ver;
		};
	} MY_Var;
	
	// set the size of OSVERSIONINFO, before calling the function
	versionInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);

	// Get the version information
	if ( !::GetVersionEx (&versionInfo) )
	{
		m_stInfo = (_T ("Not able to get OS information"));
		return RV;
	}

	// If NT ,then check if its server or workstation.
	if ( versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
	{
		// There is no direct way of telling from ODVERSIONINFO thats is it 
		// workstation or server version. There we need to check in the registry.
		m_stInfo =  (_T ("Windows NT"));

		dataSize = sizeof (data);
		
		result = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE,
								 "System\\CurrentControlSet\\Control\\ProductOptions",
								 0, KEY_QUERY_VALUE, &hKey);

		// If there is error in opening the registry key, return
		if (result != ERROR_SUCCESS) 
		{
			SetLastError (result);
			return RV;
		}

		result = ::RegQueryValueEx (hKey, _T("ProductType"), NULL, NULL,
									(LPBYTE) data, &dataSize);

		RegCloseKey (hKey);// Make sure to close the reg key

		if (result != ERROR_SUCCESS)
		{
			SetLastError (result);
			return RV;
		}

		// Check what string has been returned
		if (lstrcmpi (data, "WinNT") == 0) 
		{
			m_stInfo = _T ("Windows NT Workstation");
		}
		else if (lstrcmpi (data, "ServerNT") == 0) 
		{
			m_stInfo = _T ("Windows NT Server");
		}
		else 
		{
			m_stInfo = _T ("Windows NT Server - Domain Controller");
		}

		// Check the version number
		if (versionInfo.dwMajorVersion == 3 || versionInfo.dwMinorVersion == 51) 
		{
			m_stServVersion = _T ("3.51");
		}
		else if (versionInfo.dwMajorVersion == 4 )
		{
			m_stServVersion = _T ("4.0");
		}
		else if (versionInfo.dwMajorVersion == 5 )
		{
			m_stInfo = _T ("Windows 2000");
			m_stServVersion = _T ("5.0");
		}		
	}
	else if (versionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) 
	{
		m_stInfo = _T ("Windows 95");

		if ( (versionInfo.dwMajorVersion > 4) || 
			 ((versionInfo.dwMajorVersion == 4) && (versionInfo.dwMinorVersion > 0)) )
		{
			m_stInfo = _T ("Windows 98");
			RV = 0;
		}	
	}
	else
	{
		m_stInfo = _T ("Windows 3.1");
	}

	// Get service pack information.
	MY_Var.dwVer = versionInfo.dwBuildNumber;
	m_stServVersion.Format("%d.%d.%d %s", MY_Var.ver.ver1, MY_Var.ver.ver2, MY_Var.ver.ver3, versionInfo.szCSDVersion );	
	return RV;
}
//***********************************************************
//函数类别： 系统操作
//函数名称： GetAppExeFileFullName
//函数用途： 获得当前应用程序的全名
//原始作者： 陆  宏  伟
//原始日期： 12/4 /1999 
//***********************************************************
CString GetAppExeFileFullName()
{
	char ModuleFileName[_MAX_PATH];
	GetModuleFileName( NULL, ModuleFileName, _MAX_PATH);
	return CString( ModuleFileName );
}

void L_TraceRect(CString VarName, CRect rect)
{
	TRACE("%s LeftUp point is (%d ,%d) , RightBottom point is (%d ,%d) w,h = (%d ,%d)\n",\
		   VarName, rect.left,  rect.top,  rect.right, rect.bottom, rect.Width(), rect.Height());
}

void Wormhole_CycleIndex( LPRGBQUAD pe )
{
	int                 reg[15];
    int                 k;
	int					k15;
       
    for(k=0;k<15;k++)
    {
        reg[k]=pe[k+30].rgbRed;
    }
    for(k=45;k<255;k++)
    {                                                            
        pe[k-15].rgbRed=pe[k].rgbRed;
    }
    for(k=0;k<15;k++)
    {
        pe[k+240].rgbRed=reg[k];
    }
    for(k=0;k<15;k++)
    {
        reg[k]=pe[k+30].rgbGreen;
    }
    for(k=45;k<255;k++)
    {
        pe[k-15].rgbGreen=pe[k].rgbGreen;
    }
    for(k=0;k<15;k++)
    {
        pe[k+240].rgbGreen=reg[k];
    }
    for(k=0;k<15;k++)
    {
        reg[k]=pe[k+30].rgbBlue;
    }
    for(k=45;k<255;k++)
    {
        pe[k-15].rgbBlue=pe[k].rgbBlue;
    }
    for(k=0;k<15;k++)
    {
        pe[k+240].rgbBlue=reg[k];
    }

    for(k=2;k<17;k++)
    {
		k15 = k * 15;
        reg[k-2]=pe[k15+14].rgbRed;
        pe[k15+14].rgbRed	= pe[k15+13].rgbRed;
        pe[k15+13].rgbRed	= pe[k15+12].rgbRed;
        pe[k15+12].rgbRed	= pe[k15+11].rgbRed;
        pe[k15+11].rgbRed	= pe[k15+10].rgbRed;
        pe[k15+10].rgbRed	= pe[k15+9].rgbRed;
        pe[k15+9].rgbRed	= pe[k15+8].rgbRed;
        pe[k15+8].rgbRed	= pe[k15+7].rgbRed;
        pe[k15+7].rgbRed	= pe[k15+6].rgbRed;
        pe[k15+6].rgbRed	= pe[k15+5].rgbRed;
        pe[k15+5].rgbRed	= pe[k15+4].rgbRed;
        pe[k15+4].rgbRed	= pe[k15+3].rgbRed;
        pe[k15+3].rgbRed	= pe[k15+2].rgbRed;
        pe[k15+2].rgbRed	= pe[k15+1].rgbRed;
        pe[k15+1].rgbRed	= pe[k15].rgbRed;
        pe[k15].rgbRed		= reg[k-2];

        reg[k-2]=pe[k15+14].rgbGreen;
        pe[k15+14].rgbGreen	= pe[k15+13].rgbGreen;
        pe[k15+13].rgbGreen = pe[k15+12].rgbGreen;
        pe[k15+12].rgbGreen = pe[k15+11].rgbGreen;
        pe[k15+11].rgbGreen = pe[k15+10].rgbGreen;
        pe[k15+10].rgbGreen = pe[k15+9].rgbGreen;
        pe[k15+9].rgbGreen	= pe[k15+8].rgbGreen;
        pe[k15+8].rgbGreen	= pe[k15+7].rgbGreen;
        pe[k15+7].rgbGreen	= pe[k15+6].rgbGreen;
        pe[k15+6].rgbGreen	= pe[k15+5].rgbGreen;
        pe[k15+5].rgbGreen	= pe[k15+4].rgbGreen;
        pe[k15+4].rgbGreen	= pe[k15+3].rgbGreen;
        pe[k15+3].rgbGreen	= pe[k15+2].rgbGreen;
        pe[k15+2].rgbGreen	= pe[k15+1].rgbGreen;
        pe[k15+1].rgbGreen	= pe[k15].rgbGreen;
        pe[k15].rgbGreen=reg[ k-2];

        reg[k-2]=pe[k15+14].rgbBlue;
        pe[k15+14].rgbBlue	= pe[k15+13].rgbBlue;
        pe[k15+13].rgbBlue	= pe[k15+12].rgbBlue;
        pe[k15+12].rgbBlue	= pe[k15+11].rgbBlue;
        pe[k15+11].rgbBlue	= pe[k15+10].rgbBlue;
        pe[k15+10].rgbBlue	= pe[k15+9].rgbBlue;
        pe[k15+9].rgbBlue	= pe[k15+8].rgbBlue;
        pe[k15+8].rgbBlue	= pe[k15+7].rgbBlue;
        pe[k15+7].rgbBlue	= pe[k15+6].rgbBlue;
        pe[k15+6].rgbBlue	= pe[k15+5].rgbBlue;
        pe[k15+5].rgbBlue	= pe[k15+4].rgbBlue;
        pe[k15+4].rgbBlue	= pe[k15+3].rgbBlue;
        pe[k15+3].rgbBlue	= pe[k15+2].rgbBlue;
        pe[k15+2].rgbBlue	= pe[k15+1].rgbBlue;
        pe[k15+1].rgbBlue	= pe[k15].rgbBlue;
        pe[k15].rgbBlue=reg[k -2];
    }
}
//-------------- 保存工程设置类
#include <fstream>
#include <iostream>
using namespace std; 
CIni::CIni()
{
	csLineEnd = "\r\n";
}

CIni::~CIni()
{
	Clear();
}

bool CIni::Read(const char * cFileName)
{
	Clear();
	char buf[1024];
	ifstream ifs(cFileName);
	while (ifs.good())
	{
		ifs.getline(buf, 1023);
		CString cs(buf);
		csList.Add(cs);
	}
	return true;
}

bool CIni::Write(const char * cFileName)
{
	ASSERT(cFileName);
	ofstream ofs(cFileName);
	int t, max = csList.GetSize();
	for (t = 0; t < max; t++)
	{
		ofs << (LPCTSTR)csList.GetAt(t) << "\n";
	}
	return true;
}

// **********************************************************************************

void CIni::Clear()
{
	csList.RemoveAll();
}

// **********************************************************************************

int CIni::FindSection(const char * cSection)
{
	int t, max = csList.GetSize();
	CString csSection;
	csSection.Format("[%s]", cSection);

	for (t = 0; t < max; t++)
		if (csList.GetAt(t) == csSection) return t;

	return -1;
}

int CIni::InsertSection(const char * cSection)
{
	ASSERT(cSection);
	if (!cSection) return -1;

	int idx = FindSection(cSection);
	if (idx < 0)
	{
		CString csSection;
		csSection.Format("[%s]", cSection);
		idx = csList.Add(csSection);
	}
	return idx;
}

int CIni::FindItem(const int iSection, const char * cItem, CString &csVal)
{
	ASSERT(iSection >= 0);
	ASSERT(cItem);

	int max = csList.GetSize(), t;
	CString csItem(cItem), csLook;
	csItem += " = ";
	int iLen = csItem.GetLength();

	for (t = iSection; t < max; t++)
	{
		if (!IsSection(t))
		{
			csLook = csList.GetAt(t);
			if (csLook.GetLength() >= iLen)
			{
				if (csLook.Left(iLen) == csItem) 
				{
					if (csLook.GetLength() == iLen) csVal = "";
					else csVal = csLook.Right(csLook.GetLength() - iLen);
					return t;
				}
			}
		}
		else return -1;
	}
	return -1;
}

int CIni::FindMultiItem(const int iSection, const char * cItem, CString &csVal)
{
	ASSERT(iSection >= 0);
	ASSERT(cItem);

	int max = csList.GetSize(), t, i;
	CString csItem(cItem), csLook;
	csItem += " = \"";
	int iLen = csItem.GetLength();

	for (t = iSection; t < max; t++)
	{
		if (!IsSection(t))
		{
			csLook = csList.GetAt(t);
			if (csLook == csItem)
			{
				csVal = "";
				for (i = t + 1; i < max; i++)
				{
					csLook = csList.GetAt(i);
					if (csLook == '\"' || IsSection(i)) 
					{
						i = max; 
					}
					else 
					{
						if (csVal != "") csVal += csLineEnd;
						csVal += csLook;
					}
				}
				return t;
			}
		}
		else return -1;
	}
	return -1;
}

bool CIni::IsSection(const int iSection)
{
	ASSERT(iSection >= 0 && iSection < csList.GetSize());
	if (iSection >= 0 && iSection < csList.GetSize())
	{
		CString csItem = csList.GetAt(iSection);
		if (csItem.GetLength() > 2 && csItem.Left(1) == '[' && csItem.Right(1) == ']') return true;
	}
	return false;
}

bool CIni::RemoveSection(const char * cSection)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		for (;;)
		{
			csList.RemoveAt(idx);
			if (idx >= csList.GetSize()) return true;
			if (IsSection(idx)) return true;
		}
	}
	return true;
}

void CIni::RemoveMultiLineItem(const int idx)
{
	int max = csList.GetSize(), t;
	CString csLook;

	for (t = idx; t < max; t++)
	{
		if (!IsSection(t))
		{
			csLook = csList.GetAt(t);
			if (csLook == '\"')
			{
				csList.RemoveAt(t);
				return;
			}
			csList.RemoveAt(t);
		}
		else return;
	}
}

// **********************************************************************************

bool CIni::SetValue(const char * cSection, const char * cItem, const bool bVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %s", cItem, bVal ? "true" : "false");
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const COLORREF crVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %d", cItem, (DWORD) crVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const char * cVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %s", cItem, cVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const double dbVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %20.20f", cItem, dbVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const float fVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %f", cItem, fVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const long lVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %d", cItem, lVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const int iVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %d", cItem, iVal);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetMultiValue(const char * cSection, const char * cItem, const char * cVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = %s", cItem, cVal);
		char * c = csVal.LockBuffer();

		int i = csVal.Find('\r');
		while (i >= 0)
		{
			c[i] = '}';
			i = csVal.Find('\r');
		}
		i = csVal.Find('\n');
		while (i >= 0)
		{
			c[i] = '|';
			i = csVal.Find('\n');
		}

		csVal.UnlockBuffer();
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const CRect rcVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = RECT(%d,%d,%d,%d)", cItem, rcVal.left, rcVal.top, rcVal.right, rcVal.bottom);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}

bool CIni::SetValue(const char * cSection, const char * cItem, const CPoint ptVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		int iIdx = FindItem(idx+1, cItem, csVal);
		csVal.Format("%s = POINT(%d,%d)", cItem, ptVal.x, ptVal.y);
		if (iIdx >= 0) csList.SetAt(iIdx, csVal);
		else csList.InsertAt(idx+1, csVal);
		return true;
	}
	return false;
}


// **********************************************************************************

bool CIni::GetValue(const char * cSection, const char * cItem, COLORREF &crVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			crVal = (COLORREF) (DWORD) atol(csVal);
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, bool &bVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			if (csVal.Find("true") >= 0) bVal = true; else bVal = false;
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, CString &cVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		if (FindItem(idx+1, cItem, cVal) > 0)
			return true;
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, double &dbVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			dbVal =  atof(csVal);
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, float &fVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			fVal = (float) atof(csVal);
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, long &lVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			lVal = (long) atol(csVal);
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, int &iVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			iVal = (int) atoi(csVal);
			return true;
		}
	}
	return false;
}

bool CIni::GetMultiValue(const char * cSection, const char * cItem, CString &cVal)
{
	int idx = FindSection(cSection);
	if (idx >= 0)
	{
		if (FindItem(idx+1, cItem, cVal) > 0)
		{
			char * ch = cVal.LockBuffer();
			int i = cVal.Find('}');
			while (i >= 0)
			{
				ch[i] = '\r';
				i = cVal.Find('}');
			}
			i = cVal.Find('|');
			while (i >= 0)
			{
				ch[i] = '\n';
				i = cVal.Find('|');
			}
			cVal.UnlockBuffer();
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, CRect &rcVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			char * pt = csVal.LockBuffer();
			int pf, t = 0, l = 0, r = 0, b = 0;
			pf = sscanf_s(csVal, "RECT(%d,%d,%d,%d)", &l, &t, &r, &b);
			ASSERT(pf == 4);
			csVal.UnlockBuffer();
			rcVal.SetRect(l, t, r, b);
			return true;
		}
	}
	return false;
}

bool CIni::GetValue(const char * cSection, const char * cItem, CPoint &ptVal)
{
	int idx = InsertSection(cSection);
	if (idx >= 0)
	{
		CString csVal;
		if (FindItem(idx+1, cItem, csVal) > 0)
		{
			char * pt = csVal.LockBuffer();
			int pf, x = 0, y = 0;
			pf = sscanf_s(csVal, "POINT(%d,%d)", &x, &y);
			ASSERT(pf == 2);
			csVal.UnlockBuffer();
			ptVal.x = x;
			ptVal.y = y;
			return true;
		}
	}
	return false;
}
