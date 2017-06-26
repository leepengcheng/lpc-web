/*--------------------------------------------------------------------------------------+
|   clipPlotFaceCmdDlg.r
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\dlogids.r.h>
#include <Mstn\MicroStation.r.h>
#include <Mstn\MdlApi\dlogbox.r.h>
#include <Mstn\MdlApi\keys.r.h>
#include <Mstn\cmdlist.r.h>
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>
#include <Mstn\cmdlist.r.h>

#include "MsdiMsCaeSi.h"
#include "MsdiMsCaeSiids.h"
#include "MsdiMsCaeSitxt.h"

CmdItemListRsc ITEMLISTID_ClipPlotFace = 
{{
	
	{{2*XC, GENY(1), 11*XC, 0}, ToggleButton, TOGGLEID_ClipPlotFace_SideSelect, ON, 0, "", ""},

}};
//切云图选择保留面
DItem_ToggleButtonRsc TOGGLEID_ClipPlotFace_SideSelect = 
{
	NOCMD, MCMD, NOSYNONYM,	NOHELP, MHELP, NOHOOK, NOARG,
	NOMASK, NOINVERT,	
	"保留法向侧",
	"g_outRebarDlgInfo.clipPlotNormalSide"
};
