#pragma once

#if !defined (__MSdiMsCaeSi__)
#define __MSdiMsCaeSi__

#define RTYPE_Mcsi				'Mcsi'
#define RSCID_Mcsi				1

/************************************************************************/
/* CMDNAME资源IDs                                                       */
/************************************************************************/
enum
{
	COMMAND_IMPORTCAEINFO = 1,
	COMMAND_SHOWCAESOLIDALL,
	COMMAND_SHOWCAESOLIDPART,
	COMMAND_SETCAERSTDISP,
	COMMAND_CLEARCONTOURLINE,
	COMMAND_CLEARCONTOURPLOT,
	COMMAND_EXPORTTOPARASOLID,
	COMMAND_SHOWCLIPEFACEPLOT,
	COMMAND_SHOWCLIPEFACELINE,
	COMMAND_SHOWCLIPEFACEPVECTOR,
	COMMAND_CLEARCLIPEFACEPLOT,
	COMMAND_CLEARCLIPEFACELINE,
	COMMAND_CLEARCLIPEFACEPVECTOR,
	COMMAND_CLIPPLOTEFACE,
	COMMAND_CLEARPLOTCUTFACE,
	COMMAND_MOVECLIPPLOTFACE,
	COMMAND_SAVEPICTURE,
	COMMAND_CLIPEFACE,
	COMMAND_EXPORTCAL,
	COMMAND_CALCREBAR,
	COMMAND_FENCEDISPLAY,
	COMMAND_RECOVERDISPLAY,
	COMMAND_CASECOMPARE,
	COMMAND_CLIPRESULT,
	COMMAND_SURFACESHOW,
	COMMAND_OUTPUTREBAR,
	COMMAND_SHOWCONTOUR,
	COMMAND_DELETEDISPLAY,
	COMMAND_NODEVIEWER
};

/************************************************************************/
/* PROMPT资源IDs                                                        */
/************************************************************************/
enum
{
	PROMPT_SoildExport = 1,
	PROMPT_SolidAllShow,
	PROMPT_ImportCAESolid,
	PROMPT_SELECT_FIRST_ELEMENT,
	PROMPT_SELECT_NEXT_ELEMENT_OR_ACCEPT,
	PROMPT_ACCEPT,
	MESSAGEID_ModelDataDialogTitle,
	MESSAGEID_ModelDataFilterString,
	MESSAGEID_ResultDataDialogTitle,
	MESSAGEID_ResultDataFilterString,

	PROMPT_CalcRebar_ClickFirst,
	PROMPT_CalcRebar_ClickNext,
	PROMPT_CalcRebar_ClickError,

	PROMPT_Recover_SelectInstance,
	PROMPT_Fence_SelectInstance,

	PROMPT_Surface_LockX,
	PROMPT_Surface_LockY,
	PROMPT_Surface_LockZ,
	PROMPT_Surface_LockNone,

	PROMPT_ClipResults_CentrePointStt,
	PROMPT_ClipResults_CentrePointEnd,
	PROMPT_ClipResults_ArcStt,
	PROMPT_ClipResults_ArcEnd,
};

/************************************************************************/
/* Message资源IDs                                                       */
/************************************************************************/
enum
{
	MESSAGE_ReadXMTDialogTitle = 1,
	MESSAGE_WriteXMTDialogTitle,
	MESSAGE_InvalidBodySize,
	MESSAGE_EnterDataPoint,
	ERROR_UnableToSave,
	MESSAGE_FileExist,
	MESSAGE_ConvertComplete,
	MESSAGE_XmtExtension,
};

/************************************************************************/
/* Dialog资源IDs                                                        */
/************************************************************************/
enum
{
	DIALOGID_ImportCaeInf = 1,
	DIALOGID_SetContourColor,
	DIALOGID_SolidContourDisp,
	DIALOGID_SurfaceShow,
	DIALOGID_ElementExport,
	DIALOGID_ExportXMT,
	DIALOGID_SolidAllShow,
	DIALOGID_SolidPartShow,
	DIALOGID_ColorChooser,
	DIALOGID_ExportCalculation,
	DIALOGID_CaseCompare,
	DIALOGID_ClipResultShow,
	DIALOGID_RebarCalculate,
	DIALOGID_AddRebarLine,
	DIALOGID_AddClipFace,
};

