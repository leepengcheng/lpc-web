/*--------------------------------------------------------------------------------------+
|   ExportCalculation.r
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"

/*----------------------------------------------------------------------+
|生成计算书对话框                 |
-----------------------------------------------------------------------*/
//对话框设计文本信息定义
#define	TXT_EXPORTCALC_DIALOG	"生成计算书"

#define	TXT_EXPORTCALC_TABPAGE1	"工程概况"
#define	TXT_EXPORTCALC_TABPAGE2	"计算工况及载荷"
#define	TXT_EXPORTCALC_TABPAGE3	"计算模型"
#define	TXT_EXPORTCALC_TABPAGE4	"计算成果"
#define	TXT_EXPORTCALC_TABPAGE4_1	"位移计算成果"
#define	TXT_EXPORTCALC_TABPAGE4_2	"应力计算成果"
#define	TXT_EXPORTCALC_TABPAGE4_3	"配筋参数表"

#define TXT_EXPORTCALC_PUSHBTN_Submit	"生成"
#define	TXT_EXPORTCALC_PUSHBTN_Cancel	"取消"

//对话框相关尺寸定义
#define DIALOG_W	60*XC
#define	DIALOG_H	34*YC

#define PUSHBTN_H	2*YC
#define PUSHBTN_W	10*XC

#define TABPAGE_X	1*XC		//注意tabpage中控件的坐标是以对话框为参照的
#define	TABPAGE_Y	2*YC
#define	TABPAGE_W	(DIALOG_W-2*TABPAGE_X)
#define	TABPAGE_H	(DIALOG_H-TABPAGE_Y-PUSHBTN_H)
#define TABPAGE_SUBX	(TABPAGE_X + XC)
#define	TABPAGE_SUBY	(TABPAGE_Y + 2*YC)
#define TABPAGE_SUBW	(TABPAGE_W - 2*XC)
#define TABPAGE_SUBH	(TABPAGE_H - 2.5*YC)

#define	LISTBOX_W	(DIALOG_W - 4*XC)
#define	LISTBOX_H	10*YC
#define LISTBOX_SUBW	(TABPAGE_SUBW - 2*XC)
#define LISTBOX_SUBH	7*YC

//对话框定义
DialogBoxRsc DIALOGID_ExportCalculation =
{
	DIALOGATTR_DEFAULT |  DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	DIALOG_W, DIALOG_H,
	NOHELP, MHELP,
	HOOKDIALOG_ExpCalc, NOPARENTID,
	TXT_EXPORTCALC_DIALOG,
	{
		//选择输出的工程
		{{8*XC, 0.5*YC,  17*XC, 1.4*YC}, ComboBox,   COMBOBOXID_ExportCalc_InstanceLists,  ON,0, "", ""}, 

		//计算书生成窗口的TabPage分页
		{{TABPAGE_X, TABPAGE_Y, TABPAGE_W, TABPAGE_H}, TabPageList, TPLISTID_ExportCalc_Main, ON, 0, "", ""},

		//按钮—生成
		{{DIALOG_W/2 - BUTTON_STDWIDTH, DIALOG_H - PUSHBTN_H + 0.2*YC, BUTTON_STDWIDTH, 0}, PushButton,PUSHBTNID_ExportCalc_Submit, ON, ALIGN_CENTER, "", ""},

		//按钮—取消
		{{DIALOG_W/2 + BUTTON_STDWIDTH/2, DIALOG_H - PUSHBTN_H + 0.2*YC, BUTTON_STDWIDTH, 0}, PushButton,PUSHBTNID_ExportCalc_Cancel, ON, ALIGN_CENTER, "", ""},
	}
};

//TabPage列表定义
DItem_TabPageListRsc  TPLISTID_ExportCalc_Main =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_TABSFITPAGEWIDTH,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Project,		ON, 0,"",""},		//page1
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Case,			ON, 0,"",""},		//page2		
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Model,		ON, 0,"",""},		//page3
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_Results,		ON, 0,"",""},		//page4
	}								
};							

//TabPage1定义
DItem_TabPageRsc TABPAGEID_ExportCalc_Project =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE1,
	{	
		//工程概况
		{{TABPAGE_X + XC, TABPAGE_Y + 2*YC, 56*XC, TABPAGE_H - 4.5*YC}, MLText, MLTEXTID_ExportCalc_ProjectDescr, ON, 0, "", ""},

		//导入概况文档
		{{TABPAGE_SUBX + 45*XC ,  30*YC, 0, 0}, PushButton, PUSHBTNID_ExportCalc_Import, ON, ALIGN_CENTER, "", ""},
	}
};

