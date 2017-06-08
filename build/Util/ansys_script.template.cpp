#include <iostream>
#include <fstream>
#include <windows.h>
#include "resource.h"
using namespace std;
//@@HEADER
char* path;
int IsLicenseValid();
void AnsysShowDialog(char*);
void AnsysExportScript(char*);
//////////////////////////////
void CoutScript_ManagerMaterial();//���Ͽ�
void CoutScript_ExportResult();//�������
void CoutScript_ImportParaSolid();//ģ�͵���
void CoutScript_PressureWave();//��ѹ��
void CoutScript_ManagerCase();//��������
void CoutScript_PressureConcentrated();//�µ���
void CoutScript_PressureStatic();//��ˮѹ
void CoutScript_PressureDynamic();//��ˮѹ
void CoutScript_PressureUplift();//��ѹ��
void CoutScript_ManagerComponent();//���������
void CoutScript_SeismicTimeHistory();//ʱ������
void CoutScript_SeismicSpectrum();//��Ӧ�׷���
void CoutScript_ToolKitSeismic();//����������߼�
void CoutScript_ToolKitLoad();//�غ�ʩ�ӹ��߼�
int main(int argc, char *argv[])
{
	if (argc != 3) {
		return 1;
	}
	char* modulename = argv[1];
	path = argv[2];
	int verify = IsLicenseValid();
	if (verify == 0)
	{
		AnsysExportScript(modulename);
	}
	else if (verify == 1) {
		AnsysShowDialog("Can Not Find The License! ");
	}
	else {
		AnsysShowDialog("Can Not Find The DLL! ");
	}
	return 0;
}
void AnsysShowDialog(char* msg)
{
	cout << "set path " << path <<"/locale"<< endl;
	cout << "::msgcat::mcload  $path"<< endl;
	cout << "tk_messageBox -icon warning "
		"-title [::msgcat::mc \"Tips \"] "
		"-message  [::msgcat::mc \"" << msg << "\"]";
}

int IsLicenseValid()
{
	//@@DECODE
}
//���ģ���Ƿ����
void AnsysExportScript(char* modulename)
{
	if (strcmp(modulename, "ManagerMaterial") == 0)
	{
		CoutScript_ManagerMaterial();
	}
	else if (strcmp(modulename, "ExportResult") == 0)
	{
		CoutScript_ExportResult();
	}
	else if (strcmp(modulename, "ImportParaSolid") == 0)
	{
		CoutScript_ImportParaSolid();
	}
	else if (strcmp(modulename, "PressureConcentrated") == 0)
	{
		CoutScript_PressureConcentrated();
	}
	else if (strcmp(modulename, "PressureStatic") == 0)
	{
		CoutScript_PressureStatic();
	}
	else if (strcmp(modulename, "PressureDynamic") == 0)
	{
		CoutScript_PressureDynamic();
	}
	else if (strcmp(modulename, "PressureUplift") == 0)
	{
		CoutScript_PressureUplift();
	}
	else if (strcmp(modulename, "PressureWave") == 0)
	{
		CoutScript_PressureWave();
	}
	else if (strcmp(modulename, "ManagerComponent") == 0)
	{
		CoutScript_ManagerComponent();
	}
	else if (strcmp(modulename, "SeismicTimeHistory") == 0)
	{
		CoutScript_SeismicTimeHistory();
	}
	else if (strcmp(modulename, "SeismicSpectrum") == 0)
	{
		CoutScript_SeismicSpectrum();
	}
	else if (strcmp(modulename, "ToolKitLoad") == 0)
	{
		CoutScript_ToolKitLoad();
	}
	else if (strcmp(modulename, "ManagerCase") == 0)
	{
		CoutScript_ManagerCase();
	}
	else if (strcmp(modulename, "ToolKitSeismic") == 0)
	{
		CoutScript_ToolKitSeismic();
	}
	else
	{
		AnsysShowDialog("Can Not Find The Module!");
	}
}

//////////////////////////////
//�������Ͽ�
void CoutScript_ManagerMaterial()
{
//@@ManagerMaterial
}

//�������
void CoutScript_ExportResult()
{
//@@ExportResult
}

//����ParaSolidģ��
void CoutScript_ImportParaSolid()
{
//@@ImportParaSolid
}

//��ѹ��
void CoutScript_PressureWave()
{
//@@PressureWave
}

//��������
void CoutScript_ManagerCase()
{
//@@ManagerCase
}

//������
void CoutScript_PressureConcentrated()
{
//@@PressureConcentrated
}
void CoutScript_PressureStatic()
{
//@@PressureStatic
}
void CoutScript_PressureDynamic()
{
//@@PressureDynamic
}
void CoutScript_PressureUplift()
{
//@@PressureUplift
}
void CoutScript_ManagerComponent()
{
//@@ManagerComponent
}
void CoutScript_SeismicTimeHistory()
{
//@@SeismicTimeHistory
}
void CoutScript_SeismicSpectrum()
{
//@@SeismicSpectrum
}
void CoutScript_ToolKitSeismic()
{
//@@ToolKitSeismic
}
void CoutScript_ToolKitLoad()
{
//@@ToolKitLoad
}