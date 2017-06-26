
/*----------------------------------------------------------------------+
|           Message List for MsdiMsCaeSi Application                       |
+----------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>

#include "..\MsdiMsCaeSiids.h"

MessageList STRINGLISTID_Commands =
{
	{
		{COMMAND_IMPORTCAEINFO			,"模型导出" },
		{COMMAND_SHOWCAESOLIDALL		,"模型展示" },
		{COMMAND_SHOWCAESOLIDPART		,"局部显示" },
		{COMMAND_SETCAERSTDISP			,"模型展示" },
		{COMMAND_CLEARCONTOURLINE		,"模型导出" },
		{COMMAND_CLEARCONTOURPLOT		,"模型展示" },
		{COMMAND_EXPORTTOPARASOLID		,"模型导出" },
		{COMMAND_SHOWCLIPEFACEPLOT		,"模型展示" },
		{COMMAND_SHOWCLIPEFACELINE		,"模型导出" },
		{COMMAND_SHOWCLIPEFACEPVECTOR	,"模型导出" },
		{COMMAND_CLEARCLIPEFACEPLOT		,"模型展示" },
		{COMMAND_CLEARCLIPEFACELINE		,"模型导出" },
		{COMMAND_CLEARCLIPEFACEPVECTOR	,"模型展示" },
		{COMMAND_CLIPPLOTEFACE			,"云图剖切" },
		{COMMAND_CLEARPLOTCUTFACE		,"模型展示" },
		{COMMAND_MOVECLIPPLOTFACE		,"模型导出" },
		{COMMAND_SAVEPICTURE			,"模型导出" },
		{COMMAND_CLIPEFACE				,"截面剖切" },
		{COMMAND_EXPORTCAL				,"导出计算书" },
		{COMMAND_CALCREBAR				,"配筋计算" },
		{COMMAND_FENCEDISPLAY			,"框选显示" },
		{COMMAND_RECOVERDISPLAY			,"恢复显示" },
		{COMMAND_CASECOMPARE			,"方案比选"	},			
		{COMMAND_CLIPRESULT				,"位移输出"	},
		{COMMAND_SURFACESHOW			,"表面展示"	},
		{COMMAND_OUTPUTREBAR			,"配筋输出" },
		{COMMAND_DELETEDISPLAY			,"删除模型" },		
		{COMMAND_NODEVIEWER				,"节点查看" },
		
	}
};

MessageList STRINGLISTID_Prompts =
{
	{
		{ PROMPT_SoildExport, "点击鼠标左键选择需要导出的模型/点击鼠标右键退出。" },
		{ PROMPT_SolidAllShow, "点击鼠标左键选择模型的放置位置/点击鼠标右键退出。" },
		{ PROMPT_ImportCAESolid, " " },
		{ PROMPT_SELECT_FIRST_ELEMENT, " " },
		{ PROMPT_SELECT_NEXT_ELEMENT_OR_ACCEPT, " " },
		{ PROMPT_ACCEPT, " " },

		{ MESSAGEID_ModelDataDialogTitle, "选择CAE网格数据文件" },
		{ MESSAGEID_ModelDataFilterString, "*.cdb" },
		{ MESSAGEID_ResultDataDialogTitle, "选择CAE计算结果数据文件" },
		{ MESSAGEID_ResultDataFilterString, "*.xml" },
		
		{ PROMPT_CalcRebar_ClickFirst,			"左键选择配筋线起点/右键退出工具。" },
		{ PROMPT_CalcRebar_ClickNext,			"左键选择配筋终点/右键重新选择起点。" },
		{ PROMPT_CalcRebar_ClickError,			"选择的配筋线起点或终点需要在截面范围内，请重新选择！" },

		{ PROMPT_Recover_SelectInstance,		"左键选择需要复原显示的CAE模型/右键退出工具." },
		{ PROMPT_Fence_SelectInstance,			"框选CAE模型需要展示的部分/右键退出工具." },

		{PROMPT_Surface_LockX,					"锁定X轴"},
		{PROMPT_Surface_LockY,					"锁定Y轴"},
		{PROMPT_Surface_LockZ,					"锁定Z轴"},
		{PROMPT_Surface_LockNone,				"任意轴"},

		{PROMPT_ClipResults_CentrePointStt,		"左键选择圆心定位的起点/右键退出工具"},
		{PROMPT_ClipResults_CentrePointEnd,		"左键选择圆心定位的终点/右键退出工具"},
		{PROMPT_ClipResults_ArcStt,				"左键选择圆弧的起点/右键退出工具"},
		{PROMPT_ClipResults_ArcEnd,				"左键选择圆弧的终点/右键退出工具"},
	}
};