//TabPage2定义
DItem_TabPageRsc TABPAGEID_ExportCalc_Case =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE2,
	{
		//工况列表
		{{TABPAGE_X + XC, TABPAGE_Y + 2*YC, LISTBOX_W, LISTBOX_H*2.75}, ListBox, LISTBOXID_ExportCalc_CaseList, ON, 0, "", ""},
	}
};

//TabPage3定义
DItem_TabPageRsc TABPAGEID_ExportCalc_Model =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE3,
	{
		//提示文本
		{{TABPAGE_X + XC ,  TABPAGE_Y + 3*YC,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "计算网格模型图路径：", ""},

		//选择模型图路径按钮
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_Y + 2*YC + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelModelPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_Y + 2*YC + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddModelPath, ON, ALIGN_CENTER, "", ""},

		//已选模型图路径列表
		{{TABPAGE_X + XC, TABPAGE_Y + 4*YC, LISTBOX_W, LISTBOX_H}, ListBox, LISTBOXID_ExportCalc_ModelPaths, ON, 0, "", ""},

		//总单元数
		{{TABPAGE_X + 10*XC, TABPAGE_Y + 4.5*YC + LISTBOX_H, 8*XC, 0}, Text, TEXTID_ExportCalc_CellCount, ON, 0, "", ""},

		//节点数
		{{TABPAGE_X + 30*XC, TABPAGE_Y + 4.5*YC + LISTBOX_H, 8*XC, 0}, Text, TEXTID_ExportCalc_NodeCount, ON, 0, "", ""},

		//计算模型参数列
		{{TABPAGE_X + XC, TABPAGE_Y + 5.5*YC +LISTBOX_H, LISTBOX_W, LISTBOX_H + 4*YC}, ListBox, LISTBOXID_ExportCalc_ModelParamList, ON, 0, "", ""},
	}
};

//TabPage4定义
DItem_TabPageRsc TABPAGEID_ExportCalc_Results =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4,
	{
		//计算成果页的Tabpage分页
		{{TABPAGE_SUBX, TABPAGE_SUBY, TABPAGE_SUBW, TABPAGE_SUBH}, TabPageList, TPLISTID_ExportCalc_Sub, ON, 0, "", ""},
	}
};

//TabPage列表定义
DItem_TabPageListRsc  TPLISTID_ExportCalc_Sub =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_DEFAULT,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsWY,		ON, 0,"",""},		//page1
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsYL,		ON, 0,"",""},		//page2
		{{0,0,0,0}, TabPage, TABPAGEID_ExportCalc_ResultsPJ,		ON, 0,"",""},		//page3
	}								
};	

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsWY =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_1,
	{
		//位移参数表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH * 1.4}, ListBox, LISTBOXID_ExportCalc_WYParamList, ON, 0, "", ""},

		//提示文本
		{{TABPAGE_SUBX + XC ,  TABPAGE_SUBY + 3*YC + LISTBOX_SUBH * 1.4,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "云图或等值线图路径：", ""},

		//选择路径按钮
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH * 1.4 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelWYCloudPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH * 1.4 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddWYCloudPath, ON, ALIGN_CENTER, "", ""},

		//位移云图或等值线图路径列表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 4*YC + LISTBOX_SUBH * 1.4, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_WYCloudPaths, ON, 0, "", ""},

	}
};

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsYL =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_2,
	{
		//应力参数表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLParamList, ON, 0, "", ""},

		//提示文本
		{{TABPAGE_SUBX + XC ,   TABPAGE_SUBY + 3*YC + LISTBOX_SUBH,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "云图或等值线图路径：", ""},

		//选择路径按钮
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelYLCloudPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 2*YC + LISTBOX_SUBH + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddYLCloudPath, ON, ALIGN_CENTER, "", ""},

		//应力云图或等值线图路径列表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 4*YC + LISTBOX_SUBH, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLCloudPaths, ON, 0, "", ""},

		//提示文本
		{{TABPAGE_SUBX + XC ,    TABPAGE_SUBY + 5*YC + LISTBOX_SUBH*2,  25*XC, 0}, Label, 0, ON, ALIGN_LEFT, "应力数值图路径：", ""},

		//选择路径按钮
		{{TABPAGE_SUBX + 36*XC ,  TABPAGE_SUBY + 4*YC + LISTBOX_SUBH*2 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_DelYLPath, ON, ALIGN_CENTER, "", ""},
		{{TABPAGE_SUBX + 45*XC ,  TABPAGE_SUBY + 4*YC + LISTBOX_SUBH*2 + 0.2*YC, 8*XC, 0}, PushButton, PUSHBTNID_ExportCalc_AddYLPath, ON, ALIGN_CENTER, "", ""},

		//应力数值图路径列表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 6*YC + LISTBOX_SUBH*2, LISTBOX_SUBW, LISTBOX_SUBH}, ListBox, LISTBOXID_ExportCalc_YLPaths, ON, 0, "", ""},
	}
};

