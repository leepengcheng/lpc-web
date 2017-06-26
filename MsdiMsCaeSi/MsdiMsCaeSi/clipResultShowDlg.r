/*--------------------------------------------------------------------------------------+
|   clipResultShowDlg.r
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
|项目管理对话框                 |
-----------------------------------------------------------------------*/
#define XSIZE       (70*XC)
#define YSIZE       (20*YC)

#define TREE_W		(18*XC)
#define LABEL_W		(12*XC)
#define TEXT_W		(6*XC)
#define GAP_X		(XC)
#define GAP_Y		(0.5*YC)

#define TABPAGE_X	(GAP_X)

#define TXT_CLIPRESULT_DIALOG	"数值输出"
#define TXT_CLIPRESULT_TREE		"截面列表"
#define	TXT_CLIPRESULT_TABPAGE1	"笛卡尔坐标系"
#define	TXT_CLIPRESULT_TABPAGE2	"柱坐标系"
#define TXT_CLIPRESULT_PUSHBTN_Clear "清除"
#define TXT_CLIPRESULT_PUSHBTN_Submit "预览"
#define TXT_CLIPRESULT_PUSHBTN_OutDGN "保存"

/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_ClipResultShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_GROWABLE | DIALOGATTR_MOTIONTOITEMS | DIALOGATTR_AUTOUNLOADAPP | DIALOGATTRX_NETDOCKABLE,
	XSIZE, YSIZE,
	NOHELP, MHELP,
	HOOKDIALOG_ClipResult, NOPARENTID,
	TXT_CLIPRESULT_DIALOG,
	{
		//截面列表树
		{ { 0, 0, 0, 0 }, Tree, TREEID_ClipResult_ClipList, ON, 0, "", "" },

		{ { TREE_W, 0, 0, 0 }, Sash, SASHID_ClipResult_V, ON, 0, "", "" },

		{ { 0, 0, 0, 0 }, ContainerPanel, CTPANELID_ClipResult_Detail, ON, 0, "", "" },	
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
DItem_SashRsc SASHID_ClipResult_V =
{
	NOHELP, MHELP,
	HOOKSASH_ClipResult_V, 0,
	3 * XC, 3 * XC,
	SASHATTR_VERTICAL | SASHATTR_ALLGRAB | SASHATTR_SOLIDTRACK |
	SASHATTR_WIDE | SASHATTR_SAVEPOSITION | SASHATTR_NOENDS
};

/*----------------------------------------------------------------------+
|                                                                       |
|   ContainerPanel Item Resources                                       |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ContainerPanelRsc CTPANELID_ClipResult_Detail =
{
	NOHELP, MHELP, NOHOOK, NOARG,	0,
	CONTAINERID_ClipResult_Setting, 0,
	""
};

DItem_ContainerRsc CONTAINERID_ClipResult_Setting =
{
	NOCMD, LCMD, NOHELP, MHELP, NOHOOK, NOARG, 0,
	DILISTID_ClipResult_Setting
};

/*----------------------------------------------------------------------+
|                                                                       |
|   DialogItemList Resources                                            |
|                                                                       |
+----------------------------------------------------------------------*/

DialogItemListRsc DILISTID_ClipResult_Setting =
{
	{
		//当前选中工况显示
		{ { LABEL_W, D_ROW(1.0), 0 , 0 }, Text, TEXTID_ClipResult_Current, ON, 0, "", "" },

		//数值图标题
		{ { LABEL_W, D_ROW(2.0), 2*LABEL_W + 3*TEXT_W, 0 }, Text, TEXTID_ClipResult_TableTitle, ON, 0, "", "" },

		//标题行字号 
		{ { LABEL_W, D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_TitleFont, ON, 0, "", "" },
		//工况行字号
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_ContentFont, ON, 0, "", "" },

		//X坐标间隔数&Y坐标间隔数&坐标字体大小
		{ { LABEL_W, D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_XSpaceCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_YSpaceCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(4.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_CoorFont, ON, 0, "", "" },

		//X数值个数&Y数值个数&数值字体大小
		{ { LABEL_W, D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_XValueCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W), D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_YValueCnt, ON, 0, "", "" },
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(5.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_ValueFont, ON, 0, "", "" },

		//小数点位数
		{ { LABEL_W + (LABEL_W+TEXT_W)*2, D_ROW(3.0), TEXT_W , 0 }, Text, TEXTID_ClipResult_Decimal, ON, 0, "", "" },

		//笛卡尔积&柱坐标
		{ { TABPAGE_X, D_ROW(6.0), 3*(LABEL_W + TEXT_W) - GAP_X, 10*YC}, TabPageList, TPLISTID_ClipResult_Main, ON, 0, "", ""},

		//清除
		{ { LABEL_W*1/4 +(LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_Clear, ON, ALIGN_CENTER, "", ""},	

		//预览
		{ { LABEL_W*101/90 + (LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_Submit, ON, ALIGN_CENTER, "", ""},	
		
		//保存
		{ { LABEL_W*2 + (LABEL_W+TEXT_W), 18*YC, BUTTON_STDWIDTH, 0}, PushButton, PUSHBTNID_ClipResult_OutDGN, ON, ALIGN_CENTER, "", ""},	
	}
}

/*----------------------------------------------------------------------+
|									|
|   Tree Resources							|
|									|
+----------------------------------------------------------------------*/
DItem_TreeRsc TREEID_ClipResult_ClipList =
{
	NOHELP, MHELP,
	HOOKTREE_ClipResult_ClipList, NOARG,
	TREEATTR_DYNAMICSCROLL | TREEATTR_LINESDIM | TREEATTR_LINESDOTTED |
	TREEATTR_NOROOTHANDLE | TREEATTR_NOSHOWROOT | TREEATTR_DOUBLECLICKEXPANDS |
	TREEATTR_EDITABLE | TREEATTR_FOCUSOUTLOOK | TREEATTR_DRAGSELECTION,
	6, 0, "",
	{
		{ 0, 256 * XC, 256, 0, TXT_CLIPRESULT_TREE },
	}
};

//当前选择
DItem_TextRsc TEXTID_ClipResult_Current =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, READONLY,
	"当前选择",
	"g_clipResultDlgInfo.current"
};

//数值图标题
DItem_TextRsc TEXTID_ClipResult_TableTitle =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	256, "", "", "", "", NOMASK, NOCONCAT,
	"数值图标题",
	"g_clipResultDlgInfo.tableTitle"
};

//标题行字号
DItem_TextRsc TEXTID_ClipResult_TitleFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"标题行字号",
	"g_clipResultDlgInfo.titleFont"
};

//工况行字号
DItem_TextRsc TEXTID_ClipResult_ContentFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"工况行字号",
	"g_clipResultDlgInfo.contentFont"
};

//高度系数
DItem_TextRsc TEXTID_ClipResult_HeightCoe =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"高度系数",
	"g_clipResultDlgInfo.heightCoe"
};

//X向标尺段数
DItem_TextRsc TEXTID_ClipResult_XSpaceCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"X向标尺段数",
	"g_clipResultDlgInfo.XSpaceCnt"
};

//Y向标尺段数
DItem_TextRsc TEXTID_ClipResult_YSpaceCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"Y向标尺段数",
	"g_clipResultDlgInfo.YSpaceCnt"
};

