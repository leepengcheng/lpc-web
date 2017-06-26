/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSi.cpp
|
+--------------------------------------------------------------------------------------*/
#include    "MsdiMsCaeSipch.h"
#include    "BentlyCommonfile.h"

// 注册信息
//typedef bool(*RegisterFunc)();

/*******************全局变量定义**********************/
ImpInfDlgInfo		g_ImpDlgInfo;
SolidDispDlgInfo	g_solidDispDlgInfo;
ReBarCalDlgInfo		g_rebarCalDlgInfo;
ExpCalculationDlgInfo	g_expCalculationInfo;
PartialDisDlgInfo	g_partialDisDlgInfo;
CaseCompDlgInfo		g_caseCompDlgInfo;
ClipResultDlgInfo	g_clipResultDlgInfo;
SurfaceShowDlgInfo	g_surfaceShowDlgInfo;
OutRebarDlgInfo		g_outRebarDlgInfo;
ColorSettingDlgInfo	g_colorSettingDlgInfo;

BSIColorDescr*	m_pBackgroundColor;
BSIColorDescr*	m_pForegroundColor;

//主色带，主轮廓显示&工况对比左侧显示
DrawColorShape  s_DrawColorShape;
//副色带，仅用于工况对比右侧显示
DrawColorShape	s_DrawColorShape_temp;
//色带指针，由于之前代码都是缺省使用的主色带，导致暂时无法将色带作为参数传入，因此定义该色带指针
//之前所有使用主色带的位置改为使用该指针所指色带，在调用绘制轮廓相关函数之前需要手动修改该指针的指向，在main函数中缺省指向主色带
DrawColorShape*	s_pDCShape;

// CAE结果类型名
bmap<WString, WString> g_RstType_Name;
// CAE结果类型单位
bmap<WString, WString> g_RstType_Unit;

bvector<WString> vecRstTypeCodes;

DgnPlatform::BSIColorPalette*  ppPalette;
bool alreadyUnload;

void ApplyCaeDisplayStyle(WString styleName, int viewIndex)
{
	ViewGroupCollectionCR viewGroupCollection = mdlDgnFileObj_getMasterFile()->GetViewGroups();
	DisplayStyleCP	tempStyle = DisplayStyleManager::FindDisplayStyleByName(styleName.GetWCharCP(), mdlDgnFileObj_getMasterFile());;

	if (tempStyle != NULL)
	{
		// 判断显示式样是否包含在本Dgn文件中
		if (DisplayStyleManager::DoesDisplayStyleExistInFile(styleName.GetWCharCP(), *mdlDgnFileObj_getMasterFile()) != true)
		{
			// 没有包含时，写入到本Dgn文件中
			DisplayStyleManager::WriteDisplayStyleToFile(*tempStyle, *mdlDgnFileObj_getMasterFile());
		}

		// 应用指定的显示式样
		DisplayStyleManager::ApplyDisplayStyleToView(*tempStyle, *viewGroupCollection.GetActiveViewInfoP(viewIndex));
	}

	mdlView_updateSingle(viewIndex);
}

int copyFile(const char *SourceFile, const char *NewFile)
{
	ifstream in;
	ofstream out;
	in.open(SourceFile, ios::binary);//打开源文件
	if (in.fail())//打开源文件失败
	{
		cout << "Error 1: Fail to open the source file." << endl;
		in.close();
		out.close();
		return 0;
	}
	out.open(NewFile, ios::binary);//创建目标文件 
	if (out.fail())//创建文件失败
	{
		cout << "Error 2: Fail to create the new file." << endl;
		out.close();
		in.close();
		return 0;
	}
	else//复制文件
	{
		out << in.rdbuf();
		out.close();
		in.close();
		return 1;
	}
}

