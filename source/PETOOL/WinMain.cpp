//#include<TlHelp32.h>
#include<tchar.h>
#include "resource.h"
#include "EnumProcess.h"
#include "PETool.h"

std::string FilePath;
std::string container_path;
std::string content_path;

int CreateProc(char*, PROCESS_INFORMATION*);//�������ܽ���
int UnmapView(PROCESS_INFORMATION);//ж�ؿ��ܽ����ڴ�ӳ��
int Injection(PROCESS_INFORMATION);//ʵ��ע��
DWORD GetImageSize(char*);//��ȡSizeOfImage
DWORD GetEntryPoint();//��ȡOEP
DWORD GetImageBase(PROCESS_INFORMATION pi);//��ȡ��ַ
DWORD GetImageBase();//��ȡ��ַ

CONTEXT context;//�����߳������Ľṹ
HANDLE hfile;//Ҫע����ļ��ľ��
char* pBuffer;//���ļ������ڴ��ָ��

//�����ڵĻص�����
BOOL CALLBACK DialogFunc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK DialogFunc_Pe(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK DialogFunc_Section(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Ŀ¼��Ļص�����
BOOL CALLBACK DialogFunc_Directory(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// shell
BOOL CALLBACK DialogFunc_Shell(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

HINSTANCE hPeInstance;

EnumProcess Process;

int WINAPI WinMain(
	_In_  HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
)
{
	hPeInstance = hInstance;
	DialogBoxA(hInstance, (LPCSTR)IDD_DIALOG_Main, NULL, DialogFunc);
}

BOOL CALLBACK DialogFunc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		Process.InitListClass(hDlg);//��ʼ���˵���
		Process.Enum_Process(hDlg);
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_NOTIFY:
	{
		NMHDR* hParam = (NMHDR*)lParam;
		if (wParam == IDC_LIST_Process && hParam->code == NM_CLICK)
		{
			Process.Enum_Module(hDlg);
		}
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDC_BUTTON_Exit:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}

		case IDC_BUTTON_Updata:
		{
			Process.Enum_Process(hDlg);
			return TRUE;
		}
		case IDC_BUTTON_PE:
		{
			DialogBox(hPeInstance, (LPCSTR)IDD_DIALOG_Pe, NULL, DialogFunc_Pe);
			return TRUE;
		}
		case IDC_BUTTON_Project:
		{
			DialogBox(hPeInstance, (LPCSTR)IDD_DIALOG_SHELL, NULL, DialogFunc_Shell);
			return TRUE;
		}
		case IDC_BUTTON_About:
		{
			MessageBox(NULL, _T("PETOOL 1.0"), _T("����"), MB_OK);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

//PE���ڵĻص�����
BOOL CALLBACK DialogFunc_Pe(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		FilePath = PETool::OpenCommonDialog(hDlg);
		PETool::OpenFile(hDlg, FilePath.c_str());
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case IDC_BUTTON_Pe_Close:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}
		case IDC_BUTTON_Pe_Section:
		{
			DialogBox(hPeInstance, (LPSTR)IDD_DIALOG_Section, NULL, DialogFunc_Section);
			return TRUE;
		}
		case IDC_BUTTON_PE_Directory:
		{
			DialogBox(hPeInstance, (LPSTR)IDD_DIALOG_Directory, NULL, DialogFunc_Directory);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

//���δ��ڵĻص�����
BOOL CALLBACK DialogFunc_Section(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_INITDIALOG:
	{
		PETool::InitClass_Section(hDlg);
		PETool::Printf_SectionInfo(hDlg);
		return TRUE;
	}
	}
	return FALSE;
}

//Ŀ¼��Ļص�����
BOOL CALLBACK DialogFunc_Directory(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		PETool::Printf_Directory(hDlg);
		return TRUE;
	}
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}
	case WM_COMMAND:
	{
		switch (wParam)
		{
		case  IDC_BUTTON_Directory_Close:
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}


BOOL CALLBACK DialogFunc_Shell(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		EndDialog(hDlg, 0);
		return TRUE;
	}

	case WM_COMMAND:
	{
		switch (wParam)
		{

		case IDC_BUTTON_FILE_PATH:
		{
			container_path = PETool::OpenCommonDialog(hDlg);
			::SetWindowTextA(::GetDlgItem(hDlg, IDC_EDIT_FILE_NAME), container_path.c_str());
			return TRUE;
		}
		case IDC_BUTTON_SHELL_PATH:
		{
			content_path = PETool::OpenCommonDialog(hDlg);
			::SetWindowTextA(::GetDlgItem(hDlg, IDC_EDIT_SHELL_NAME), content_path.c_str());
			return TRUE;
		}
		case IDC_BUTTON_Start:
		{
			PROCESS_INFORMATION pi;
			if (CreateProc((char*)container_path.c_str(), &pi) == 0)//�������ܽ���
			{
				return TRUE;
			}
			if (UnmapView(pi) != 0)//ж��ӳ��
			{
				return TRUE;
			}
			if (Injection(pi) == 0)//ʵ��ע��
			{
				return TRUE;
			}
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

int CreateProc(char* path, PROCESS_INFORMATION* pi)
{
	STARTUPINFOA si;
	ZeroMemory(&si, sizeof(si));//��ʼ��Ϊ0
	si.cb = sizeof(si);
	ZeroMemory(pi, sizeof(pi));
	//�Թ���ķ�ʽ��������	
	return CreateProcessA(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, pi);
}

int UnmapView(PROCESS_INFORMATION pi)
{
	typedef NTSTATUS(WINAPI* ZwUnmapViewOfSection)(HANDLE, LPVOID);//���庯��ָ�� 
	ZwUnmapViewOfSection UnmapViewOfSection = (ZwUnmapViewOfSection)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "ZwUnmapViewOfSection");//��ȡ������ַ
	context.ContextFlags = CONTEXT_ALL;
	GetThreadContext(pi.hThread, &context);//��ȡ�߳�������
	DWORD base;
	ReadProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), &base, sizeof(DWORD), NULL);//��ȡ���ܽ��̻�ַ
	return UnmapViewOfSection(pi.hProcess, (LPVOID)base);//ж�ؿ��ܽ���ӳ��
}

int Injection(PROCESS_INFORMATION pi)
{
	DWORD result = 0;
	DWORD sizeImage = GetImageSize((char*)content_path.c_str());//��ȡҪע����̵�ImageSize
	DWORD ImageBase = GetImageBase();//��ȡIMageBase
	context.Eax = (GetEntryPoint() + ImageBase);//��ȡҪע��Ľ��̵���ڵ�,eax�б�������ڵ�
	VirtualAllocEx(pi.hProcess, (LPVOID)ImageBase, sizeImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);//�ڿ��ܽ���������Ҫע��Ľ�������Ҫ�Ŀռ��С��ע����ע������ݵ�ImageBaseΪ��ַ�������Ͳ����޸��ض�λ��IAT
	if (!WriteProcessMemory(pi.hProcess, (LPVOID)ImageBase, pBuffer, PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer)->OptionalHeader.SizeOfHeaders, NULL))//��Ҫע���HEADERӳ�䵽���ܽ���
	{
		return result;
	}

	PIMAGE_SECTION_HEADER psection = IMAGE_FIRST_SECTION(PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer));
	for (DWORD i = 0; i < PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer)->FileHeader.NumberOfSections; i++)
	{
		if (!WriteProcessMemory(pi.hProcess, (LPVOID)(ImageBase + psection->VirtualAddress), pBuffer + psection->PointerToRawData, psection->SizeOfRawData, NULL))//��Ҫע�������ӳ�䵽���ܽ���
		{
			return result;
		}
		++psection;
	}

	if (!WriteProcessMemory(pi.hProcess, (BYTE*)context.Ebx + 8, &ImageBase, sizeof(DWORD), NULL))//��Ҫע����޸��߳��������е�ImageBase
	{
		return result;
	}

	SetThreadContext(pi.hThread, &context);//�����޸ĺ���߳�������
	ResumeThread(pi.hThread);//�ָ��߳�
	result = 1;
	return result;
}

DWORD GetImageSize(char* path)//�����ļ����ڴ�
{
	DWORD result = 0;
	hfile = CreateFileA(content_path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //��Ҫע����ļ�
	if (hfile != INVALID_HANDLE_VALUE)
	{
		DWORD filesize = GetFileSize(hfile, NULL);
		pBuffer = new char[filesize];
		ReadFile(hfile, pBuffer, filesize, &filesize, NULL);
		PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER(pBuffer);
		if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
		{
			PIMAGE_NT_HEADERS pNtHeaders = PIMAGE_NT_HEADERS(pDosHeader->e_lfanew + pBuffer);
			result = pNtHeaders->OptionalHeader.SizeOfImage; //����PE�ṹ�õ�SizeOfImage
		}
	}
	return result;
}

DWORD GetEntryPoint()
{
	DWORD result = 0;
	PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER(pBuffer);
	if (pDosHeader->e_magic = IMAGE_DOS_SIGNATURE)
	{
		return  result;
	}
	PIMAGE_NT_HEADERS pNtHeaders = PIMAGE_NT_HEADERS(pDosHeader->e_lfanew + pBuffer);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = &pNtHeaders->OptionalHeader;
	result = OptionalHeader->AddressOfEntryPoint; //����PE�ṹ�õ�AddressOfEntryPoint
	return result;
}

DWORD GetImageBase()
{
	PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER(pBuffer);
	PIMAGE_NT_HEADERS pNtHeaders = PIMAGE_NT_HEADERS(pDosHeader->e_lfanew + pBuffer);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = &pNtHeaders->OptionalHeader;
	return OptionalHeader->ImageBase;//��PE�ṹ�ҵ�ImageBase //400000 //ASLR
}

