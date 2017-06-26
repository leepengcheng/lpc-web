#include "MsdiMsCaeSipch.h"

extern SolidDispDlgInfo  g_solidDispDlgInfo;
extern SurfaceShowDlgInfo g_surfaceShowDlgInfo;
// CAE结果类型名
extern bmap<WString, WString> g_RstType_Name;
// CAE结果类型单位
extern bmap<WString, WString> g_RstType_Unit;

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

//仅用于本对话框的所有内容，记录用户点击树中工况的所有数据
WString	ngName, caseName, sUnit;
int		iProjectId, caseID;

int m_iCamera;// 相机状态
int m_iRenderMode;// 渲染模式

MSDialogP jdoytBar = NULL;
WCharP 	  jdwcMessageText = NULL;
int       jdPercent = 0;
bool	bConditionUpdFlg;
extern bvector<WString> vecRstTypeCodes;
extern bool alreadyUnload;

/*----------------------------------------------------------------------+
* The command entry point for user function.				            *
* @param        unparsed     unused unparsed argument to the command    *
+----------------------------------------------------------------------*/
void SetCAERstDispCmd(WCharCP unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
	if (Dlialog)
	{
		mdlDialog_show(Dlialog);
	}
	else
	{
		mdlDialog_open(NULL, DIALOGID_SolidContourDisp);
	}

}
void startShowCutFaceContour(DrawColorShape* pDCShape)
{

	ClearclipfaceShow();
	//打开色带
	pDCShape->StartDrawFunction();
	pDCShape->showType = g_surfaceShowDlgInfo.indexShowType;

	WString plotNmae = NGNAME_PREFIX_CAECLIP;
	DgnPlatform::LevelId levelId;
	if (g_surfaceShowDlgInfo.indexShowType == 0)//显示云图
	{
		// 创建或取得CAE云图的图层
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE切面云图", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE切面云图");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFacePlot, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	else if (g_surfaceShowDlgInfo.indexShowType == 1)//显示等值线图
	{
		// 创建或取得CAE云图的图层
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE切面等值线图", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE切面等值线图");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceLine, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	else if (g_surfaceShowDlgInfo.indexShowType == 2)//显示矢量图
	{
		// 创建或取得CAE云图的图层
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE切面矢量图", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE切面矢量图");
		ClipFace::ShowByType(plotNmae, ClipFace::DrawFaceVector, levelId, caseID, pDCShape->ResultTypeCode, sUnit);
	}
	//如果需要显示最大值最小值
	if (g_surfaceShowDlgInfo.bShowExtremum == -1)
	{
		bvector<DPoint3d> meshminpoints;
		bvector<DPoint3d> meshmaxpoints;
		DPoint3d minpoint;
		DPoint3d maxpoint;
		double tStrValue, tEndValue;
		GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, pDCShape->ProjectId, pDCShape->CaseID, pDCShape->ResultTypeCode, minpoint, maxpoint);

		s_pDCShape->m_vecMaxPts.clear();
		s_pDCShape->m_vecMinPts.clear();

		s_pDCShape->m_vecMaxPts.push_back(maxpoint);
		s_pDCShape->m_vecMinPts.push_back(minpoint);
		s_pDCShape->m_bExtremum = true;
	}
	else
	{
		s_pDCShape->m_bExtremum = false;
	}
	mdlLevelTable_rewrite(MASTERFILE);
}

void startShowContour(DrawColorShape* pDCShape, DPoint3dCP BasePt /*= NULL*/, bool bClear /*= true*/)
{
	//先删除之前绘制的轮廓图
	if (bClear)
		ClearContour();

	ShowCAEModel();

	//根据打开色带
	pDCShape->StartDrawFunction();

	//记录生成轮廓结果所在层ID
	LevelId lvID_contour;
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15);
	if (pDCShape->showType == 0)//显示云图
	{
		lvID_contour = showContourPlot(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
	}
	else if (pDCShape->showType == 1)//显示等值线图
	{
		lvID_contour = showContourLine(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
		showContourOutFaceLine(pDCShape->NgName, pDCShape, lvID_contour, BasePt);
	}
	else if (pDCShape->showType == 2)//显示矢量图
	{
		lvID_contour = showContourVector(pDCShape->NgName, pDCShape->CaseID, pDCShape->ResultTypeCode, pDCShape, BasePt);
		showContourOutFaceLine(pDCShape->NgName, pDCShape, lvID_contour, BasePt);
	}
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 90);
	//如果需要显示最大值最小值
	if (s_pDCShape->m_bExtremum)
	{
		//综合偏移值得到真实坐标并在此处绘制标识
		DPoint3d basePt;
		WString sUnit;
		LevelId  lvID;
		MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, lvID, pDCShape->NgName);

		//根据色带的DbStartValue和DbEndValue检索数据库获取对应的Node坐标
		DPoint3d ptMax = getNodeCoordinate(pDCShape->ProjectId, pDCShape->CaseID, ws2s(pDCShape->ResultTypeCode.data()).data(), pDCShape->DbEndValue - 0.000001);
		ptMax.Scale(UOR_PER_UNIT(sUnit));
		DPoint3d ptMin = getNodeCoordinate(pDCShape->ProjectId, pDCShape->CaseID, ws2s(pDCShape->ResultTypeCode.data()).data(), pDCShape->DbStartValue);
		ptMin.Scale(UOR_PER_UNIT(sUnit));

		ptMax.Add(basePt);
		ptMin.Add(basePt);

		pDCShape->m_vecMaxPts.clear();
		pDCShape->m_vecMinPts.clear();

		pDCShape->m_vecMaxPts.push_back(ptMax);
		pDCShape->m_vecMinPts.push_back(ptMin);

	}

	mdlLevelTable_rewrite(MASTERFILE);

	//将视图聚焦到对应的CAE模型位置
	focusOnCAEModel(pDCShape->NgName);
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 100);
	mdlDialog_completionBarClose(jdoytBar);
}

