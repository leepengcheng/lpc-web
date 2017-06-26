/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSi.r
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
#define DLLAPP_MsdiMsCaeSi  1

DllMdlApp DLLAPP_MsdiMsCaeSi =
{
	L"MsdiMsCaeSi", L"MsdiMsCaeSi" // taskid, dllName
}

MessageList STRINGLISTID_XMTMessages =
	{
		{
			{ MESSAGE_ReadXMTDialogTitle, "Import Parasolid XMT File" },
			{ MESSAGE_WriteXMTDialogTitle, "导出Parasolid文件" },
			{ MESSAGE_InvalidBodySize, "Body is larger than the maximum permissible size in Parasolid.  This may create problems later on." },
			{ MESSAGE_EnterDataPoint, "Enter datapoint or reset to end" },
			{ ERROR_UnableToSave, "Unable to write XMT file" },
		}
	};

/*----------------------------------------------------------------------+
|									|
|   Icon bitmaps							|
|									|
+----------------------------------------------------------------------*/
IconRsc ICONID_Model =
{
	16, 16, ICONFORMAT_WINDOWSICO, BLACK_INDEX, "", "model"
};

IconRsc ICONID_Result =
{
	16, 16, ICONFORMAT_WINDOWSICO, BLACK_INDEX, "", "result"
};

IconRsc ICONID_Section =
{
	16, 16, ICONFORMAT_WINDOWSICO, BLACK_INDEX, "", "section"
};

IconRsc ICONID_RebarLine =
{
	16, 16, ICONFORMAT_WINDOWSICO, BLACK_INDEX, "", "rebarline"
};