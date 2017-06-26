#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"

#define  W1		(12*XC)						/* 网格模型 text #: width */
#define  W2		(5*XC)						/* 网格模型 text #: width */
#define  W3		(15*XC)						/* 网格模型 text #: width */
#define  W4		(5*XC)						/* 选择 buttom #: width */
#define  ListboxBW  W1 + W2 + W3 + (7*XC)
#define  H1		10.0*YC

#define  X1		(9.0*XC)
#define  X2		(X1 + W1)
#define  X3		(X2 + W2)
#define  X4		(X3 + W3)
#define  X5		(1.5*XC)
#define  X6		(X5 + 10*XC)
#define  X7		(X6 + BUTTON_STDWIDTH + 6*XC)

#define  Y1     GENY(1.2)				/* 网格模型 ToggleButton position */
#define  Y2     Y1 - GENY(0.8)          /* 网格模型 Text/ComboBox position */
#define  Y3     Y1 + GENY(2.8)            /* 网格模型 Text/ComboBox position */
#define  Y4     Y3 + GENY(0.5)            /* 网格模型 Text/ComboBox position */
#define  Y5     Y4 + GENY(2)            /* 网格模型 Text/ComboBox position */
#define  Y6     Y3 + GENY(6.7)          /* 选择 PushButton position */

#define  OVERALLWIDTH   X5 + ListboxBW + W4 + 2*XC		/* CAE成果导入窗体 width */
#define  OVERALLHEIGHT  Y6 + 2*YC		/* CAE成果导入窗体 height */

DialogBoxRsc DIALOGID_SolidPartShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_MODAL | DIALOGATTR_ALWAYSSETSTATE,
	OVERALLWIDTH, OVERALLHEIGHT,
	NOHELP, MHELPTOPIC,
	NOHOOK, NOPARENTID,
	"条件设置",
	{
		{ { X1, Y1, W1, 0 }, ComboBox, COMBOBOXID_ConditionType, ON, 0, "", "" },	//条件类型
		{ { X2, Y1, W2, 0 }, ComboBox, COMBOBOXID_Condition, ON, 0, "", "" },		//条件等式
		{ { X3, Y1, W3, 0 }, ComboBox, COMBOBOXID_ConditionVal, ON, 0, "", "" },	//条件等值
		{ { X4, Y2, W4, 0 }, PushButton, BUTTONID_ConditionAdd, ON, 0, "增加", "" },
		{ { X5, Y3, ListboxBW, 0 }, ListBox, LISTBOXID_ConditionList, ON, 0, "", "" },
		{ { X4, Y4, W4, 0 }, PushButton, BUTTONID_ConditionDel, ON, 0, "删除", "" },
		{ { X4, Y5, W4, 0 }, PushButton, BUTTONID_ConditionClr, ON, 0, "清空", "" },
		{ { X6, Y6, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_OK, ON, 0, "确定", "" },
		{ { X7, Y6, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_Cancel, ON, 0, "取消", "" },
	}
};

DItem_PushButtonRsc BUTTONID_ConditionAdd =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_PartDis_Add, NOARG,
	NOCMD, LCMD, "",
	"ConditionAdd"
};

DItem_PushButtonRsc BUTTONID_ConditionDel =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_PartDis_Del, NOARG,
	NOCMD, LCMD, "",
	"ConditionDel"
};

DItem_PushButtonRsc BUTTONID_ConditionClr =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_PartDis_Clear, NOARG,
	NOCMD, LCMD, "",
	"ConditionClr"
};

//筛选条件名
DItem_ComboBoxRsc COMBOBOXID_ConditionType =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKCOMBOBOX_PartDis_CondName, NOARG,
	200, "%I64u", "%I64u", "", "", NOMASK,
	STRINGLISTID_ConditionType, 5, 0, W1, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE,
	"条件类型",
	"g_partialDisDlgInfo.indexCName",
	{
		{ W1, 4, ALIGN_LEFT, TXT_NULL },
	}
};

StringList STRINGLISTID_ConditionType =
{
	1,
	{
		{ { 0 }, "单元类型" },
		{ { 1 }, "材料类型" },
		{ { 2 }, "实常数" },
	}
};

//筛选条件操作符
DItem_ComboBoxRsc COMBOBOXID_Condition =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	32, "", "", "", "", NOMASK,
	STRINGLISTID_Condition, 5, 0, W2, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE,
	TXT_NULL,
	"g_partialDisDlgInfo.indexCOpt",
	{
		{ W2, 2, ALIGN_LEFT, TXT_NULL },	/* set width to 0 for dynamic width capability */
	}
};

StringList STRINGLISTID_Condition =
{
	1,
	{
		{ { 0 }, "=" },
		{ { 1 }, "<>" },
	}
};

//筛选条件值
DItem_ComboBoxRsc COMBOBOXID_ConditionVal =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKCOMBOBOX_PartDis_CondVal, NOARG,
	32, "", "", "", "", NOMASK,
	0, 5, 1, W3, 0,
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE,
	TXT_NULL,
	"g_partialDisDlgInfo.indexCVal",
	{
		{ 0, 32, ALIGN_LEFT, TXT_NULL },	/* set width to 0 for dynamic width capability */
	}
};

//筛选条件列表
DItem_ListBoxRsc LISTBOXID_ConditionList =
{
	NOHELP, MHELP, HOOKLISTBOX_PartDis_Conditions, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_GRID ,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "筛选名" },
		{ 8 * XC, 45, ALIGN_LEFT, "操作符" },
		{ 8 * XC, 45, ALIGN_LEFT, "筛选值" },
	}
};

#undef  W1		
#undef  W2		
#undef  W3		
#undef  W4		
#undef  ListboxBW  
#undef  H1		

#undef  X1		
#undef  X2		
#undef  X3		
#undef  X4		
#undef  X5	
#undef  X6	
#undef  X7		

#undef  Y1   		
#undef  Y2           
#undef  Y3            
#undef  Y4           
#undef  Y5         
#undef  Y6     

#undef  OVERALLWIDTH  	
#undef  OVERALLHEIGHT  	