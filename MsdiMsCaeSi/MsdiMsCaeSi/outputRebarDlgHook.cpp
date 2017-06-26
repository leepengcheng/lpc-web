#include "MsdiMsCaeSipch.h"

#define  espinion 1e-10
#define  iszero(x) ((x) > -espinion && (x) < espinion)

extern ReBarCalDlgInfo g_rebarCalDlgInfo;
extern void GetAllNineDatas(bmap<WString, bvector<double>>& allDatas, int caseID, int ProjecID);

extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);
// CAE结果类型名
extern bmap<WString, WString> g_RstType_Name;
// CAE结果类型单位
extern bmap<WString, WString> g_RstType_Unit;
extern bvector<WString> vecRstTypeCodes;

// KEY=项目ID$工况ID$截面ModelID$元素ID
const int KEY_IDX_PROJECTID = 0;//项目ID
const int KEY_IDX_CASEID = 1;//工况ID
const int KEY_IDX_SECTIONID = 2;//截面ModelID 
const int KEY_IDX_NGNAME = 3;//截面Model名字 
const int KEY_IDX_ELEMID = 4;//元素ID

//  钢筋抗拉强度列表
double m_rebarStrengthList[7] = { 210.0, 270.0, 300.0, 360.0, 360.0, 420.0, 360.0 };
//  混凝土抗拉强度列表
double m_concrStrengthList[11] = { 0.64, 0.91, 1.10, 1.27, 1.43, 1.57, 1.71, 1.80, 1.89, 1.96, 2.04 };
//  标准钢筋直径
double m_diameterList[16] = { 6, 6.5, 8, 10, 12, 14, 16, 18, 20, 22, 25, 28, 32, 36, 40, 50 };

bvector<WString> oCalcResultItemVec;

//所有配筋结果，在工具加载时从XAttribution提取，画面关闭后重写回XAttribution
bmap<WString, ReBarInfo> m_RebarResults;
//配筋面积
double m_dAs = 0.0;  

bool TesionCmpDesc(const double& dFirst, const double& dSecond)
{
	return dFirst > dSecond;
}
void InitReBarCalDlgInfo()
{
	swprintf(g_rebarCalDlgInfo.strCurrentKey, L"%s", L"");
	swprintf(g_rebarCalDlgInfo.rebarName, L"%s", L"");
	swprintf(g_rebarCalDlgInfo.strCurrentSel, L"%s", L"");

	g_rebarCalDlgInfo.dStruFt = 1.2;					//结构系数
	g_rebarCalDlgInfo.dStruSignifyFt = 1.0;				//结构重要系数
	g_rebarCalDlgInfo.dDesignFt = 1.0;					//设计状况系数
	g_rebarCalDlgInfo.dDefPointSpan = 200;				//默认点距(mm)
	g_rebarCalDlgInfo.iRebarType = 0;					//钢筋种类下标，用于获取fy
	g_rebarCalDlgInfo.dRebarTesion = 0.0;				//钢筋抗拉强度
	g_rebarCalDlgInfo.iConcrType = 0;					//混凝土种类下标，用于获取ft
	g_rebarCalDlgInfo.dConcrTesion = 0.0;				//混凝土轴心抗拉强度
	g_rebarCalDlgInfo.dSectionHeight = 1;				//截面高度
	g_rebarCalDlgInfo.dDiameter = 6.0;					//钢筋直径
	g_rebarCalDlgInfo.iDameterIndex = 0;			
	g_rebarCalDlgInfo.iRebarCount = 1;					//钢筋根数
	g_rebarCalDlgInfo.bCalcStress = false;				//是否计算弯矩、轴力、剪力
}

// 初始化窗体数据
bool InitFormData()
{
	//界面参数初始化
	InitReBarCalDlgInfo();

	// 配筋计算结果项目初始化
	bvector<WString>().swap(oCalcResultItemVec);
	//应力数据最大点号
	oCalcResultItemVec.push_back(L"应力数据最大点号 N:");
	//0应力对应点号
	oCalcResultItemVec.push_back(L"0应力对应点号 No:");
	//0应力点位置
	oCalcResultItemVec.push_back(L"0应力点位置(mm):");
	//总拉应力面积
	oCalcResultItemVec.push_back(L"总拉应力面积(mMPa):");
	//混凝土强度对应点号
	oCalcResultItemVec.push_back(L"混凝土强度对应点号 Nc:");
	//混凝土强度对应位置
	oCalcResultItemVec.push_back(L"混凝土强度对应位置(mm):");
	//混凝土拉应力面积
	oCalcResultItemVec.push_back(L"混凝土拉应力面积(mMPa):");
	//计算配筋面积
	oCalcResultItemVec.push_back(L"计算配筋面积(mm2)");
	//参考配筋
	oCalcResultItemVec.push_back(L"参考配筋:");
	//弯矩
	oCalcResultItemVec.push_back(L"弯矩(N):");
	//轴力
	oCalcResultItemVec.push_back(L"轴力(N):");
	//剪力XY
	oCalcResultItemVec.push_back(L"XY方向剪力(N):");
	//剪力YZ
	oCalcResultItemVec.push_back(L"YZ方向剪力(N):");
	//剪力XZ
	oCalcResultItemVec.push_back(L"XZ方向剪力(N):");
	// 读取配筋数据
	readRebarResultsFromX(m_RebarResults);

	return true;
}

// 初始化所有ListBox的数据
void InitListBoxData(MSDialogP dbP)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	ListCellP	pCell = NULL;
	int iRowCount;

	// 取得应力ListBox对象
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	// 取得应力ListModel对象
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		// 清除已有数据
		mdlListModel_empty(pListModel, true);
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}

	// 刷新应力图显示
	DialogItem *ctPDiP;
	/* Redraw the ContainerPanel */
	ctPDiP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);
	if (ctPDiP)
	{
		mdlDialog_itemDrawEx(dbP, ctPDiP->itemIndex, true);
	}

	// 取得计算结果ListBox对象
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_CalResult, 0);
	// 取得计算结果ListModel对象
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);

		// 清除显示结果
		for (int iRow = 0; iRow < iRowCount; iRow++)
		{
			// 计算结果列
			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);
			mdlListCell_setStringValue(pCell, L"", TRUE);
		}
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}

	// 取得配筋结果ListBox对象
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_RebarInfo, 0);
	// 取得配筋结果ListModel对象
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);
		// 清除显示结果
		for (int iRow = 0; iRow < iRowCount; iRow++)
		{
			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 0);
			mdlListCell_setStringValue(pCell, L"", TRUE);

			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
			mdlListCell_setStringValue(pCell, L"", TRUE);

			pCell = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);
			mdlListCell_setStringValue(pCell, L"", TRUE);
		}
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
	}
}

// 保存配筋数据
void writeRebarResultsToX(bmap<WString, ReBarInfo>& mapRebarResults)
{
	if (mapRebarResults.size() <= 0) return;

	//所有的配筋结果存放到Default模型的XAttribution中
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);

	//先将bvector转为数组再进行写入
	int cnt = (int)mapRebarResults.size();
	ReBarInfo* arrayResults = new ReBarInfo[cnt];

	int iRowCnt = 0;
	bmap<WString, ReBarInfo>::iterator iter;
	for (iter = mapRebarResults.begin(); iter != mapRebarResults.end(); iter++, iRowCnt++)
	{
		arrayResults[iRowCnt] = iter->second;
	}

	appSet.SaveSetting(handlerId, XAttrID_RebarResults, arrayResults, sizeof(ReBarInfo) * cnt);

	delete[] arrayResults;
	mdlModelRef_freeWorking(model);
}

// 读取配筋数据
void readRebarResultsFromX(bmap<WString, ReBarInfo>& mapRebarResults)
{
	mapRebarResults.clear();

	WString lsKey;
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
	DgnModelRefP model;
	mdlModelRef_createWorking(&model, mdlDgnFileObj_getMasterFile(), DEFAULTMODEL, TRUE, TRUE);
	ApplicationSettings appSet = IDgnSettings::GetCurrentSettings().GetModelApplicationSettings(*model);
	XAttributeHandle xah(appSet.GetElementRef(), handlerId, XAttrID_RebarResults);

	if (xah.IsValid() && xah.IsValid())
	{
		//先将读取的数据存到数组中再转为bvector
		int size = xah.GetSize();
		int cnt = size / (int)sizeof(ReBarInfo);
		if (cnt != 0)
		{
			ReBarInfo* arrayResults = (ReBarInfo*)xah.PeekData();
			for (int i = 0; i < cnt; i++)
			{
				// 项目ID&工况ID&截面ModelID&截面Model名&元素ID
				lsKey.Sprintf(L"%d%s%d%s%d%s%s%s%d", arrayResults[i].projectID, SeparatorAnd, arrayResults[i].caseID, SeparatorAnd, arrayResults[i].SectionID, SeparatorAnd, arrayResults[i].NgName, SeparatorAnd, arrayResults[i].ElemID);
				mapRebarResults[lsKey] = arrayResults[i];
			}
		}
	}

	mdlModelRef_freeWorking(model);
}

// 取得配筋线指定间距位置上的应力值
bool getRebarLineStressbyDistances(ReBarInfo& orebarInfo, const bvector<bvector<PointOutData>>& m_FacetVec, CurveVectorPtr curve, double dLineLength, TransformCP tMatrix, RotMatrixCR rotMatrix)
{
	bvector<double> distances; // 配筋线指定间距的位置
	bvector<CurveLocationDetail> locations;// 配筋线指定间距位置的坐标

	// 生成配筋线指定间距的位置数组
	int i = 1;
	double dDistance = g_rebarCalDlgInfo.dDefPointSpan;
	while (dDistance < dLineLength)
	{
		// 间距需要乘主单位
		distances.push_back(dDistance * UOR_PER_MM);
		i = i + 1;
		dDistance = g_rebarCalDlgInfo.dDefPointSpan * i;
	}

	// 取得配筋线的起点和终点
	DPoint3d StartPoint, EndPoint;
	curve->GetStartEnd(StartPoint, EndPoint);

	// 计算配筋线起点位置坐标所对应的应力值
	bvector<DPoint3d> outPoints;
	outPoints.push_back(StartPoint);

	// 取得配筋线指定间距位置的坐标数组
	if (curve->AddSpacedPoints(distances, locations) == true)
	{
		// 计算配筋线指定间距位置坐标所对应的应力值
		for (int iCnt = 0; iCnt < locations.size(); iCnt++)
		{
			outPoints.push_back(locations.at(iCnt).point);
		}
	}
	// 计算配筋线终点位置坐标所对应的应力值
	outPoints.push_back(EndPoint);

	bvector<bvector<PointOutData>> outPointAndValues;
	bvector<bvector<DPoint3d>> alloutPoints;
	alloutPoints.push_back(outPoints);

	// 计算各输出点的数值
	CalcOutPointValues(outPointAndValues, alloutPoints, m_FacetVec, tMatrix, rotMatrix);

	// 保存应力值
	WString strStress, shearSXY, shearSYZ, shearSXZ;
	WString buffer_sy, buffer_sxy, buffer_syz, buffer_sxz;
	for (int i = 0; i < outPointAndValues.at(0).size(); i++)
	{
		PointOutData oPtDat = outPointAndValues.at(0).at(i);
		// 设置显示格式
		buffer_sy.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SY]);
		buffer_sxy.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SXY]);
		buffer_syz.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SYZ]);
		buffer_sxz.Sprintf(L"%.3g", oPtDat.ResultVals[RESULTTYPE_SXZ]);
		if (i > 0)
		{
			strStress.append(SeparatorAnd);
			shearSXY.append(SeparatorAnd);
			shearSYZ.append(SeparatorAnd);
			shearSXZ.append(SeparatorAnd);
		}
		strStress.append(buffer_sy);
		shearSXY.append(buffer_sxy);
		shearSYZ.append(buffer_syz);
		shearSXZ.append(buffer_sxz);
	}

	swprintf(orebarInfo.strStress, L"%s", strStress.data());//应力值SY
	swprintf(orebarInfo.shearSXY, L"%s", shearSXY.data());//应力值SXY
	swprintf(orebarInfo.shearSYZ, L"%s", shearSYZ.data());//应力值SYZ
	swprintf(orebarInfo.shearSXZ, L"%s", shearSXZ.data());//应力值SXZ

	return true;
} 

