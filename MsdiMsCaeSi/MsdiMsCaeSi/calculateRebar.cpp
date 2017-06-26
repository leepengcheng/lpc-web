#include "MsdiMsCaeSipch.h"

#pragma warning(disable:4310)

//所有配筋结果，在工具加载时从XAttribution提取，画面关闭后重写回XAttribution
extern bmap<WString, ReBarInfo> m_RebarResults;
extern ReBarCalDlgInfo	g_rebarCalDlgInfo;

extern void GetAllNineDatas(bmap<WString, bvector<double>>& allDatas, int caseID, int ProjecID);

ReinforceTool::ReinforceTool(int toolName, int toolPrompt, MSDialogP dbP, int proID, int iCaseID, WString sNgName) : DgnElementSetTool(toolName)
{
	//初始化操作标识符
	m_bFirstClicked = false;

	//初始化操作Z值
	DPoint3d minPt, maxPt;
	mdlSystem_computeDesignRange(&minPt, &maxPt, NULL, -1, NULL, NULL);
	m_dPlaneZ = minPt.z;

	m_dbP = dbP;

	//初始化配筋线信息
	InitRebarInfo(orebarInfo);
	orebarInfo.projectID = proID;
	orebarInfo.caseID = iCaseID;
	swprintf(orebarInfo.NgName, L"%s", sNgName);

	//用于计算书输出
	bvector<WString> namelist;
	BeStringUtilities::Split(g_rebarCalDlgInfo.strCurrentSel, L">", namelist);
	if (namelist.size() > 2) 
	{
		swprintf(orebarInfo.caseName, L"%s", namelist[1]);
		swprintf(orebarInfo.sectionName, L"%s", namelist[2]);
	}

	m_modelOpt = MASTERFILE;

	// 取得所有节点的UX,UY,UZ,SX,SY,SZ,SXY,SXZ,SYZ
	GetAllNineDatas(allNineDatas, iCaseID, proID);

	// 获取截面的转换矩阵(三维截面->二维平面的转换矩阵)
	MsdiMsCaeSiUtility::GetSectionTransform(&tMatrix, orebarInfo.NgName);

	// 读取截面各Mesh元素所有节点的CAE数据
	GetSectionMeshDatas(vecSectionMeshDats, allNineDatas);
}

bool ReinforceTool::_OnInstall()
{
	SetCmdName(GetToolId(), PROMPT_CalcRebar_ClickFirst);
	return __super::_OnInstall();
}