/************************************************************************/
/* Text资源IDs                                                          */
/************************************************************************/
enum
{
	TEXTID_ImportCaeModel = 1,
	TEXTID_ImportCaeResult,
	TEXTID_ColorSet_StartValue,
	TEXTID_ColorSet_EndValue,
	TEXTID_colorNumLabel,
	TEXTID_showNameID,
	TEXTID_ListCell,
	TEXTID_ExportCalc_CellCount,
	TEXTID_ExportCalc_NodeCount,
	TEXTID_CalRebar_SectionHeight,
	TEXTID_CalRebar_DesignFt,
	TEXTID_CalRebar_LineName,
	TEXTID_ClipResult_Current,
	TEXTID_ClipResult_TableTitle,
	TEXTID_ClipResult_TitleFont,
	TEXTID_ClipResult_ContentFont,
	TEXTID_ClipResult_HeightCoe,
	TEXTID_ClipResult_XSpaceCnt,
	TEXTID_ClipResult_YSpaceCnt,
	TEXTID_ClipResult_CoorFont,
	TEXTID_ClipResult_XValueCnt,
	TEXTID_ClipResult_YValueCnt,
	TEXTID_ClipResult_Decimal,
	TEXTID_ClipResult_ValueFont,
	TEXTID_Surface_Current,
	TEXTID_ClipFace_clipName,
	TEXTID_CalRebar_SignifyStruFt,
	TEXTID_CalRebar_StruFt,
	TEXTID_CalRebar_RebarTesion,
	TEXTID_CalRebar_ConcrTesion,
	TEXTID_CalRebar_DefPointSpan,
	TEXTID_CalRebar_DesignCoefficient,
	TEXTID_CalRebar_Current,
	TEXTID_CalRebar_ListCell_RebarNumber,
	TEXTID_CalRebar_AddLine_Name,
};

/************************************************************************/
/* StringList资源IDs                                                    */
/************************************************************************/
enum
{
	STRINGLISTID_Commands = 1,
	STRINGLISTID_Prompts,
	STRINGLISTID_ModifyMultipleElementsCommandNames,
	STRINGLISTID_ModifyMultipleElementsTextMessages,
	STRINGLISTID_XMTMessages,
	STRINGLISTID_XMTCommandNames,
	STRINGLISTID_XMTImportCmdOptions,
	STRINGLISTID_ConditionType,
	STRINGLISTID_Condition,
	STRINGLISTID_CalRebar_RebarType,
	STRINGLISTID_CalRebar_ConcrType,
	STRINGLISTID_ShowType,
	STRINGLISTID_ShowModel,
	STRINGLISTID_CalRebar_Diameter
};

/************************************************************************/
/* OptionButton资源IDs                                                  */
/************************************************************************/
enum
{
	OPTIONBUTTONID_ElementExportMode = 1,
	OPTIONBUTTONID_ExportXMTVersion,
};

/************************************************************************/
/* ToggleButton资源IDs													*/
/************************************************************************/
enum
{
	TOGGLEID_MeshModel = 1,
	TOGGLEID_Result,
	TOGGLEID_ShowExtremum,
	TOGGLEID_Surface_ShowExtremum,

	TOGGLEID_ClipResult_D_SX,
	TOGGLEID_ClipResult_D_SY,
	TOGGLEID_ClipResult_D_SZ,
	TOGGLEID_ClipResult_D_SC,
	TOGGLEID_ClipResult_D_EPTOXY,
	TOGGLEID_ClipResult_D_EPTOYZ,
	TOGGLEID_ClipResult_D_EPTOXZ,
	TOGGLEID_ClipResult_D_UX,
	TOGGLEID_ClipResult_D_UY,
	TOGGLEID_ClipResult_D_UZ,
	TOGGLEID_ClipResult_D_ALL,