/*-----------------------------------------------------------------
*  功能描述: 在指定的Dialog指定的Item中绘制应力图
*	@param	dbP	IN	指定的Dialog指针
*	@param	diP	IN	指定的DialogItem指针
*
*  @return	无
-----------------------------------------------------------------*/
void drawStressView(MSDialogP dbP, DialogItemP diP)
{
	ViewFlags       viewflags;
	MSElementDescr *cellDP = NULL;
	bool            nDices;

	DialogItem  *diP_tree = NULL;
	ListModel   *pListModel = NULL;
	int iRowCount = 0;

	diP_tree = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	pListModel = mdlDialog_listBoxGetListModelP(diP_tree->rawItemP);

	if (pListModel != NULL)
	{
		iRowCount = mdlListModel_getRowCount(pListModel);
	}
	if (iszero(iRowCount)) return;

	ListRowP pRow = mdlListRow_create(pListModel);

	double dStress = 0.0;
	double dMaxDistance = 0.0;
	double dMaxStress = 0.0, dMinStress = 0.0;

	for (int iRow = 0; iRow < iRowCount; iRow++)
	{
		ListCellP cell_Stress = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);

		//应力值（MPa）
		MSValueDescr cellValue;
		mdlListCell_getValue(cell_Stress, cellValue);
		dStress = _wtof(cellValue.GetWCharCP());
		if (iszero(iRow))
		{
			dMaxStress = dStress;
			dMinStress = dStress;
		}
		else
		{
			if (dMaxStress < dStress)
				dMaxStress = dStress;

			if (dMinStress > dStress)
				dMinStress = dStress;
		}

		if (iRow == iRowCount - 1)
		{
			ListCellP cell_distance = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
			mdlListCell_getValue(cell_distance, cellValue);
			dMaxDistance = _wtof(cellValue.GetWCharCP()) * 0.65;
		}
	}

	// 求出应力轴的总长
	double dStressLen = abs(dMaxStress) + abs(dMinStress);

	bvector<DPoint3d> vecAxis;// 距离轴线数组
	bvector<DPoint3d> vecVertex;// 应力线数组
	bvector<DSegment3d> vecScale;// 应力轴线数组
	double dDistances = 0.0;
	// 根据应力结果表中的数据，生成绘制应力图的数据
	for (int iRow = 0; iRow < iRowCount; iRow++)
	{
		ListCellP cell_distance = mdlListModel_getCellAtIndexes(pListModel, iRow, 1);
		ListCellP cell_Stress = mdlListModel_getCellAtIndexes(pListModel, iRow, 2);

		DPoint3d ptAxis;// 距离轴线坐标
		DPoint3d ptVertex;// 应力线坐标
		DSegment3d ptScale;// 应力轴线

		//距离(mm)
		MSValueDescr cellValue;
		mdlListCell_getValue(cell_distance, cellValue);
		dDistances = -1 * _wtof(cellValue.GetWCharCP()) * UOR_PER_MM;

		//应力值（MPa）
		mdlListCell_getValue(cell_Stress, cellValue);
		dStress = (_wtof(cellValue.GetWCharCP()) / dStressLen) * dMaxDistance * UOR_PER_MM;

		// 距离轴线上各点的坐标
		ptAxis = DPoint3d::From(0.0, dDistances, 0.0);
		// 各应力线的坐标
		ptVertex = DPoint3d::From(dStress, dDistances, 0.0);

		// 求出应力轴线的起始与终点
		ptScale.Init(ptAxis, ptVertex);

		// 保存到距离轴线数组中
		vecAxis.push_back(ptAxis);
		// 保存到应力线数组中
		vecVertex.push_back(ptVertex);
		// 保存到应力轴线数组中
		vecScale.push_back(ptScale);
	}

	//将所有元素放在一个单元中整合显示
	EditElementHandle cellHeader;
	DPoint3d origin = { 0, 0, 0 };
	RotMatrix rMatrix;
	mdlRMatrix_getIdentity(&rMatrix);
	NormalCellHeaderHandler::CreateCellElement(cellHeader, L"StressView", origin, rMatrix, true, *MASTERFILE);

	// 根据距离生成数值轴线
	EditElementHandle ehLineStringAxis;
	if (createLineString(ehLineStringAxis, vecAxis, ACTIVEMODEL) == true)
	{
		NormalCellHeaderHandler::AddChildElement(cellHeader, ehLineStringAxis);
	}

	// 根据应力值生成应力线
	EditElementHandle ehLineStringVertex;
	if (createLineString(ehLineStringVertex, vecVertex, ACTIVEMODEL) == true)
	{
		NormalCellHeaderHandler::AddChildElement(cellHeader, ehLineStringVertex);
	}

	// 根据应力值生成应力轴线
	for (int j = 0; j < vecScale.size(); j++)
	{
		// 生成应力轴线
		EditElementHandle ehLine;
		if (createLine(ehLine, vecScale.at(j), ACTIVEMODEL) == true)
		{
			NormalCellHeaderHandler::AddChildElement(cellHeader, ehLine);
		}

		// 生成应力轴线上的文字
		WString tText;
		tText.Sprintf(L"%d", j + 1);
		double fwidth = (g_rebarCalDlgInfo.dDefPointSpan) * 0.35 * UOR_PER_MM;

		// 取得应力轴线上文字的放置坐标
		DPoint3d tPos;
		vecScale.at(j).GetStartPoint(tPos);

		// 生成文字
		EditElementHandle eehText;
		if (MsdiMsCaeSiUtility::CreateText(eehText, ACTIVEMODEL, tPos, tText, fwidth, fwidth, 0.0, (UInt32)TextElementJustification::RightMiddle) == true)
		{
			NormalCellHeaderHandler::AddChildElement(cellHeader, eehText);
		}
	} 
	
	nDices = mdlModelRef_is3D(MASTERFILE);

	memset(&viewflags, 0, sizeof(viewflags));
	viewflags.patterns = 1;
	viewflags.ed_fields = 1;
	viewflags.on_off = 1;
	viewflags.constructs = 1;
	viewflags.dimens = 1;

	cellHeader.AddToModel();
	Draw3DCell(cellHeader.GetElementDescrP(), dbP, &diP->rect, &viewflags, nDices);
	cellHeader.DeleteFromModel();
}

void Draw3DCell
(
MSElementDescr      *cellP,
MSDialogP           dbP,
BSIRect             *localRectP,
ViewFlags           *viewflagsP,
bool                nDices
)
{
	RotMatrix       rMatrixArray;
	Dpoint3d        newextent;
	DRange3d        dRangeVec;

	UInt32          colorMap[MAX_CMAPENTRIES];
	byte            colorTable[MAX_CTBLBYTES];

	mdlColor_getColorTableByModelRef(colorTable, MASTERFILE);
	mdlColor_matchLongColorMap(colorMap, colorTable, 0, false);
	// 背景设为白色
	colorMap [255] = 0xffffff;	

	//set up the views
	mdlView_getStandard(&rMatrixArray, StandardView::Top);

	mdlCnv_scanRangeToDRange(&dRangeVec, &cellP->el.hdr.dhdr.range);

	mdlRMatrix_multiplyRange(&dRangeVec.low, &dRangeVec.high, &rMatrixArray);
	mdlVec_subtractPoint(&newextent, &dRangeVec.high, &dRangeVec.low);

	mdlRMatrix_unrotatePoint(&dRangeVec.low, &rMatrixArray);

	if (newextent.z < fc_1) newextent.z = fc_1000;
	if (newextent.x < fc_1) newextent.x = fc_1000;
	if (newextent.y < fc_1) newextent.y = fc_1000;

	mdlWindow_clipRectSet((MSWindow *)dbP, localRectP);

	mdlElmdscr_extendedDisplayToWindow((MSWindow *)dbP, localRectP, viewflagsP, cellP, &rMatrixArray,
		&dRangeVec.low, &newextent, 0, -1, colorMap, true, NULL);

	mdlDialog_rectDrawEdge(dbP, localRectP, true);
	mdlWindow_clipRectSet((MSWindow *)dbP, NULL);
}

bool createLine(EditElementHandleR eeh, DSegment3dCR segment, DgnModelRefP modelRef)
{
	if (SUCCESS == LineHandler::CreateLineElement(eeh, NULL, segment, modelRef->Is3d(), *modelRef))
		return true;

	return false;
}

bool createLineString(EditElementHandleR eeh, bvector<DPoint3d> &points, DgnModelRefP modelRef)
{
	if (SUCCESS == LineStringHandler::CreateLineStringElement(eeh, NULL, &points[0], points.size(), modelRef->Is3d(), *modelRef))
		return true;

	return false;
}