bool ReinforceTool::_OnDataButton(DgnButtonEventCR ev)
{
	bool bRet = true;
	DPoint3d ptPfirst;
	ptPfirst.x = ev.GetPoint()->x;
	ptPfirst.y = ev.GetPoint()->y;
	ptPfirst.z = ev.GetPoint()->z;

	m_bErrorClicked = false;

	if (isPointInFace(ptPfirst))
	{
		//若当前未选择第一个点，则保存本次点击位置
		if (!m_bFirstClicked)
		{
			ptPfirst.z = m_dPlaneZ;
			m_clickPts.SetStartPoint(ptPfirst);

			m_bFirstClicked = true;
			SmartLook(ptPfirst);
			_BeginDynamics();
		}
		//若当前已选择了第一个点，则本次点击位置与第一个点连成配筋线并进行计算
		else
		{
			ptPfirst.z = m_dPlaneZ;
			m_clickPts.SetEndPoint(ptPfirst);
			m_bFirstClicked = false;

			//遍历获取model中的所有Mesh
			DPoint3d ptStt, ptEnd;
			m_clickPts.GetStartPoint(ptStt);
			m_clickPts.GetEndPoint(ptEnd);

			DVec3d DvecX_Cur = DVec3d::FromStartEndNormalize(ptStt, ptEnd);
			DVec3d dpt = DVec3d::From(1.0, 0.0, 0.0);
			RotMatrix inMatrix;
			mdlRMatrix_fromVectorToVector(&inMatrix, &dpt, &DvecX_Cur);
			mdlRMatrix_transpose(&inMatrix, &inMatrix);

#pragma region
			//绘制配筋线
			EditElementHandle eeh;
			LineHandler::CreateLineElement(eeh, NULL, m_clickPts, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
			eeh.AddToModel();

			//设置配筋线信息
			orebarInfo.ElemID = eeh.GetElementId();

			// 取得配筋线
			CurveVectorPtr  curve = ICurvePathQuery::ElementToCurveVector(eeh);

			// 取得配筋线长度
			double dLineLength = curve->Length() / UOR_PER_MM;
			orebarInfo.dLineLength = dLineLength;

			// 取得配筋线指定间距位置上的应力值
			getRebarLineStressbyDistances(orebarInfo, vecSectionMeshDats, curve, dLineLength, tMatrix, inMatrix);
#pragma endregion

			//保存到配筋线数据中
			WString lsKey;
			lsKey.Sprintf(L"%d%s%d%s%d%s%s%s%d", orebarInfo.projectID, SeparatorAnd, orebarInfo.caseID, SeparatorAnd, orebarInfo.SectionID, SeparatorAnd, orebarInfo.NgName, SeparatorAnd, orebarInfo.ElemID);
			m_RebarResults[lsKey] = orebarInfo;

			InsertTreeNode();

			mdlAccuDraw_setEnabledState(false);
			_EndDynamics();
			_ExitTool();
		}
	}
	else
	{
		m_bErrorClicked = true;
		bRet = false;
	}

	SetupAndPromptForNextAction();

	return bRet;
}

void ReinforceTool::InsertTreeNode()
{
	DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(m_dbP, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
	
	GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
	int	numSelect = -1;
	Point2d* selectP = NULL;
	if (SUCCESS == mdlDialog_treeGetSelections(&numSelect, &selectP, tTree->rawItemP) && numSelect != 0)
	{
		GuiTreeCell*	treeCell = NULL;
		GuiTreeNode*	treeNode = NULL;

		treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, selectP[0].y, selectP[0].x);
		treeNode = mdlTreeCell_getNode(treeCell);

		//判断节点的类型
		int nodeLevel = mdlTreeNode_getLevel(treeNode);

		// 截面节点以及配筋线节点以外，退出处理;
		if (nodeLevel == 3)
			treeNode = mdlTreeNode_getParent(treeNode);

		GuiTreeNode* rebarline = mdlTreeNode_create(pModel, FALSE);

		mdlTreeNode_setDisplayText(rebarline, orebarInfo.rebarName);
		mdlTreeNode_setLeafIcon(rebarline, ICONID_RebarLine, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

		WString	 RebarValue;
		//保存内容——(项目ID&工况ID&截面ModelID&截面Model名&元素ID)
		RebarValue.Sprintf(L"%d%s%d%s%d%s%s%s%d", orebarInfo.projectID, SeparatorAnd, orebarInfo.caseID, SeparatorAnd, orebarInfo.SectionID, SeparatorAnd, orebarInfo.NgName, SeparatorAnd, orebarInfo.ElemID);
		MSValueDescr	nodeVal;
		nodeVal.SetWChar(RebarValue.data());
		mdlTreeNode_setValue(rebarline, nodeVal, FALSE);

		// 向树目录中添加节点
		mdlTreeNode_insertChild(treeNode, rebarline, -1);

		// 刷新画面
		mdlDialog_treeModelUpdated(tTree->rawItemP, true);
		mdlDialog_itemsSynch(m_dbP);
	}
}

bool ReinforceTool::_OnResetButton(DgnButtonEventCR ev)
{
	//若用户还未选择起点则右键表示退出工具
	if (!m_bFirstClicked)
	{
		_EndDynamics();
		_ExitTool();
	}
	//否则表示重新选择起点
	else
	{
		_BeginDynamics();
		m_bFirstClicked = false;
		SetupAndPromptForNextAction();
	}
	return true;
}

// 实现绘制配筋线的动态效果
void ReinforceTool::_OnDynamicFrame(DgnButtonEventCR ev)
{
	if (m_bFirstClicked)
	{
		DPoint3d pt;
		DSegment3d tmpPts;
		// 取得动态配筋线的起点坐标
		m_clickPts.GetStartPoint(pt);
		tmpPts.SetStartPoint(pt);

		// 取得动态配筋线的终点坐标
		pt.x = ev.GetPoint()->x;
		pt.y = ev.GetPoint()->y;
		pt.z = m_dPlaneZ;

		tmpPts.SetEndPoint(pt);

		// 绘制动态的配筋线
		EditElementHandle eeh;
		LineHandler::CreateLineElement(eeh, NULL, tmpPts, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
		if (eeh.IsValid())
		{
			// 以临时线方式，实现动态效果
			RedrawElems redrawElems;

			redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
			redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
			redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);

			redrawElems.DoRedraw(eeh);
		}
	}
	__super::_OnDynamicFrame(ev);
}

void ReinforceTool::SetupAndPromptForNextAction()
{
	UInt32      msgId;
	msgId = PROMPT_CalcRebar_ClickFirst;

	if (m_bFirstClicked)
		msgId = PROMPT_CalcRebar_ClickNext;

	if (m_bErrorClicked)
		msgId = PROMPT_CalcRebar_ClickError;

	mdlOutput_rscPrintf(MSG_PROMPT, NULL, STRINGLISTID_Prompts, msgId);
}

void ReinforceTool::_OnCleanup()
{
	s_pDCShape->CloseDrawFunction();
	ClearclipfaceShow();
}

void ReinforceTool::InstallNewInstance(int toolId, int toolPrompt, MSDialogP dbP, int proID, int CaseID, WString sNgName)
{
	ReinforceTool* tool = new ReinforceTool(toolId, toolPrompt, dbP, proID, CaseID, sNgName);
	tool->InstallTool();

	/*	绘制配筋线时显示云图*/
	//先删除之前绘制的轮廓图并显示CAE模型
	ClearclipfaceShow();

	//打开色带
	s_pDCShape->showType = 0;
	s_pDCShape->StartDrawFunction();
	WString plotNmae = NGNAME_PREFIX_CAECLIP;

	DgnPlatform::LevelId levelId;
	// 创建或取得CAE云图的图层
	if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE切面云图", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE切面云图");

	ClipFace::ShowByType(plotNmae, ClipFace::DrawFacePlot, levelId, s_pDCShape->CaseID, s_pDCShape->ResultTypeCode, s_pDCShape->ProjectUnit);
	mdlLevelTable_rewrite(MASTERFILE);
}

bool ReinforceTool::isPointInFace(DPoint3d pt)
{
	bvector<bvector<PointOutData>>::iterator iter;
	for (iter = vecSectionMeshDats.begin(); iter != vecSectionMeshDats.end(); iter++)
	{
		if (MsdiMsCaeSiUtility::PointInPolygon(pt, *iter) > 0)
		{
			return true;
		}
	}

	return false;
}

void ReinforceTool::_OnRestartTool()
{
	InstallNewInstance(GetToolId(), GetToolPrompt(), m_dbP, orebarInfo.projectID, orebarInfo.caseID, orebarInfo.NgName);
}

bool ReinforceTool::_OnPostLocate(HitPathCP path, WStringR cantAcceptReason)
{
	if (!__super::_OnPostLocate(path, cantAcceptReason))
	{
		return false;
	}
	DPoint3d ptPfirst;
	mdlHitPath_getHitPoint(path, ptPfirst);
	ElementHandle eeh(mdlDisplayPath_getElem((DisplayPathP)path, 0), mdlDisplayPath_getPathRoot((DisplayPathP)path));

	int elemType = eeh.GetElementType();
	if (LINE_ELM == elemType || 105 == elemType)
	{
		m_lineList.push_back(eeh.GetElementId());
	}
	return true;
}

void ReinforceTool::_OnPostInstall()
{
	__super::_OnPostInstall();

	mdlAccuSnap_enableLocate(FALSE);
	mdlAccuDraw_setEnabledState(FALSE);

	mdlLocate_allowLocked();
	__super::_SetLocateCursor(true);
}

void ReinforceTool::SmartLook(DPoint3d pt)
{
	DgnModelRefP pModel = MASTERFILE;
	for (auto it = m_lineList.begin(); it != m_lineList.end(); ++it)
	{
		EditElementHandle eeh;
		eeh.FindByID(*it, pModel);

		bool findLine = false;
		DPoint3d linePts[2];
		switch (eeh.GetElementType())
		{
		case LINE_ELM:
		{
			int num = 0;
			mdlLinear_extract(linePts, &num, eeh.GetElementCP(), pModel);
			findLine = MsdiMsCaeSiUtility::PtIsOnline(pt, linePts[1], linePts[0]);
			break;
		}
		case 105:
		{
			bvector<DPoint3d> vertices;
			int nVertices = 0;
			mdlMesh_getVertices(eeh.GetElementDescrP(), &vertices, &nVertices, FALSE);
			for (int i = 0; i < (int)vertices.size() - 1; i++)
			{
				findLine = MsdiMsCaeSiUtility::PtIsOnline(pt, vertices[i + 1], vertices[i]);
				if (findLine)
				{
					linePts[0] = vertices[i];
					linePts[1] = vertices[i + 1];
					break;
				}
			}
			break;
		}
		default:
			break;
		}
		if (findLine)
		{
			// 实现垂直当前线的锁轴功能
			DVec3d Dvec_Line = DVec3d::FromStartEndNormalize(linePts[0], linePts[1]);
			DVec3d dpt = DVec3d::From(1.0, 0.0, 0.0);
			RotMatrix RtMatrix;
			mdlRMatrix_fromVectorToVector(&RtMatrix, &dpt, &Dvec_Line);
			mdlRMatrix_transpose(&RtMatrix, &RtMatrix);
			mdlState_setAccudrawContext(
				ACCUDRAW_SetOrigin | ACCUDRAW_SetRMatrix | ACCUDRAW_Lock_X,
				&pt,
				NULL,
				NULL,
				NULL,
				&RtMatrix
				);
			break;
		}
	}
	bvector<ElementId>().swap(m_lineList);
	mdlAccuDraw_setEnabledState(true);
}

void ReinforceTool::InitRebarInfo(rebarInfo& orebarInfo)
{
	orebarInfo.projectID		= -1;
	orebarInfo.caseID			= -1;
	orebarInfo.SectionID		= mdlModelRef_getModelID(ACTIVEMODEL);
	swprintf(orebarInfo.NgName, L"%s", "");
	orebarInfo.ElemID			= -1;
	swprintf(orebarInfo.caseName, L"%s", "");
	swprintf(orebarInfo.sectionName, L"%s", "");
	swprintf(orebarInfo.rebarName, L"%s", g_rebarCalDlgInfo.rebarName);
	orebarInfo.dLineLength		= -1;
	orebarInfo.dStruSignifyFt	= g_rebarCalDlgInfo.dStruSignifyFt;
	orebarInfo.dDesignFt		= g_rebarCalDlgInfo.dDesignFt;
	orebarInfo.dStruFt			= g_rebarCalDlgInfo.dStruFt;
	orebarInfo.dDefPointSpan	= g_rebarCalDlgInfo.dDefPointSpan;
	orebarInfo.iRebarType		= g_rebarCalDlgInfo.iRebarType;
	orebarInfo.dRebarTesion		= g_rebarCalDlgInfo.dRebarTesion;
	orebarInfo.iConcrType		= g_rebarCalDlgInfo.iConcrType;
	orebarInfo.dConcrTesion		= g_rebarCalDlgInfo.dConcrTesion;
	orebarInfo.dSectionHeight	= g_rebarCalDlgInfo.dSectionHeight;
	orebarInfo.bCalcStress		= g_rebarCalDlgInfo.bCalcStress;
	swprintf(orebarInfo.strStress, L"%s", "");

	orebarInfo.bendingStress	= 0.0;
	orebarInfo.axialStress		= 0.0;
	swprintf(orebarInfo.shearSXY, L"%s", "");
	swprintf(orebarInfo.shearSYZ, L"%s", "");
	swprintf(orebarInfo.shearSXZ, L"%s", "");

	swprintf(orebarInfo.strResult, L"%s", "");
	orebarInfo.iRebarCount		= 1;
	orebarInfo.dRebarDiameter	= 6.0;
	orebarInfo.dRebarArea		= 0.0;
}


