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
#define	TXT_CASECOMPARE_DIALOG	"工况对比"

#define TXT_CASECOMPARE_COMBOBOX_InstanceL	"项目1"
#define TXT_CASECOMPARE_COMBOBOX_InstanceR	"项目2"
#define TXT_CASECOMPARE_COMBOBOX_ResultType	"结果类型"
#define TXT_CASECOMPARE_COMBOBOX_ShowType	"显示类型"

#define TXT_CASECOMPARE_TREEL	"工况列表1"
#define TXT_CASECOMPARE_TREER	"工况列表2"

#define TXT_CASECOMPARE_PUSHBTN_Setting	"设置"
#define TXT_CASECOMPARE_PUSHBTN_Submit	"对比"

//对话框相关尺寸定义
#define DIALOG_W	50*XC
#define	DIALOG_H	14*YC

#define PUSHBTN_H	2*YC
#define PUSHBTN_W	10*XC

//对话框定义
DialogBoxRsc DIALOGID_CaseCompare =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_ALWAYSSETSTATE | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	DIALOG_W, DIALOG_H,
	NOHELP, MHELP,
	HOOKDIALOG_CaseCompare, NOPARENTID,
	TXT_CASECOMPARE_DIALOG,
	{
		//左侧控件
		{{6*XC, 0.8*YC, 14*XC, 0}, ComboBox,   COMBOBOXID_CaseComp_InstanceL,  ON,0, "", ""},
		{{19.8*XC, 0.6*YC, 5*XC, 1.4*YC}, PushButton, PUSHBTNID_CaseComp_ColorSettingL, ON, ALIGN_CENTER, "", ""},
		{{XC, 2*YC, DIALOG_W/2 - 1.5 * XC, 7*YC }, Tree, TREEID_CaseComp_CaseListL, ON, 0, "", "" },

		//右侧控件
		{{5.5*XC + DIALOG_W/2, 0.8*YC, 14*XC, 0}, ComboBox,   COMBOBOXID_CaseComp_InstanceR,  ON,0, "", ""},
		{{19.3*XC + DIALOG_W/2, 0.6*YC, 5*XC, 1.4*YC}, PushButton, PUSHBTNID_CaseComp_ColorSettingR, ON, ALIGN_CENTER, "", ""},
		{{0.5*XC + DIALOG_W/2	, 2*YC, DIALOG_W/2 - 1.5 * XC, 7*YC }, Tree, TREEID_CaseComp_CaseListR, ON, 0, "", "" },

		//下方控件
		//COMBOBOX-结果类型
		{{8.2*XC, 9.5*YC,  16.5*XC, 0}, ComboBox,   COMBOBOXID_CaseComp_ResultTypes,  ON,0, "", ""}, 

		//COMBOBOX-显示类型
		{{32.5*XC, 9.5*YC,  10*XC, 0}, ComboBox,   COMBOBOXID_CaseComp_ShowTypes,  ON,0, "", ""},

		//COMBOBOX-显示模式
		{{8.2*XC, 11.0*YC,  10*XC, 0}, ComboBox,   COMBOBOXID_CaseComp_ShowModel,  ON,0, "", ""},
		
		//PUSHBTN-对比
		{{DIALOG_W - PUSHBTN_W*3/2, DIALOG_H - PUSHBTN_H, PUSHBTN_W, 0}, PushButton, PUSHBTNID_CaseComp_Submit, ON, ALIGN_CENTER, "", ""},

	}
};

//ComboBox定义
DItem_ComboBoxRsc COMBOBOXID_CaseComp_InstanceL = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CaseComp_InstanceL, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_CASECOMPARE_COMBOBOX_InstanceL, 
	"g_caseCompDlgInfo.indexInstanL", 
	{ {20*XC, 20, 0, ""}, } 
};

DItem_ComboBoxRsc COMBOBOXID_CaseComp_InstanceR = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CaseComp_InstanceR, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_CASECOMPARE_COMBOBOX_InstanceR,
	"g_caseCompDlgInfo.indexInstanR", 
	{ {20*XC, 20, 0, ""}, } 
};

DItem_ComboBoxRsc COMBOBOXID_CaseComp_ResultTypes = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CaseComp_ResultType, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_CASECOMPARE_COMBOBOX_ResultType, 
	"g_caseCompDlgInfo.indexReType", 
	{ 
		{20*XC, 20, 0, ""}, 
	} 
};

DItem_ComboBoxRsc COMBOBOXID_CaseComp_ShowTypes = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CaseComp_ShowType,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowType, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_CASECOMPARE_COMBOBOX_ShowType,
	"g_caseCompDlgInfo.indexShowType",
	{
		{20*XC, 20, 0, ""}, 
	}
};

DItem_ComboBoxRsc COMBOBOXID_CaseComp_ShowModel = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_CaseComp_ShowModel,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowModel, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_ComboBox3,
	"g_caseCompDlgInfo.ShowModel",
	{
		{20*XC, 20, 0, ""}, 
	}
};

DItem_PushButtonRsc PUSHBTNID_CaseComp_ColorSettingL = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_CaseComp_SettingL, NOARG, NOCMD, LCMD, "", TXT_CASECOMPARE_PUSHBTN_Setting  
};

DItem_PushButtonRsc PUSHBTNID_CaseComp_ColorSettingR = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_CaseComp_SettingR, NOARG, NOCMD, LCMD, "", TXT_CASECOMPARE_PUSHBTN_Setting  
};

DItem_PushButtonRsc PUSHBTNID_CaseComp_Submit = 
{ 
	DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_CaseComp_Compare, NOARG, NOCMD, LCMD, "", TXT_CASECOMPARE_PUSHBTN_Submit  
};

DItem_TreeRsc TREEID_CaseComp_CaseListL =
{
	NOHELP, MHELP,
	HOOKTREE_CaseComp_CaseList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED | 
    TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS | TREEATTR_MULTISELECTION ,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_CASECOMPARE_TREEL },
	}
};

DItem_TreeRsc TREEID_CaseComp_CaseListR =
{
	NOHELP, MHELP,
	HOOKTREE_CaseComp_CaseList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED | 
    TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS | TREEATTR_MULTISELECTION ,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_CASECOMPARE_TREER },
	}
};

#undef DIALOG_W
#undef DIALOG_H

#undef PUSHBTN_H
#undef PUSHBTN_W