void HookSash_SolidDisp_vSashHook
(
DialogItemMessage   *dimP    /* => a ptr to a dialog item message */
)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.buttonTrans == BUTTONTRANS_DOWN)
		{
			BSIRect     contentRect;
			mdlWindow_contentRectGetLocal(&contentRect, (MSWindowP)dimP->db);
		}
		else if (dimP->u.button.buttonTrans == BUTTONTRANS_UP)
		{
			Sash_ButtonUpAuxInfo *buaiP = (Sash_ButtonUpAuxInfo *)dimP->auxInfoP;

			/* use buaiP->newXPos to determine where upperLeft corner
			of sash beveled rect will go.  This message is sent after
			sash has been erased from old position & moved, but before
			it has been drawn */
			if (buaiP->newXPos != buaiP->oldXPos)
				SolidDisp_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_SolidDisp(DialogItemMessage *dimP)
{
	RawItemHdr      *riP = dimP->dialogItemP->rawItemP;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		GuiTreeNode     *pRoot;
		MSValueDescr     valueDescr;

		GuiTreeModel* pModel = mdlTreeModel_create(1);

		pRoot = mdlTreeModel_getRootNode(pModel);
		mdlTreeNode_setDisplayText(pRoot, L"ROOT");

		NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
		for each (NamedGroupPtr namedGroup in *ngcCAESolid)
		{
			WString ngName = namedGroup->GetName();

			WString instanceName;
			if (splitNgName_Model(ngName, UNUSE_WSTRING, iProjectId, UNUSE_INT, instanceName, sUnit))
			{
				//创建实例节点
				GuiTreeNode*	pProjectNode = NULL;
				pProjectNode = mdlTreeNode_create(pModel, TRUE);

				//设置节点的显示内容
				mdlTreeNode_setDisplayText(pProjectNode, instanceName.data());

				//将对应的命名组名保存
				MSValueDescr    valueDescr;
				valueDescr.SetWChar(ngName.GetWCharCP());
				mdlTreeNode_setValue(pProjectNode, valueDescr, FALSE);

				mdlTreeNode_setCollapsedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
				mdlTreeNode_setExpandedSelectedIcon(pProjectNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//将节点添加到根节点下
				mdlTreeNode_addChild(pRoot, pProjectNode);

				bvector<int> vecCaseIDs;
				bvector<WString> vecCaseNames;
				bvector<WString> vecCaseProps;
				getCaseInfoByProjectid(iProjectId, vecCaseIDs, vecCaseNames, vecCaseProps);
				for (int i = 0; i < vecCaseNames.size(); i++)
				{
					// 设置工况节点显示内容
					WString caseName = vecCaseNames.at(i);
					GuiTreeNode*	pCaseNode = NULL;
					pCaseNode = mdlTreeNode_create(pModel, FALSE);
					mdlTreeNode_setDisplayText(pCaseNode, caseName.c_str());

					// 设置工况节点值
					MSValueDescr    valueDescr;
					valueDescr.SetInt32(vecCaseIDs.at(i));
					mdlTreeNode_setValue(pCaseNode, valueDescr, FALSE);

					mdlTreeNode_setLeafIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

					// 添加节点到控件中
					mdlTreeNode_addChild(pProjectNode, pCaseNode);
				}
			}
		}

		mdlTreeNode_expand(pRoot);

		mdlDialog_treeSetTreeModelP(riP, pModel);
		break;
	}
	case DITEM_MESSAGE_STATECHANGED://点击工况后响应
	{
		//点击tree中结点后，需要更新当前的内容
		wcscpy(g_solidDispDlgInfo.current, L"未选择工况");

		//缺省将部分控件设不可用，当选择了工况后将其设为可用
		enableWidget(dimP->db, false);

		GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(riP);
		int	numSelect = -1;
		Point2d* selectP = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, riP) && numSelect != 0)
		{
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
			treeNode = mdlTreeCell_getNode(treeCell);

			//根据用户用户选择的结点更新“当前选择”的内容
			wcscpy(g_solidDispDlgInfo.current, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());

			//若该节点已经没有子节点了认为选择到了工况节点
			if (!mdlTreeNode_getAllowsChildren(treeNode))
			{
				MSValueDescr  nodeVal;

				WString ngNameNew;
				mdlTreeNode_getValue(mdlTreeNode_getParent(treeNode), nodeVal);
				nodeVal.GetWString(ngNameNew);

				//若此次选择的实例名与上一次的不一致,则进行如下操作
				if (ngName.CompareTo(ngNameNew) != 0)
				{
					//关闭上一个实例的云图显示并关闭色带（若存在的话）
					ShowCAEModel();
					ClearContour();
					s_pDCShape->CloseDrawFunction();
					s_drawtempLine.CloseDrawFunction();
					//聚焦到新实例位置
					focusOnCAEModel(ngNameNew);

					//清空部分显示筛选条件
					DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
					ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
					mdlListModel_empty(pListModel, true);
					mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);

					//重新加载对应的条件值
					initConditionValuesByTypes(dimP->db);
				}
				//CaseID
				mdlTreeNode_getValue(treeNode, nodeVal);
				s_pDCShape->CaseID = nodeVal.GetInt32();

				// 根据项目ID和工况ID，设置结果类型
				SetResultType(dimP, COMBOBOXID_ResultType, iProjectId, s_pDCShape->CaseID, vecRstTypeCodes);

				//当结果类型发生变化时，需要重新计算色带值
				s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_solidDispDlgInfo.ResultType);

				//将部分控件设为可用
				enableWidget(dimP->db, true);
				dimP->db->Refresh();

				//根据选择的工况节点数据赋值给色带的成员变量
				//Header
				s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
				//Unit
				s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

				//showType
				s_pDCShape->showType = g_solidDispDlgInfo.ShowType;

				//ProjectName & ProjectId
				splitNgName_Model(ngNameNew, s_pDCShape->ProjectName, s_pDCShape->ProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

				//CaseName
				mdlTreeNode_getDisplayTextWString(treeNode, s_pDCShape->CaseName);

				//NgName
				s_pDCShape->NgName = ngNameNew;

				//重置为自动设置色带
				s_pDCShape->m_bAutomatic = true;
				//根据新实例计算色带的最大值最小值
				s_pDCShape->initColorRangeFromDB();
				//自动等分色带的值
				s_pDCShape->divideColorRange();

				ngName = ngNameNew;
			}
		}
		mdlDialog_itemsSynch(dimP->db);

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void SolidDisp_adjustVSashDialogItems
(
MSDialogP   db,
BSIRect     *pOldContent,
bool        refreshItems
)
{
	DialogItem    *sashDiP;
	DialogItem    *treeDiP;
	DialogItem    *ctPanelDiP;

	/* Get the 3 main items on the dialog */
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_SolidDis_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_SolidDis_ProjList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_SolidDis_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookDialog_SolidDisp(DialogMessage *dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_CREATE:
	{
		dmP->u.create.interests.windowMoving = TRUE;
		dmP->u.create.interests.resizes = TRUE;
		dmP->u.create.interests.updates = TRUE;
		break;
	}
	case DIALOG_MESSAGE_UPDATE:
	{
		DialogItem    *diP;

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_SolidDis_V, 0);
		if (!diP)
			break;

		SolidDisp_adjustVSashDialogItems(dmP->db, NULL, TRUE);
		break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING:
	{
		int        minSize = (30 * XC);

		/* Don't process if only moving dialog box */
		if (dmP->u.windowMoving.whichCorners == CORNER_ALL ||
			dmP->u.windowMoving.whichCorners == CORNER_ALL_RESIZED)
			break;

		/* Minimum size for dialog */
		if (dmP->u.windowMoving.newWidth < minSize)
			dmP->u.windowMoving.newWidth = minSize;
		if (dmP->u.windowMoving.newHeight < minSize)
			dmP->u.windowMoving.newHeight = minSize;

		dmP->u.windowMoving.handled = TRUE;

		break;
	}

	case DIALOG_MESSAGE_RESIZE:
	{
		BSIRect     oldContent;

		/* Don't process if only moving dialog box */
		if (dmP->u.resize.whichCorners == CORNER_ALL)
			break;

		mdlWindow_pointToLocal(&oldContent.origin, (MSWindowP)dmP->db, &dmP->u.resize.oldContent.origin);
		mdlWindow_pointToLocal(&oldContent.corner, (MSWindowP)dmP->db, &dmP->u.resize.oldContent.corner);

		SolidDisp_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		// 关闭视图的相机并保存相机状态
		ViewFlags flags;
		mdlView_getFlags(&flags, tcb->lstvw);
		m_iCamera = flags.camera;// 保存视图的相机状态
		m_iRenderMode = flags.renderMode; // 保存视图的渲染模式
		flags.camera = 0;// 关闭视图的相机
		bConditionUpdFlg = false;
		mdlView_setFlags(&flags, tcb->lstvw);

		//缺省将部分控件设不可用，当选择了工况后将其设为可用
		enableWidget(dmP->db, false);

		//初始化控件显示
		wcscpy(g_solidDispDlgInfo.current, L"未选择工况");
		g_solidDispDlgInfo.ResultType = g_solidDispDlgInfo.ShowType = 0;
		//默认以光滑模式显示
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);

		mdlDialog_itemsSynch(dmP->db);

		//此功能使用主色带
		s_pDCShape = &s_DrawColorShape;

	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		// MS主程序关闭时，该处理不执行
		if (alreadyUnload != true)
		{
			ClearContour();
			ShowCAEModel();
			ClearBoundary();

			s_pDCShape->CloseDrawFunction();
			s_drawtempLine.CloseDrawFunction();
			// 复原视图设置
			ViewFlags flags;
			mdlView_getFlags(&flags, tcb->lstvw);
			flags.camera = m_iCamera;
			flags.renderMode = m_iRenderMode;
			mdlView_setFlags(&flags, tcb->lstvw);

			//以网格模式显示
			ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// 网格
		}
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_ShowSet(DialogItemMessage *dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		openColorShapeSetting(s_pDCShape, SourceTypeDCS::SolidDisplay);
		break;
	};
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_ResultType(DialogItemMessage *dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		//当结果类型发生变化时，需要重新计算色带值
		s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(g_solidDispDlgInfo.ResultType);

		//Header
		s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
		//Unit
		s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];


		s_pDCShape->m_bAutomatic = true;

		s_pDCShape->initColorRangeFromDB();
		s_pDCShape->divideColorRange();
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
void HookComboBox_SolidDisp_ShowModel(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
		g_solidDispDlgInfo.ShowModel = 0;
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// 光滑
		break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_solidDispDlgInfo.ShowModel == 1)
			ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// 网格
		else
			ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// 光滑	
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
void HookComboBox_SolidDisp_CondType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_solidDispDlgInfo.indexCType = g_solidDispDlgInfo.indexCOpt = g_solidDispDlgInfo.indexCValue = 0;
	}
	break;
	case DITEM_MESSAGE_INIT:
	case DITEM_MESSAGE_STATECHANGED:
	{
		//当“条件类型”初始化和更新时，查询数据库中可能的值重新为“条件值”赋值
		initConditionValuesByTypes(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_CondVal(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		break;
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	case DITEM_MESSAGE_INIT:
	{

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_AddC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//根据对应comboBox中的内容，向表格中插入对应数据并保存其实际的内容
		DialogItem* dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		ListModel*	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
		if (pListModel == NULL)
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);

		ListRowP	pRow = mdlListRow_create(pListModel);
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		//通过资源文件中StringList定义获取comboxBox的显示值
		RscFileHandle   rfHandle;
		mdlResource_openFile(&rfHandle, L"partialDisplayDlg.r", RSC_READONLY);
		StringListP sl = NULL;
		WString		wsCBoxValue;

		//筛选条件，保存其下标
		cell = mdlListRow_getCellAtIndex(pRow, 0);

		sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_ConditionType);
		mdlStringList_getMemberString(wsCBoxValue, NULL, sl, (long)g_solidDispDlgInfo.indexCType);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());

		cellValue.SetInt32(g_solidDispDlgInfo.indexCType);
		mdlListCell_setValue(cell, cellValue, FALSE);

		//筛选操作符，保存其下标
		cell = mdlListRow_getCellAtIndex(pRow, 1);

		sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_Condition);
		mdlStringList_getMemberString(wsCBoxValue, NULL, sl, (long)g_solidDispDlgInfo.indexCOpt);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());

		cellValue.SetInt32(g_solidDispDlgInfo.indexCOpt);
		mdlListCell_setValue(cell, cellValue, FALSE);

		//筛选条件值，保存其实际ID
		ListModel*	pListModel_CValue = mdlDialog_comboBoxGetListModelP(mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0)->rawItemP);
		cell = mdlListModel_getCellAtIndexes(pListModel_CValue, g_solidDispDlgInfo.indexCValue, 0);
		mdlListCell_getDisplayTextWString(cell, wsCBoxValue);
		mdlListCell_getValue(cell, cellValue);

		cell = mdlListRow_getCellAtIndex(pRow, 2);
		mdlListCell_setDisplayText(cell, wsCBoxValue.data());
		mdlListCell_setValue(cell, cellValue, FALSE);

		mdlListModel_addRow(pListModel, pRow);

		mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
		mdlDialog_itemsSynch(dimP->db);

		bConditionUpdFlg = true;
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_DelC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//删除列表中的选中行条件
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		int row = -1, col = -1;
		mdlDialog_listBoxGetLocationCursor(&row, &col, dip_cList->rawItemP);

		if (row != -1)
		{
			ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
			mdlListModel_removeRowAtIndex(pListModel, row, true);
			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
			mdlDialog_itemsSynch(dimP->db);

			bConditionUpdFlg = true;
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_SolidDisp_ClearC(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//清空列表中的所有筛选条件
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
		ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);

		if (pListModel != NULL && mdlListModel_getRowCount(pListModel) > 0)
		{
			mdlListModel_empty(pListModel, true);
			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
			mdlDialog_itemsSynch(dimP->db);

			bConditionUpdFlg = true;
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// 体展示->结果查看
void HookButton_SolidDisp_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		MSDialog *db = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
		if (db == NULL)
			return;
		//缺省将部分控件设不可用，当选择了工况后将其设为可用
		enableWidget(dimP->db, false);

		//进度条
		size_t len = strlen(TXT_CAEShowIng) + 1;
		jdwcMessageText = (WCharP)malloc(len*sizeof(WChar));
		BeStringUtilities::CurrentLocaleCharToWChar(jdwcMessageText, TXT_CAEShowIng, len);
		// 打开进度条
		jdoytBar = mdlDialog_completionBarOpen(jdwcMessageText);

		if (bConditionUpdFlg == true)
		{
			//根据云图部分展示条件修改数据库中的标志位
			stringstream strSql;
			Db m_db;

			if (OpenDB(m_db) != BE_SQLITE_OK)
				return;

			Statement *m_statment = new Statement();
			Savepoint *m_savept = new Savepoint(m_db, "asd", true);
			m_savept->Begin();

			DialogItem* dip_cList = mdlDialog_itemGetByTypeAndId(db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
			ListModel*	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
			int rowCnt;
			if (pListModel != NULL && (rowCnt = mdlListModel_getRowCount(pListModel)) != 0)
			{
				MSValueDescr value;

				//先将数据库中的所有ContourShowFlg置为0
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 0 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName ");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				Utf8String sNgName;
				BeStringUtilities::WCharToUtf8(sNgName, ngName.c_str());
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();

				//遍历所有的筛选条件拼接成update sql语句将部分ContourShowFlg置为1
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 1 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND EXISTS(SELECT a.ElemID FROM ELEMENTS a WHERE a.ProjectId = @ProID AND  a.ElemID = FACES.ElemID ");
				for (int i = 0; i < rowCnt; i++)
				{
					ListCellP cell;
					strSql << (" AND ");

					//筛选条件名
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 0);
					mdlListCell_getValue(cell, value);
					strSql << (value.GetInt32() == 0 ? "a.ElemType" : (value.GetInt32() == 1 ? "a.MaterialNumber" : "a.RealConstantNumber"));

					//筛选操作符
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 1);
					mdlListCell_getValue(cell, value);
					strSql << (value.GetInt32() == 0 ? "=" : "<>");

					//筛选值ID
					cell = mdlListModel_getCellAtIndexes(pListModel, i, 2);
					mdlListCell_getValue(cell, value);
					strSql << value.GetInt32();
				}
				strSql << (")");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();
			}
			else
			{
				//清除数据库中的所有筛选条件
				strSql.str("");
				strSql << ("UPDATE FACES SET ContourShowFlg = 1 WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName ");
				m_statment->Prepare(m_db, strSql.str().data());
				m_statment->BindInt(1, iProjectId);
				Utf8String sNgName;
				BeStringUtilities::WCharToUtf8(sNgName, ngName.c_str());
				m_statment->BindUtf8CP(2, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
				m_statment->Step();
				m_statment->Finalize();
			}

			m_savept->Commit();

			delete m_savept;
			delete m_statment;

			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 5);
			m_db.CloseDb();

			//自动设置时，重新计算色带的值
			if (s_pDCShape->m_bAutomatic == true)
			{
				//根据新实例计算色带的最大值最小值
				s_pDCShape->initColorRangeFromDB();
				//自动等分色带的值
				s_pDCShape->divideColorRange();
			}
			bConditionUpdFlg = false;
		}

		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 10);
		//根据色带设置显示轮廓
		startShowContour(s_pDCShape);
		jdoytBar = NULL;
		jdwcMessageText = L"";
		jdPercent = 0;

		//缺省将部分控件设不可用，当选择了工况后将其设为可用
		enableWidget(dimP->db, true);

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_Damage_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{

		MSDialog *db = mdlDialog_find(DIALOGID_SolidContourDisp, NULL);
		if (db == NULL)
			return;
		//缺省将部分控件设不可用，当选择了工况后将其设为可用
		enableWidget(dimP->db, false);

		int ProjectId;
		bvector<ElementId> shows;

		NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
		for each (NamedGroupPtr namedGroup in *ngcCAESolid)
		{

			WString ngName = namedGroup->GetName();
			splitNgName_Model(ngName, UNUSE_WSTRING, ProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

			NamedGroupPtr namedGroup = NamedGroupCollection::Create(*ACTIVEMODEL)->FindByName(ngName.data());
			if (namedGroup.IsValid())
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);

				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();

					EditElementHandle eeh;
					eeh.FindByID(elemid, ACTIVEMODEL);

					XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
					XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
					MeshProperty *meshPro = NULL;

					if (xh.IsValid())
						meshPro = (MeshProperty *)xh.PeekData();

					if (meshPro != NULL)
					{
						shows.push_back(elemid);
					}

				}
			}

			DgnModelRefP target = NULL;
			if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), ProjectId, TRUE, TRUE))
			{
				if (!mdlModelRef_areSame(MASTERFILE, target))
				{
					mdlModelRef_activateAndDisplay(target);
				}
			}
			Db m_db;
			StatusInt rc = OpenDB(m_db);
			if (rc == BE_SQLITE_OK)
			{
				Savepoint *m_savept = new Savepoint(m_db, "asd", true);
				m_savept->Begin();
				Statement *m_statment = new Statement();
				m_statment->BindInt(1, ProjectId);
				AString lsSql;
				//把ProjectId格式化
				char id[50];
				sprintf(id,"%d", ProjectId);
				lsSql.append("select ElemId from damage where damage.ProjectId=");
				lsSql.append(id);
				lsSql.append("  and damage.Status=0;");
				rc = m_statment->Prepare(m_db, lsSql.c_str());
			//	rc = m_statment->Prepare(m_db, "select ElemId from damage where damage.ProjectId=0  and damage.Status=0;");
				if (rc == BE_SQLITE_OK)
				{
					DgnModelRefP target = NULL;
					mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), ProjectId, TRUE, TRUE);
					SelectionSetManagerR selectSetM = SelectionSetManager::GetManager();
					while (m_statment->Step() == BE_SQLITE_ROW)
					{
						int eid = m_statment->GetValueInt(0);
						for (auto iter = shows.begin(); iter != shows.end(); iter++)
						{
							EditElementHandle eeh;
							eeh.FindByID(*iter, ACTIVEMODEL);

							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
							XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
							MeshProperty *meshPro = NULL;

							if (xh.IsValid())
								meshPro = (MeshProperty *)xh.PeekData();

							if (meshPro != NULL)
							{
								if (eid == meshPro->ElemID)
								{	
									selectSetM.AddElement(eeh.GetElementRef(), MASTERFILE);
									break;
								}
							}

						}
					}
					ElementAgenda eAgenda;
					selectSetM.BuildAgenda(eAgenda);
					eAgenda.Hilite();
					mdlModelRef_freeWorking(target);
				}
			}
			mdlModelRef_freeWorking(target);
		}

	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_SolidDisp_ShowType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		s_pDCShape->showType = g_solidDispDlgInfo.ShowType;
		s_pDCShape->m_bExtremum = g_solidDispDlgInfo.bShowExtremum == -1 ? true : false;

		DialogItemP diP_Widget = NULL;
		bool bEnable = false;

		// 只在云图显示时，剖面查看和节点查看功能才能使用
		if (g_solidDispDlgInfo.ShowType == 0)
			bEnable = true;

		//云图剖面查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, bEnable, false);

		//节点查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, bEnable, false);

		//mdlDialog_itemGetState

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void enableWidget(MSDialogP db, bool enable)
{
	DialogItemP diP_Widget = NULL;

	//部分展示相关控件
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionType, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionOpt, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_AddC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_DelC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_SolidDisp_ClearC, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ListBox, LISTBOXID_SolidDisp_ConditionList, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

	//显示云图
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_Show, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	//破坏显示
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_Damage, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	

	// 只在云图显示时，剖面查看和节点查看功能才能使用
	if (g_solidDispDlgInfo.ShowType == 0)
	{
		//云图剖面查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

		//节点查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
	}
	else
	{
		//云图剖面查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ClipPlot, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, false, false);

		//节点查看
		diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_NodeVal, 0);
		mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, false, false);
	}

	//显示类型
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_ShowType, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);

	//设置
	diP_Widget = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, BUTTONID_ShowSet, 0);
	mdlDialog_itemSetEnabledState(db, diP_Widget->itemIndex, enable, false);
}