//标尺字体大小
DItem_TextRsc TEXTID_ClipResult_CoorFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"标尺字号",
	"g_clipResultDlgInfo.coorFont"
};

//X向数值个数
DItem_TextRsc TEXTID_ClipResult_XValueCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"X向间矩(mm)",
	"g_clipResultDlgInfo.XValueCnt"
};

//Y向数值个数
DItem_TextRsc TEXTID_ClipResult_YValueCnt =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"Y向间矩(mm)",
	"g_clipResultDlgInfo.YValueCnt"
};
//小数点位数
DItem_TextRsc TEXTID_ClipResult_Decimal =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%d", "%d", "", "", NOMASK, NOCONCAT,
	"小数点位数",
	"g_clipResultDlgInfo.decimal"
};

//数值字体大小
DItem_TextRsc TEXTID_ClipResult_ValueFont =
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP,
	NOHOOK, NOARG,
	8, "%.2f", "%f", "", "", NOMASK, NOCONCAT,
	"数值字号",
	"g_clipResultDlgInfo.valueFont"
};

//TabPage列表定义
DItem_TabPageListRsc  TPLISTID_ClipResult_Main =
{
    0, 0,
    NOSYNONYM, 
    NOHELP, MHELP, NOHOOK, NOARG,
    TABATTR_DEFAULT,
    "",
	{
		{{0,0,0,0}, TabPage, TABPAGEID_ClipResult_Cartesian,		ON, 0,"",""},		//笛卡尔
		{{0,0,0,0}, TabPage, TABPAGEID_ClipResult_Cylind,		ON, 0,"",""},			//柱坐标	
	}								
};	