#pragma region 截图功能
/*-----------------------------------------------------------------
*  功能描述: 将指定视图的内容保存为BMP图片形式
*	@param	viewIndex	IN	指定视图索引
*  @param	imageFile	IN	BMP图片保存路径
*
*  @return	无
-----------------------------------------------------------------*/
void captureViewToImage(int viewIndex, WCharCP imageFile)
{
	mdlView_updateSingle(viewIndex);
	BSIRect viewRect;
	byte*	pImage = NULL;
	Point2d size;
	MSWindow* p = mdlWindow_viewWindowGet(viewIndex);
	mdlView_getViewRectangle(&viewRect, p, DgnPlatform::DgnCoordSystem::Frustum);
	viewRect.Offset(0, 70);
	
	size.x = viewRect.corner.x - viewRect.origin.x + 1;
	size.y = viewRect.corner.y - viewRect.origin.y + 1;
	mdlImage_captureScreen(&pImage, &viewRect, viewIndex, IMAGEFORMAT_RGB);
	mdlImage_RGBToRGBSeparate(pImage, pImage, &size, ImageColorMode::RGB);
	WString wfile;
	wfile.append(imageFile);
	
	if (wfile.find(L"jpg") != WString::npos)//JPG格式
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_JPEG, ImageColorMode::RGB, &size, pImage);
	}
	else if (wfile.find(L"tif") != WString::npos)
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_TIFF, ImageColorMode::RGB, &size, pImage);
	}
	else
	{
		mdlImage_createFileFromRGB(imageFile, IMAGEFILE_BMP, ImageColorMode::RGB, &size, pImage);
	}
	mdlImage_freeImage(pImage, &size, IMAGEFORMAT_RGB);
}

void SavePicture(WCharCP unparsed)
{
	int result;
	BeFileName File_path;
	result = mdlDialog_fileCreate(File_path, 0, 0, L"", L"*.bmp,*.jpg,*.tif", L"d:\\", L"保存结果图片");
	if (result == SUCCESS)
	{
		captureViewToImage(0, File_path.GetWCharCP());
	}

}
#pragma endregion 截图功能

void Select_exportHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		exportTool::InstallNewInstance(0);
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void All_exportHook
(
DialogItemMessage       *dimP   /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		/*if (SUCCESS == mdlSelect_allElements())
		{
		exportTool::ExportSelect_toParasolid();
		}*/
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ExportToParasolidCmd(WCharCP unparsed)
{
	exportTool::InstallNewInstance(COMMAND_EXPORTTOPARASOLID);
}

// 读取位移输出画面参数
void loadClipResultDlgParams(void)
{
	int           status = ERROR;
	RscFileHandle userPrefsH;

	memset(&g_clipResultDlgInfo, 0, sizeof(g_clipResultDlgInfo));

	/* --- load Section and Hatching Preferences --- */
	if (SUCCESS == mdlDialog_userPrefFileOpen(&userPrefsH, RSC_READWRITE))
	{
		ClipResultDlgInfo *clipResultDlgRscP = NULL;

		if (NULL != (clipResultDlgRscP = (ClipResultDlgInfo *)mdlResource_loadByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, mdlSystem_getCurrTaskID())))
		{
			ULong rscSize;

			mdlResource_query(&rscSize, clipResultDlgRscP, RSC_QRY_SIZE);

			if (rscSize == sizeof(ClipResultDlgInfo))
			{
				status = SUCCESS;
				g_clipResultDlgInfo = *clipResultDlgRscP;
			}

			/* --- Free the memory used by the rsc --- */
			mdlResource_free(clipResultDlgRscP);
		}

		mdlResource_closeFile(userPrefsH);
	}

	/* --- if no resource was loaded load defaults now --- */
	if (status != SUCCESS)
	{
		//初始化输入值
		g_clipResultDlgInfo.titleFont = O_titleFont;
		g_clipResultDlgInfo.contentFont = O_contentFont;
		g_clipResultDlgInfo.heightCoe = O_heightCoe;
		g_clipResultDlgInfo.XSpaceCnt = O_XSpaceCnt;
		g_clipResultDlgInfo.YSpaceCnt = O_YSpaceCnt;
		g_clipResultDlgInfo.coorFont = O_coorFont;
		g_clipResultDlgInfo.XValueCnt = O_XValueCnt;
		g_clipResultDlgInfo.YValueCnt = O_YValueCnt;
		g_clipResultDlgInfo.valueFont = O_valueFont;
		g_clipResultDlgInfo.decimal =   O_decimal;
	}
}

// 保存位移输出画面参数
void saveClipResultDlgParams(void)
{
	RscFileHandle  userPrefsH;
	if (SUCCESS == mdlDialog_userPrefFileOpen(&userPrefsH, RSC_READWRITE))
	{
		/* --- delete original section parameters --- */
		mdlResource_deleteByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, mdlSystem_getCurrTaskID());

		/* --- add new section parameters --- */
		mdlResource_addByAlias(userPrefsH, RTYPE_Mcsi, RSCID_Mcsi, &g_clipResultDlgInfo, sizeof(ClipResultDlgInfo), mdlSystem_getCurrTaskID());

		/* --- Close user preference file ---- */
		mdlResource_closeFile(userPrefsH);
	}
}