	TOGGLEID_ClipResult_Z_SR,
	TOGGLEID_ClipResult_Z_SA,
	TOGGLEID_ClipResult_Z_SH,
	TOGGLEID_ClipResult_Z_SRA,
	TOGGLEID_ClipResult_Z_SAH,
	TOGGLEID_ClipResult_Z_SRH,
	TOGGLEID_ClipResult_Z_SC,
	TOGGLEID_ClipResult_Z_UX,
	TOGGLEID_ClipResult_Z_UY,
	TOGGLEID_ClipResult_Z_UZ,
	TOGGLEID_ClipResult_Z_ALL,

	TOGGLEID_ColorSet_Automatic,
	TOGGLEID_ClipPlotFace_SideSelect,

	TOGGLEID_CalRebar_calcStress,
	TOGGLEID_ColorSet_Gray
};

/************************************************************************/
/* ComboBox资源IDs														*/
/************************************************************************/
enum
{
	COMBOBOXID_ImportCaeModel = 1,
	COMBOBOXID_ConditionType,
	COMBOBOXID_Condition,
	COMBOBOXID_ConditionVal,
	COMBOBOXID_ResultType,
	COMBOBOXID_ShowType,
	COMBOBOXID_ShowModel,
	COMBOBOXID_SolidDisp_ConditionType,
	COMBOBOXID_SolidDisp_ConditionOpt,
	COMBOBOXID_SolidDisp_ConditionValue,
	COMBOBOXID_XMTFileUnits,

	COMBOBOXID_ExportCalc_InstanceLists,

	COMBOBOXID_CalRebar_RebarType,
	COMBOBOXID_CalRebar_ConcrType,

	COMBOBOXID_CaseComp_InstanceL,
	COMBOBOXID_CaseComp_InstanceR,
	COMBOBOXID_CaseComp_ResultTypes,
	COMBOBOXID_CaseComp_ShowTypes,
	COMBOBOXID_CaseComp_ShowModel,

	COMBOBOXID_CalRebar_CaseList,

	COMBOBOXID_SurfaceResultType,
	COMBOBOXID_Surface_ShowType,
	COMBOBOXID_Surface_Model,
	COMBOBOXID_CalRebar_ListCell_Diameter
};

/************************************************************************/
/* MultiLineText资源IDs													*/
/************************************************************************/
enum
{
	MLTEXTID_ExportCalc_ProjectDescr = 1,
};

/************************************************************************/
/* Group资源IDs															*/
/************************************************************************/

/************************************************************************/
/* ICON资源IDs															*/
/************************************************************************/
enum
{
	ICONID_ColorPatch = 1,
	ICONID_Model,
	ICONID_Result,
	ICONID_Section,
	ICONID_RebarLine,
	ICONID_LockX,
	ICONID_LockY,
	ICONID_LockZ,
	ICONID_LockNone,
};

/************************************************************************/
/* Tree资源IDs															*/
/************************************************************************/
enum
{
	TREEID_SolidDis_ProjList = 1,

	TREEID_CaseComp_CaseListL,
	TREEID_CaseComp_CaseListR,

	TREEID_ClipResult_ClipList,

	TREEID_Surface_ClipList,

	TREEID_CalRebar_SectionList,
};

/************************************************************************/
/* Container资源IDs														*/
/************************************************************************/
enum
{
	CONTAINERID_ColorChooser = 1,
	COLORCHOOSERID_Color,
	CONTAINERID_SolidDis_Setting,
	CONTAINERID_Surface_Setting,
	CONTAINERID_ClipResult_Setting,
	CONTAINERID_CalRebar_Setting,
};

/************************************************************************/
/* MenuBar资源IDs														*/
/************************************************************************/

/************************************************************************/
/* ListBox资源IDs														*/
/************************************************************************/
enum
{
	LISTBOXID_CaeResultFile = 1,
	LISTBOXID_ColorSet_ColorList,
	LISTBOXID_CaeProject,
	LISTBOXID_ConditionList,
	LISTBOXID_SingleColumn,

	LISTBOXID_SolidDisp_ConditionList,