void HookGeneric_CalRebar_Canva(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_DRAW:
	{
		BSIRect   clipRect;
		BSIColorDescr   *bgColorP;

		clipRect = dimP->dialogItemP->rect;
		mdlRect_inset(&clipRect, -2, -2);
		mdlDialog_rectDrawBeveled(dimP->db, &clipRect, FALSE, TRUE);

		bgColorP = mdlColorPal_getColorDescr(NULL, 0);
		mdlDialog_rectFillCD(dimP->db, &dimP->dialogItemP->rect, &dimP->dialogItemP->rect, bgColorP, NULL);

		drawStressView(dimP->db, dimP->dialogItemP);

		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void     mdlDialog_rectInset
(
BSIRect        *rP,
int            deltaX,        /* positive moves in */
int            deltaY
)
{
	int width, height;

	width = rP->Width();
	height = rP->Height();
	if (width - 2 * deltaX <= 0 ||
		height - 2 * deltaY <= 0)
	{
		rP->origin.x = 0;
		rP->origin.y = 0;
		rP->corner.x = 0;
		rP->corner.y = 0;

		return;
	}

	rP->origin.x += deltaX;
	rP->corner.x -= deltaX;
	rP->origin.y += deltaY;
	rP->corner.y -= deltaY;
}

void HookDialog_CalRebar_InterfaceDynamic(DialogMessage *dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_CREATE:
	{
		dmP->u.create.interests.windowMoving = TRUE;
		dmP->u.create.interests.resizes = TRUE;
		dmP->u.create.interests.updates = TRUE;
		// 画面鼠标前置事件响应
		dmP->u.create.interests.preButtons = TRUE;
		dmP->u.create.interests.nonDataPoints = TRUE;

		// 初始化窗体数据
		InitFormData();

		break;
	}
	case DIALOG_MESSAGE_UPDATE:
	{
		DialogItem    *diP;

		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Sash, SASHID_CalRebar_V, 0);
		if (!diP)
			break;

		CalRebar_adjustVSashDialogItems(dmP->db, NULL, TRUE);
		break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING:
	{
		int        minSize = (60 * XC);

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

		CalRebar_adjustVSashDialogItems(dmP->db, &oldContent,
			(dmP->u.resize.oldContent.origin.x != dmP->u.resize.newContent.origin.x ||
			dmP->u.resize.oldContent.origin.y != dmP->u.resize.newContent.origin.y));
		break;
	}
	case DIALOG_MESSAGE_PREBUTTON:
	{
		if ((dmP->u.button.buttonTrans == BUTTONTRANS_DOWN) &&
			(dmP->u.button.buttonNumber == RESET))//右键菜单
		{
			CalRebar_setupPopupMenu(dmP);
		}
		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		break;
	}
	case DIALOG_MESSAGE_DESTROY:
	{
		writeRebarResultsToX(m_RebarResults);
		break;
	}
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void CalRebar_setupPopupMenu(DialogMessageP dmP)
{
	int			 iItem;
	int			 menuId;
	RawItemHdr		*popupP;
	DialogItem		*popupDiP;
	DialogItem		*downDiP;
	DItem_PulldownMenu	*popupMenuP;
	MSDialogP		dbP = dmP->db;

	/* find the index of the item the reset button went down in */
	iItem = mdlDialog_findItemIndex(dbP, &dmP->u.button.pt, FALSE);
	if (iItem < 0)
		return;

	popupDiP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_PopupMenu, POPUPMENUID_CalRebar_PopuMenu, 0);
	if (!popupDiP)
		return;
	popupP = popupDiP->rawItemP;

	/* Popup Menus are a restricted form of MenuBar (they only have 1 top
	level menu). mdlDialog_menuBarxxx functions can be called with ptrs
	to popup menus. */
	popupMenuP = mdlDialog_menuBarGetMenu(popupP, 0);
	if (!popupMenuP)
		return;

	downDiP = mdlDialog_itemGetByIndex(dbP, iItem);
	switch (downDiP->type)
	{
	case RTYPE_Tree:

		int rowIndex, colIndex;
		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, downDiP->rawItemP))
		{
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(downDiP->rawItemP);
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//判断节点的类型
			int nodeLevel = mdlTreeNode_getLevel(treeNode);

			// 截面节点以及配筋线节点时，显示右键菜单
			// 其它节点，不显示右键菜单;
			if (nodeLevel < 2)
				return;

			DItem_PulldownMenuItem  targetItem;
			mdlDialog_menuBarFindItem(&targetItem, &popupMenuP, popupP, NULL, PDMID_CalRebar_PopuMenu, MSID_DelRebarLine);
			if (nodeLevel == 2)
			{
				// 当选择截面节点时，显示右键菜单中的删除配筋线不可使用
				mdlDialog_textPDMItemSetEnabled(&targetItem, FALSE);
			}
			else
			{
				// 当选择配筋线节点时，显示右键菜单中的删除配筋线可使用
				mdlDialog_textPDMItemSetEnabled(&targetItem, TRUE);
			}

			menuId = PDMID_CalRebar_PopuMenu;
		}
		else
		{
			return;
		}

		break;

	default:
		return;
	}

	if (popupMenuP->id != menuId)
	{
		mdlDialog_menuBarDeleteMenu(popupMenuP, popupP, NULL, 0);
		mdlDialog_menuBarInsMenu(popupP, RTYPE_PulldownMenu, menuId, NULL);
	}

	/* force the button event to go to the popupMenu item */
	dmP->u.button.buttonRiP = popupP;
}

void HookPushBtn_CalRebar_Calculate(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//根据左边树选择的配筋线在bmap<WString, ReBarInfo> m_RebarResults中查询key,把面积等信息写入key对应的value
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// 选择节点为配盘线时，进入配筋计算
				int nodeType = mdlTreeNode_getLevel(pTreeNode);
				if (3 == nodeType)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						//第三个参数为了修改该配筋线对应的ReBarInfo结构体
						if (calculateRebarArea(it_find->second.dLineLength, it_find->second) == true)
						{
							//控件值保存到配筋线属性
							GetDataFromControls(it_find->second);

							//结果写入计算结果listbox
							SetResultValToListBox(dimP->db, it_find->second);

							//结果写入钢筋listbox
							SetRebarInfoToListBox(dimP->db, it_find->second);
						}
					}
				}
			}
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

//刷新
void HookPushBtn_CalRebar_Refresh(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		bvector<WString> vecData;
		BeStringUtilities::Split(g_rebarCalDlgInfo.strCurrentKey, SeparatorAnd, vecData);

		// 配筋线以外，不处理。
		if (vecData.size() < 3) return;
		rebarInfo& orebarInfo = m_RebarResults[g_rebarCalDlgInfo.strCurrentKey];

#pragma region
		bmap<WString, bvector<double>> allNineDatas;
		// 取得所有节点的UX,UY,UZ,SX,SY,SZ,SXY,SXZ,SYZ
		GetAllNineDatas(allNineDatas, wtoi(vecData.at(KEY_IDX_CASEID).data()), wtoi(vecData.at(KEY_IDX_PROJECTID).data()));

		// 读取截面各Mesh元素所有节点的CAE数据
		bvector<bvector<PointOutData>> vecSectionMeshDats;
		GetSectionMeshDatas(vecSectionMeshDats, allNineDatas);

		EditElementHandle lineHandle(orebarInfo.ElemID, ACTIVEMODEL);
		DPoint3d linePts[2];
		int num = 0;
		mdlLinear_extract(linePts, &num, lineHandle.GetElementCP(), ACTIVEMODEL);
		
		DVec3d DvecX_Cur = DVec3d::FromStartEndNormalize(linePts[0], linePts[1]);
		DVec3d dpt = DVec3d::From(1.0, 0.0, 0.0);
		RotMatrix inMatrix;
		mdlRMatrix_fromVectorToVector(&inMatrix, &dpt, &DvecX_Cur);
		mdlRMatrix_transpose(&inMatrix, &inMatrix);

		// 获取截面的转换矩阵(三维截面->二维平面的转换矩阵)
		TransformP tMatrix = NULL;
		MsdiMsCaeSiUtility::GetSectionTransform(&tMatrix, vecData.at(KEY_IDX_NGNAME).data());

		//元素ID
		ElementHandle eh(_wtoi64(vecData.at(KEY_IDX_ELEMID).data()), ACTIVEMODEL);

		// 取得配筋线
		CurveVectorPtr  curve = ICurvePathQuery::ElementToCurveVector(eh);

		// 取得配筋线长度
		double dLineLength = curve->Length() / UOR_PER_MM;

		// 取得配筋线指定间距位置上的应力值
		getRebarLineStressbyDistances(orebarInfo, vecSectionMeshDats, curve, dLineLength, tMatrix, inMatrix);
#pragma endregion

		GetDataFromControls(orebarInfo);

		// 将应力值显示到ListBox中
		SetStressValToListBox(dimP->db, orebarInfo);

		// 刷新应力图显示
		DialogItem *ctPDiP;
		/* Redraw the ContainerPanel */
		ctPDiP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);
		if (ctPDiP)
			mdlDialog_itemDrawEx(dimP->db, ctPDiP->itemIndex, true);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_CalRebar_OutPut(DialogItemMessage* dimP)
{

}

void CalRebar_adjustVSashDialogItems(MSDialogP   db, BSIRect  *pOldContent, bool refreshItems)
{
	DialogItem    *sashDiP;
	DialogItem    *treeDiP;
	DialogItem    *ctPanelDiP;

	/* Get the 3 main items on the dialog */
	sashDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Sash, SASHID_CalRebar_V, 0);
	treeDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
	ctPanelDiP = mdlDialog_itemGetByTypeAndId(db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);

	/* Resize, reposition and draw the items */
	mdlDialog_itemsAdjustFlushWithSash(db, NULL, pOldContent, sashDiP,
		treeDiP, refreshItems, refreshItems,
		ctPanelDiP, TRUE, refreshItems);
}