bool UnloadFunction(UnloadProgramReason unloadType)
{
	// 保存位移输出画面参数
	saveClipResultDlgParams();

	if (alreadyUnload)
		return false;

	ClearContour();
	ShowCAEModel();

	IViewManager::GetManager().DropViewDecoration(&s_DrawColorShape);
	IViewManager::GetManager().DropViewDecoration(&s_DrawColorShape_temp);
	s_drawtempLine.CloseDrawFunction();
	alreadyUnload = true;

	return false;
}

void ExitDesignFileFunction()
{
	if (alreadyUnload)
		return;

	alreadyUnload = true;
	ClearContour();
	ShowCAEModel();

	s_DrawColorShape.CloseDrawFunction();
	s_DrawColorShape_temp.CloseDrawFunction();
	s_drawtempLine.CloseDrawFunction();
}

/*----------------------------------------------------+
|    constructsolidElemString    		    |
+----------------------------------------------------*/
void constructsolidElemString(char* descrStr, char *elemName, ElementRefP elRef, DgnModelRefP modelRef)
{
	MSElementDescrP  edP = NULL;
	mdlElmdscr_getByElemRef(&edP, elRef, modelRef, FALSE, NULL);
	if (NULL != edP)
	{
		sprintf(descrStr, elemName);
	}
}

void initProgramData()
{
	// 读取位移输出画面参数
	loadClipResultDlgParams();

	//两个色带数据的部分初始化
	s_DrawColorShape.initialize();
	s_DrawColorShape_temp.initialize();

	s_DrawColorShape.DrawPos.x = -200;
	s_DrawColorShape.DrawPos.y = 50;
	s_DrawColorShape_temp.DrawPos.x = -200;
	s_DrawColorShape_temp.DrawPos.y = 400;

	IViewManager::GetManager().AddViewDecoration(&s_DrawColorShape);
	IViewManager::GetManager().AddViewDecoration(&s_DrawColorShape_temp);

	//色带指针缺省指向主色带
	s_pDCShape = &s_DrawColorShape;

	mdlColorPal_create(&ppPalette, 255);

	//检查所有命名组，若其中已没有元素则删除之
	for each (NamedGroupPtr namedGroup in *NamedGroupCollection::Create(*MASTERFILE))
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		// 如果命名组中没有元素,就从Dgn中删除。
		if (graphicMembers == 0)
		{
			namedGroup->DeleteFromFile();
		}
	}
	
	// 取得CAE结果类型
	getRstTypeInfFromDB(g_RstType_Name, g_RstType_Unit);

	alreadyUnload = false;
}

static  MdlCommandName cmdNames[] =
{
	{ ImportCAEInfCmd, "ImportCAEInf" },
	{ ShowCAESolidAllCmd, "ShowCAESolidAll" },
	{ ShowCAESolidPartCmd, "ShowCAESolidPart" },
	{ SetCAERstDispCmd, "SetCAERstDisp" },
	{ ExportToParasolidCmd, "EXPORTTOPARASOLID" },
	0,
};

