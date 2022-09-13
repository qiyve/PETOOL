#include "PETool.h"
#include "resource.h"

PETool PE;

PETool::PETool()
{
	pDosHeader = 0;
	pNtHeaders = 0;
	pFileHeader = 0;
	pOptionalHeader = 0;
	pSectionHeader = 0;
	pTempFileBuffer = 0;
}

PETool::~PETool()
{
	free(PE.pTempFileBuffer);
	//MessageBoxA(0, "�����ͷ���", 0, 0);
}

std::string PETool::OpenCommonDialog(HWND hDlg)
{
	//��ͨ�öԻ���
	OPENFILENAMEA stOpenFile;
	//������
	char szPeFileExt[MAX_PATH] = "* PE File(*.exe;*.dll;*.sys;*.*dmp)\0*.exe;*.dll;*.sys;*.dmp\0ȫ���ļ�(*.*)\0*.*\0\0";
	char szFileName[MAX_PATH];
	memset(szFileName, 0, MAX_PATH);
	memset(&stOpenFile, 0, sizeof(OPENFILENAMEA));
	stOpenFile.lStructSize = sizeof(OPENFILENAMEA);
	stOpenFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	stOpenFile.hwndOwner = hDlg;
	//ѡ���ļ��ľ���·��
	stOpenFile.lpstrFilter = szPeFileExt;
	stOpenFile.lpstrFile = szFileName;
	stOpenFile.nMaxFile = MAX_PATH;
	GetOpenFileName(&stOpenFile);
	return std::string(stOpenFile.lpstrFile);
}

int PETool::OpenFile(HWND hDlg, const char* FileName)
{
	DWORD FileSize = 0;
	FILE* pFile = 0;

	char Buffer[0x128];
	sprintf(Buffer, "%s", FileName);
	SetWindowTextA(hDlg, Buffer);
	pFile = fopen(Buffer, "rb");

	if (pFile == 0)
	{
		return FailTOOpenFile;
	}
	//��ȡ��������С
	fseek(pFile, 0, SEEK_END);
	FileSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	PE.pTempFileBuffer = malloc(FileSize);
	if (!PE.pTempFileBuffer)
	{
		fclose(pFile);
		return FaliToMalloc;
	}
	size_t n = fread(PE.pTempFileBuffer, FileSize, 1, pFile);
	//��ʼ����PEͷ
	PE.pDosHeader = (PIMAGE_DOS_HEADER)PE.pTempFileBuffer;
	if (*PWORD(PE.pDosHeader) != IMAGE_DOS_SIGNATURE)
	{
		MessageBoxA(0, "������Ч��PE�ļ�", "��ʾ:", 0);
		return NoPeFile;
	}
	//��������Ϊɶ����һ���ֽ�
	PE.pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)PE.pDosHeader + PE.pDosHeader->e_lfanew);
	if (*PDWORD(PE.pNtHeaders) != IMAGE_NT_SIGNATURE)
	{
		MessageBoxA(0, "������Ч��PE�ļ�", "��ʾ:", 0);
		return NoPeFile;
	}
	PE.pFileHeader = &PE.pNtHeaders->FileHeader;
	PE.pOptionalHeader = &PE.pNtHeaders->OptionalHeader;
	PE.pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)PE.pOptionalHeader + PE.pFileHeader->SizeOfOptionalHeader);
	//��ʼ��༭���������
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_OEP, PE.pOptionalHeader->AddressOfEntryPoint);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_CheckSum, PE.pOptionalHeader->CheckSum);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT3_ImageBase, PE.pOptionalHeader->ImageBase);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_BaseofCode, PE.pOptionalHeader->BaseOfCode);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_SizeofImage, PE.pOptionalHeader->SizeOfImage);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_BaseOfData, PE.pOptionalHeader->BaseOfData);
	PETool::Printf_EditInfo(hDlg, DC_EDIT_SectionAligment, PE.pOptionalHeader->SectionAlignment);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_FILEAligent, PE.pOptionalHeader->FileAlignment);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_Magic, PE.pOptionalHeader->Magic);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_Subsystem, PE.pOptionalHeader->Subsystem);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_NumberofSection, PE.pFileHeader->NumberOfSections);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_TimeDataStap, PE.pFileHeader->TimeDateStamp);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_SizeofHeader, PE.pOptionalHeader->SizeOfHeaders);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_Characteristics, PE.pFileHeader->Characteristics);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_SizeofOptionalHeadel, PE.pFileHeader->SizeOfOptionalHeader);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_NumberofRvaAndSize, PE.pOptionalHeader->NumberOfRvaAndSizes);
	fclose(pFile);
	return FileSize;
}

void PETool::InitClass_Section(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//��ʼ��								
	memset(&lv, 0, sizeof(LV_COLUMN));

	//SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//��ȡIDC_LIST_PROCESS���								
	hListProcess = GetDlgItem(hDlg, IDC_LIST_Section);

	//��һ��								
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = LPSTR("����");				//�б���				
	lv.cx = 80;								//�п�
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);
	//�ڶ���								
	lv.pszText = LPSTR("R.�ļ�ƫ��");
	lv.cx = 80;
	lv.iSubItem = 1;
	//ListView_InsertColumn(hListProcess, 1, &lv);								
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);
	//������								
	lv.pszText = LPSTR("R.�ļ���С");
	lv.cx = 80;
	lv.iSubItem = 2;
	ListView_InsertColumn(hListProcess, 2, &lv);
	//������								
	lv.pszText = LPSTR("V.�ڴ�ƫ��");
	lv.cx = 80;
	lv.iSubItem = 3;
	ListView_InsertColumn(hListProcess, 3, &lv);

	//������								
	lv.pszText = LPSTR("V.�ڴ��С");
	lv.cx = 80;
	lv.iSubItem = 4;
	ListView_InsertColumn(hListProcess, 4, &lv);

	//������								
	lv.pszText = LPSTR("��������");
	lv.cx = 100;
	lv.iSubItem = 5;
	ListView_InsertColumn(hListProcess, 5, &lv);
}