	LISTBOXID_ExportCalc_ModelPaths,
	LISTBOXID_ExportCalc_WYCloudPaths,
	LISTBOXID_ExportCalc_YLCloudPaths,
	LISTBOXID_ExportCalc_YLPaths,
	LISTBOXID_ExportCalc_CaseList,
	LISTBOXID_ExportCalc_ModelParamList,
	LISTBOXID_ExportCalc_WYParamList,
	LISTBOXID_ExportCalc_YLParamList,
	LISTBOXID_ExportCalc_PJParamList,

	LISTBOXID_CalRebar_StressVal,
	LISTBOXID_CalRebar_CalResult,
	LISTBOXID_CalRebar_RebarInfo,
};

/************************************************************************/
/* ItemList资源IDs														*/
/************************************************************************/
enum
{
	ITEMLIST_ElementExport = 1,
	ITEMLISTID_CalcRebar,
	ITEMLISTID_ClipFace,
	ITEMLISTID_ClipPlotFace,
};

/************************************************************************/
/* PushButton资源IDs													*/
/************************************************************************/
enum
{
	BUTTONID_SelectCaeModelFile = 1,
	BUTTONID_AddResultFile,
	BUTTONID_DelResultFile,
	BUTTONID_UpDown,
	BUTTONID_Show,
	BUTTONID_Damage,
	BUTTONID_ClipPlot,
	BUTTONID_NodeVal,
	BUTTONID_ShowBoundary,
	BUTTONID_ShowSet,
	BUTTONID_SelectExport2,
	BUTTONID_AllExport2,
	BUTTONID_ConditionAdd,
	BUTTONID_ConditionDel,
	BUTTONID_ConditionClr,
	BUTTONID_ColorChooser,

	PUSHBTNID_SolidDisp_AddC,
	PUSHBTNID_SolidDisp_DelC,
	PUSHBTNID_SolidDisp_ClearC,

	PUSHBTNID_ExportCalc_Submit,
	PUSHBTNID_ExportCalc_Cancel,
	PUSHBTNID_ExportCalc_Import,
	PUSHBTNID_ExportCalc_AddModelPath,
	PUSHBTNID_ExportCalc_AddWYCloudPath,
	PUSHBTNID_ExportCalc_AddYLCloudPath,
	PUSHBTNID_ExportCalc_AddYLPath,
	PUSHBTNID_ExportCalc_DelModelPath,
	PUSHBTNID_ExportCalc_DelWYCloudPath,
	PUSHBTNID_ExportCalc_DelYLCloudPath,
	PUSHBTNID_ExportCalc_DelYLPath,

	PUSHBTNID_CaseComp_ColorSettingL,
	PUSHBTNID_CaseComp_ColorSettingR,
	PUSHBTNID_CaseComp_Submit,

	PUSHBTNID_ClipResult_Submit,
	PUSHBTNID_ClipResult_Clear,
	PUSHBTNID_ClipResult_OutDGN,

	PUSHBTNID_Surface_Setting,
	PUSHBTNID_Surface_Submit,
	PUSHBTNID_Surface_NodeVal,

	PUSHBTNID_CalRebar_OutPut,
	PUSHBTNID_CalRebar_Calculate,
	PUSHBTNID_CalRebar_Refresh,

	PUSHBTNID_ColorSet_OK,
	PUSHBTNID_ColorSet_Cancel,
	PUSHBTNID_ColorSet_Apply,
};

