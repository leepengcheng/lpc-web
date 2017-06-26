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
|									|
|   Export XMT Dialog Box						|
|									|
+----------------------------------------------------------------------*/
#undef	    XC
#define	    XC		(DCOORD_RESOLUTION/2)

#define COL_GAP     (2*XC)								/* gap between columns */
#define LST_W	    (16*XC)								/* list box width      */
#define BUT_W	    (BUTTON_STDWIDTH + 2*XC)			/* button width	       */
#define TXT_W	    (18*XC)								/* text field width    */
#define SBR_W       (3*XC)								/* scroll bar width    */
#define ADJ			(0.35*XC)							/* list x adjustment   */
#define TYX_W	    (12*XC)								/* type extra width    */
#define TYO_W       (16*XC)								/* type option width   */

#define COL1	    (COL_GAP)							/* column 1 x  */
#define COL2	    (COL1 + LST_W + COL_GAP + SBR_W)    /* column 2 x  */
#define COL3	    (COL2 + LST_W + 10*XC + SBR_W + XC) /* column 3 x  */

#define DLG_W	    (COL3 + BUT_W + 2*XC)				/* dialog width	       */
#define DRO_X	    (COL2 + TYX_W + 2*XC)				/* drives option x     */
#define DRO_W       (12*XC)								/* drives option width */
#define DIR_W       (TXT_W + 25*XC)						/* dir label width     */
#define DRCX_W      (TXT_W + 9*XC)						/* drives ComboBox width */

#define LBL_Y       (3.0*YC)							/* label y	       */
#define TXT_Y       (3.5*YC)							/* text y	       */
#define	LST_Y       (TXT_Y + GENY(2))					/* list box y          */
#define OPT_Y       (LST_Y + GENY(10.5)) 				/* option button y     */
#define BY1			(TXT_Y+2.5*YC)								/* button 1 y origin   */
#define BY2			(BY1 + 2.5*YC)						/* button 2	       */
#define BY3			(BY2 + 2.5*YC)						/* button 3	       */
#define TOG_Y       (OPT_Y + GENY(2))       			/* read-only toggle    */
#define OFF_Y       (2*DLG_H)							/* move off the dialog */

#define DLG_H	    (OPT_Y + GENY(2))

DialogBoxRsc DIALOGID_ExportXMT =
{
	DIALOGATTR_MODAL | DIALOGATTR_ALWAYSSETSTATE,
	DLG_W, DLG_H,
	NOHELP, MHELPTOPIC,
	NOHOOK, NOPARENTID,
	TXT_ParasolidExportTitle,
	{
		{ { 13 * XC, 0.7 * YC, 0, 0 }, OptionButton, OPTIONBUTTONID_ExportXMTVersion, ON, 0, "", "" },//导出版本
		{ { 34 * XC, 0.7 * YC, 0, 0 }, OptionButton, COMBOBOXID_XMTFileUnits, ON, 0, "", "" },//导出单位
		{ { COL2 + 7 * XC, TXT_Y, TXT_W + 5 * XC, 0 }, Text, TEXTID_FileOpenFileName, ON, TEXT_MASK_LABELABOVE, TXT_FilesLabel, "" },//文件名
		{ { 0, OFF_Y, TXT_W, 0 }, Text, TEXTID_FileOpenFilter, ON, 0, "\0", "" },
		{ { COL1 - ADJ, LST_Y, 0, 0 }, ListBox, LISTID_FileOpenExtDirs, ON, 0, "\0", "" },
		{ { COL2 + 7 * XC, LST_Y, 0 + 23 * XC, 0 }, ListBox, LISTID_FileOpenExtFiles, ON, 0, "\0", "" },
		{ { COL3 + XC, BY2, BUT_W, 0 }, PushButton, PUSHBUTTONID_Cancel, ON, 0, "取消", "" },
		{ { COL3 + XC, BY1, BUT_W, 0 }, PushButton, PUSHBUTTONID_OK, ON, 0, "确定", "" },
		{ { COL1, OPT_Y+20*YC, TYO_W, 0 }, OptionButton, OPTIONBUTTONID_CreateFromSeedFilters, ON, 0, "导出类型:", "" },
		{ { COL1, TXT_Y, DRCX_W, 0 }, ComboBox, COMBOBOXID_FileListDrivesEx, ON, 0, "盘符:", "" },
		{ { COL1, OPT_Y , 0, 0 }, Label, 0, ON, ALIGN_LEFT, TXT_DirectoryLabel2, "" },//路径
		{ { COL1 + 3 * XC, OPT_Y, DIR_W, 0 }, Text, TEXTID_FileOpenDirLabel, ON, 0, "", "" },//路径
	}
};


/*----------------------------------------------------------------------+
|                                                                       |
|   OptionButton Items                                                  |
|                                                                       |
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_ExportXMTVersion =
{
	NOSYNONYM, NOHELP, MHELP, NOHOOK, OPTNBTNATTR_NEWSTYLE | NOARG,
	TXT_XMTVersionLabel,
	"*xmtVersionP",
	{
		{ NOTYPE, NOICON, NOCMD, MCMD, 0, NOMASK, ON, "19.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 180, NOMASK, ON, "18.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 160, NOMASK, ON, "16.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 150, NOMASK, ON, "15.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 141, NOMASK, ON, "14.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 140, NOMASK, ON, "14.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 131, NOMASK, ON, "13.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 130, NOMASK, ON, "13.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 121, NOMASK, ON, "12.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 120, NOMASK, ON, "12.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 111, NOMASK, ON, "11.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 110, NOMASK, ON, "11.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 101, NOMASK, ON, "10.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 100, NOMASK, ON, "10.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 91, NOMASK, ON, "9.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 90, NOMASK, ON, "9.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 80, NOMASK, ON, "8.0" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 71, NOMASK, ON, "7.1" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 70, NOMASK, ON, "7.0" },
	}
};

/*----------------------------------------------------------------------+
|            								|
|	*** Combobox item section					|
|									|
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc COMBOBOXID_XMTFileUnits =
{
	NOSYNONYM, NOHELP, MHELP, NOHOOK, OPTNBTNATTR_NEWSTYLE | NOARG,
	TXT_XMTFileUnits,
	"*masterUnitsP",
	{
		{ NOTYPE, NOICON, NOCMD, MCMD, 0, NOMASK, ON, "Meters" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 1, NOMASK, ON, "kilometers" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 2, NOMASK, ON, "Centimeters" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 3, NOMASK, ON, "Milimeters" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 4, NOMASK, ON, "Micrometers" },

		{ NOTYPE, NOICON, NOCMD, MCMD, 5, NOMASK, ON, "Miles" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 6, NOMASK, ON, "Yards" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 7, NOMASK, ON, "Feet" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 8, NOMASK, ON, "Inches" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 9, NOMASK, ON, "Mils" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 10, NOMASK, ON, "MicroInches" },

		{ NOTYPE, NOICON, NOCMD, MCMD, 11, NOMASK, ON, "US Survey Miles" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 12, NOMASK, ON, "US Survey Feet" },
		{ NOTYPE, NOICON, NOCMD, MCMD, 13, NOMASK, ON, "US Survey Inches" },
	}
};