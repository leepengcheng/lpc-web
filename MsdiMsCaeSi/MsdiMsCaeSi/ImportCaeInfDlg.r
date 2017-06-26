/*--------------------------------------------------------------------------------------+
|   ImportCaeInfDlg.r
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
| Required resource for a native-code-only application.                 |
-----------------------------------------------------------------------*/
#define  W1			(30*XC)						/* 网格模型 text #: width */
#define  W2			(5*XC)						/* 选择 buttom #: width */
#define  W3			(43.5*XC)					/* 选择 buttom #: width */
#define  ListboxBW  (36*XC)
#define  ToggleBW   (20*XC)

#define  H1		 10.0*YC

#define  X1		(8*XC)
#define  X2		(X1 + 12*XC)
#define  X3		(X1 + W1 + 0.5*XC)
#define  X4		(X1 - 7*XC)
#define  X5		(X1 - 6*XC)
#define  X6		(X5 + ListboxBW + 5*XC)
#define  X7		(X2 + W2 + 5*XC)

#define  Y1     GENY(1.0)				/* 网格模型 ToggleButton position */
#define  Y2     Y1 + GENY(2.0)          /* 网格模型 Text/ComboBox position */
#define  Y3     Y2 - GENY(0.8)          /* 选择 PushButton position */
#define  Y4     Y2 + GENY(2.3)          /* CAE计算结果表 GroupBox position */
#define  Y5     Y4 + GENY(2.3)          /* 计算结果表 ListBox position */
#define  Y6     Y5 + GENY(0.5)          /* 添加 PushButton position */
#define  Y7     Y6 + GENY(2.0)          /* 删除 PushButton position */
#define  Y8		Y4 + H1 + GENY(0.9)		/* 导入 PushButton position */

#define  OVERALLWIDTH   X4 + W3 + 1.4*XC /* CAE成果导入窗体 width */
#define  OVERALLHEIGHT  Y8 + 2*YC		 /* CAE成果导入窗体 height */

DialogBoxRsc DIALOGID_ImportCaeInf =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_ALWAYSSETSTATE | DIALOGATTR_SINKABLE,
	OVERALLWIDTH, OVERALLHEIGHT,
	NOHELP, MHELP,
	HOOKDIALOGID_ImportCaeInf, NOPARENTID,
	"CAE成果导入",
	{
		{ { X1, Y1, 0, 0 }, ToggleButton, TOGGLEID_MeshModel, ON, 0, "", "" },//网格模型
		{ { X2, Y1, 0, 0 }, ToggleButton, TOGGLEID_Result, ON, 0, "", "" },//计算结果
		{ { X1, Y2, W1, 0 }, ComboBox, COMBOBOXID_ImportCaeModel, ON, 0, "", "" },//网格模型
		{ { X1, Y2, W1, 0 }, Text, TEXTID_ImportCaeModel, ON, 0, "", "" },//网格模型
		{ { X3, Y3, W2, 0 }, PushButton, BUTTONID_SelectCaeModelFile, ON, 0, "选择", "" },
		{ { X4, Y4, W3, H1 }, GroupBox, 0, ON, 0, "CAE计算结果表", "" },
		{ { X5, Y5, ListboxBW, 0 }, ListBox, LISTBOXID_CaeResultFile, ON, 0, "", "" },
		{ { X3, Y6, W2, 0 }, PushButton, BUTTONID_AddResultFile, ON, 0, "添加", "" },
		{ { X3, Y7, W2, 0 }, PushButton, BUTTONID_DelResultFile, ON, 0, "删除", "" },
		{ { X2, Y8, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_OK, ON, 0, "导入", "" },
		{ { X7, Y8, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_Cancel, ON, 0, "取消", "" },
	}
};

// 网格模型
DItem_ComboBoxRsc COMBOBOXID_ImportCaeModel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	HOOKCOMBOBOX_SelectCaeModel, NOARG,
	32, "", "", "", "", NOMASK,
	0, 5, 1, 0, 0,
	COMBOATTR_READONLY | COMBOATTR_SORT | COMBOATTR_USEMODELVALUE,
	"网格模型",
	"g_ImpDlgInfo.ProjectId",
	{
		{ 0, 32, ALIGN_LEFT, TXT_NULL },	/* set width to 0 for dynamic width capability */
	}
};

DItem_ToggleButtonRsc TOGGLEID_MeshModel =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_TBSelImpDat,
	NOARG, NOMASK, NOINVERT,
	"网格模型",
	"g_ImpDlgInfo.MeshModelVal"
};

DItem_ToggleButtonRsc TOGGLEID_Result =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_TBSelImpDat,
	NOARG, NOMASK, NOINVERT,
	"计算结果",
	"g_ImpDlgInfo.ResultVal"
};

DItem_TextRsc TEXTID_ImportCaeModel =
{
	NOCMD, MCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	180, "%s", "%s", "", "", NOMASK, NOCONCAT,
	"网格模型",
	""
};

DItem_TextRsc TEXTID_ImportCaeResult =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	180, "%s", "%s", "", "", NOMASK, NOCONCAT,
	"计算结果",
	""
};

DItem_PushButtonRsc BUTTONID_SelectCaeModelFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_SelectCaeModel, NOARG,
	NOCMD, LCMD, "",
	"SelectCaeModelFile"
};

DItem_PushButtonRsc BUTTONID_AddResultFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_AddResultFile, NOARG,
	NOCMD, LCMD, "",
	"AddResultFile"
};

DItem_PushButtonRsc BUTTONID_DelResultFile =
{
	NOT_DEFAULT_BUTTON,
	NOHELP, MHELP,
	HOOKBUTTON_DelResultFile, NOARG,
	NOCMD, LCMD, "",
	"DelResultFile"
};

// 计算结果表
DItem_ListBoxRsc LISTBOXID_CaeResultFile =
{
	NOHELP, MHELP, NOHOOK, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_GRID | LISTATTR_INDEPENDENTCOLS,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "计算结果文件" },
	}
};

#undef  W1		 	/* 网格模型 text #: width */
#undef  W2		 	/* 选择 buttom #: width */
#undef  W3		 	/* 选择 buttom #: width */
#undef  ListboxBW  
#undef  ToggleBW 
#undef  H1	 
#undef  X1	 
#undef  X2	 
#undef  X3	 
#undef  X4	 
#undef  X5	 
#undef  X6	 
#undef  X7	 
#undef  Y1    	/* 网格模型 ToggleButton position */
#undef  Y2      /* 网格模型 Text/ComboBox position */
#undef  Y3      /* 选择 PushButton position */
#undef  Y4      /* CAE计算结果表 GroupBox position */
#undef  Y5      /* 计算结果表 ListBox position */
#undef  Y6      /* 添加 PushButton position */
#undef  Y7      /* 删除 PushButton position */
#undef  Y8		/* 导入 PushButton position */

#undef  OVERALLWIDTH	/* CAE成果导入窗体 width */
#undef  OVERALLHEIGHT	/* CAE成果导入窗体 height */	

