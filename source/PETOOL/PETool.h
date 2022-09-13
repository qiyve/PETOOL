#pragma once
#include <Windows.h>
#include <iostream>
#include <commctrl.h>	
#pragma comment(lib,"comctl32.lib")		
#pragma warning(disable:4996)

#define FailTOOpenFile		-1 //打开文件失败
#define FaliToMalloc		 -2//申请缓冲区失败
#define NoPeFile			  -3//不是有效的PE文件

class PETool
{
public:
	PETool();
	~PETool();
public:
	//打开通用对话框、
	//返回所选的文件绝对路径
	static  std::string OpenCommonDialog(HWND hDlg);
	//打开文件
	static  int OpenFile(HWND hDlg,const char* FileName);
	//打印信息
	template<class T>
	static  void Printf_EditInfo(HWND hDlg,UINT Msg, T Context);
	//列出列表的头
	static  void InitClass_Section(HWND hDlg);
	//打印节表信息
	static  void Printf_SectionInfo(HWND hDlg);
	//按照指定方式进行内存对齐
	static DWORD MemoryAlignment(DWORD Data, DWORD Mode);
	//打印出15个目录表
	static void Printf_Directory(HWND hDlg);
private:
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders ;
	PIMAGE_FILE_HEADER pFileHeader ;
	PIMAGE_OPTIONAL_HEADER pOptionalHeader ;
	PIMAGE_SECTION_HEADER pSectionHeader ;
	LPVOID pTempFileBuffer;
};

