#include "MsdiMsCaeSipch.h"

extern CaseCompDlgInfo		g_caseCompDlgInfo;
// CAE结果类型名
extern bmap<WString, WString> g_RstType_Name;
// CAE结果类型单位
extern bmap<WString, WString> g_RstType_Unit;
extern bvector<WString> vecRstTypeCodes;

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

#define CASECOMP_PROJECT_ID_SELECTED_L (g_caseCompDlgInfo.indexInstanL == -1 ? -1 : readInstanceInfo()[g_caseCompDlgInfo.indexInstanL].projectId)
#define CASECOMP_PROJECT_ID_SELECTED_R (g_caseCompDlgInfo.indexInstanR == -1 ? -1 : readInstanceInfo()[g_caseCompDlgInfo.indexInstanR].projectId)

//工况对比时第一个模型的绘制基点
DPoint3d s_SecCountourBasePt;

void caseCompareCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_CaseCompare, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_CaseCompare);
}

void HookComboBox_CaseComp_InstanceL(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexInstanL = -1;
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case DITEM_MESSAGE_INIT:
	{		
		pListModel = mdlDialog_comboBoxGetListModelP(pThis);

		ListRowP	pRow = NULL;
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		//实例列表comboBox中显示实例名，并保存其对应的工程ID
		bvector<InstanceInfo> instanceInfos = readInstanceInfo();
		for (int i = 0; i < (int)instanceInfos.size(); i++)
		{
			pRow = mdlListRow_create(pListModel);
			cell = mdlListRow_getCellAtIndex(pRow, 0);

			//设置对应的实例名作为显示
			WString ngName = instanceInfos[i].NgName;
			WString instanceName;
			int projectID;
			splitNgName_Model(ngName, UNUSE_WSTRING, projectID, UNUSE_INT, instanceName, UNUSE_WSTRING);
			mdlListCell_setDisplayText(cell, instanceName.data());

			//对应的工程ID保存在cell中
			cellValue.SetInt32(projectID);
			mdlListCell_setValue(cell, cellValue, FALSE);

			mdlListModel_addRow(pListModel, pRow);
			g_caseCompDlgInfo.indexInstanL = 0;
		}

		mdlDialog_comboBoxSetListModelP(pThis, pListModel);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		//当切换实例时，刷新树显示，同时清除所有显示内容，并关闭色带
		initCaseListTree(TREEID_CaseComp_CaseListL);

		ClearContour();
		ShowCAEModel();
		s_DrawColorShape.CloseDrawFunction();
		s_drawtempLine.CloseDrawFunction();
		checkCompareEnable(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_InstanceR(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexInstanR = -1;
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case DITEM_MESSAGE_INIT:
	{
		pListModel = mdlDialog_comboBoxGetListModelP(pThis);

		ListRowP	pRow = NULL;
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		//实例列表comboBox中显示实例名，并保存其对应的工程ID
		bvector<InstanceInfo> instanceInfos = readInstanceInfo();
		for (int i = 0; i < (int)instanceInfos.size(); i++)
		{
			pRow = mdlListRow_create(pListModel);
			cell = mdlListRow_getCellAtIndex(pRow, 0);

			//设置对应的实例名作为显示
			WString ngName = instanceInfos[i].NgName;
			WString instanceName;
			int projectID;
			splitNgName_Model(ngName, UNUSE_WSTRING, projectID, UNUSE_INT, instanceName, UNUSE_WSTRING);
			mdlListCell_setDisplayText(cell, instanceName.data());

			//对应的工程ID保存在cell中
			cellValue.SetInt32(projectID);
			mdlListCell_setValue(cell, cellValue, FALSE);

			mdlListModel_addRow(pListModel, pRow);
			g_caseCompDlgInfo.indexInstanR = 0;
		}

		mdlDialog_comboBoxSetListModelP(pThis, pListModel);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		//当切换实例时，刷新树显示，同时清除所有显示内容，并关闭色带
		initCaseListTree(TREEID_CaseComp_CaseListR);

		ClearContour();
		ShowCAEModel();
		s_DrawColorShape_temp.CloseDrawFunction();
		s_drawtempLine.CloseDrawFunction();
		checkCompareEnable(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_SettingL(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//左侧设置修改主色带参数
		openColorShapeSetting(&s_DrawColorShape, SourceTypeDCS::CaseCompare);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_SettingR(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//右侧设置修改副色带参数
		openColorShapeSetting(&s_DrawColorShape_temp, SourceTypeDCS::CaseCompare);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_CaseComp_Compare(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//先删除之前绘制的轮廓图并显示CAE模型
		ClearContour();
		ShowCAEModel();

		//将视图聚焦到左侧工程的CAE模型位置
		focusOnCAEModel(s_DrawColorShape.NgName);

		//根据显示类型进行绘制
		placeCompareContoursTool::InstallNewInstance(0, 0);

	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_CaseComp_CaseList(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_INIT:
		{
			initCaseListTree(dimP->dialogItemP->id);
		}break;
		case DITEM_MESSAGE_STATECHANGED:
		{
			//当树被点击时重新为对应色带赋值
			RawItemHdr*	riP = dimP->dialogItemP->rawItemP;
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(riP);

			//根据被选中节点更新对应的caseID，并对应初始化色带
			int caseID;
			WString caseName;
			int	numSelect = -1;
			Point2d* selectP = NULL;
			if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, riP) && numSelect != 0)
			{
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;
				MSValueDescr  nodeVal;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
				treeNode = mdlTreeCell_getNode(treeCell);

				mdlTreeNode_getValue(treeNode, nodeVal);
				caseID = nodeVal.GetInt32();

				mdlTreeNode_getDisplayTextWString(treeNode, caseName);

				//根据选择的工况节点数据赋值给色带的成员变量
				DrawColorShape* optDCS = NULL;
				int instanceComboBoxIndex;
				if (dimP->dialogItemP->id == TREEID_CaseComp_CaseListL)
				{
					optDCS = &s_DrawColorShape;
					instanceComboBoxIndex = g_caseCompDlgInfo.indexInstanL;

					// 根据项目ID和工况ID，设置结果类型
					SetResultType(dimP, COMBOBOXID_CaseComp_ResultTypes, readInstanceInfo().at(instanceComboBoxIndex).projectId, caseID, vecRstTypeCodes);
				}
				else
				{
					optDCS = &s_DrawColorShape_temp;
					instanceComboBoxIndex = g_caseCompDlgInfo.indexInstanR;
				}

				//当结果类型发生变化时，需要重新计算色带值
				optDCS->ResultTypeCode = vecRstTypeCodes.at(g_caseCompDlgInfo.indexReType);

				//Header
				optDCS->Header = g_RstType_Name[optDCS->ResultTypeCode];
				//Unit
				optDCS->Unit = g_RstType_Unit[optDCS->ResultTypeCode];

				//showType
				optDCS->showType = g_caseCompDlgInfo.indexShowType;

				//ProjectName & ProjectId
				WString instanceName;
				splitNgName_Model(readInstanceInfo().at(instanceComboBoxIndex).NgName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, instanceName, UNUSE_WSTRING);
				int index;
				splitInstanceName(instanceName, optDCS->ProjectName, index);
				optDCS->ProjectId = readInstanceInfo().at(instanceComboBoxIndex).projectId;

				//CaseName
				mdlTreeNode_getDisplayTextWString(treeNode, optDCS->CaseName);

				//CaseID
				optDCS->CaseID = nodeVal.GetInt32();

				//NgName				
				optDCS->NgName = makeNgName_Model(optDCS->ProjectId, index);

				//重置为自动设置色带
				optDCS->m_bAutomatic = true;
				//根据新实例计算色带的最大值最小值
				optDCS->initColorRangeFromDB();
				//自动等分色带的值
				optDCS->divideColorRange();

			}
			checkCompareEnable(dimP->db);
		}break;
		default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookDialog_CaseCompare(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
	case  DIALOG_MESSAGE_INIT:
	{
		checkCompareEnable(dmP->db);
	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		ClearContour();
		ShowCAEModel();
		s_drawtempLine.CloseDrawFunction();
		s_DrawColorShape.CloseDrawFunction();
		s_DrawColorShape_temp.CloseDrawFunction();
	}break;
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ResultType(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_caseCompDlgInfo.indexReType = 0;
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	break;
	case  DITEM_MESSAGE_STATECHANGED:
	{
		//当结果类型发生变化时，需要重新计算色带值
		s_DrawColorShape.ResultTypeCode = vecRstTypeCodes.at(g_caseCompDlgInfo.indexReType);
		//Header
		s_DrawColorShape.Header = g_RstType_Name[s_DrawColorShape.ResultTypeCode];
		//Unit
		s_DrawColorShape.Unit = g_RstType_Unit[s_DrawColorShape.ResultTypeCode];

		s_DrawColorShape.m_bAutomatic = true;

		s_DrawColorShape.initColorRangeFromDB();
		s_DrawColorShape.divideColorRange();

		//m_iResultTypeID
		s_DrawColorShape_temp.ResultTypeCode = s_DrawColorShape.ResultTypeCode;
		//Header
		s_DrawColorShape_temp.Header = g_RstType_Name[s_DrawColorShape_temp.ResultTypeCode];
		//Unit
		s_DrawColorShape_temp.Unit = g_RstType_Unit[s_DrawColorShape_temp.ResultTypeCode];

		s_DrawColorShape_temp.m_bAutomatic = true;

		s_DrawColorShape_temp.initColorRangeFromDB();
		s_DrawColorShape_temp.divideColorRange();
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ShowType(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
		s_DrawColorShape.showType = s_DrawColorShape_temp.showType = g_caseCompDlgInfo.indexShowType;
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_CaseComp_ShowModel(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_caseCompDlgInfo.ShowModel = 0;
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// 光滑
	}
		break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_caseCompDlgInfo.ShowModel == 1)
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

void initCaseListTree(RscId treeID)
{
	MSDialog	*dbP = mdlDialog_find(DIALOGID_CaseCompare, NULL);
	if (dbP == NULL)
		return;
	DialogItem	*diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_Tree, treeID, 0);
	if (diP == NULL)
		return;

	RawItemHdr*		riP = diP->rawItemP;
	GuiTreeModel*	pModel = NULL;

	pModel = mdlTreeModel_create(1);

	GuiTreeNode*	pRoot = mdlTreeModel_getRootNode(pModel);

	//遍历当前选中实例下的所有工况，作为子节点插入
	int iProjectID = treeID == TREEID_CaseComp_CaseListL ? CASECOMP_PROJECT_ID_SELECTED_L : CASECOMP_PROJECT_ID_SELECTED_R;

	bvector<int> vecCaseIDs;
	bvector<WString> vecCaseNames;
	bvector<WString> vecCaseProps;
	getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

	for (int i = 0; i < (int)vecCaseNames.size(); i++)
	{
		GuiTreeNode*	pNode = mdlTreeNode_create(pModel, FALSE);

		//设置显示值——工况名
		mdlTreeNode_setDisplayText(pNode, vecCaseNames[i].data());

		//设置隐藏值——工况ID
		MSValueDescr    valueDescr;
		valueDescr.SetInt32(vecCaseIDs[i]);
		mdlTreeNode_setValue(pNode, valueDescr, FALSE);

		mdlTreeNode_addChild(pRoot, pNode);
	}

	mdlTreeNode_expand(pRoot);
	mdlDialog_treeSetTreeModelP(riP, pModel);

	mdlDialog_itemSynch(dbP, diP->itemIndex);
}

void checkCompareEnable(MSDialogP db)
{
	//根据是否两个工况树都有选中将“对比”按钮置为不/可用
	DialogItemP button = mdlDialog_itemGetByTypeAndId(db, RTYPE_PushButton, PUSHBTNID_CaseComp_Submit, 0);
	for (int i = 0; i < 2; i++)
	{
		int	numSelect = -1;
		Point2d* selectP = NULL;
		DialogItem*	diP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, i == 0 ? TREEID_CaseComp_CaseListL : TREEID_CaseComp_CaseListR, 0);
		RawItemHdr*	riP = diP->rawItemP;
		if (SUCCESS != mdlDialog_treeGetSelections(&numSelect, &selectP, riP) || numSelect == 0)
		{
			mdlDialog_itemSetEnabledState(db, button->itemIndex, false, false);
			return;
		}
	}

	mdlDialog_itemSetEnabledState(db, button->itemIndex, true, false);

}
placeCompareContoursTool::placeCompareContoursTool(int toolId, int toolPrompt)
	: DgnPrimitiveTool(toolId, toolPrompt)
{}

void placeCompareContoursTool::_OnPostInstall()
{
	__super::_OnPostInstall();
	_EndDynamics();
	mdlAccuSnap_enableSnap(true);		//开启精确捕捉
	mdlAccuDraw_setEnabledState(false);	//关闭精确坐标系
}

bool placeCompareContoursTool::_OnDataButton(DgnButtonEventCR ev)
{
	s_SecCountourBasePt = *ev.GetPoint();
	startShowContour(&s_DrawColorShape);
	startShowContour(&s_DrawColorShape_temp, ev.GetPoint(), false);

	mdlState_startDefaultCommand();
	return true;
}

void placeCompareContoursTool::InstallNewInstance(int toolId, int toolPrompt)
{
	placeCompareContoursTool* tool = new placeCompareContoursTool(toolId, toolPrompt);
	tool->InstallTool();
}