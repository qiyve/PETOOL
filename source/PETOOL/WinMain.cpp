//#include<TlHelp32.h>
#include<tchar.h>
#include "resource.h"
#include "EnumProcess.h"
#include "PETool.h"

std::string FilePath;
std::string container_path;
std::string content_path;

int CreateProc(char*, PROCESS_INFORMATION*);//创建傀儡进程
int UnmapView(PROCESS_INFORMATION);//卸载傀儡进程内存映射
int Injection(PROCESS_INFORMATION);//实现注入
DWORD GetImageSize(char*);//获取SizeOfImage
DWORD GetEntryPoint();//获取OEP
DWORD GetImageBase(PROCESS_INFORMATION pi);//获取基址
DWORD GetImageBase();//获取基址

CONTEXT context;//定义线程上下文结构
HANDLE hfile;//要注入的文件的句柄
char* pBuffer;//将文件读入内存的指针

//主窗口的回调函数
BOOL CALLBACK DialogFunc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK DialogFunc_Pe(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CALLBACK DialogFunc_Section(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//目录表的回调函数
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
		Process.InitListClass(hDlg);//初始化菜单栏
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
			MessageBox(NULL, _T("PETOOL 1.0"), _T("关于"), MB_OK);
			return TRUE;
		}
		}
	}
	}
	return FALSE;
}

//PE窗口的回调函数
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

//区段窗口的回调函数
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

//目录表的回调函数
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
			if (CreateProc((char*)container_path.c_str(), &pi) == 0)//创建傀儡进程
			{
				return TRUE;
			}
			if (UnmapView(pi) != 0)//卸载映射
			{
				return TRUE;
			}
			if (Injection(pi) == 0)//实现注入
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
	ZeroMemory(&si, sizeof(si));//初始化为0
	si.cb = sizeof(si);
	ZeroMemory(pi, sizeof(pi));
	//以挂起的方式创建进程	
	return CreateProcessA(path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, pi);
}

int UnmapView(PROCESS_INFORMATION pi)
{
	typedef NTSTATUS(WINAPI* ZwUnmapViewOfSection)(HANDLE, LPVOID);//定义函数指针 
	ZwUnmapViewOfSection UnmapViewOfSection = (ZwUnmapViewOfSection)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "ZwUnmapViewOfSection");//获取函数基址
	context.ContextFlags = CONTEXT_ALL;
	GetThreadContext(pi.hThread, &context);//获取线程上下文
	DWORD base;
	ReadProcessMemory(pi.hProcess, (LPVOID)(context.Ebx + 8), &base, sizeof(DWORD), NULL);//读取傀儡进程基址
	return UnmapViewOfSection(pi.hProcess, (LPVOID)base);//卸载傀儡进程映射
}

int Injection(PROCESS_INFORMATION pi)
{
	DWORD result = 0;
	DWORD sizeImage = GetImageSize((char*)content_path.c_str());//获取要注入进程的ImageSize
	DWORD ImageBase = GetImageBase();//获取IMageBase
	context.Eax = (GetEntryPoint() + ImageBase);//获取要注入的进程的入口点,eax中保存着入口点
	VirtualAllocEx(pi.hProcess, (LPVOID)ImageBase, sizeImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);//在傀儡进程中申请要注入的进程所需要的空间大小，注意以注入的内容的ImageBase为基址，这样就不用修复重定位和IAT
	if (!WriteProcessMemory(pi.hProcess, (LPVOID)ImageBase, pBuffer, PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer)->OptionalHeader.SizeOfHeaders, NULL))//将要注入的HEADER映射到傀儡进程
	{
		return result;
	}

	PIMAGE_SECTION_HEADER psection = IMAGE_FIRST_SECTION(PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer));
	for (DWORD i = 0; i < PIMAGE_NT_HEADERS(PIMAGE_DOS_HEADER(pBuffer)->e_lfanew + pBuffer)->FileHeader.NumberOfSections; i++)
	{
		if (!WriteProcessMemory(pi.hProcess, (LPVOID)(ImageBase + psection->VirtualAddress), pBuffer + psection->PointerToRawData, psection->SizeOfRawData, NULL))//将要注入的区块映射到傀儡进程
		{
			return result;
		}
		++psection;
	}

	if (!WriteProcessMemory(pi.hProcess, (BYTE*)context.Ebx + 8, &ImageBase, sizeof(DWORD), NULL))//将要注入的修改线程上下文中的ImageBase
	{
		return result;
	}

	SetThreadContext(pi.hThread, &context);//设置修改后的线程上下文
	ResumeThread(pi.hThread);//恢复线程
	result = 1;
	return result;
}

DWORD GetImageSize(char* path)//读入文件到内存
{
	DWORD result = 0;
	hfile = CreateFileA(content_path.c_str(), GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); //打开要注入的文件
	if (hfile != INVALID_HANDLE_VALUE)
	{
		DWORD filesize = GetFileSize(hfile, NULL);
		pBuffer = new char[filesize];
		ReadFile(hfile, pBuffer, filesize, &filesize, NULL);
		PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER(pBuffer);
		if (pDosHeader->e_magic == IMAGE_DOS_SIGNATURE)
		{
			PIMAGE_NT_HEADERS pNtHeaders = PIMAGE_NT_HEADERS(pDosHeader->e_lfanew + pBuffer);
			result = pNtHeaders->OptionalHeader.SizeOfImage; //遍历PE结构拿到SizeOfImage
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
	result = OptionalHeader->AddressOfEntryPoint; //遍历PE结构拿到AddressOfEntryPoint
	return result;
}

DWORD GetImageBase()
{
	PIMAGE_DOS_HEADER pDosHeader = PIMAGE_DOS_HEADER(pBuffer);
	PIMAGE_NT_HEADERS pNtHeaders = PIMAGE_NT_HEADERS(pDosHeader->e_lfanew + pBuffer);
	PIMAGE_OPTIONAL_HEADER OptionalHeader = &pNtHeaders->OptionalHeader;
	return OptionalHeader->ImageBase;//从PE结构找到ImageBase //400000 //ASLR
}