void initConditionValuesByTypes(MSDialogP db)
{
	DialogItem* dip_cValue = mdlDialog_itemGetByTypeAndId(db, RTYPE_ComboBox, COMBOBOXID_SolidDisp_ConditionValue, 0);
	if (dip_cValue == NULL)
		return;
	RawItemHdr*	rItem_cValue = dip_cValue->rawItemP;
	ListModel*	pListModel = mdlDialog_comboBoxGetListModelP(rItem_cValue);

	if (pListModel == NULL)
		mdlListModel_create(MAX_COLUMN_COUNT);
	else
		mdlListModel_empty(pListModel, true);

	//根据当前选取的实例对应的工程ID获取其筛选条件值ID,并根据ID获取其对应的名称
	bvector<int> vecValueIDs;
	bvector<WString> vecValueNames;
	switch (g_solidDispDlgInfo.indexCType)
	{
		//单元类型
	case 0:
	{
		getElemTypeIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString elemTypeName;
			getElemNameByPK(iProjectId, vecValueIDs[i], elemTypeName);
			vecValueNames.push_back(elemTypeName);
		}
	}break;
	//材料类型
	case 1:
	{
		getMaterialIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString mName, mType;
			getMaterialAllByPK(iProjectId, vecValueIDs[i], mName, mType, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING);

			//Type-Name拼成材料属性
			if (mType.length() > 0)
				mType.append(L"-");
			mType.append(mName);
			vecValueNames.push_back(mType);
		}
	}break;
	//实常数
	case 2:
	{
		getRealConsIDByProjectid(iProjectId, vecValueIDs);
		for (int i = 0; i < (int)vecValueIDs.size(); i++)
		{
			WString realConsProp;
			getRealConsPropByPK(iProjectId, vecValueIDs[i], realConsProp);
			vecValueNames.push_back(realConsProp);
		}
	}break;
	default:
		break;
	}

	if (vecValueIDs.empty())
		return;

	//遍历筛选条件值和名称
	ListRowP	pRow = NULL;
	ListCellP	cell = NULL;
	MSValueDescr cellValue;

	for (int i = 0; i < (int)vecValueIDs.size(); i++)
	{
		pRow = mdlListRow_create(pListModel);
		cell = mdlListRow_getCellAtIndex(pRow, 0);

		//条件名称作为显示内容
		mdlListCell_setDisplayText(cell, vecValueNames[i].data());

		//对应ID存储在其Value中			
		cellValue.SetInt32(vecValueIDs[i]);
		mdlListCell_setValue(cell, cellValue, FALSE);
		mdlListModel_addRow(pListModel, pRow);
	}

	mdlDialog_comboBoxSetListModelP(rItem_cValue, pListModel);

	g_solidDispDlgInfo.indexCValue = 0;

	mdlDialog_itemsSynch(db);
}


