/*--------------------------------------------------------------------------------------+
|   SolidMeshShowDlg.r
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
#define  ListboxBW  (34*XC)

#define  H1		 10.0*YC

#define  X1		(1.5*XC)
#define  X2		(X1 + 7.0*XC)
#define  X3		(X2 + 12*XC)

#define  Y1     GENY(1.0)				/* 网格模型 ToggleButton position */
#define  Y2     Y1 + GENY(2.5)          /* 网格模型 Text/ComboBox position */
#define  Y3     Y2 + GENY(6.5)          /* 选择 PushButton position */

#define  OVERALLWIDTH   X1 + ListboxBW + 1.4*XC		/* CAE成果导入窗体 width */
#define  OVERALLHEIGHT  Y3 + 2*YC		/* CAE成果导入窗体 height */

DialogBoxRsc DIALOGID_SolidAllShow =
{
	DIALOGATTR_DEFAULT | DIALOGATTR_SINKABLE,
	OVERALLWIDTH, OVERALLHEIGHT,
	NOHELP, MHELPTOPIC,
	HOOKDIALOGID_SolidAllShow, NOPARENTID,
	"网格模型展示",
	{
		{ { X1, Y1, 0, 0 }, Label, 0, ON, ALIGN_LEFT, TXT_CaeProjectShow, "" },//请在下面的列表中选择需要显示的CAE工程
		{ { X1, Y2, ListboxBW, 0 }, ListBox, LISTBOXID_CaeProject, ON, 0, "", "" },
		{ { X2, Y3, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_OK, ON, 0, "确定", "" },
		{ { X3, Y3, BUTTON_STDWIDTH, 0 }, PushButton, PUSHBUTTONID_Cancel, ON, 0, "取消", "" },
	}
};

// 计算结果表
DItem_ListBoxRsc LISTBOXID_CaeProject =
{
	NOHELP, MHELP, NOHOOK, NOARG,
	LISTATTR_DRAWPREFIXICON | LISTATTR_GRID | LISTATTR_INDEPENDENTCOLS,
	6, 0, "",
	{
		{ 8 * XC, 45, ALIGN_LEFT, "CAE网格模型" },
	}
};

#undef  ListboxBW
#undef  H1		
#undef  X1		
#undef  X2		
#undef  X3		
#undef  Y1   
#undef  Y2   
#undef  Y3   

#undef  OVERALLWIDTH	/* CAE成果导入窗体 width */
#undef  OVERALLHEIGHT	/* CAE成果导入窗体 height */

