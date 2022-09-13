#include "EnumProcess.h"

void EnumProcess::InitListClass(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//��ʼ��								
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡIDC_LIST_PROCESS���								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Process);
	//��������ѡ��								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = LPSTR("����");					//�б���				
	lv.cx = 230;								//�п�
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	//�ڶ���								
	lv.pszText = LPSTR("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	ListView_InsertColumn(hListProcess, 1, &lv);
	//SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
	//������								
	lv.pszText = LPSTR("�����ַ");
	lv.cx = 120;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//������								
	lv.pszText = LPSTR("�����С");
	lv.cx = 120;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

	//��ȡIDC_LIST_Moudle���
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Moudle);
	//��������ѡ��								
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPSTR)"ģ���ַ";				//�б���		
	lv.cx = 320;								//�п�
	lv.iSubItem = 0;
	//����Ϣ�������б��
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = (LPSTR)"ģ������";				//�б���		
	lv.cx = 200;								//�п�
	lv.iSubItem = 1;
	//����Ϣ�������б��
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
}

void EnumProcess::Enum_Process(HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess = GetDlgItem(hDlg, IDC_LIST_Process);

	//��ʼ��				
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;

	// PROCESSENTRY32 �ṹ �����������ʱפ����ϵͳ��ַ�ռ��еĽ����б��е���Ŀ
	PROCESSENTRY32 Pe32{ 0 };
	// MODULEENTRY32 �ṹ ��������ָ�����̵�ģ���б��е���Ŀ��
	MODULEENTRY32 me{ 0 };
	Pe32.dwSize = sizeof(PROCESSENTRY32);
	me.dwSize = sizeof(MODULEENTRY32);
	char ProcessID[128];
	char ModBaseAddr[0x128];
	char ModBaseSize[0x128];
	HANDLE ProcessSnopShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  //TH32CS_SNAPPROCESS �ڿ����а���ϵͳ�е����н��̡���Ҫö�ٽ���
	HANDLE ModuleSnopShot = 0;

	if (ProcessSnopShot != INVALID_HANDLE_VALUE)
	{
		bool bRet = Process32First(ProcessSnopShot, &Pe32); // �����й���ϵͳ�����������ĵ�һ�����̵���Ϣ

		int a = 0;
		while (bRet)
		{
			vitem.pszText = Pe32.szExeFile;
			//�ڼ���
			vitem.iItem = a;
			//�ڼ���
			vitem.iSubItem = 0;
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

			sprintf(ProcessID, "%d", Pe32.th32ProcessID);
			vitem.pszText = ProcessID;
			//�ڼ���
			vitem.iItem = a;
			//�ڼ���
			vitem.iSubItem = 1;
			ListView_SetItem(hListProcess, &vitem);
			//if (Pe32.th32ProcessID && Pe32.th32ProcessID != 4 && Pe32.th32ProcessID != 8)
			ModuleSnopShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Pe32.th32ProcessID);
			bool RET = Module32First(ModuleSnopShot, &me);
			while (RET)
			{
				if (strcmp(Pe32.szExeFile, me.szModule) == 0)//�ж��ǲ�����ģ��
				{
					sprintf(ModBaseAddr, "%p", me.modBaseAddr);
					vitem.pszText = ModBaseAddr;
					//�ڼ���
					vitem.iItem = a;
					//�ڼ���
					vitem.iSubItem = 2;
					ListView_SetItem(hListProcess, &vitem);

					sprintf(ModBaseSize, "%p", me.modBaseSize);
					vitem.pszText = ModBaseSize;
					//�ڼ���
					vitem.iItem = a;
					//�ڼ���
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

	//��ʼ��
	memset(&lv, 0, sizeof(LV_ITEM));
	memset(szPid, 0, 0x20);
	//��ȡѡ����
	dwRowID = SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	//��ȡPID
	lv.iSubItem = 1;					//Ҫ��ȡ����
	lv.pszText = szPid;				//����PID�Ļ�����
	lv.cchTextMax = 0x20;			//��������С
	SendMessageA(hListProcess, LVM_GETITEMTEXT, dwRowID, (DWORD)&lv);
	//����ģ��
	LV_ITEM vitem;

	//��ʼ��				
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
			//�ڼ���
			vitem.iItem = a;
			//�ڼ���
			vitem.iSubItem = 0;
			SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

			vitem.pszText = me.szExePath;
			//�ڼ���
			vitem.iItem = a;
			//�ڼ���
			vitem.iSubItem = 1;
			ListView_SetItem(hListProcess, &vitem);
			bRet = Module32Next(ModuleSnopShot, &me);
			a++;
		}
		CloseHandle(ModuleSnopShot);
	}
}