/************************************************************************/
/* Hook资源IDs															*/
/************************************************************************/
enum
{
	HOOKDIALOGID_ImportCaeInf = 1,
	HOOKBUTTON_SelectCaeModel,
	HOOKBUTTON_AddResultFile,
	HOOKITEMID_ListBox_DialogDemo,
	HOOKITEMID_PBtnColorChooser,
	HOOKITEMID_ColorChooser,
	HOOKITEMID_Updownbutton,
	HOOKDIALOGID_SolidContourDisp,
	HOOKDIALOGID_SurfaceShow,
	HOOKITEMID_Tree,
	HOOKSASH_SolidDisp_VExample,
	HOOKITEMID_CTPanel,
	HOOKITEMID_SingleListBox,
	HOOKITEMID_MultiListBox,
	HOOKITEMID_ComboBoxes,
	HOOKITEMID_Simple,
	HOOKITEMID_TreeComboBox,
	HOOKITEMID_ComboBoxes2,
	HOOKBUTTON_ShowSet,
	HOOKCOMBOBOX_SolidDisp_ShowModel,
	HOOKCOMBOBOX_SolidDisp_CondType,
	HOOKCOMBOBOX_SolidDisp_CondVal,
	HOOKBUTTON_SelectExport,
	HOOKBUTTON_AllExport,
	HOOKITEMID_TBSelImpDat,
	HOOKBUTTON_DelResultFile,
	HOOKCOMBOBOX_SelectCaeModel,
	HOOKDIALOGID_SolidAllShow,
	HOOKTEXT_CellOfColor,
	HOOKSolidContourDisp_ResultType,
	HOOKBUTTON_SolidDisp_AddC,
	HOOKBUTTON_SolidDisp_DelC,
	HOOKBUTTON_SolidDisp_ClearC,
	HOOKPUSHBTN_SolidDisp_Show,
	HOOKPUSHBTN_Damage_Show,
	HOOKCOMBOBOX_SolidDisp_ShowType,
	HOOKPUSHBTN_BoundaryShow,

	HOOKDIALOG_ExpCalc,
	HOOKPUSHBTN_ExpCalc_Import,
	HOOKPUSHBTN_ExpCalc_AddPath,
	HOOKPUSHBTN_ExpCalc_DelPath,
	HOOKPUSHBTN_ExpCalc_Submit,
	HOOKLISTBOX_ExpCalc_CaseList,
	HOOKLISTBOX_ExpCalc_ModelParamList,
	HOOKLISTBOX_ExpCalc_WYParamList,
	HOOKLISTBOX_ExpCalc_YLParamList,
	HOOKLISTBOX_ExpCalc_PJParamList,
	HOOKMLTEXT_ExpCalc_ProDescr,
	HOOKCOMBOBOX_ExpCalc_ProjectLists,

	HOOKCOMBOBOX_CalRebar_RebarType,
	HOOKCOMBOBOX_CalRebar_ConcrType,
	//HOOKCOMBOBOX_CalRebar_CaseList,
	//HOOKTEXT_CalRebar_DesignFt,

	HOOKCOMBOBOX_PartDis_CondName,
	HOOKCOMBOBOX_PartDis_CondVal,
	HOOKBUTTON_PartDis_Add,
	HOOKBUTTON_PartDis_Del,
	HOOKBUTTON_PartDis_Clear,
	HOOKLISTBOX_PartDis_Conditions,

	HOOKDIALOG_CaseCompare,
	HOOKCOMBOBOX_CaseComp_InstanceL,
	HOOKCOMBOBOX_CaseComp_InstanceR,
	HOOKBUTTON_CaseComp_SettingL,
	HOOKBUTTON_CaseComp_SettingR,
	HOOKBUTTON_CaseComp_Compare,
	HOOKTREE_CaseComp_CaseList,
	HOOKCOMBOBOX_CaseComp_ResultType,
	HOOKCOMBOBOX_CaseComp_ShowType,
	HOOKCOMBOBOX_CaseComp_ShowModel,

	HOOKDIALOG_ClipResult,
	HOOKTREE_ClipResult_ClipList,
	HOOKOPTBUTTON_ClipResult_D_ALL,
	HOOKOPTBUTTON_ClipResult_Z_ALL,
	HOOKBUTTON_ClipResult_Clear,
	HOOKBUTTON_ClipResult_Submit,
	HOOKBUTTON_ClipResult_OutDGN,
	HOOKTABPAGE_ClipResult_ResultType,

	HOOKTREE_Surface_ClipList,
	HOOKCOMBOBOX_Surface_Model,
	HOOKCOMBOBOX_Surface_ResultType,

	HOOKBUTTON_Surface_ResultShow,
	HOOKCOMBOBOX_SurfaceDisp_ShowType,
	HOOKPDM_Surface_Add,
	HOOKPDM_Surface_Del,
	HOOKBUTTON_Surface_ShowSet,