void HookSash_CalRebar_vSashHook(DialogItemMessage* dimP)
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
				CalRebar_adjustVSashDialogItems(dimP->db, NULL, TRUE);
		}
		break;
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookTree_CalRebar_SectionTree(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	WString lsKey;

	//WString myString;
	//myString.Sprintf(L"messageType: %d", dimP->messageType);
	//mdlDialog_dmsgsPrint(myString.GetWCharCP());

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		// 初始化树显示(截面信息)
		InitSectionTreeDisp(dimP->dialogItemP);

		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		GuiTreeModel*  pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
		int rowIndex = 0,
			colIndex = 0;
		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
		{
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//txt显示选中信息
			wcscpy(g_rebarCalDlgInfo.strCurrentSel, MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(treeNode).data());
		}

		mdlDialog_itemsSynch(dimP->db);
		break;
	}
	case DITEM_MESSAGE_BUTTON:
	{
		if (dimP->u.button.clicked)
		{
			int rowIndex, colIndex;
			if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, dimP->dialogItemP->rawItemP))
			{
				GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
				GuiTreeCell*	treeCell = NULL;
				GuiTreeNode*	treeNode = NULL;

				treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
				treeNode = mdlTreeCell_getNode(treeCell);

				//判断点击的节点的类型
				MSValueDescr uvalue;
				mdlTreeNode_getValue(treeNode, uvalue);
				
				//KEY=项目ID$工况ID$截面ModelID$截面组名
				uvalue.GetWString(lsKey);
				bvector<WString> vecData;
				BeStringUtilities::Split(lsKey.data(), SeparatorAnd, vecData);

				//配筋线节点
				int iNodeLevel = mdlTreeNode_getLevel(treeNode);
				if (2 == iNodeLevel)
				{
					//初始化绘制云图的参数
					InitDrawColorShape(dimP, treeNode, vecData);
				}
				if (3 == iNodeLevel)
				{
				 	const rebarInfo& orebarInfo = m_RebarResults[lsKey];
					wcscpy(g_rebarCalDlgInfo.strCurrentKey, lsKey.data()); //当前显示节点Key

					g_rebarCalDlgInfo.dStruSignifyFt = orebarInfo.dStruSignifyFt;	//结构重要系数
					g_rebarCalDlgInfo.dDesignFt		= orebarInfo.dDesignFt;			//设计状况系数
					g_rebarCalDlgInfo.dStruFt		= orebarInfo.dStruFt;			//结构系数
					g_rebarCalDlgInfo.dDefPointSpan = orebarInfo.dDefPointSpan;		//默认点距(mm)
					g_rebarCalDlgInfo.iRebarType	= orebarInfo.iRebarType;		//钢筋种类下标，用于获取fy
					g_rebarCalDlgInfo.dRebarTesion	= orebarInfo.dRebarTesion;		//钢筋抗拉强度
					g_rebarCalDlgInfo.iConcrType	= orebarInfo.iConcrType;		//混凝土种类下标，用于获取ft
					g_rebarCalDlgInfo.dConcrTesion	= orebarInfo.dConcrTesion;		//混凝土轴心抗拉强度
					g_rebarCalDlgInfo.dSectionHeight= orebarInfo.dSectionHeight;	//截面高度
					g_rebarCalDlgInfo.bCalcStress	= orebarInfo.bCalcStress;		//是否计算弯矩轴力剪力

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						
						// 将应力值显示到ListBox中
						SetStressValToListBox(dimP->db, orebarInfo);

						//结果写入计算结果listbox
						SetResultValToListBox(dimP->db, orebarInfo);

						//结果写入钢筋listbox
						SetRebarInfoToListBox(dimP->db, orebarInfo);

						// 刷新应力图显示
						DialogItem *ctPDiP;
						/* Redraw the ContainerPanel */
						ctPDiP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ContainerPanel, CTPANELID_CalRebar_Detail, 0);

						if (ctPDiP)
							mdlDialog_itemDrawEx(dimP->db, ctPDiP->itemIndex, true);

						// 配筋线高亮显示
						EditElementHandle eh(orebarInfo.ElemID, target);
						ElementAgenda eAgenda;
						SelectionSetManagerR selectSetM = SelectionSetManager::GetManager();
						selectSetM.EmptyAll();
						selectSetM.AddElement(eh.GetElementRef(), target);
						selectSetM.BuildAgenda(eAgenda);
						eAgenda.Hilite();

						mdlModelRef_freeWorking(target);
						MsdiMsCaeSiUtility::fitView(0);						
					}
				}
				else
				{
					//双击定位截面
					if (2 == iNodeLevel && 2 == dimP->u.button.upNumber)
					{
						DgnModelRefP target = NULL;
						if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
						{
							if (!mdlModelRef_areSame(MASTERFILE, target))
							{
								s_pDCShape->CloseDrawFunction();
								s_drawtempLine.CloseDrawFunction();
								ClearclipfaceShow();
								mdlModelRef_activateAndDisplay(target);
							}
							mdlModelRef_freeWorking(target);
							MsdiMsCaeSiUtility::fitView(0);
						}
					}
					// 配筋线以外节点时，初始化所有ListBox的数据
					InitListBoxData(dimP->db);
				}

				mdlDialog_itemsSynch(dimP->db);
			}
		}
		break;
	}
	case DIALOG_MESSAGE_DESTROY:
	{
		GuiTreeModel   *pTreeModel = mdlDialog_treeGetTreeModelP(dimP->dialogItemP->rawItemP);
		mdlTreeModel_destroy(pTreeModel, TRUE);

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// 初始化树显示(截面信息)
void InitSectionTreeDisp(DialogItemP dimp)
{
	WString			instanceValue;
	WString			caseValue;
	WString			sectionValue;
	WString			RebarValue;
	GuiTreeNode     *pRoot;
	MSValueDescr	nodeVal;
	GuiTreeModel*	pModel = mdlTreeModel_create(1);

	pRoot = mdlTreeModel_getRootNode(pModel);
	mdlTreeNode_setDisplayText(pRoot, L"ROOT");

	//读取XAttribution得到全部实例的数据
	bvector<InstanceInfo> allInstance = readInstanceInfo();

#pragma region 项目节点的生成
	//遍历所有实例，为树增加节点
	for (int iPro = 0; iPro < (int)allInstance.size(); iPro++)
	{
		WString ngName = allInstance[iPro].NgName;

		GuiTreeNode* instanceNode = mdlTreeNode_create(pModel, TRUE);
		mdlTreeNode_addChild(pRoot, instanceNode);

		//显示内容——实例名
		WString instanceName;
		int unuse1 = 0;
		splitNgName_Model(ngName, UNUSE_WSTRING, unuse1, UNUSE_INT, instanceName, UNUSE_WSTRING);

		mdlTreeNode_setDisplayText(instanceNode, instanceName.data());

		mdlTreeNode_setCollapsedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
		mdlTreeNode_setExpandedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
		mdlTreeNode_setExpandedSelectedIcon(instanceNode, ICONID_Model, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

		//保存内容——"ModelID&的NgName"
		instanceValue.Sprintf(L"%d%s%s", allInstance[iPro].modelId, SeparatorAnd, ngName);
		nodeVal.SetWChar(instanceValue.data());

		mdlTreeNode_setValue(instanceNode, nodeVal, FALSE);

#pragma region 工况节点的生成
		//遍历所有model，找到属于该实例的截面model
		ngName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

		//根据工程ID查询数据库得到相应的工况列表
		bvector<int> vecCaseIDs;
		bvector<WString> vecCaseNames;
		bvector<WString> vecCaseProps;
		getCaseInfoByProjectid(allInstance[iPro].projectId, vecCaseIDs, vecCaseNames, vecCaseProps);

		for (int iCase = 0; iCase < vecCaseNames.size(); iCase++)
		{
			GuiTreeNode*	pCaseNode = NULL;
			pCaseNode = mdlTreeNode_create(pModel, TRUE);

			mdlTreeNode_setCollapsedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
			mdlTreeNode_setExpandedSelectedIcon(pCaseNode, ICONID_Result, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

			//显示内容——工况名					
			mdlTreeNode_setDisplayText(pCaseNode, vecCaseNames[iCase].data());

			//保存内容——(项目ID$工况ID)
			caseValue.Sprintf(L"%d%s%d", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase));
			nodeVal.SetWChar(caseValue.data());
			mdlTreeNode_setValue(pCaseNode, nodeVal, FALSE);

			mdlTreeNode_addChild(instanceNode, pCaseNode);

#pragma region 截面节点的生成
			DgnIndexIteratorP   cellIterator = mdlModelIterator_create(mdlDgnFileObj_getMasterFile());
			DgnIndexItemP       IndexItem;
			mdlModelIterator_getFirst(cellIterator);
			mdlModelIterator_setAcceptCellsOnly(cellIterator, FALSE);
			while (NULL != mdlModelIterator_getNext(cellIterator))
			{
				IndexItem = mdlModelIterator_getCurrent(cellIterator);
				WChar wModelName[MAX_CELLNAME_LENGTH];
				//获取model名
				mdlModelItem_getName(IndexItem, wModelName, MAX_CELLNAME_LENGTH);
				WString wsModelName;
				wsModelName.append(wModelName);

				if (wsModelName.FindI(ngName.data()) != string::npos)
				{
					GuiTreeNode* clipNode = mdlTreeNode_create(pModel, TRUE);

					//显示内容——截面名
					WString clipName;
					splitNgName_Clip(wsModelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, clipName, UNUSE_WSTRING, UNUSE_WSTRING);
					mdlTreeNode_setDisplayText(clipNode, clipName.data());

					mdlTreeNode_setCollapsedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
					mdlTreeNode_setExpandedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());
					mdlTreeNode_setExpandedSelectedIcon(clipNode, ICONID_Section, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

					//保存内容——(项目ID&工况ID&截面ModelID)
					sectionValue.Sprintf(L"%d%s%d%s%d%s%s", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase), SeparatorAnd, IndexItem->GetModelId(), SeparatorAnd, IndexItem->GetName());
					nodeVal.SetWChar(sectionValue.data());
					mdlTreeNode_setValue(clipNode, nodeVal, FALSE);

					mdlTreeNode_addChild(pCaseNode, clipNode);

#pragma region 配筋线节点的生成
					bmap<WString, ReBarInfo>::iterator iter;
					for (iter = m_RebarResults.begin(); iter != m_RebarResults.end(); iter++)
					{
						/*arrayResults[iCnt] = iter->second;*/
						if (iter->first.StartsWith(sectionValue.data()))
						{
							GuiTreeNode* rebarline = mdlTreeNode_create(pModel, FALSE);

							mdlTreeNode_setDisplayText(rebarline, iter->second.rebarName);
							mdlTreeNode_setLeafIcon(rebarline, ICONID_RebarLine, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

							//保存内容——(项目ID&工况ID&截面ModelID&截面Model名&元素ID)
							RebarValue.Sprintf(L"%d%s%d%s%d%s%s%s%d", allInstance[iPro].projectId, SeparatorAnd, vecCaseIDs.at(iCase), SeparatorAnd, IndexItem->GetModelId(), SeparatorAnd, IndexItem->GetName(), SeparatorAnd, iter->second.ElemID);
							nodeVal.SetWChar(RebarValue.data());
							mdlTreeNode_setValue(rebarline, nodeVal, FALSE);

							mdlTreeNode_addChild(clipNode, rebarline);
						}
					}
#pragma endregion 配筋线节点的生成
				}
			}

			mdlModelIterator_free(cellIterator);
#pragma endregion 截面节点的生成
		}
#pragma endregion 工况节点的生成

		if (0 == mdlTreeNode_getChildCount(instanceNode))
			mdlTreeNode_setAllowsChildren(instanceNode, FALSE);
	}
#pragma endregion 项目节点的生成

	//展开节点
	GuiTreeNode* treeNode = pRoot;
	while (mdlTreeNode_isParentPopulated(treeNode) && mdlTreeNode_getLevel(treeNode) < 3)
	{
		mdlTreeNode_expand(treeNode);
		treeNode = mdlTreeNode_getFirstChild(treeNode);
	}
	mdlDialog_treeSetTreeModelP(dimp->rawItemP, pModel);
}

void SetStressValToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	WChar buffer[256];

	// 取得应力值
	bvector<WString> pointStress;
	BeStringUtilities::Split(info.strStress, SeparatorAnd, pointStress);

	// 取得应力ListBox对象
	diP = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_StressVal, 0);
	// 取得应力ListModel对象
	pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);

	if (pListModel != NULL)
	{
		// 存在数据时，消除已有数据
		mdlListModel_empty(pListModel, true);
	}
	// 创建新的ListModel
	pListModel = mdlListModel_create(3);

	// 将应力数组中的数据写入到ListBox中
	for (int i = 0; i < pointStress.size(); i++)
	{
		// 创建新的ListRow
		ListRowP	pRow = mdlListRow_create(pListModel);
		ListCellP	cell = NULL;// 单元格对象
		WString cellWs;
		MSValueDescr cellValue;

		//No
		swprintf(buffer, L"%d", i + 1);
		cell = mdlListRow_getCellAtIndex(pRow, 0);
		cellValue.SetWChar(buffer);
		mdlListCell_setValue(cell, cellValue, TRUE);

		//距离(mm)
		if (i == pointStress.size() - 1)
		{
			// 最后一条数据时，距离值为配筋线的长度
			swprintf(buffer, L"%f", info.dLineLength);
		}
		else
		{
			// 距离值 = 间隔值 * i 
			swprintf(buffer, L"%d", g_rebarCalDlgInfo.dDefPointSpan*i);
		}
		cell = mdlListRow_getCellAtIndex(pRow, 1);
		cellValue.SetWChar(buffer);
		mdlListCell_setValue(cell, cellValue, TRUE);

		//应力值(MPa)
		cell = mdlListRow_getCellAtIndex(pRow, 2);
		cellValue.SetWChar(pointStress.at(i).data());
		mdlListCell_setValue(cell, cellValue, TRUE);

		// 将行数据增加到ListBox中
		mdlListModel_addRow(pListModel, pRow);
	}

	mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
}