void PETool::Printf_SectionInfo(HWND hDlg)
{
	LV_ITEM vitem;
	HWND hListProcess = GetDlgItem(hDlg, IDC_LIST_Section);

	//��ʼ��
	memset(&vitem, 0, sizeof(LV_ITEM));
	vitem.mask = LVIF_TEXT;
	//��ʼ�����ڱ���Ϣ
	PIMAGE_SECTION_HEADER TempSectionHeader = PE.pSectionHeader;
	//������Ҫ�����ķ�9���ֽ� ��Ȼ������ 
	char Info[256];
	// int a = 0;
	for (size_t i = 0; i < PE.pFileHeader->NumberOfSections; i++)
	{
		sprintf(Info, "%s", TempSectionHeader->Name);
		vitem.pszText = Info;
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 0;
		SendMessage(hListProcess, LVM_INSERTITEM, 0, (DWORD)&vitem);

		sprintf(Info, "%08x", TempSectionHeader->PointerToRawData);
		vitem.pszText = _strupr(Info);
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 1;
		ListView_SetItem(hListProcess, &vitem);

		sprintf(Info, "%08x", TempSectionHeader->SizeOfRawData);
		vitem.pszText = _strupr(Info);
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 2;
		ListView_SetItem(hListProcess, &vitem);

		sprintf(Info, "%08x", TempSectionHeader->VirtualAddress);
		vitem.pszText = _strupr(Info);
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 3;
		ListView_SetItem(hListProcess, &vitem);
		sprintf(Info, "%08x", TempSectionHeader->Misc.VirtualSize);
		vitem.pszText = _strupr(Info);
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 4;
		ListView_SetItem(hListProcess, &vitem);
		sprintf(Info, "%08x", TempSectionHeader->Characteristics);
		vitem.pszText = _strupr(Info);
		//�ڼ���
		vitem.iItem = i;
		//�ڼ���
		vitem.iSubItem = 5;
		ListView_SetItem(hListProcess, &vitem);
		TempSectionHeader++;
	}
}

DWORD PETool::MemoryAlignment(DWORD Data, DWORD Mode)
{
	if (Data % Mode == 0)
	{
		return Data;
	}
	else
	{
		return ((Data / Mode) + 1) * Mode;
	}
}

void PETool::Printf_Directory(HWND hDlg)
{
	DWORD EditRvaMsg[] = {
		IDC_EDIT_Rva_Export, IDC_EDIT_Rva_Import,IDC_EDIT3_RvaRESOURCE,
		IDC_EDIT_Rva__EXCEPTION,IDC_EDIT_Rva_SECURITY,
		IDC_EDIT_Rva__BASERELOC,IDC_EDIT_Rva__DEBUG,IDC_EDIT_Rva__COPYRIGHT,
		IDC_EDIT_Rva_GLOBALPTR,IDC_EDIT_Rva_TLS,IDC_EDIT_Rva__LOAD_CONFIG,
		IDC_EDIT_Rva__BOUND_IMPORT,IDC_EDIT_Rva__IAT,IDC_EDIT_Rva__DELAY_IMPORT,
		IDC_EDIT_PE_COM_DESCRIPTOR,IDC_EDIT_Rva_lparsent
	};
	DWORD EditSizeMsg[] = { IDC_EDIT_Size_Export ,IDC_EDIT_Size_Import,IDC_EDIT_Siez_RESOURCE,IDC_EDIT_Size__EXCEPTION,
	IDC_EDIT_Siez_SECURITY,IDC_EDIT_Size__BASERELOC,IDC_EDIT_Siez__DEBUG,IDC_EDIT_Size__COPYRIGHT,IDC_EDIT__Size__GLOBALPTR,IDC_EDIT_Size__TLS,
	IDC_EDIT_Size_LOAD_CONFIG,IDC_EDIT_Size__BOUND_IMPORT,IDC_EDIT_Size_IAT,IDC_EDIT_Size_DELAY_IMPORT,IDC_EDIT_Size_COM_DESCRIPTOR
	,IDC_EDIT_Size_lparent

	};
	PIMAGE_DATA_DIRECTORY TempDataDIRECTORY = PE.pOptionalHeader->DataDirectory;
	for (size_t i = 0; i < 16; i++)
	{
		PETool::Printf_EditInfo(hDlg, EditRvaMsg[i], TempDataDIRECTORY->VirtualAddress);
		PETool::Printf_EditInfo(hDlg, EditSizeMsg[i], TempDataDIRECTORY->Size);
		TempDataDIRECTORY++;
	}
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_Rva_Export, PE.pOptionalHeader->DataDirectory->VirtualAddress);
	PETool::Printf_EditInfo(hDlg, IDC_EDIT_Size_Export, PE.pOptionalHeader->DataDirectory->Size);
}

template<class T>
void PETool::Printf_EditInfo(HWND hDlg, UINT Msg, T Context)
{
	char szBuffer[0x128];
	sprintf(szBuffer, "%08x\n", Context);

	SendDlgItemMessage(hDlg, Msg, WM_SETTEXT, 0, (DWORD)(szBuffer));

}