DItem_TabPageRsc TABPAGEID_ExportCalc_ResultsPJ =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    NOHOOK, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_EXPORTCALC_TABPAGE4_3,
	{
		//钢筋参数表
		{{TABPAGE_SUBX + XC, TABPAGE_SUBY + 2*YC, LISTBOX_SUBW, LISTBOX_SUBH * 2.0}, ListBox, LISTBOXID_ExportCalc_PJParamList, ON, 0, "", ""},
	}
};

//PushButton定义
DItem_PushButtonRsc PUSHBTNID_ExportCalc_Submit = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_Submit, NOARG, NOCMD, LCMD, "", TXT_EXPORTCALC_PUSHBTN_Submit   
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_Cancel = 
{ 
	CANCEL_BUTTON, NOHELP, MHELP, 
	HOOKITEMID_Button_StandardAction, ACTIONBUTTON_CANCEL, NOCMD, MCMD, "", TXT_EXPORTCALC_PUSHBTN_Cancel  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_Import = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_Import, NOARG, NOCMD, LCMD, "", "概况导入"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddModelPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "选择"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelModelPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "删除"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddWYCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "选择"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelWYCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "删除"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddYLCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "选择"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelYLCloudPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "删除"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_AddYLPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_AddPath, NOARG, NOCMD, LCMD, "", "选择"  
};

DItem_PushButtonRsc PUSHBTNID_ExportCalc_DelYLPath = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKPUSHBTN_ExpCalc_DelPath, NOARG, NOCMD, LCMD, "", "删除"  
};

//ListBox定义
DItem_ListBoxRsc LISTBOXID_ExportCalc_ModelPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{12*XC, 12, ALIGN_LEFT, "序号"	 },
		{30*XC, 35, ALIGN_LEFT,  "模型图路径"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_WYCloudPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "序号"	 },
		{35*XC, 40, ALIGN_LEFT,  "云图或等值线图路径"	 },
    }
};	

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLCloudPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "序号"	 },
		{35*XC, 40, ALIGN_LEFT,  "云图或等值线图路径"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLPaths =
{
    NOHELP, MHELP, NOHOOK, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "序号"	 },
		{35*XC, 40, ALIGN_LEFT,  "应力数值图路径"	 },
    }
};	

DItem_ListBoxRsc LISTBOXID_ExportCalc_CaseList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_CaseList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "工况名"	 },
		{35*XC, 40, ALIGN_LEFT,  "主要载荷"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_ModelParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_ModelParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{8*XC, 10, ALIGN_LEFT, "材料号"	 },
		{10*XC, 20, ALIGN_LEFT,  "材料属性"	 },
		{8*XC, 15, ALIGN_LEFT,  "单元数"	 },
		{10*XC, 20, ALIGN_LEFT,  "弹性模量"	 },
		{8*XC, 15, ALIGN_LEFT,  "泊松比"	 },
		{10*XC, 20, ALIGN_LEFT,  "密度"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_WYParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_WYParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{7*XC, 10, ALIGN_LEFT, "工况名"	 },
		{11*XC, 20, ALIGN_LEFT,  "最大合位移"	 },
		{11*XC, 15, ALIGN_LEFT,  "最大X向位移"	 },
		{11*XC, 20, ALIGN_LEFT,  "最大Y向位移"	 },
		{11*XC, 15, ALIGN_LEFT,  "最大Z向位移"	 },
    }
};