void HookComboBox_CalRebar_RebarType(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = true;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_rebarCalDlgInfo.dRebarTesion = m_rebarStrengthList[0];
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		int iIndex = g_rebarCalDlgInfo.iRebarType;
		//自定义放在combbox的最后一项
		int iSize = sizeof(m_rebarStrengthList) / sizeof(m_rebarStrengthList[0]);
		if (iSize == iIndex){}
		else
		{
			g_rebarCalDlgInfo.dRebarTesion = m_rebarStrengthList[iIndex];	//联动钢筋抗拉强度
		}
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = false;
		break;
	}
}

void HookComboBox_CalRebar_ConcrType(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = true;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		g_rebarCalDlgInfo.dConcrTesion = m_concrStrengthList[0];
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		int iIndex = g_rebarCalDlgInfo.iConcrType;
		int iSize = sizeof(m_concrStrengthList) / sizeof(m_concrStrengthList[0]);
		if (iSize == iIndex){}
		else
		{
			g_rebarCalDlgInfo.dConcrTesion = m_concrStrengthList[iIndex];		//联动混凝土抗拉强度
		}
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = false;
		break;
	}
}

void HookPdm_CalRebar_AddRebarLine(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		int lastAction = -1;
		if (SUCCESS == mdlDialog_openModal(&lastAction, NULL, DIALOGID_AddRebarLine))
		{
			if (lastAction == ACTIONBUTTON_OK)
			{
				DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
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
					if (nodeLevel < 2)
						return;

					//取得工况ID
					MSValueDescr uvalue;
					mdlTreeNode_getValue(treeNode, uvalue);
					WString lsData;
					uvalue.GetWString(lsData);

					bvector<WString> vecData;
					BeStringUtilities::Split(lsData.data(), SeparatorAnd, vecData);

					DgnModelRefP target = NULL;
					if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
					{
						if (!mdlModelRef_areSame(MASTERFILE, target))
						{
							s_pDCShape->CloseDrawFunction();
							s_drawtempLine.CloseDrawFunction();
							ClearclipfaceShow();
							mdlModelRef_activateAndDisplay(target);
						}
						mdlModelRef_freeWorking(target);
					}

					// 启动配筋线绘制工具
					ReinforceTool::InstallNewInstance(COMMAND_CALCREBAR, PROMPT_CalcRebar_ClickFirst, dimP->db, wtoi(vecData.at(KEY_IDX_PROJECTID).data()), wtoi(vecData.at(KEY_IDX_CASEID).data()), vecData.at(KEY_IDX_NGNAME));
				}
			}
		}
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

// 删除配筋线
void HookPdm_CalRebar_DelRebarLine(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		int rowIndex, colIndex;
		DialogItemP  tTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);

		if (SUCCESS == mdlDialog_treeLastCellClicked(&rowIndex, &colIndex, tTree->rawItemP))
		{
			GuiTreeModel*   pModel = mdlDialog_treeGetTreeModelP(tTree->rawItemP);
			GuiTreeCell*	treeCell = NULL;
			GuiTreeNode*	treeNode = NULL;

			treeCell = mdlTreeModel_getDisplayCellAtIndexes(pModel, rowIndex, colIndex);
			treeNode = mdlTreeCell_getNode(treeCell);

			//取得配筋线的KEY
			MSValueDescr uvalue;
			mdlTreeNode_getValue(treeNode, uvalue);
			WString lsKey;
			uvalue.GetWString(lsKey);

			const ReBarInfo& oRebarInfo = m_RebarResults[lsKey];
			// 删除配筋线元素  
			EditElementHandle eh(oRebarInfo.ElemID, ACTIVEMODEL);
			eh.DeleteFromModel();
			NamedGroupCollectionPtr ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
			for each (NamedGroupPtr namedGroup in *ngcCAESolid)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				// 如果命名组中没有元素,就从Dgn中删除。
				if (graphicMembers == 0)
				{
					namedGroup->DeleteFromFile();
				}
			}
			// 从配筋线数组中，删除当前的配筋线数据   
			m_RebarResults.erase(lsKey);

			if (SUCCESS == mdlTreeNode_destroy(treeNode))
			{
				mdlDialog_treeModelUpdated(tTree->rawItemP, true);
			}

			mdlDialog_itemsSynch(dimP->db);
		}
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookListBox_CalRebar_CalcResult(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;
	WChar buffer[256];

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(3);

		for (int i = 0; i < (int)oCalcResultItemVec.size(); i++)
		{
			ListRowP	pRow = mdlListRow_create(pListModel);

			ListCellP	cell = NULL;
			WString cellWs;
			MSValueDescr cellValue;

			//No
			swprintf(buffer, L"%d", i + 1);
			cell = mdlListRow_getCellAtIndex(pRow, 0);
			cellValue.SetWChar(buffer);
			mdlListCell_setValue(cell, cellValue, TRUE);

			//计算项
			cell = mdlListRow_getCellAtIndex(pRow, 1);
			cellValue.SetWChar(oCalcResultItemVec.at(i).data());
			mdlListCell_setValue(cell, cellValue, TRUE);

			//结果
			cell = mdlListRow_getCellAtIndex(pRow, 2);
			cellValue.SetWChar(L"");
			mdlListCell_setValue(cell, cellValue, TRUE);

			mdlListModel_addRow(pListModel, pRow);
		}

		mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
	}break;
	case DIALOG_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

//@return: 0/1个零点 0， 多个零点 -1,Ft大于最大应力 -2，尖点 -3
int checkTesionValid(const bvector<double>& tesionList, double dFt)
{
	int valid = -1;
	bool bSuperior = false;
	int count = (int)tesionList.size();
	for (int i = 0; i < count; ++i)
	{ 
		//Ft至少小于一个应力
		if (tesionList[i] > dFt)  
		{
			bSuperior = true;
			valid = 0;
		}
		else if (iszero(tesionList[i]))
		{
			//判断尖点
			if (i > 0 && i < count-1)
			{
				if (tesionList[i - 1] > 0 && tesionList[i + 1] > 0)
				{
					valid = -3;
					break;
				}
			}
		}
	}
	if (bSuperior)
	{
		//0点个数
		int nZero = 0;
		CountOfZeroPoint(tesionList, nZero);
		if (nZero > 1)
		{
			valid = -1;
		}
	}
	else
	{
		valid = -2;
	}
	return valid;
}
//0点个数
void CountOfZeroPoint(const bvector<double>& tesionList, int& nZero)
{
	if (tesionList.empty()) return;

	int count = (int)tesionList.size();
	int iStart = 0;
	double dBegin = 0.0;

	//取非0拉应力起点位置
	for (int i = 0; i < count; ++i)
	{
		if (iszero(tesionList[i]))
		{
			++nZero;
		}
		else
		{
			iStart = i;
			dBegin = tesionList[i];
			break;
		}
	}
	//0点个数
	if (dBegin > 0)
	{
		bool bOdd = true;  //0点个数是否为奇数,用来辅助判别拉应力从正到0/负，或者从0/负到正数
		for (int i = iStart; i < count; ++i)
		{
			//从正到 0/负
			if (bOdd)
			{
				if (tesionList[i] < 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = false;
				}
			}
			//从0/负 到正
			else
			{
				if (tesionList[i] > 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = true;
				}
			}
		}
	}
	else
	{
		bool bOdd = true;
		for (int i = iStart; i < count; ++i)
		{
			//从0/负 到正
			if (bOdd)
			{
				if (tesionList[i] > 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = false;
				}
			}
			//从正到0/负
			else
			{
				if (tesionList[i] < 0 || iszero(tesionList[i]))
				{
					++nZero;
					bOdd = true;
				}
			}
		}
	}
}

/* 如果应力有多次小于混凝土抗拉强度时，只取最接近零点那个进行计算*/
//获取用来计算的拉应力首尾位置. 
void GetValidTesionHeadAndTail(const bvector<double>& tesionList, double Ft, int& iStart, int& iEnd)
{
	if (tesionList.empty()) return;
	iStart = 0;
	iEnd = 0;

	/*遍历取最小非负应力位置*/
	int count = (int)tesionList.size();
	double dMin = 0.0;
	int iMin = 0;
	//从后往前取第一个非负应力
	for (int i = count - 1; i >= 0; --i)
	{
		if (iszero(tesionList[i]) || tesionList[i] > 0)
		{
			iMin = i;
			dMin = tesionList[i];
			break;
		}
	}

	iEnd = iMin;
	iStart = 0;
	//从后往前遍历大于Ft的位置
	if (tesionList[iMin] < Ft)
	{
		for (int i = iMin; i > 0; --i)
		{
			if (tesionList[i] > Ft)
			{
				if (tesionList[i] > tesionList[i - 1])
				{
					iStart = i;
					break;
				}
			}
		}
	}
	//小于Ft位置
	else
	{
		for (int i = iEnd; i >= 0; --i)
		{
			if (tesionList[i] < 0)
			{
				if (i == iEnd)
				{
					iStart = iEnd;
				}
				else
				{
					iStart = i + 1;
				}
				break;
			}
			else if (tesionList[i] < Ft)
			{
				if (tesionList[i] < tesionList[i - 1] || tesionList[i - 1] < 0)
				{
					iStart = i;
					break;
				}
			}
		}
	}
}

void GetConcretePositions(const bvector<double>& tesionList, double iTail, double span, double Ft, double dLineLength, double dEndSpan, int& iConcr_No, double& dConcrP)
{
	iConcr_No = 1;
	int count = (int)tesionList.size();
	if (tesionList[iTail] < Ft)
	{
		for (int i = iTail; i >= 0; --i)
		{
			if (tesionList[i] > Ft)
			{
				iConcr_No = i + 1;
				break;
			}
		}
	}
	else
	{
		for (int i = iTail; i >= 0; --i)
		{
			if (tesionList[i] < Ft)
			{
				iConcr_No = i + 1;
				break;
			}
		}
	}

	if ((count - 2) == iConcr_No || 3 == count)
	{
		dConcrP = dLineLength;
	}
	else if ((count - 3) == iConcr_No)
	{
		double scale = 0.0;
		double dM = tesionList[iConcr_No - 1];
		if (!iszero(dM))
		{
			scale = fabs(Ft - tesionList[iConcr_No - 1]) / dM;
		}
		dConcrP = span*iConcr_No - span + dEndSpan*scale;
	}
	else
	{
		double scale = 0.0;
		double dM = fabs(tesionList[iConcr_No] - tesionList[iConcr_No - 1]);
		if (!iszero(dM))
		{
			scale = fabs(Ft - tesionList[iConcr_No - 1]) / dM;
		}
		dConcrP = span*iConcr_No - span + span*scale;
	}
}