BENTLEY_NAMESPACE_NAME::StatusInt GetBoundarySolidSQL(Statement* pStatement, BeSQLiteDbCR db)
{
	AString strSql;
	strSql.append("SELECT \n");
	strSql.append(" a.ElemID,BCType,MaterialNumber,ElemType,RealConstantNumber, \n");
	strSql.append(" ID1,X1,Y1,Z1,ID2,X2,Y2,Z2,ID3,X3,Y3,Z3,ID4,X4,Y4,Z4 \n");
	strSql.append("FROM \n");
	strSql.append(" (SELECT ElemID, f.BCType as BCType,  \n");
	strSql.append(" node1.ID as ID1, node1.X as X1, node1.Y as Y1, node1.Z as Z1, \n");
	strSql.append(" node2.ID as ID2, node2.X as X2, node2.Y as Y2, node2.Z as Z2, \n");
	strSql.append(" node3.ID as ID3, node3.X as X3, node3.Y as Y3, node3.Z as Z3, \n");
	strSql.append(" node4.ID as ID4, node4.X as X4, node4.Y as Y4, node4.Z as Z4  \n");
	strSql.append(" FROM \n");
	strSql.append(" (SELECT FACES.ElemID,Node1,Node2,Node3,Node4,BCType FROM FACES INNER JOIN BOUNDARY ON FACES.NodeIdStr = BOUNDARY.NodeIdStr WHERE FACES.ProjectId = @ProjectId GROUP BY BOUNDARY.NodeIdStr HAVING COUNT(BOUNDARY.NodeIdStr) < 2) f \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node1 ON f.node1 = node1.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node2 ON f.node2 = node2.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node3 ON f.node3 = node3.ID \n");
	strSql.append(" LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node4 ON f.node4 = node4.ID ) a \n");
	strSql.append("LEFT OUTER JOIN \n");
	strSql.append(" (SELECT ElemID,MaterialNumber,ElemType,RealConstantNumber FROM ELEMENTS WHERE ProjectId = @ProjectId) b \n");
	strSql.append("ON a.ElemID=b.ElemID \n");

	return pStatement->Prepare(db, strSql.c_str());
}

