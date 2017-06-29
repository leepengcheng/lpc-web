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
void CoutScript_ManagerMaterial();//材料管理
void CoutScript_ExportResult();//结果导出
void CoutScript_ImportParaSolid();//模型导入
void CoutScript_PressureWave();//浪压力
void CoutScript_ManagerCase();//工况管理
void CoutScript_PressureConcentrated();//集中力
void CoutScript_PressureStatic();//静水压力
void CoutScript_PressureDynamic();//动水压力
void CoutScript_PressureUplift();//扬压力
void CoutScript_ManagerComponent();//组件管理
void CoutScript_SeismicTimeHistory();//时间历程
void CoutScript_SeismicSpectrum();//反应谱
void CoutScript_ToolKitSeismic();//地震分析工具
void CoutScript_ToolKitLoad();//载荷分析工具
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
//锟斤拷锟侥ｏ拷锟斤拷欠锟斤拷锟斤拷
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
//锟斤拷锟斤拷锟斤拷锟较匡拷
void CoutScript_ManagerMaterial()
{
//@@ManagerMaterial
}

//锟斤拷锟斤拷锟斤拷锟?
void CoutScript_ExportResult()
{
//@@ExportResult
}

//锟斤拷锟斤拷ParaSolid模锟斤拷
void CoutScript_ImportParaSolid()
{
//@@ImportParaSolid
}

//锟斤拷压锟斤拷
void CoutScript_PressureWave()
{
//@@PressureWave
}

//锟斤拷锟斤拷锟斤拷锟斤拷
void CoutScript_ManagerCase()
{
//@@ManagerCase
}

//锟斤拷锟斤拷锟斤拷
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