double GetTrapezoidArea(double dTop, double bBottom, double dHeigh)
{
	return (dTop + bBottom)*dHeigh*0.5;
}
//计算一段点距为span的总拉应力面积A和混泥土拉应力面积Act
void GetAAndAct(double dPrev,double dNext,double Ft,double span,double& dA,double& dAct)
{
	//令dPrev为A，dNext为B
	//A >= B
	if (dPrev >= dNext)
	{
		//A-B-Ft
		if (Ft <= dPrev && Ft <= dNext)
		{
			dA += GetTrapezoidArea(Ft, dPrev, span);
		}
		//A-Ft-B
		else if (Ft <= dPrev)
		{
			double scale = 0.0;
			double dM = fabs(dPrev - dNext);
			if (!iszero(dM))
			{
				scale = fabs(Ft - dNext) / dM;
			}
			dAct += GetTrapezoidArea(dNext, Ft, span*scale);
			dA += GetTrapezoidArea(dPrev, dNext, span);
		}
		//Ft-A-B
		else
		{
			double area = GetTrapezoidArea(dPrev, dNext, span);
			dAct += area;
			dA += area;
		}
	}
	//A < B
	else
	{
		//Ft-A-B
		if (Ft <= dPrev && Ft <= dNext)
		{
			dA += GetTrapezoidArea(Ft, dPrev, span);
		}
		//A-Ft-B
		else if (Ft <= dPrev)
		{
			double scale = 0.0;
			double dM = fabs(dNext - dPrev);
			if (!iszero(dM))
			{
				scale = fabs(Ft - dPrev) / dM;
			}
			dAct += GetTrapezoidArea(dPrev, Ft, span*scale);
			dA += GetTrapezoidArea(dPrev, dNext, span);
		}
		//A-B-Ft
		else
		{
			double area = GetTrapezoidArea(dPrev, dNext, span);
			dAct += area;
			dA += area;
		}
	}
}

//@brief 获取(x1至x2这一段的弯矩),弯矩M(M=FL，F=Sz*A)所需的F , x为间距，y为应力,dD为第一个应力到零点(以配筋线中点为0点)的距离
double GetAPeriodBendingStress(double x1, double y1, double x2, double y2,double dD)
{
	double dM = (x2 - x1);
	double k = 0.0;
	if (iszero(dM)) 
		return 0.0;
	else
		k = (y2 - y1) / dM;

	//  积分后方程f(x) = -1/3kx^3+0.5dDkx^2+0.5kx1x^2-0.5y1x^2+dDy1x-dDkx1x
	//  弯矩		= f(x2)- f(x1)
	double fx2 = -1.0 / 3.0*k*x2*x2*x2 + 0.5*dD*k*x2*x2 + 0.5*k*x1*x2*x2 - 0.5*y1*x2*x2 + dD*y1*x2 - dD*k*x1*x2;
	double fx1 = -1.0 / 3.0*k*x1*x1*x1 + 0.5*dD*k*x1*x1 + 0.5*k*x1*x1*x1 - 0.5*y1*x1*x1 + dD*y1*x1 - dD*k*x1*x1;
	return fx2 - fx1;
}

//计算总弯矩,以配筋线中点为零点进行计算
double GetBendingStress(const bvector<double>& tesionList,double span,double dLineLength)
{ 
	double	dStress = 0.0,
			dD = dLineLength * 0.5;
	int count = (int)tesionList.size();

	if (2 == count)
	{	
		double	y1 = tesionList[0],
				y2 = tesionList[1];

		double y0 = (y2 - y1) * 0.5;
		dStress += GetAPeriodBendingStress(-dD, y1, 0.0, y0, dD);
		dStress += GetAPeriodBendingStress(0.0, y0, dD, y2, dD);
	}
	else
	{
		//零点左边弯矩总和
		int nLeft = count * 0.5 - 1;
		for (int i = 0; i < nLeft; ++i)
		{
			dStress += GetAPeriodBendingStress(-dD + i*span,tesionList[i],-dD + (i + 1)*span,tesionList[i + 1],dD);
		}
		
		//中点(零点)左右两边最近的两段弯矩
		double	y1 = tesionList[nLeft],
				y2 = tesionList[nLeft + 1];
		double k  = (dD - nLeft * span) / span;
		double x1 = -k * span;
		double x2 = span + x1;    
		double y0 = k * (y2 - y1) + y1;

		dStress += GetAPeriodBendingStress(x1, y1, 0.0, y0, dD);
		dStress += GetAPeriodBendingStress(0.0, y0, x2, y2, dD);

		//零点右边除最后一段外弯矩总和
		int index = 0;
		for (int i = nLeft + 1; i < count - 2; ++i)
		{
			dStress += GetAPeriodBendingStress(x2 + index*span, tesionList[i], x2 + (index + 1)*span, tesionList[i + 1], dD);
			++index;
		}

		//最后一段弯矩
		dStress += GetAPeriodBendingStress(x2 + index * span,tesionList[count-2],dD,tesionList[count-1],dD);
	}
	return dStress;
}

//@brief 获取(x1至x2这一段的轴力)
double GetAPeriodAxialStress(double x1, double y1, double x2, double y2)
{
	double dM = (x2 - x1);
	double k = 0.0;
	if (iszero(dM))
		return 0.0;
	else
		k = (y2 - y1) / dM;
	//积分后方程f(x)= 0.5kx^2-kx1x+y1x
	//轴力 = f(x2)- f(x1)
	return 0.5*k*x2*x2 - k*x1*x2 + y1*x2 + 0.5*k*x1*x1 - y1*x1;
}
//计算总轴力
double GetAxialStress(const bvector<double>& tesionList, double span, double dLineLength)
{
	double	dStress = 0.0,
			dD = dLineLength * 0.5;
	int count = (int)tesionList.size();

	if (2 == count)
	{
		double	y1 = tesionList[0],
				y2 = tesionList[1];

		double y0 = (y2 - y1) * 0.5;
		dStress += GetAPeriodAxialStress(-dD, y1, 0.0, y0);
		dStress += GetAPeriodAxialStress(0.0, y0, dD, y2);
	}
	else
	{
		//零点左边轴力总和
		int nLeft = count * 0.5 - 1;
		for (int i = 0; i < nLeft; ++i)
		{
			dStress += GetAPeriodAxialStress(-dD + i*span, tesionList[i], -dD + (i + 1)*span, tesionList[i + 1]);
		}

		//中点(零点)左右两边最近的两段轴力
		double	y1 = tesionList[nLeft],
				y2 = tesionList[nLeft + 1];
		double k = (dD - nLeft * span) / span;
		double x1 = -k * span;
		double x2 = span + x1;
		double y0 = k * (y2 - y1) + y1;

		dStress += GetAPeriodAxialStress(x1, y1, 0.0, y0);
		dStress += GetAPeriodAxialStress(0.0, y0, x2, y2);

		//零点右边轴力总和
		int index = 0;
		for (int i = nLeft + 1; i < count - 2; ++i)
		{
			dStress += GetAPeriodAxialStress(x2 + index*span, tesionList[i], x2 + (index + 1)*span, tesionList[i + 1]);
			++index;
		}

		//最后一段轴力
		dStress += GetAPeriodAxialStress(x2 + index * span, tesionList[count - 2], dD, tesionList[count - 1]);
	}
	return dStress;
}
void getStressList(const WChar* field,bvector<double>& tesionList)
{
	bvector<WString> vecStress;
	BeStringUtilities::Split(field, SeparatorAnd, vecStress);

	for (int iDat = 0; iDat < (int)vecStress.size(); iDat++)
	{
		double dStress = _wtof(vecStress.at(iDat).data());
		tesionList.push_back(dStress);
	}
}
bool calculateRebarArea(double dLineLength, ReBarInfo& info)
{
	/*-------------------------------------------准备工作---------------------------------------------*/
	// 取出SY方向应力数据
	bvector<double> tesionList;
	getStressList(info.strStress, tesionList);

	//验证应力有效性
	if (tesionList.empty()) return false;
	double Ft = g_rebarCalDlgInfo.dConcrTesion;			//混凝土轴心抗拉强度
	int validflag = checkTesionValid(tesionList, Ft);
	
	//检查不合格提示
	if (validflag < 0)
	{
		WString strmsg;
		if (-1 == validflag)
		{
			strmsg.append(L"有多个零点!");
		}
		else if (-2 == validflag)
		{
			strmsg.append(L"您输入的混凝土抗拉强度大于当前配筋线最大的拉应力!");
		}
		else 
		{
			strmsg.append(L"尖点,请分两次配筋!");  //尖点
		}
		mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, strmsg.data(), MessageBoxIconType::Information);
		return false;
	}

	//获取参数
	double	dAs			= 0.0;							//配筋面积
	double	dA			= 0.0;							//总拉应力面积
	double  dAct		= 0.0;							//砼拉应力面积
	int		iMaxTValid_No = 0;							//最大有效点号
	int		iZeroT_No	= 0;							//0应力点号
	double  dZeroP		= 0.0;							//0应力对应位置
	int		iConcr_No	= 0;							//砼强度对应点号 	
	double  dConcrP		= 0.0;							//砼强度对应位置
	double  dReferArea	= 0.0;							//参考配筋面积
	double span = g_rebarCalDlgInfo.dDefPointSpan,		//点距
		Rd = g_rebarCalDlgInfo.dStruFt,					//结构系数
		Fy = g_rebarCalDlgInfo.dRebarTesion,			//钢筋强度设计值
		SH = g_rebarCalDlgInfo.dSectionHeight;			//截面高度

	//缩放应力
	double dCoeff = g_rebarCalDlgInfo.dStruSignifyFt * g_rebarCalDlgInfo.dDesignFt;
	int count = (int)tesionList.size();
	for (int m = 0; m < count; ++m)
	{
		tesionList[m] *= dCoeff;
	}

	//获取计算所用的应力区域
	int iHead = 0, iTail = 0;
	GetValidTesionHeadAndTail(tesionList, Ft, iHead, iTail);

	/*-------------------------------------------计算部分---------------------------------------------*/
	double dEndSpan = span;
	if (2 == count)
	{
		if (tesionList[0] < 0)
		{
			double tempSpan = tesionList[1] / (tesionList[1] - tesionList[0]) * dLineLength;
			GetAAndAct(0.0, tesionList[1], Ft, tempSpan, dA, dAct);
		}
		else if (tesionList[1] < 0)
		{
			double tempSpan = tesionList[0] / (tesionList[0] - tesionList[1]) * dLineLength;
			GetAAndAct(tesionList[0], 0.0, Ft, tempSpan, dA, dAct);
		}
		else
		{
			GetAAndAct(tesionList[0], tesionList[1], Ft, dLineLength, dA, dAct);
		}
	}
	else
	{
		//算A与Act面积
		for (int i = iHead; i < iTail; ++i)
		{
			GetAAndAct(tesionList[i], tesionList[i + 1], Ft, span, dA, dAct);
		}
		//单独算最后一段面积
		if (count - 1 == iTail)
		{
			if (int(dLineLength / span) < count)
				dEndSpan = dLineLength - span*count + 2.0*span;
		}
		GetAAndAct(tesionList[iTail - 1], tesionList[iTail], Ft, dEndSpan, dA, dAct);
	}

	/*取点号与位置*/
	//最大有效点号,0应力点号,0应力位置
	iMaxTValid_No = iTail + 1;
	iZeroT_No = iTail + 1;
	if (count - 1 == iTail || 2 == count)
	{
		dZeroP = dLineLength;
	}
	else if ((count - 2) == iTail)
	{
		double scale = 0.0;
		double dM = fabs((tesionList[iTail - 1] - tesionList[iTail]));
		if (!iszero(dM))
		{
			scale = tesionList[iTail - 1] / dM;
		}
		dZeroP = span*iTail - span + dEndSpan*scale;
	}
	else
	{
		dZeroP = span*iTail;
	}
	//砼强度对应点号,砼强度对应位置
	GetConcretePositions(tesionList, iTail, span, Ft, dLineLength, dEndSpan, iConcr_No, dConcrP);

	//结构截面宽度
	double dB = 1.0;
	//如果拉应力深度除以截面高度大于三分之二，拉应力应全部由钢筋承担
	if ((dConcrP / SH) > (3.0 / 2.0))
	{
		dAs = dB * dA * Rd / Fy;
	}
	//混凝土承担拉力不能大于总拉力30%；结构系数Rd大于0,适用新规范
	else if (Rd > 0)
	{
		if (dAct > dA*0.3)
		{
			dAct = dA*0.3;
		}
		dAs = dB*(dA*Rd - 0.6*dAct) / Fy;
	}
	else
	{
		if (dAct > dA*0.25)
		{
			dAct = dA*0.25;
		}
		dAs = (dA - dAct) / Fy;
	}
	dAs *= 1000;

	//参考配筋面积
	int number = 0;
	dReferArea = GetAreaByDiameter(number, dAs, info.dRebarDiameter);

	//弯矩
	double dBendingStress = 0.0;
	//轴力
	double dAxialStress = 0.0;
	//剪力
	double dShearSXY = 0.0,
		dShearSYZ = 0.0,
		dShearSXZ = 0.0;
	if (g_rebarCalDlgInfo.bCalcStress)
	{
		dBendingStress = GetBendingStress(tesionList, span, dLineLength);
		dAxialStress = GetAxialStress(tesionList, span, dLineLength);

		//SXY
		bvector<double>().swap(tesionList);
		getStressList(info.shearSXY, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		//不是多个0点或尖点时才计算
		if (0 == validflag || -2 == validflag)
		{
			dShearSXY = GetAxialStress(tesionList, span, dLineLength);
		}

		//SYZ
		bvector<double>().swap(tesionList);
		getStressList(info.shearSYZ, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		if (0 == validflag || -2 == validflag)
		{
			dShearSYZ = GetAxialStress(tesionList, span, dLineLength);
		}

		//SXZ
		bvector<double>().swap(tesionList);
		getStressList(info.shearSXZ, tesionList);
		validflag = checkTesionValid(tesionList, Ft);
		if (0 == validflag || -2 == validflag)
		{
			dShearSXZ = GetAxialStress(tesionList, span, dLineLength);
		}
	}

	/*-------------------------------------------结果输出部分---------------------------------------------*/
	WString strBend, strAxial, strShearSXY, strShearSYZ, strShearSXZ;
	if (g_rebarCalDlgInfo.bCalcStress)
	{
		strBend.Sprintf(L"%g", dBendingStress);
		strAxial.Sprintf(L"%g", dAxialStress);
		strShearSXY.Sprintf(L"%g", dShearSXY);
		strShearSYZ.Sprintf(L"%g", dShearSYZ);
		strShearSXZ.Sprintf(L"%g", dShearSXZ);
	}
	else
	{
		strBend.Sprintf(L"%s", " ");
		strAxial.Sprintf(L"%s", " ");
		strShearSXY.Sprintf(L"%s", " ");
		strShearSYZ.Sprintf(L"%s", " ");
		strShearSXZ.Sprintf(L"%s", " ");
	}

	//保存计算结果
	WString strResult;
	// 计算结果= 最大有效点号&0应力点号&0应力对应位置&总拉应力面积&混凝土强度对应点号&混凝土强度对应位置&混凝土拉应力面积&配筋面积&参考配筋面积
	strResult.Sprintf(
		L"%d%s%d%s%g%s%g%s%d%s%g%s%g%s%g%s%g%s%s%s%s%s%s%s%s%s%s",
		iMaxTValid_No, SeparatorAnd,	//最大有效点号
		iZeroT_No, SeparatorAnd,		//0应力点号
		dZeroP, SeparatorAnd,			//0应力对应位置
		dA, SeparatorAnd,				//总拉应力面积
		iConcr_No, SeparatorAnd,		//混凝土强度对应点号 
		dConcrP, SeparatorAnd,			//混凝土强度对应位置
		dAct, SeparatorAnd,				//混凝土拉应力面积
		dAs, SeparatorAnd,				//配筋面积
		dReferArea, SeparatorAnd,		//参考配筋面积
		strBend, SeparatorAnd,			//弯矩	
		strAxial, SeparatorAnd,			//轴力
		strShearSXY, SeparatorAnd,		//XY方向剪力
		strShearSYZ, SeparatorAnd,		//YZ方向剪力
		strShearSXZ);					//XZ方向剪力
	swprintf(info.strResult, L"%s", strResult.data());

	//保存钢筋计算结果 
	info.iRebarCount = number;
	info.dRebarArea = dReferArea;

	////打印
	//if (resultList.size() == oCalcResultItemVec.size())
	//{
	//	WString buff;
	//	int index = 0;
	//	for (auto it = oCalcResultItemVec.begin(); it != oCalcResultItemVec.end(); ++it)
	//	{
	//		buff.append(*it);
	//		buff.append(L" ");
	//		buff.append(resultList.at(index++));
	//		buff.append(L"\n");	
	//	}
	//	char msg[1024];
	//	WideCharToMultiByte(CP_ACP, 0, buff.data(), -1, msg, 1024, NULL, NULL);
	//	mdlDialog_dmsgsClear();
	//	mdlDialog_dmsgsPrintA(msg);
	//}
	return true;
}

void SetResultValToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	// 取出应力结果数据
	bvector<WString> pointResult;
	BeStringUtilities::Split(info.strResult, SeparatorAnd, pointResult);

	if (pointResult.begin() == pointResult.end())
	{
		//清空listbox
		for (int i = 0; i < (int)oCalcResultItemVec.size(); ++i)
		{
			pointResult.push_back(L"");
		}
	}

	//结果listbox
	DialogItemP dlgItemP = dbP->GetItemByTypeAndId(RTYPE_ListBox, LISTBOXID_CalRebar_CalResult);
	if (dlgItemP != NULL)
	{
		//计算项与结果项数量一致
		if (oCalcResultItemVec.size() != pointResult.size())
			return;

		ListModelP listModelP = mdlDialog_listBoxGetListModelP(dlgItemP->rawItemP);
		if (listModelP != NULL)
		{
			int index = 0;
			for (auto it = pointResult.begin(); it != pointResult.end(); ++it)
			{
				ListRowP	rowP = mdlListModel_getRowAtIndex(listModelP, index++);
				ListCellP	cellP = mdlListRow_getCellAtIndex(rowP, 2); 
				mdlListCell_setStringValue(cellP, pointResult.at(index - 1).data(), TRUE);
			}

			mdlDialog_listBoxSetListModelP(dlgItemP->rawItemP, listModelP, 0);
		}
	}
}

