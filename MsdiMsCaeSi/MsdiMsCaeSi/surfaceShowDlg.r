/*--------------------------------------------------------------------------------------+
|   surfaceShowDlg.r
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
#include "MsdiMsCaeSicmd.h"

/*----------------------------------------------------------------------+
|表面查看对话框															|
-----------------------------------------------------------------------*/
#define XSIZE       (49*XC)
#define YSIZE       (14*YC)

#define TREE_X	    (0.5*XC)
#define TREE_Y	    (1.5*YC)
#define TREE_H	    (YSIZE - TREE_Y - BUTTON_LARGEHEIGHT)
#define TREE_W	    (20*XC)
#define CBOX_W	    (13*XC)	
#define	TEXT_W		(9*XC)

#define SHOWBTX		(TEXT_W/2)

#define TXT_SURFACE_DIALOG	"截面管理与结果查看"
#define TXT_SURFACE_TREE	"截面列表"
#define	TXT_SURFACE_PUSHBTN_Setting	"设置"
#define	TXT_SURFACE_PUSHBTN_Submit	"结果查看"
#define	TXT_SURFACE_PUSHBTN_NodeVal	"节点查看"

/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_SurfaceShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOGID_SurfaceShow, NOPARENTID,
	TXT_SURFACE_DIALOG,
	{
		{{0,0,0,0},	PopupMenu, POPUPMENUID_Surface_ClipMenu,  HIDDEN, 0, "", ""},

		//截面列表Tree
		{ { 0, 0, 0, 0 }, Tree, TREEID_Surface_ClipList, ON, 0, "", "" },

		{ { TREE_W, 0, 0, 0 }, Sash, SASHID_Surface_V, ON, 0, "", "" },

		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_Surface_Detail, ON, 0, "", "" },
	}
}
extendedIntAttributes
{
	{
		{ EXTINTATTR_DLOGATTRS, DIALOGATTRX_NETDOCKABLE }
	}
};

/*----------------------------------------------------------------------+
|                                                                       |
|   Sash Item Resources                                                 |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_SashRsc SASHID_Surface_V =
{
	NOHELP, MHELP,
	HOOKSASH_Surface_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};

/*----------------------------------------------------------------------+
|									|
|   Tree Resources							|
|									|
+----------------------------------------------------------------------*/
DItem_TreeRsc TREEID_Surface_ClipList =
{
	NOHELP, MHELP,
	HOOKTREE_Surface_ClipList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_EDITABLE | TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_SURFACE_TREE },
	}
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ContainerPanel Item Resources                                       |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ContainerPanelRsc CTPANELID_Surface_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG,	0,
	CONTAINERID_Surface_Setting, 0,
	""
};

DItem_ContainerRsc CONTAINERID_Surface_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_Surface_Setting
};

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemList Resources                                            |
|                                                                       |
+----------------------------------------------------------------------*/

DialogItemListRsc DILISTID_Surface_Setting =
{
	{
		// 当前选中工况显示
		{ { 7.5*XC, D_ROW(0.8), 0, 0 }, Text, TEXTID_Surface_Current, ON, 0, "", "" },

		// 结果类型
		{ { 7.5*XC, D_ROW(1.8), CBOX_W, 0 }, ComboBox, COMBOBOXID_SurfaceResultType, ON, 0, "", "" },

		//显示类型
		{ { 7.5*XC, D_ROW(3.0), CBOX_W, 0 }, ComboBox, COMBOBOXID_Surface_ShowType, ON, 0, "", "" },

		// 打开色带设置按钮
		{ { 8.0*XC + CBOX_W, D_ROW(2.8), CBOX_W/3, 0 }, PushButton, PUSHBTNID_Surface_Setting, ON, 0, "", "" },
		
		// 显示模式
		{ { 7.5*XC, D_ROW(4.2), CBOX_W, 0 }, ComboBox, COMBOBOXID_Surface_Model, ON, 0, "", "" },

		// 最大/最小值标识
		{ { 7.5*XC, D_ROW(5.4), 0, 0 }, ToggleButton, TOGGLEID_Surface_ShowExtremum, ON, 0, "最大/小值标识", ""},	

		//结果查看按钮
		{ { SHOWBTX, D_ROW(7.0), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_Surface_Submit, ON, 0, "", "" },

		//节点查看按钮
		{ { SHOWBTX + BUTTON_STDWIDTH + TEXT_W/3, D_ROW(7.0), BUTTON_STDWIDTH, 0 }, PushButton, PUSHBTNID_Surface_NodeVal, ON, 0, "", "" },
	}
}

//当前选择
DItem_TextRsc TEXTID_Surface_Current =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"当前选择",
	"g_surfaceShowDlgInfo.current"
};