static MdlCommandNumber cmdNumbers[] =
{
	{ (CmdHandler)ImportCAEInfCmd, CMD_MSDI_MSCAESI_ACTION_IMPORTCAEINFO },
	{ (CmdHandler)ShowCAESolidAllCmd, CMD_MSDI_MSCAESI_ACTION_SHOWCAESOLIDALL },
	{ (CmdHandler)ShowCAESolidPartCmd, CMD_MSDI_MSCAESI_ACTION_SHOWCAESOLIDPART },
	{ (CmdHandler)SetCAERstDispCmd, CMD_MSDI_MSCAESI_ACTION_SETCAERSTDISP },
	{ (CmdHandler)ExportToParasolidCmd, CMD_MSDI_MSCAESI_ACTION_EXPORTTOPARASOLID },

	{ (CmdHandler)ClipFaceCmd, CMD_MSDI_MSCAESI_ACTION_CLIPEFACE },
	{ (CmdHandler)ClipPlotFaceCmd, CMD_MSDI_MSCAESI_ACTION_CLIPPLOTEFACE },
	{ (CmdHandler)SavePicture, CMD_MSDI_MSCAESI_ACTION_SAVEPICTURE },

	{ (CmdHandler)exportCalDocCmd, CMD_MSDI_MSCAESI_ACTION_EXPORTCAL },
	{ (CmdHandler)fenceDisplayCmd, CMD_MSDI_MSCAESI_ACTION_FENCEDISPLAY },
	{ (CmdHandler)recoverDisplayCmd, CMD_MSDI_MSCAESI_ACTION_RECOVERDISPLAY },
	{ (CmdHandler)caseCompareCmd, CMD_MSDI_MSCAESI_ACTION_CASECOMPARE },
	{ (CmdHandler)clipResultShowCmd, CMD_MSDI_MSCAESI_ACTION_CLIPRESULT },
	{ (CmdHandler)surfaceShowCmd, CMD_MSDI_MSCAESI_ACTION_SURFACESHOW },
	{ (CmdHandler)outputRebarCmd, CMD_MSDI_MSCAESI_ACTION_OUTPUTREBAR },
	{ (CmdHandler)deleteDisplayCmd, CMD_MSDI_MSCAESI_ACTION_DELETEDISPLAY },
	{ (CmdHandler)nodeViewerCmd, CMD_MSDI_MSCAESI_ACTION_NODEVIEWER },
	0,
};