double GetAreaByDiameter(int& number, double dAs, double diameter)
{
	number = 0;
	double r = diameter * 0.5;
	double area = fc_pi * r * r;
	number = 1;
	if (dAs > area)
	{
		dAs /= area;
		int count = dAs;
		if (count < dAs)
		{
			++count;
		}
		number = count;
	}
	return number * area;
}
double GetAreaByRebarCount(double& diameter, double dAs, int number)
{
	int maxIndex = sizeof(m_diameterList) / sizeof(m_diameterList[0]) - 1;
	double radius = sqrt((dAs / number) / fc_pi);
	diameter = radius * 2.0;
	if (diameter <= m_diameterList[0])
	{
		diameter = m_diameterList[0];
	}
	else if (diameter == m_diameterList[maxIndex])
	{
		diameter = m_diameterList[maxIndex];
	}
	else
	{
		bool bFind = false;
		for (int i = 0; i < maxIndex; ++i)
		{
			if (m_diameterList[i] < diameter && diameter < m_diameterList[i + 1])
			{
				diameter = m_diameterList[i + 1];
				bFind = true;
				break;
			}
		}
	}

	return fc_pi * diameter * diameter * 0.25 * number;
}

void HookListBox_CalRebar_RebarInfo(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		ListModelP listModelP = mdlDialog_listBoxGetListModelP(pThis);
		if (nullptr == listModelP)
		{
			listModelP = mdlListModel_create(3);
		}
		else
		{
			mdlListModel_empty(listModelP, true);
			listModelP = mdlListModel_create(3);
		}
		ListRowP	pRow = mdlListRow_create(listModelP);
		ListCellP	pCell = NULL;

		//根数
		pCell = mdlListRow_getCellAtIndex(pRow, 0);
		mdlListCell_setStringValue(pCell, L"", TRUE);
		//把txt控件与listbox的cell绑定
		mdlListCell_setEditor(pCell, RTYPE_Text, TEXTID_CalRebar_ListCell_RebarNumber, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);

		//直径
		pCell = mdlListRow_getCellAtIndex(pRow, 1);
		mdlListCell_setStringValue(pCell, L"", TRUE);
		mdlListCell_setEditor(pCell, RTYPE_ComboBox, COMBOBOXID_CalRebar_ListCell_Diameter, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);

		//配筋面积
		mdlListCell_setStringValue(pCell = mdlListRow_getCellAtIndex(pRow, 2), L"", TRUE);

		mdlListModel_addRow(listModelP, pRow);
		mdlDialog_listBoxSetListModelP(pThis, listModelP, 3);
	}break;
	case DITEM_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookItem_CalRebar_CellCombbox_Diameter(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		WCharCP textValue = L"";
		GetListBoxEditCellStringValue(dimP, LISTBOXID_CalRebar_RebarInfo, &textValue);
		g_rebarCalDlgInfo.dDiameter = _wtof(textValue);
		g_rebarCalDlgInfo.iDameterIndex = -1;
		int dcount = sizeof(m_diameterList) / sizeof(m_diameterList[0]);
		for (int i = 0; i < dcount;++i)
		{
			if (g_rebarCalDlgInfo.dDiameter == m_diameterList[i])
			{
				g_rebarCalDlgInfo.iDameterIndex = i;
				break;
			}
		}
		break;
	};
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// 选择节点为配盘线时，进入配筋计算
				if (mdlTreeNode_getLevel(pTreeNode) == 3)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						// 钢筋直径
						int dnumber = sizeof(m_diameterList) / sizeof(m_diameterList[0]);
						if (g_rebarCalDlgInfo.iDameterIndex > -1 && g_rebarCalDlgInfo.iDameterIndex < dnumber)
						{
							it_find->second.dRebarDiameter = m_diameterList[g_rebarCalDlgInfo.iDameterIndex];
						}

						bvector<WString> vecResultDat;
						BeStringUtilities::Split(it_find->second.strResult, SeparatorAnd, vecResultDat);
						// 取出配筋面积
						double dAs = _wtof(vecResultDat.at(CALCRESULT_IDX_AS).data());

						// 根据钢筋直径，计算配筋面积
						it_find->second.dRebarArea = GetAreaByDiameter(it_find->second.iRebarCount, dAs, it_find->second.dRebarDiameter);

						//结果写入钢筋listbox
						SetRebarInfoToListBox(dimP->db, it_find->second);
					}
				}
			}
		}
	}
	break;
	case DITEM_MESSAGE_DESTROY:
	{
		ListModelP pListModel = mdlDialog_listBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookItem_CalRebar_CellTxt_RebarNumber(DialogItemMessage* dmp)
{
	dmp->msgUnderstood = true;
	switch (dmp->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		WCharCP textValue = L"";
		GetListBoxEditCellStringValue(dmp, LISTBOXID_CalRebar_RebarInfo, &textValue);
		g_rebarCalDlgInfo.iRebarCount = _wtoi(textValue);
		break;
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItemP  pTree = mdlDialog_itemGetByTypeAndId(dmp->db, RTYPE_Tree, TREEID_CalRebar_SectionList, 0);
		int nSelect = -1;
		Point2d* pSelect = NULL;
		if (SUCCESS == mdlDialog_treeGetSelections(&nSelect, &pSelect, pTree->rawItemP))
		{
			GuiTreeModel*  pTreeModel = mdlDialog_treeGetTreeModelP(pTree->rawItemP);
			if (pTreeModel)
			{
				GuiTreeCell* pTreeCell = mdlTreeModel_getDisplayCellAtIndexes(pTreeModel, pSelect[0].y, pSelect[0].x);
				GuiTreeNode* pTreeNode = mdlTreeCell_getNode(pTreeCell);

				// 选择节点为配盘线时，进入配筋计算
				if (mdlTreeNode_getLevel(pTreeNode) == 3)
				{
					MSValueDescr uvalue;
					mdlTreeNode_getValue(pTreeNode, uvalue);
					auto it_find = m_RebarResults.find(uvalue.GetWCharCP());
					if (it_find != m_RebarResults.end())
					{
						// 钢筋根数
						it_find->second.iRebarCount = g_rebarCalDlgInfo.iRebarCount;

						bvector<WString> vecResultDat;
						BeStringUtilities::Split(it_find->second.strResult, SeparatorAnd, vecResultDat);
						// 取出配筋面积
						double dAs = _wtof(vecResultDat.at(CALCRESULT_IDX_AS).data());

						// 根据钢筋根数，计算配筋面积和直径
						it_find->second.dRebarArea = GetAreaByRebarCount(it_find->second.dRebarDiameter, dAs, it_find->second.iRebarCount);

						//结果写入钢筋listbox
						SetRebarInfoToListBox(dmp->db, it_find->second);
					}
				}
			}
		}
		break;
	}
	default:
		dmp->msgUnderstood = false;
		break;
	}
}
//结果写入钢筋listbox
void SetRebarInfoToListBox(MSDialogP dbP, const ReBarInfo& info)
{
	DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_RebarInfo, 0);
	ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
	ListCell*		pCell = NULL;

	//清空listbox
	if (0 == wcscmp(info.strResult, L""))
	{
		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 0);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 1);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 0, 2);
		mdlListCell_setStringValue(pCell, L"", TRUE);

		mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);//刷新listbox
		return;
	}

	//将修改后的CELL值反写到listbox中
	WChar buffer[256];
	// 根数
	swprintf(buffer, L"%d", info.iRebarCount);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 0);
	mdlListCell_setStringValue(pCell, buffer, TRUE);
	// 钢筋直径
	swprintf(buffer, L"%g", info.dRebarDiameter);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 1);
	mdlListCell_setStringValue(pCell, buffer, TRUE);

	// 钢筋面积
	swprintf(buffer, L"%g", info.dRebarArea);
	pCell = mdlListModel_getCellAtIndexes(pModel, 0, 2);
	mdlListCell_setStringValue(pCell, buffer, TRUE);

	mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);//刷新listbox

	//更新结果listbox参考面积
	pLBox = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ListBox, LISTBOXID_CalRebar_CalResult, 0);
	pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);

	if (pModel != NULL)
	{
		DialogItem* pRebarCmb = mdlDialog_itemGetByTypeAndId(dbP, RTYPE_ComboBox,COMBOBOXID_CalRebar_RebarType,0);
		StringList* sl = mdlDialog_comboBoxGetStrListP(pRebarCmb->rawItemP);
		
		//RscFileHandle rfHandle;
		//mdlResource_openFile(&rfHandle, L"outputRebarDlg.r", RSC_READONLY);
		//StringListP sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_CalRebar_RebarType);
		WString strRebarType;
		mdlStringList_getMemberString(strRebarType, NULL, sl, (long)g_rebarCalDlgInfo.iRebarType);

		// 更新参考配筋行
		swprintf(buffer, L"参考配筋: %s，%d根%g", strRebarType, info.iRebarCount, info.dRebarDiameter);
		pCell = mdlListModel_getCellAtIndexes(pModel, 8, 1);
		mdlListCell_setStringValue(pCell, buffer, TRUE);

		pCell = mdlListModel_getCellAtIndexes(pModel, 8, 2);
		swprintf(buffer, L"%g", info.dRebarArea);
		mdlListCell_setStringValue(pCell, buffer, TRUE);

		mdlDialog_listBoxSetListModelP(pLBox->rawItemP, pModel, 0);
	}
}