void BuildBoundaryFaces(int projectId, int caseId, WCharCP ngName)
{
	// 创建或取得边界信息的图层
	DgnPlatform::LevelId levelId_boundary = 0;
	if (SUCCESS != mdlLevel_create(&levelId_boundary, MASTERFILE, L"BOUNDARY", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_boundary, MASTERFILE, LEVEL_NULL_ID, L"BOUNDARY");
	mdlLevel_setActive(levelId_boundary);
	
	//打开数据库
	Db db;
	if (BE_SQLITE_OK != OpenDB(db)) return;

	Statement* pStatement = new Statement();
	if (BE_SQLITE_OK != GetBoundarySolidSQL(pStatement,db)) return;
	// 设置查询条件
	pStatement->BindInt(1, projectId);

	//获取basePt
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);

	// 循环CAE单元体数据，在MS中生成单元体
	MSElementDescrP resultEdp;
	while (pStatement->Step() == BE_SQLITE_ROW)
	{
		bvector<int> vecNodeIds;			// Mesh体相对应的CAE结果节点集合
		bvector<int> vecIndices;			// Mesh体的节点集合
		bvector<DPoint3d> vecPoints;		// Mesh体的顶点坐标集合
		bvector<WString> vecFaceNodeStr;

		// 取得CAE的Mesh体的节点和顶点坐标集合
		GetBoundaryFacesInfo(vecIndices, vecPoints, vecFaceNodeStr, vecNodeIds, &basePt, sUnit, pStatement);
		// 当节点数>=3，生成单元面或单元体
		if (vecPoints.size() >= 3)
		{
			//生成三角面
			MSElementDescrP edpFace = NULL;
			if (SUCCESS == mdlMesh_newVariablePolyface(&resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
			{
				// 生成Mesh体的ElementHandle
				EditElementHandle eeh(resultEdp, true, true, ACTIVEMODEL);

				ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
				//边界类型(2,3,4,5,6,7,8,10,12,14,20,24,31,36,37)
				propsSetter->SetColor(5);
				propsSetter->SetLevel(levelId_boundary);
				propsSetter->Apply(eeh); 
				// ElementHandle添加到视图中
				eeh.AddToModel();
			}
		}
	}

	pStatement->Finalize();

	db.CloseDb();
	mdlLevelTable_rewrite(MASTERFILE);
	MsdiMsCaeSiUtility::fitView(0);
	//mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);

	//提取外轮廓
	//ShowOuterContour(projectId,caseId,levelId_boundary,basePt);
}

void GetBoundaryFacesInfo(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt, WStringCR sUnit, Statement* pStatement)
{
	int iIdx = 5;
	// 面的节点ID
	bvector<int> vecNodes;
	for (int i = 0; i < 4; i++)
	{
		int iNodeID = pStatement->GetValueInt(iIdx);
		if (iNodeID < 1) continue;

		auto l_it = find(vecNodes.begin(), vecNodes.end(), iNodeID);
		if (l_it == vecNodes.end())
		{
			vecNodes.push_back(iNodeID);
			vecPoints.push_back(GetPointFromBasePoint(basePt, sUnit, iIdx, pStatement));
		}
		iIdx += 4;
	}
	if (vecNodes.size() == 4)
	{
		//数量为4时，四边矩形面
		MakeBoundaryFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 2, 1);
	}
	else if (vecNodes.size() == 3)
	{
		//数量为3时，三角面
		MakeBoundaryFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 2, 1, -1);
	}
	bvector<int>().swap(vecNodes);
}

