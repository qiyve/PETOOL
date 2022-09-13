#include "EnumProcess.h"

void EnumProcess::InitListClass(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//初始化								
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取IDC_LIST_PROCESS句柄								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Process);
	//设置整行选中								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第一列								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = LPSTR("进程");					//列标题				
	lv.cx = 230;								//列宽
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	//第二列								
	lv.pszText = LPSTR("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
	//第三列								
	lv.pszText = LPSTR("镜像基址");
	lv.cx = 120;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//第四列								
	lv.pszText = LPSTR("镜像大小");
	lv.cx = 120;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

	//获取IDC_LIST_Moudle句柄
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Moudle);
	//设置整行选中								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPSTR)"模块地址";				//列标题		
	lv.cx = 320;								//列宽
	lv.iSubItem = 0;
	//将消息发送至列表框
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPSTR)"模块名称";				//列标题		
	lv.cx = 200;								//列宽
	lv.iSubItem = 1;
	//将消息发送至列表框
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
}

void EnumProcess::Enum_Process(HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess = GetDlgItem(hDlg, IDC_LIST_Process);

	//初始化				
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	// PROCESSENTRY32 结构 描述拍摄快照时驻留在系统地址空间中的进程列表中的条目
	PROCESSENTRY32 Pe32{ 0 };
	// MODULEENTRY32 结构 描述属于指定进程的模块列表中的条目。
	MODULEENTRY32 me{ 0 };
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	me.dwSize = sizeof(MODULEENTRY32);
	char ProcessID[128];
	char ModBaseAddr[0x128];
	char ModBaseSize[0x128];
	HANDLE ProcessSnopShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //TH32CS_SNAPPROCESS 在快照中包括系统中的所有进程。若要枚举进程
	HANDLE ModuleSnopShot = 0;

	if (ProcessSnopShot != INVALID_HANDLE_VALUE)
	{
		bool bRet = Process32First(ProcessSnopShot, &Pe32); // 检索有关在系统快照中遇到的第一个进程的信息

		int a = 0;
		while (bRet)
		{
			vitem.pszText = Pe32.szExeFile;
			//第几行
			vitem.iItem = a;
			//第几列
			vitem.iSubItem = 0;
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

			sprintf(ProcessID, "%d", Pe32.th32ProcessID);
			vitem.pszText = ProcessID;
			//第几行
			vitem.iItem = a;
			//第几列
			vitem.iSubItem = 1;
			ListView_SetItem(hListProcess, &vitem);
			//if (Pe32.th32ProcessID && Pe32.th32ProcessID != 4 && Pe32.th32ProcessID != 8)
			ModuleSnopShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Pe32.th32ProcessID);
			bool RET = Module32First(ModuleSnopShot, &me);
			while (RET)
			{
				if (strcmp(Pe32.szExeFile, me.szModule) == 0)//判断是不是主模块
				{
					sprintf(ModBaseAddr, "%p", me.modBaseAddr);
					vitem.pszText = ModBaseAddr;
					//第几行
					vitem.iItem = a;
					//第几列
					vitem.iSubItem = 2;
					ListView_SetItem(hListProcess, &vitem);

					sprintf(ModBaseSize, "%p", me.modBaseSize);
					vitem.pszText = ModBaseSize;
					//第几行
					vitem.iItem = a;
					//第几列
					vitem.iSubItem = 3;
					ListView_SetItem(hListProcess, &vitem);


				}
				RET = Module32Next(ModuleSnopShot, &me);
			}
			CloseHandle(ModuleSnopShot);

			a++;
			bRet = Process32Next(ProcessSnopShot, &Pe32);
		}
		CloseHandle(ProcessSnopShot);
	}
}

void EnumProcess::Enum_Module(HWND hDlg)
{
	DWORD dwRowID;
	char szPid[0x20];
	HWND hListProcess = GetDlgItem(hDlg, IDC_LIST_Process);
	LV_ITEM lv;

	//初始化
	memset(&lv, 0, sizeof(LV_ITEM));
	memset(szPid, 0, 0x20);
	//获取选择行
	dwRowID = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	//获取PID
	lv.iSubItem = 1;					//要获取的列
	lv.pszText = szPid;				//储存PID的缓冲区
	lv.cchTextMax = 0x20;			//缓冲区大小
	SendMessageA(hListProcess, LVM_GETITEMTEXT, dwRowID, (DWORD)&lv);
	//遍历模块
	LV_ITEM vitem;

	//初始化				
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Moudle);
	MODULEENTRY32 me{ 0 };
	me.dwSize = sizeof(MODULEENTRY32);
	DWORD pid = atoi(szPid);
	HANDLE ModuleSnopShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	int a = 0;
	if (ModuleSnopShot != 0)
	{
		bool bRet = Module32First(ModuleSnopShot, &me);
		while (bRet)
		{
			vitem.pszText = me.szModule;
			//第几行
			vitem.iItem = a;
			//第几列
			vitem.iSubItem = 0;
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

			vitem.pszText = me.szExePath;
			//第几行
			vitem.iItem = a;
			//第几列
			vitem.iSubItem = 1;
			ListView_SetItem(hListProcess, &vitem);
			bRet = Module32Next(ModuleSnopShot, &me);
			a++;
		}
		CloseHandle(ModuleSnopShot);
	}
}