//结果类型
DItem_ComboBoxRsc COMBOBOXID_SurfaceResultType = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_Surface_ResultType, 
	NOARG, 10, "", "", "", "", NOMASK, 0, 10, 0, 
	0, 0, COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	"结果类型", 
	"g_surfaceShowDlgInfo.indexReType", 
	{ 
		{20*XC, 20, 0, ""}, 
	} 
};

//显示类型
DItem_ComboBoxRsc COMBOBOXID_Surface_ShowType = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowType, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_ComboBox2,
	"g_surfaceShowDlgInfo.indexShowType",
	{
		{20*XC, 20, 0, ""}, 
	}
};

//色带设置
DItem_PushButtonRsc PUSHBTNID_Surface_Setting =
{
	NOT_DEFAULT_BUTTON,	NOHELP, MHELP, HOOKBUTTON_Surface_ShowSet, NOARG,
	NOCMD, LCMD, "",
	TXT_SURFACE_PUSHBTN_Setting
};

//显示模式
DItem_ComboBoxRsc COMBOBOXID_Surface_Model = 
{ 
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_Surface_Model,
	NOARG, 10, "", "", "", "", NOMASK, 
	STRINGLISTID_ShowModel, 10, 0, 0, 0, 
	COMBOATTR_READONLY | COMBOATTR_INDEXISVALUE |COMBOATTR_DRAWPREFIXICON, 
	TXT_ComboBox3,
	"g_surfaceShowDlgInfo.indexShowModel",
	{
		{20*XC, 20, 0, ""}, 
	}
};

//最大/小值标识
//是否显示显示最大最小值标识,0代表非选中, -1代表被选中
DItem_ToggleButtonRsc TOGGLEID_Surface_ShowExtremum = 
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKCOMBOBOX_SurfaceDisp_ShowType,
	NOARG, NOMASK, NOINVERT,
	"",
	"g_surfaceShowDlgInfo.bShowExtremum"
};
//显示按钮
DItem_PushButtonRsc PUSHBTNID_Surface_Submit =
{
	DEFAULT_BUTTON,	NOHELP, MHELP, HOOKBUTTON_Surface_ResultShow, NOARG,
	NOCMD, LCMD, "",
	TXT_SURFACE_PUSHBTN_Submit
};

//节点显示
DItem_PushButtonRsc PUSHBTNID_Surface_NodeVal =
{
	NOT_DEFAULT_BUTTON,	NOHELP, MHELP, NOHOOK, NOARG,
	CMD_MSDI_MSCAESI_ACTION_NODEVIEWER, LCMD, "",
	TXT_SURFACE_PUSHBTN_NodeVal
};

// 右键菜单
DItem_PopupMenuRsc POPUPMENUID_Surface_ClipMenu = 
    {
    NOHELP, LHELP, NOHOOK, NOARG, 0,
    PulldownMenu, PDMID_Surface_ClipMenu    
    };

DItem_PulldownMenuRsc PDMID_Surface_ClipMenu = 
{
    NOHELP, OHELPTASKIDCMD,
    HOOKPULLMENU_Surface_ClipMenu,
    ON | ALIGN_LEFT, "",
	{	
		{"创建截面", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_Surface_Add, 0,	NOCMD, OTASKID, ""},
		{"删除截面", NOACCEL, ON,  NOMARK, 0, NOSUBMENU, NOHELP, OHELPTASKIDCMD,	HOOKPDM_Surface_Del, 0,	NOCMD, OTASKID, ""},
	}
};

#undef XSIZE
#undef YSIZE
#undef TREE_H	 
#undef TREE_W	 
#undef CBOX_W
#undef TEXT_W	 
#undef SHOWBTX	 
