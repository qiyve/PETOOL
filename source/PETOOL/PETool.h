#pragma once
#include <Windows.h>
#include <iostream>
#include <commctrl.h>	
#pragma comment(lib,"comctl32.lib")		
#pragma warning(disable:4996)

#define FailTOOpenFile		-1 //���ļ�ʧ��
#define FaliToMalloc		 -2//���뻺����ʧ��
#define NoPeFile			  -3//������Ч��PE�ļ�

class PETool
{
public:
	PETool();
	~PETool();
public:
	//��ͨ�öԻ���
	//������ѡ���ļ�����·��
	static  std::string OpenCommonDialog(HWND hDlg);
	//���ļ�
	static  int OpenFile(HWND hDlg,const char* FileName);
	//��ӡ��Ϣ
	template<class T>
	static  void Printf_EditInfo(HWND hDlg,UINT Msg, T Context);
	//�г��б��ͷ
	static  void InitClass_Section(HWND hDlg);
	//��ӡ�ڱ���Ϣ
	static  void Printf_SectionInfo(HWND hDlg);
	//����ָ����ʽ�����ڴ����
	static DWORD MemoryAlignment(DWORD Data, DWORD Mode);
	//��ӡ��15��Ŀ¼��
	static void Printf_Directory(HWND hDlg);
private:
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders ;
	PIMAGE_FILE_HEADER pFileHeader ;
	PIMAGE_OPTIONAL_HEADER pOptionalHeader ;
	PIMAGE_SECTION_HEADER pSectionHeader ;
	LPVOID pTempFileBuffer;
};