DPoint3d GetPointFromBasePoint(DPoint3dCP basePt, WStringCR sUnit, int iIdx, Statement* pStatement)
{
	DPoint3d ptTmp;

	ptTmp.x = basePt->x + pStatement->GetValueDouble(iIdx + 1) *UOR_PER_UNIT(sUnit);
	ptTmp.y = basePt->y + pStatement->GetValueDouble(iIdx + 2) *UOR_PER_UNIT(sUnit);
	ptTmp.z = basePt->z + pStatement->GetValueDouble(iIdx + 3) *UOR_PER_UNIT(sUnit);

	return ptTmp;
}

void MakeBoundaryFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4)
{
	WString sNodeIdKey;
	int iNodeId1 = -1, iNodeId2 = -1, iNodeId3 = -1, iNodeId4 = -1;
	iNodeId1 = vecNodes.at(node1 - 1);
	iNodeId2 = vecNodes.at(node2 - 1);
	iNodeId3 = vecNodes.at(node3 - 1);
	if (node4 > 0)
		iNodeId4 = vecNodes.at(node4 - 1);

	vecIndices.push_back(node1);
	vecIndices.push_back(node2);
	vecIndices.push_back(node3);
	if (node4 > 0)
		vecIndices.push_back(node4);
	vecIndices.push_back(0);

	vecNodeId.push_back(iNodeId1);
	vecNodeId.push_back(iNodeId2);
	vecNodeId.push_back(iNodeId3);
	if (node4 > 0)
		vecNodeId.push_back(iNodeId4);
	vecNodeId.push_back(0);

	sNodeIdKey = MakeNodeIdKey(iNodeId1, iNodeId2, iNodeId3, iNodeId4);
	vecFaceNodeStr.push_back(sNodeIdKey);
}