DItem_TabPageRsc TABPAGEID_ClipResult_Cartesian =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKTABPAGE_ClipResult_ResultType, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_CLIPRESULT_TABPAGE1,
	{	
		{ { TABPAGE_X + GAP_X, D_ROW(7.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_SX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_SY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_SZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(8.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_EPTOXY, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_EPTOYZ, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_EPTOXZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(9.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_UX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_UY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_UZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(10.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_D_SC, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(10.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_D_ALL, ON, 0, "", ""},	
	}
};

DItem_TabPageRsc TABPAGEID_ClipResult_Cylind =
{
    NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, 
    HOOKTABPAGE_ClipResult_ResultType, NOARG,
    TABATTR_DEFAULT,
    NOTYPE, NOICON,
	TXT_CLIPRESULT_TABPAGE2,
	{	
		{ { TABPAGE_X + GAP_X, D_ROW(7.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_SR, ON, 0, "", ""},// 径向	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SH, ON, 0, "", ""},// 环向
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(7.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SA, ON, 0, "", ""},// 轴向

		{ { TABPAGE_X + GAP_X, D_ROW(8.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_SRA, ON, 0, "", ""},	// 径轴向	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SAH, ON, 0, "", ""},// 环轴向
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(8.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SRH, ON, 0, "", ""},// 径环向	

		{ { TABPAGE_X + GAP_X, D_ROW(9.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_UX, ON, 0, "", ""},	
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_UY, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W)*2, D_ROW(9.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_UZ, ON, 0, "", ""},

		{ { TABPAGE_X + GAP_X, D_ROW(10.5), 0 , 0 }, ToggleButton, TOGGLEID_ClipResult_Z_SC, ON, 0, "", ""},
		{ { TABPAGE_X + GAP_X + (LABEL_W+TEXT_W), D_ROW(10.5), 0, 0}, ToggleButton, TOGGLEID_ClipResult_Z_ALL, ON, 0, "", ""},	
	}
};

//笛卡尔积- X向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"X向正应力",
	"g_clipResultDlgInfo.D_SX"
};

//笛卡尔积- Y向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Y向正应力",
	"g_clipResultDlgInfo.D_SY"
};

//笛卡尔积- Z向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Z向正应力",
	"g_clipResultDlgInfo.D_SZ"
};

//笛卡尔积- 次主应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_SC = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"次主应力",
	"g_clipResultDlgInfo.D_SC"
};

//笛卡尔积- XY向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOXY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"XY向剪应力",
	"g_clipResultDlgInfo.D_SXY"
};

//笛卡尔积- YZ向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOYZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"YZ向剪应力",
	"g_clipResultDlgInfo.D_SYZ"
};

//笛卡尔积- XZ向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_EPTOXZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"XZ向剪应力",
	"g_clipResultDlgInfo.D_SXZ"
};

//笛卡尔积- X向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"X向位移",
	"g_clipResultDlgInfo.D_UX"
};

//笛卡尔积- Y向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Y向位移",
	"g_clipResultDlgInfo.D_UY"
};

//笛卡尔积- Z向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_UZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"Z向位移",
	"g_clipResultDlgInfo.D_UZ"
};

//笛卡尔积- 全部选择
DItem_ToggleButtonRsc TOGGLEID_ClipResult_D_ALL = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKOPTBUTTON_ClipResult_D_ALL, 
	NOARG, NOMASK, NOINVERT,	
	"全部选择",
	"g_clipResultDlgInfo.D_All"
};

//柱坐标- 径向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SR = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"径向正应力",
	"g_clipResultDlgInfo.Z_SR"
};

//柱坐标- 环向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"环向正应力",
	"g_clipResultDlgInfo.Z_SH"
};

//柱坐标- 轴向正应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SA = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"轴向正应力",
	"g_clipResultDlgInfo.Z_SA"
};

//柱坐标- 径轴向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SRA = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"径轴向正应力",
	"g_clipResultDlgInfo.Z_SRA"
};

//柱坐标- 轴环向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SAH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"环轴向正应力",
	"g_clipResultDlgInfo.Z_SAH"
};
//柱坐标- 径环向剪应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SRH = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"径环向正应力",
	"g_clipResultDlgInfo.Z_SRH"
};

//柱坐标- 径向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UX = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"径向位移",
	"g_clipResultDlgInfo.Z_UX"
};

//柱坐标- 环向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UY = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"环向位移",
	"g_clipResultDlgInfo.Z_UY"
};

//柱坐标- 轴向位移
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_UZ = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"轴向位移",
	"g_clipResultDlgInfo.Z_UZ"
};

//柱坐标- 次主应力
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_SC = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, NOHOOK, 
	NOARG, NOMASK, NOINVERT,	
	"次主应力",
	"g_clipResultDlgInfo.Z_SC"
};

//柱坐标- 全部选择
DItem_ToggleButtonRsc TOGGLEID_ClipResult_Z_ALL = 
{
	NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKOPTBUTTON_ClipResult_Z_ALL, 
	NOARG, NOMASK, NOINVERT,	
	"全部选择",
	"g_clipResultDlgInfo.Z_All"
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_Clear = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_Clear, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_Clear 
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_Submit = 
{ 
	DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_Submit, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_Submit  
};
DItem_PushButtonRsc PUSHBTNID_ClipResult_OutDGN = 
{ 
	NOT_DEFAULT_BUTTON, NOHELP, MHELP, 
	HOOKBUTTON_ClipResult_OutDGN, NOARG, NOCMD, LCMD, "", TXT_CLIPRESULT_PUSHBTN_OutDGN  
};

#undef XSIZE
#undef YSIZE
#undef TREE_W
#undef LABEL_W
#undef TEXT_W
#undef GAP_X
#undef GAP_Y
#undef TABPAGE_X