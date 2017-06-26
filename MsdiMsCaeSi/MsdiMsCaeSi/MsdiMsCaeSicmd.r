/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSicmd.r
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#pragma         suppressREQCmds

#include        <Mstn\MdlApi\rscdefs.r.h>
#include        <Mstn\MdlApi\cmdclass.r.h>

#include		"MsdiMsCaeSiids.h"

/*----------------------------------------------------------------------+
|                                                                       |
|   Local Defines                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#define	    CT_NONE	    0
#define	    CT_MAIN	    1
#define	    CT_SUB	    2
#define     CT_ACTION	3
#define     CT_ACTYPE   4

/*----------------------------------------------------------------------+
|                                                                       |
|   Application command syntax   					|
|                                                                       |
+----------------------------------------------------------------------*/
CommandTable CT_MAIN =
{ 
    { 1, CT_SUB, PLACEMENT, REQ,	"MSDI" }, 
};

CommandTable CT_SUB =
{ 
    { 1, CT_ACTION, PLACEMENT, REQ,	"MSCAESI", NoCmdNum, NoItemList}, 
};

CommandTable CT_ACTION =
{ 
    { 1, CT_ACTYPE,     INHERIT,	NONE,      "ACTION", NoCmdNum, NoItemList },
};

CommandTable CT_ACTYPE =
{ 
    { 1,	CT_NONE,		INHERIT,	NONE,		"IMPORTCAEINFO",			COMMAND_IMPORTCAEINFO			, NoItemList },
	{ 2,	CT_NONE,		INHERIT,	NONE,		"SHOWCAESOLIDALL",			COMMAND_SHOWCAESOLIDALL			, NoItemList },
	{ 3,	CT_NONE,		INHERIT,	NONE,		"SHOWCAESOLIDPART",			COMMAND_SHOWCAESOLIDPART		, NoItemList },
	{ 4,	CT_NONE,		INHERIT,	NONE,		"SETCAERSTDISP",			COMMAND_SETCAERSTDISP			, NoItemList },
	{ 5,	CT_NONE,		INHERIT,	NONE,		"EXPORTTOPARASOLID",		COMMAND_EXPORTTOPARASOLID		, NoItemList },
	{ 6,	CT_NONE,		INHERIT,	NONE,		"CLIPPLOTEFACE",			COMMAND_CLIPPLOTEFACE			, ITEMLISTID_ClipPlotFace},
	{ 7,	CT_NONE,		INHERIT,	NONE,		"SAVEPICTURE",				COMMAND_SAVEPICTURE				, NoItemList },
	{ 8,	CT_NONE,		INHERIT,	NONE,		"CLIPEFACE",				COMMAND_CLIPEFACE				, ITEMLISTID_ClipFace },
	{ 9,	CT_NONE,		INHERIT,	NONE,		"EXPORTCAL",				COMMAND_EXPORTCAL				, NoItemList },	
	{ 10,	CT_NONE,		INHERIT,	NONE,		"CALCREBAR",				COMMAND_CALCREBAR				, ITEMLISTID_CalcRebar },		
	{ 11,	CT_NONE,		INHERIT,	NONE,		"FENCEDISPLAY",				COMMAND_FENCEDISPLAY			, NoItemList },	
	{ 12,	CT_NONE,		INHERIT,	NONE,		"RECOVERDISPLAY",			COMMAND_RECOVERDISPLAY			, NoItemList },	
	{ 13,	CT_NONE,		INHERIT,	NONE,		"CASECOMPARE",				COMMAND_CASECOMPARE				, NoItemList },	
	{ 14,	CT_NONE,		INHERIT,	NONE,		"CLIPRESULT",				COMMAND_CLIPRESULT				, NoItemList },	
	{ 15,	CT_NONE,		INHERIT,	NONE,		"SURFACESHOW",				COMMAND_SURFACESHOW				, NoItemList },	
	{ 16,	CT_NONE,		INHERIT,	NONE,		"OUTPUTREBAR",				COMMAND_OUTPUTREBAR				, NoItemList },
	{ 17,	CT_NONE,		INHERIT,	NONE,		"SHOWCONTOUR",				COMMAND_SHOWCONTOUR				, NoItemList },	
	{ 18,	CT_NONE,		INHERIT,	NONE,		"DELETEDISPLAY",			COMMAND_DELETEDISPLAY			, NoItemList },
	{ 19,	CT_NONE,		INHERIT,	NONE,		"NODEVIEWER",				COMMAND_NODEVIEWER				, NoItemList },	
};