BENTLEY_NAMESPACE_NAME::WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4)
{
	WString sKey;
	int iTmp;

	if (NodeId4 != -1)
	{
		if (NodeId1 < NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1 < NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId1 < NodeId4) { iTmp = NodeId1; NodeId1 = NodeId4; NodeId4 = iTmp; }
		if (NodeId2 < NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2 < NodeId4) { iTmp = NodeId2; NodeId2 = NodeId4; NodeId4 = iTmp; }
		if (NodeId3 < NodeId4) { iTmp = NodeId3; NodeId3 = NodeId4; NodeId4 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d-%d", NodeId1, NodeId2, NodeId3, NodeId4);
	}
	else
	{
		if (NodeId1 < NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1 < NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2 < NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d", NodeId1, NodeId2, NodeId3);
	}

	return sKey;
}

//获取ngName后执行边界显示
void HookButton_Boundary_Show(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{    
	case DITEM_MESSAGE_BUTTON:
	{
		WString ngName;
		DialogItem*   pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_SolidDis_ProjList, 0);
		if (pTree)
		{
			GuiTreeModel* pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			int	numSelect = -1;
			Point2d* selectP = NULL;
			if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, pTree->rawItemP) && numSelect != 0)
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, selectP[0].y, selectP[0].x);
				treeNode = mdlTreeCell_getNode(treeCell);

				//若该节点已经没有子节点了认为选择到了工况节点
				MSValueDescr  nodeVal;
				if (!mdlTreeNode_getAllowsChildren(treeNode))
				{
					mdlTreeNode_getValue(mdlTreeNode_getParent(treeNode), nodeVal);
				}
				else
				{
					mdlTreeNode_getValue(treeNode, nodeVal);
				}
				nodeVal.GetWString(ngName);
			}
			if (!ngName.empty())
			{
				bvector<WString> names;
				int projectId, caseId;
				splitNgName_Model(ngName, UNUSE_WSTRING, projectId, caseId, UNUSE_WSTRING, UNUSE_WSTRING);
				BuildBoundaryFaces(iProjectId,caseId, ngName.data());

			}
		}
		break;
	}

	default:
		break;
	}
}