	HOOKDIALOG_ColorSetting,
	HOOKTOGGLE_ColorSet_Automatic,
	HOOKTEXT_ColorSet_ColorValue,
	HOOKPUSHBTN_ColorSet_Apply,
	HOOKTOGGLE_ColorSet_Gray,

	HOOKSASH_SolidDisp_V,
	HOOKSASH_Surface_V,
	HOOKSASH_ClipResult_V,
	HOOKSASH_CalRebar_RebarCalculate_V,
	HOOKPULLMENU_Surface_ClipMenu,

	HOOKDIALOGID_CalRebar_RebarCalculate,
	HOOKDIALOGID_CalRebar_AddRebarLine,
	HOOKITEMID_CalRebar_SectionTree,
	HOOKPUSHBTN_CalRebar_Calculate,
	HOOKPUSHBTN_CalRebar_OutPut,
	HOOKPUSHBTN_CalRebar_Refresh,

	HOOKGENERIC_CalRebar_Canva,
	HOOKPULLMENU_CalRebar_PopuMenu,
	HOOKPDM_CalRebar_AddRebarLine,
	HOOKPDM_CalRebar_DelRebarLine,
	HOOKLISTBOX_CalRebar_CalcResult,
	HOOKLISTBOX_CalRebar_RebarInfo,
	HOOKITEMID_CalRebar_CellCombbox_Diameter,
	HOOKITEMID_CalRebar_CellTxt_RebarNumber,
	HOOKTXTID_CalRebar_RebarTesion,
	HOOKTXTID_CalRebar_ConcrTesion,

	HOOKITEMID_Surface_LockAxis,
	HOOKDIALOGID_Surface_AddClipFace,
};

/************************************************************************/
/* Generic资源IDs													*/
/************************************************************************/
enum
{
	GENERICID_CalRebar_CANVA = 1,
};

/************************************************************************/
/* TabPageList资源IDs													*/
/************************************************************************/
enum
{
	TPLISTID_ExportCalc_Main = 1,
	TPLISTID_ExportCalc_Sub,
	TPLISTID_ClipResult_Main,

	TABPAGEID_ExportCalc_PlotShow,
	TABPAGEID_ExportCalc_WYOutput,
};

/************************************************************************/
/* TabPage资源IDs														*/
/************************************************************************/
enum
{
	TABPAGEID_ExportCalc_Project = 1,
	TABPAGEID_ExportCalc_Case,
	TABPAGEID_ExportCalc_Model,
	TABPAGEID_ExportCalc_Results,
	TABPAGEID_ExportCalc_ResultsWY,
	TABPAGEID_ExportCalc_ResultsYL,
	TABPAGEID_ExportCalc_ResultsPJ,

	TABPAGEID_ClipResult_Cartesian,
	TABPAGEID_ClipResult_Cylind,
};

enum
{
	DEPENDENCYID_ForbiddenModified,
};

enum
{
	DILISTID_SolidDis_Setting = 1,
	DILISTID_Surface_Setting,
	DILISTID_ClipResult_Setting,
	DILISTID_Rebar_Setting,
};

enum
{
	CTPANELID_SolidDis_Detail = 1,
	CTPANELID_Surface_Detail,
	CTPANELID_ClipResult_Detail,
	CTPANELID_CalRebar_Detail,
};

enum
{
	SASHID_SolidDis_V = 1,
	SASHID_Surface_V,
	SASHID_ClipResult_V,
	SASHID_CalRebar_V,
};

enum
{
	POPUPMENUID_Surface_ClipMenu = 1,
	POPUPMENUID_CalRebar_PopuMenu,
};

enum
{
	PDMID_Surface_ClipMenu = 1,
	PDMID_CalRebar_PopuMenu,
};

enum
{
	MSID_AddRebarLine = 1,
	MSID_DelRebarLine,
};
enum
{
	BGROUPID_Surface_LockAxis = 1,
};

#endif /* #if !defined (__myExportH__) */