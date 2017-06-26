#pragma once

void SetCAERstDispCmd(WCharCP unparsed);

void startShowCutFaceContour(DrawColorShape* pDCShape);

void startShowContour(DrawColorShape* pDCShape, DPoint3dCP BasePt = NULL, bool bClear = true);

void enableWidget(MSDialogP db, bool enable);

void initConditionValuesByTypes(MSDialogP db);

void SolidDisp_adjustVSashDialogItems(MSDialogP   db, BSIRect     *pOldContent, bool        refreshItems);

void HookTree_SolidDisp(DialogItemMessage   *dimP);

void HookDialog_SolidDisp(DialogMessage *dmP);

void HookButton_SolidDisp_ShowSet(DialogItemMessage *dmP);

void HookComboBox_SolidDisp_ResultType(DialogItemMessage *dimP);

void HookComboBox_SolidDisp_ShowModel(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_CondType(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_CondVal(DialogItemMessage* dimP);

void HookButton_SolidDisp_AddC(DialogItemMessage* dimP);

void HookButton_SolidDisp_DelC(DialogItemMessage* dimP);

void HookButton_SolidDisp_ClearC(DialogItemMessage* dimP);

void HookButton_SolidDisp_Show(DialogItemMessage* dimP);

void HookButton_Damage_Show(DialogItemMessage* dimP);

void HookComboBox_SolidDisp_ShowType(DialogItemMessage* dimP);

void HookSash_SolidDisp_vSashHook(DialogItemMessage* dimP);

void HookButton_Boundary_Show(DialogItemMessage* dimP);

//获取CAE边界信息单元体的sql语句
StatusInt GetBoundarySolidSQL(Statement* pStatement, BeSQLiteDbCR db);
//创建边界信息所有的面
void BuildBoundaryFaces(int projectId,int caseId, WCharCP ngName);
/**
* @brief  取得CAE的Mesh体的节点和节点坐标集合
* @param[out] vecIndices		Mesh体的节点集合
* @param[out] vecPoints			Mesh体的节点坐标集合
* @param[out] oFaceNodeStrMap	Mesh体各面的节点字符串集合(如：1-2-3-4)
* @param[in]  vecNodeId			nodeID集合
* @param[in]  basePt			CAE模型的放置坐标点(相对于原点的偏移值）
* @return 无返回值
*/
void GetBoundaryFacesInfo(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt, WStringCR sUnit, Statement* pStatement);
void MakeBoundaryFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4);
WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4);
//为坐标加上basePt确定真实坐标
DPoint3d GetPointFromBasePoint(DPoint3dCP basePt, WStringCR sUnit, int iIdx, Statement* pStatement);

//清除边界信息
void ClearBoundary();

////提取轮廓线代码
//#pragma region
////获取CAE所有mesh面和面节点集合
//bool GetFaceInfos(int projectId, int caseId, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt);
////提取外轮廓
//void ShowOuterContour(int projectId, int caseId, int levelId, DPoint3d basePt);
//#pragma endregion