DItem_ListBoxRsc LISTBOXID_ExportCalc_YLParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_YLParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{7*XC, 10, ALIGN_LEFT, "工况名"	 },
		{9*XC, 20, ALIGN_LEFT,  "X向拉应力"	 },
		{9*XC, 15, ALIGN_LEFT,  "Y向拉应力"	 },
		{9*XC, 20, ALIGN_LEFT,  "Z向拉应力"	 },
		{9*XC, 15, ALIGN_LEFT,  "第一主应力" },
		{9*XC, 15, ALIGN_LEFT,  "第三主应力" },
    }
};
	
DItem_ListBoxRsc LISTBOXID_ExportCalc_PJParamList =
{
    NOHELP, MHELP, HOOKLISTBOX_ExpCalc_PJParamList, NOARG,
    LISTATTR_DYNAMICSCROLL | LISTATTR_SELBROWSE | LISTATTR_HORIZSCROLLBAR | 
    LISTATTR_GRID | LISTATTR_NOSELECTION | LISTATTR_RESIZABLECOLUMNS |
    LISTATTR_NEWCOLHEADORIGIN | LISTATTR_NOTRAVERSAL | LISTATTR_DRAWPREFIXICON ,
    5, 0, "",
    {
		{10*XC, 20, ALIGN_LEFT,	"工况名"	 },
		{10*XC, 20, ALIGN_LEFT,	"截面名"	 },
		{10*XC, 20, ALIGN_LEFT,	"配筋线名"},
		{8*XC,  10, ALIGN_LEFT, "结构系数"},
		{12*XC, 10, ALIGN_LEFT, "结构重要性系数"},
		{10*XC, 10, ALIGN_LEFT, "设计状况系数"},
		{10*XC, 10, ALIGN_LEFT, "钢筋抗拉强度"},
		{12*XC, 10, ALIGN_LEFT, "混凝土抗拉强度"},
		{16*XC, 20, ALIGN_LEFT,	"总拉应力面积(mMPa)"},
		{16*XC, 20, ALIGN_LEFT,	"混凝土拉应力面积(mMPa)"},
		{16*XC, 20, ALIGN_LEFT,	"计算配筋面积(mMPa)"	 },
		{12*XC, 20, ALIGN_LEFT, "参考配筋"},
		{8*XC,  20, ALIGN_LEFT,	"弯矩(N)" },
		{8*XC,  20, ALIGN_LEFT,	"轴力(N)"},
		{12*XC, 20, ALIGN_LEFT,	"XY方向剪力(N)"},
		{12*XC, 20, ALIGN_LEFT,	"YZ方向剪力(N)"},
		{12*XC, 20, ALIGN_LEFT,	"XZ方向剪力(N)"}
    }
};


//Text定义
DItem_TextRsc TEXTID_ExportCalc_CellCount = 
{ 
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, NOARG, 
	20, "%d", "%d", "", "", NOMASK, TEXT_READONLY , "总单元数：", 
	"g_expCalculationInfo.nCellCnt" 
};

DItem_TextRsc TEXTID_ExportCalc_NodeCount = 
{ 
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, NOARG, 
	20, "%d", "%d", "", "", NOMASK, TEXT_READONLY , "节点数：", 
	"g_expCalculationInfo.nNodeCnt" 
};

//MLText定义
DItem_MultilineTextRsc MLTEXTID_ExportCalc_ProjectDescr= 
{ 
	NOSYNONYM, NOHELP, MHELP, HOOKMLTEXT_ExpCalc_ProDescr, 1, 
	MLTEXTATTR_SBARINWIDTH , 10, "" 
}; 

//ComboBox定义
DItem_ComboBoxRsc COMBOBOXID_ExportCalc_InstanceLists= 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_ExpCalc_ProjectLists, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, "项目名称", "g_expCalculationInfo.proIndex", 
	{ {20*XC, 20, 0, ""}, } 
};

#undef DIALOG_W	
#undef	DIALOG_H	

#undef PUSHBTN_H	
#undef PUSHBTN_W	

#undef TABPAGE_X	
#undef	TABPAGE_Y	
#undef	TABPAGE_W	
#undef	TABPAGE_H	
#undef TABPAGE_SUBX
#undef	TABPAGE_SUBY
#undef TABPAGE_SUBW
#undef TABPAGE_SUBH

#undef	LISTBOX_W	
#undef	LISTBOX_H	
#undef LISTBOX_SUBW
#undef LISTBOX_SUBH