void HOOKText_CalRebar_RebarTesion(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	case DITEM_MESSAGE_STATECHANGED:
	{
		int count = sizeof(m_rebarStrengthList) / sizeof(m_rebarStrengthList[0]);
		g_rebarCalDlgInfo.iRebarType = count;
		double stress = g_rebarCalDlgInfo.dRebarTesion;
		for (int loop = 0; loop < count; ++loop)
		{
			if (stress == m_rebarStrengthList[loop])
			{
				g_rebarCalDlgInfo.iRebarType = loop;
				break;
			}
		}
		mdlDialog_itemsSynch(dimP->db);
		break;
	};

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HOOKText_CalRebar_ConcrTesion(DialogItemMessageP dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	case DITEM_MESSAGE_STATECHANGED:
	{
		int count = sizeof(m_concrStrengthList) / sizeof(m_concrStrengthList[0]);
		g_rebarCalDlgInfo.iConcrType = count;
		double stress = g_rebarCalDlgInfo.dConcrTesion;
		for (int loop = 0; loop < count; ++loop)
		{
			if (stress == m_concrStrengthList[loop])
			{
				g_rebarCalDlgInfo.iConcrType = loop;
				break;
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

void HookDialog_CalRebar_AddRebarLine(DialogMessage *dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_ACTIONBUTTON:
	{
		if (dmP->u.actionButton.actionType == ACTIONBUTTON_OK)
		{
			WString strChk;
			strChk.append(g_rebarCalDlgInfo.rebarName);
			strChk.Trim();

			if (strChk.length() == 0)
			{
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"请输入配筋线名称！", MessageBoxIconType::Information);
				//取消对话框的关闭
				dmP->u.actionButton.abortAction = TRUE;
			}
			else
			{
				wcscpy(g_rebarCalDlgInfo.rebarName, strChk.data());
			}
		}

		break;
	}
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void outputRebarCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_RebarCalculate, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_RebarCalculate);
}

void GetDataFromControls(rebarInfo& orebarInfo)
{
	orebarInfo.dStruSignifyFt	= g_rebarCalDlgInfo.dStruSignifyFt;	//结构重要系数
	orebarInfo.dDesignFt		= g_rebarCalDlgInfo.dDesignFt;		//设计状况系数
	orebarInfo.dStruFt			= g_rebarCalDlgInfo.dStruFt;		//结构系数
	orebarInfo.dDefPointSpan	= g_rebarCalDlgInfo.dDefPointSpan;	//默认点距(mm)
	orebarInfo.iRebarType		= g_rebarCalDlgInfo.iRebarType;		//钢筋种类下标，用于获取fy
	orebarInfo.dRebarTesion		= g_rebarCalDlgInfo.dRebarTesion;	//钢筋抗拉强度
	orebarInfo.iConcrType		= g_rebarCalDlgInfo.iConcrType;		//混凝土种类下标，用于获取ft
	orebarInfo.dConcrTesion		= g_rebarCalDlgInfo.dConcrTesion;	//混凝土轴心抗拉强度
	orebarInfo.dSectionHeight	= g_rebarCalDlgInfo.dSectionHeight;	//截面高度
	orebarInfo.bCalcStress		= g_rebarCalDlgInfo.bCalcStress;	//是否计算弯矩、轴力、剪力
}

void GetListBoxEditCellStringValue(DialogItemMessage* dimP, RscId listboxId, WCharCP* textValue)
{
	DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, listboxId, 0);
	if (pLBox)
	{
		int		    	row = -1, col = -1;
		MSValueDescr	uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		mdlListCell_getStringValue(pCell, textValue);
	}
}

void InitDrawColorShape(DialogItemMessage* dimP, GuiTreeNode* pRebarLineNode, const bvector<WString>& vecData)
{
	// 取得工况节点
	GuiTreeNode* caseNode = mdlTreeNode_getParent(pRebarLineNode);
	DgnModelRefP target = NULL;
	if (SUCCESS == mdlModelRef_createWorking(&target, mdlDgnFileObj_getMasterFile(), wtoi(vecData.at(KEY_IDX_SECTIONID).data()), TRUE, TRUE))
	{
		if (!mdlModelRef_areSame(MASTERFILE, target))
		{
			s_pDCShape->CloseDrawFunction();
			s_drawtempLine.CloseDrawFunction();
			ClearclipfaceShow();
			mdlModelRef_activateAndDisplay(target);
			//ApplyCaeDisplayStyle(L"CAEMesh:Edge", tcb->lstvw);// 网格
			//ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);// 光滑	
		}
		mdlModelRef_freeWorking(target);
		MsdiMsCaeSiUtility::fitView(0);
	}

	// 项目ID
	int iProjectId = wtoi(vecData.at(KEY_IDX_PROJECTID).data());
	// 工况ID
	int iCaseID = wtoi(vecData.at(KEY_IDX_CASEID).data());

	//获取结果类型
	bvector<WString> vecRstTypeNames;
	getRstTypeByPjIdAndCaseId(iProjectId, iCaseID, vecRstTypeCodes, vecRstTypeNames);
	vecRstTypeNames.clear();
	bvector<WString>().swap(vecRstTypeNames);

	double tStrValue = 0;
	double tEndValue = 0;
	s_pDCShape->m_bAutomatic = true;
	//通过用户的选择获取结果类型(取值下标对应数据库的RSTYPE表的索引号减1,获取结果类型的函数中从第二个索引开始取)
	s_pDCShape->ResultTypeCode = vecRstTypeCodes.at(10);
	//Header
	s_pDCShape->Header = g_RstType_Name[s_pDCShape->ResultTypeCode];
	//Unit
	s_pDCShape->Unit = g_RstType_Unit[s_pDCShape->ResultTypeCode];

	DPoint3d minPos, maxPos;
	GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, iProjectId, iCaseID, s_pDCShape->ResultTypeCode, minPos, maxPos);
	//根据选择的具体工况和结果类型设置色带
	s_pDCShape->ProjectId = iProjectId;
	s_pDCShape->CaseID = iCaseID;
	s_pDCShape->StartValue = tStrValue;
	s_pDCShape->EndValue = tEndValue;
	bvector<DPoint3d>().swap(s_pDCShape->m_vecMaxPts);
	bvector<DPoint3d>().swap(s_pDCShape->m_vecMinPts);
	s_pDCShape->m_vecMaxPts.push_back(maxPos);
	s_pDCShape->m_vecMinPts.push_back(minPos);
	//并修改色带中的proName和caseName
	getProjectInfoById(s_pDCShape->ProjectName, s_pDCShape->ProjectUnit, iProjectId);

	WString strCaseName;
	mdlTreeNode_getDisplayTextWString(mdlTreeNode_getParent(caseNode), strCaseName);
	s_pDCShape->CaseName = strCaseName;

	//重置为自动设置色带
	s_pDCShape->m_bAutomatic = true;
	//自动等分色带的值
	s_pDCShape->divideColorRange();

	mdlDialog_itemsSynch(dimP->db);
}