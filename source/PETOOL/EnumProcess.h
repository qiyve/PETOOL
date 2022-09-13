#pragma once
#include <Windows.h>
#include <commctrl.h>	
#include "resource.h"
#include "tlhelp32.h"
#include <iostream>
#pragma comment(lib,"comctl32.lib")			
#pragma warning(disable:4996)

class EnumProcess
{
public:
	void InitListClass(HWND hDlg);
	void Enum_Process(HWND hDlg);
	void Enum_Module(HWND hDlg);
};