void ClearBoundary()
{
	//清除边界显示的临时信息
	DgnPlatform::LevelId levelId;
	// 取得CAE云图的图层
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"BOUNDARY"))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}
}

////提取轮廓线代码
//#pragma region    
//bool GetFaceInfos(int projectId, int caseId, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt)
//{
//	Db db;
//	Statement *l_statment;
//	WString strSql;
//	StatusInt rc;
//
//	strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z ");
//	strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z ");
//	strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z ");
//	strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z ");
//	strSql.append(L"\nFROM (SELECT ProjectId,Node1,Node2,Node3,Node4 FROM FACES WHERE ProjectId = @ProjectId GROUP BY NodeIdStr HAVING COUNT(NodeIdStr) < 2) a \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n1 ON a.ProjectId = n1.ProjectId AND a.Node1 = n1.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n2 ON a.ProjectId = n2.ProjectId AND a.Node2 = n2.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n3 ON a.ProjectId = n3.ProjectId AND a.Node3 = n3.ID \n");
//	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n4 ON a.ProjectId = n4.ProjectId AND a.Node4 = n4.ID \n");
//
//	rc = OpenDB(db);
//	if (rc == BE_SQLITE_OK)
//	{
//		l_statment = new Statement();
//
//		size_t len = strSql.length() + 1;
//		CharP cSQL = (CharP)malloc(len*sizeof(char));
//		strSql.ConvertToLocaleChars(cSQL);
//
//		rc = l_statment->Prepare(db, cSQL);
//
//		l_statment->BindInt(1, projectId);
//		l_statment->BindInt(2, caseID);
//
//		if (rc == BE_SQLITE_OK)
//		{
//			bvector<ContourPoint>	FacePoints;
//
//			while (l_statment->Step() == BE_SQLITE_ROW)
//			{
//				FacePoints.clear();
//				MyPushback(FacePoints, l_statment, basePt);
//
//				vecFaces.push_back(FacePoints);
//			}
//
//			bvector<ContourPoint>().swap(FacePoints);
//		}
//
//		l_statment->Finalize();
//
//		return true;
//	}
//	else
//		return false;
//}
//
//void ShowOuterContour(int projectId, int caseId, int levelId, DPoint3d basePt)
//{
//	bvector<bvector<ContourPoint>> vecFaces;
//
//	// 取得CAE的所有Mesh面集合和面节点集合
//	if (true == GetFaceInfos(projectId, caseId, vecFaces, basePt))
//	{
//		map<std::string, bvector<bvector<ContourPoint>>> faceDatasMap;
//		bvector<std::string> normalstring;
//		if (vecFaces.size() < 1)
//		{
//			return ;
//		}
//		DrawContourOutFaceLine(normalstring, faceDatasMap, vecFaces);
//
//		bvector<DPoint3d>().swap(s_drawtempLine.linePoints);
//		for (size_t i = 0; i < normalstring.size(); i++)
//		{
//			std::string normalKey = normalstring.at(i);
//			DrawoutFaceLine(normalKey, faceDatasMap, levelId, s_drawtempLine.linePoints);
//		}
//		if (s_drawtempLine.linePoints.size() > 1)
//		{
//			s_drawtempLine.StartDrawFunction();
//		}
//		mdlLevel_setDisplay(MASTERFILE, levelId, false);
//	}
//}
//#pragma  endregion