Public DialogHookInfo uHooks[] =
{
	{ HOOKDIALOGID_ImportCaeInf, (PFDialogHook)ImportCaeInfDialog_mdlDialogHook },
	{ HOOKITEMID_TBSelImpDat, (PFDialogHook)ImportCaeInfDialog_TBSelImpDatHook },
	{ HOOKBUTTON_DelResultFile, (PFDialogHook)ImportCaeInfDialog_DelResultFileHook },
	{ HOOKCOMBOBOX_SelectCaeModel, (PFDialogHook)ImportCaeInfDialog_SelectCaeModelHook },

	{ HOOKBUTTON_SelectCaeModel, (PFDialogHook)SelectCaeModelPath },
	{ HOOKBUTTON_AddResultFile, (PFDialogHook)ImportCaeInfDialog_AddResultFileHook },

	{ HOOKDIALOG_ColorSetting, (PFDialogHook)HookDialog_ColorSetting },
	{ HOOKTOGGLE_ColorSet_Automatic, (PFDialogHook)HookToggle_ColorSet_Automatic },
	{ HOOKTEXT_ColorSet_ColorValue, (PFDialogHook)HookText_ColorSet_ColorValue },
	{ HOOKPUSHBTN_ColorSet_Apply, (PFDialogHook)HookPushBtn_ColorSet_Apply },
	{ HOOKITEMID_PBtnColorChooser, (PFDialogHook)HookPushBtn_ColorSet_ColorChooser },
	
	{ HOOKTOGGLE_ColorSet_Gray, (PFDialogHook)HookPushBtn_ColorSet_Gray },
	{ HOOKITEMID_ColorChooser, (PFDialogHook)TColorChooser },
	{ HOOKITEMID_ListBox_DialogDemo, (PFDialogHook)ListBoxCreate },
	{ HOOKITEMID_Updownbutton, (PFDialogHook)Updownbutton },
	{ HOOKTEXT_CellOfColor, (PFDialogHook)HookText_CellOfColor },

	{ HOOKDIALOGID_SolidContourDisp, (PFDialogHook)HookDialog_SolidDisp },
	{ HOOKITEMID_Tree, (PFDialogHook)HookTree_SolidDisp },
	{ HOOKBUTTON_ShowSet, (PFDialogHook)HookButton_SolidDisp_ShowSet },
	{ HOOKSolidContourDisp_ResultType, (PFDialogHook)HookComboBox_SolidDisp_ResultType },
	{ HOOKCOMBOBOX_SolidDisp_ShowModel, (PFDialogHook)HookComboBox_SolidDisp_ShowModel },
	{ HOOKCOMBOBOX_SolidDisp_CondType, (PFDialogHook)HookComboBox_SolidDisp_CondType },
	{ HOOKCOMBOBOX_SolidDisp_CondVal, (PFDialogHook)HookComboBox_SolidDisp_CondVal },
	{ HOOKBUTTON_SolidDisp_AddC, (PFDialogHook)HookButton_SolidDisp_AddC },
	{ HOOKBUTTON_SolidDisp_DelC, (PFDialogHook)HookButton_SolidDisp_DelC },
	{ HOOKBUTTON_SolidDisp_ClearC, (PFDialogHook)HookButton_SolidDisp_ClearC },
	{ HOOKPUSHBTN_SolidDisp_Show, (PFDialogHook)HookButton_SolidDisp_Show },
	
	{ HOOKPUSHBTN_Damage_Show, (PFDialogHook)HookButton_Damage_Show },
	{ HOOKCOMBOBOX_SolidDisp_ShowType, (PFDialogHook)HookComboBox_SolidDisp_ShowType },
	// 画面的分割条
	{ HOOKSASH_SolidDisp_V, (PFDialogHook)HookSash_SolidDisp_vSashHook },
	{ HOOKPUSHBTN_BoundaryShow, (PFDialogHook)HookButton_Boundary_Show },
	

	{ HOOKBUTTON_SelectExport, (PFDialogHook)Select_exportHook },
	{ HOOKBUTTON_AllExport, (PFDialogHook)All_exportHook },

	{ HOOKDIALOGID_SolidAllShow, (PFDialogHook)SolidMeshShowDlg_SolidAllShowHook },

	{ HOOKDIALOG_ExpCalc, (PFDialogHook)HookDialog_expCalc },
	{ HOOKPUSHBTN_ExpCalc_Import, (PFDialogHook)HookPushBtn_ExpCalc_Import },
	{ HOOKPUSHBTN_ExpCalc_AddPath, (PFDialogHook)HookPushBtn_ExpCalc_AddPath },
	{ HOOKPUSHBTN_ExpCalc_DelPath, (PFDialogHook)HookPushBtn_ExcCalc_DelPath },
	{ HOOKPUSHBTN_ExpCalc_Submit, (PFDialogHook)HookPushBtn_ExpCalc_Submit },
	{ HOOKLISTBOX_ExpCalc_CaseList, (PFDialogHook)HookListBox_ExpCalc_CaseList },
	{ HOOKLISTBOX_ExpCalc_ModelParamList, (PFDialogHook)HookListBox_ExpCalc_ModelParamList },
	{ HOOKLISTBOX_ExpCalc_WYParamList, (PFDialogHook)HookListBox_ExpCalc_WYParamList },
	{ HOOKLISTBOX_ExpCalc_YLParamList, (PFDialogHook)HookListBox_ExpCalc_YLParamList },
	{ HOOKLISTBOX_ExpCalc_PJParamList, (PFDialogHook)HookListBox_ExpCalc_PJParamList },
	{ HOOKMLTEXT_ExpCalc_ProDescr, (PFDialogHook)HookMlText_ExpCalc_ProDescr },
	{ HOOKCOMBOBOX_ExpCalc_ProjectLists, (PFDialogHook)HookComboBox_ExpCalc_ProjectLists },

	{ HOOKCOMBOBOX_CalRebar_RebarType, (PFDialogHook)HookComboBox_CalRebar_RebarType },
	{ HOOKCOMBOBOX_CalRebar_ConcrType, (PFDialogHook)HookComboBox_CalRebar_ConcrType },

	{ HOOKCOMBOBOX_PartDis_CondName, (PFDialogHook)HookComboBox_PartDis_CondName },
	{ HOOKCOMBOBOX_PartDis_CondVal, (PFDialogHook)HookComboBox_PartDis_CondVal },
	{ HOOKBUTTON_PartDis_Add, (PFDialogHook)HookButton_PartDis_Add },
	{ HOOKBUTTON_PartDis_Del, (PFDialogHook)HookButton_PartDis_Del },
	{ HOOKBUTTON_PartDis_Clear, (PFDialogHook)HookButton_PartDis_Clear },
	{ HOOKLISTBOX_PartDis_Conditions, (PFDialogHook)HookListBox_PartDis_Conditions },

	{ HOOKDIALOG_CaseCompare, (PFDialogHook)HookDialog_CaseCompare },
	{ HOOKCOMBOBOX_CaseComp_InstanceL, (PFDialogHook)HookComboBox_CaseComp_InstanceL },
	{ HOOKCOMBOBOX_CaseComp_InstanceR, (PFDialogHook)HookComboBox_CaseComp_InstanceR },
	{ HOOKBUTTON_CaseComp_SettingL, (PFDialogHook)HookButton_CaseComp_SettingL },
	{ HOOKBUTTON_CaseComp_SettingR, (PFDialogHook)HookButton_CaseComp_SettingR },
	{ HOOKBUTTON_CaseComp_Compare, (PFDialogHook)HookButton_CaseComp_Compare },
	{ HOOKTREE_CaseComp_CaseList, (PFDialogHook)HookTree_CaseComp_CaseList },
	{ HOOKCOMBOBOX_CaseComp_ResultType, (PFDialogHook)HookComboBox_CaseComp_ResultType },
	{ HOOKCOMBOBOX_CaseComp_ShowType, (PFDialogHook)HookComboBox_CaseComp_ShowType },
	{ HOOKCOMBOBOX_CaseComp_ShowModel, (PFDialogHook)HookComboBox_CaseComp_ShowModel },

	{ HOOKDIALOG_ClipResult, (PFDialogHook)HookDialog_ClipResult },
	{ HOOKTREE_ClipResult_ClipList, (PFDialogHook)HookTree_ClipResult_ClipList },
	{ HOOKTABPAGE_ClipResult_ResultType, (PFDialogHook)HookTabpage_ClipResult_ResultType },
	{ HOOKOPTBUTTON_ClipResult_D_ALL, (PFDialogHook)HookOptButton_ClipResult_D_ALL },
	{ HOOKOPTBUTTON_ClipResult_Z_ALL, (PFDialogHook)HookOptButton_ClipResult_Z_ALL },
	{ HOOKBUTTON_ClipResult_Clear, (PFDialogHook)HookButton_ClipResult_Clear },
	{ HOOKBUTTON_ClipResult_Submit, (PFDialogHook)HookButton_ClipResult_Submit },
	{ HOOKBUTTON_ClipResult_OutDGN, (PFDialogHook)HookButton_ClipResult_OutDGN },
	// 画面的分割条
	{ HOOKSASH_ClipResult_V, (PFDialogHook)HookSash_ClipResult_vSashHook },

	{ HOOKDIALOGID_SurfaceShow, (PFDialogHook)HookDialog_SurfaceShow },
	{ HOOKTREE_Surface_ClipList, (PFDialogHook)HookTree_Surface_ClipList },
	{ HOOKCOMBOBOX_Surface_Model, (PFDialogHook)HookComboBox_Surface_Model },
	{ HOOKCOMBOBOX_Surface_ResultType, (PFDialogHook)HookComboBox_Surface_ResultType },
	{ HOOKBUTTON_Surface_ResultShow, (PFDialogHook)HookButton_Surface_ResultShow },
	{ HOOKCOMBOBOX_SurfaceDisp_ShowType, (PFDialogHook)HookButton_Surface_ShowMinMax},
	{ HOOKPDM_Surface_Add, (PFDialogHook)HookPdm_Surface_Add },
	{ HOOKPDM_Surface_Del, (PFDialogHook)HookPdm_Surface_Del },
	{ HOOKBUTTON_Surface_ShowSet, (PFDialogHook)HookButton_Surface_ShowSet },
	// 画面的分割条
	{ HOOKSASH_Surface_V, (PFDialogHook)HookSash_Surface_vSashHook },

	{ HOOKDIALOGID_CalRebar_RebarCalculate, (PFDialogHook)HookDialog_CalRebar_InterfaceDynamic },	
	{ HOOKITEMID_CalRebar_SectionTree, (PFDialogHook)HookTree_CalRebar_SectionTree },
	{ HOOKSASH_CalRebar_RebarCalculate_V, (PFDialogHook)HookSash_CalRebar_vSashHook },
	{ HOOKPUSHBTN_CalRebar_Calculate, (PFDialogHook)HookPushBtn_CalRebar_Calculate},
	{ HOOKPUSHBTN_CalRebar_Refresh, (PFDialogHook)HookPushBtn_CalRebar_Refresh },
	{ HOOKPUSHBTN_CalRebar_OutPut, (PFDialogHook)HookPushBtn_CalRebar_OutPut },
	{ HOOKGENERIC_CalRebar_Canva, (PFDialogHook)HookGeneric_CalRebar_Canva },
	{ HOOKLISTBOX_CalRebar_CalcResult, (PFDialogHook)HookListBox_CalRebar_CalcResult },
	{ HOOKLISTBOX_CalRebar_RebarInfo, (PFDialogHook)HookListBox_CalRebar_RebarInfo},
	{ HOOKITEMID_CalRebar_CellCombbox_Diameter, (PFDialogHook) HookItem_CalRebar_CellCombbox_Diameter},
	{ HOOKITEMID_CalRebar_CellTxt_RebarNumber, (PFDialogHook)HookItem_CalRebar_CellTxt_RebarNumber },
	{ HOOKTXTID_CalRebar_RebarTesion, (PFDialogHook)HOOKText_CalRebar_RebarTesion },
	{ HOOKTXTID_CalRebar_ConcrTesion,(PFDialogHook)HOOKText_CalRebar_ConcrTesion },
	{ HOOKPDM_CalRebar_AddRebarLine, (PFDialogHook)HookPdm_CalRebar_AddRebarLine },
	{ HOOKPDM_CalRebar_DelRebarLine, (PFDialogHook)HookPdm_CalRebar_DelRebarLine },

	{ HOOKDIALOGID_CalRebar_AddRebarLine, (PFDialogHook)HookDialog_CalRebar_AddRebarLine },
	{ HOOKITEMID_Surface_LockAxis, (PFDialogHook)HookItem_LockAxis},
	{ HOOKDIALOGID_Surface_AddClipFace, (PFDialogHook)HookDialog_Surface_AddClipFace },
};

//bool CheckRegister()
//{
//	HMODULE hModule = LoadLibrary("Decode.dll");
//	bool rt = false;
//	if (hModule != NULL)
//	{
//		RegisterFunc func = (RegisterFunc)GetProcAddress(hModule, "CheckRegister");
//		if (func == NULL)
//		{
//			rt = false;
//		}
//		else
//		{
//			rt = func();
//		}
//		FreeLibrary(hModule);
//	}
//	return rt;
//}

/*----------------------------------------------------------------------------------+
* 功能描述: 工具入口															    *
* @param        argc     The count of args passed to the application			    *
* @param        argv[]   The array of args passed to this application as char arrays*
* @return																		    *
+-----------------------------------------------------------------------------------*/
extern "C" DLLEXPORT  void MdlMain(int argc, WCharCP argv[])
{
	RscFileHandle   rfHandle;
	SymbolSet       *setP;
	StatusInt		iStat;

	//if (!CheckRegister())
	//{
	//	mdlInput_sendKeyin(L"mdl unload MsdiMsCaeSi", 0, INPUTQ_EOQ, NULL);
	//	return;
	//}

	mdlResource_openFile(&rfHandle, NULL, RSC_READONLY);

	mdlState_registerStringIds(STRINGLISTID_Commands, STRINGLISTID_Prompts);

	mdlSystem_registerCommandNames(cmdNames);
	mdlSystem_registerCommandNumbers(cmdNumbers);
	mdlParse_loadCommandTable(NULL);

	/* set up the variables we are going to set from dialog boxes */
	setP = mdlCExpression_initializeSet(VISIBILITY_DIALOG_BOX, 0, FALSE);

	// 发布结构类型变量
	iStat = mdlDialog_publishComplexVariable(setP, "impInfDlgInfo", "g_ImpDlgInfo", &g_ImpDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "solidDispDlgInfo", "g_solidDispDlgInfo", &g_solidDispDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "expCalculationDlgInfo", "g_expCalculationInfo", &g_expCalculationInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "reBarCalDlgInfo", "g_rebarCalDlgInfo", &g_rebarCalDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "partialDisDlgInfo", "g_partialDisDlgInfo", &g_partialDisDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "caseCompDlgInfo", "g_caseCompDlgInfo", &g_caseCompDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "clipResultDlgInfo", "g_clipResultDlgInfo", &g_clipResultDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "surfaceShowDlgInfo", "g_surfaceShowDlgInfo", &g_surfaceShowDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "outRebarDlgInfo", "g_outRebarDlgInfo", &g_outRebarDlgInfo);
	iStat = mdlDialog_publishComplexVariable(setP, "colorSettingDlgInfo", "g_colorSettingDlgInfo", &g_colorSettingDlgInfo);
	
	// 用钩函数地址发布钩函数ID编号
	mdlDialog_hookPublish(sizeof(uHooks) / sizeof(DialogHookInfo), uHooks);

	// 设定程序卸载时要调动的函数
	SystemCallback::SetUnloadProgramFunction(UnloadFunction);

	SystemCallback::SetExitDesignFileStateFunction(ExitDesignFileFunction);

	initProgramData();
}
