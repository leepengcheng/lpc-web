/*--------------------------------------------------------------------------------------+
|
|  $Source: CreateCAESolid.cpp $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "MsdiMsCaeSipch.h"
extern MSDialogP jdoytBar;
extern WCharP 	  jdwcMessageText;
extern int       jdPercent;
bool CreateCAESolidTool::CreateElement(DPoint3dCP basePt)
{
	double index = 1.0;
	bvector<int> vecNodeIds; // Mesh体相对应的CAE结果节点集合
	bvector<int> vecIndices; // Mesh体的节点集合
	bvector<DPoint3d> vecPoints; // Mesh体的顶点坐标集合
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupFlags ngfCAESolid;
	NamedGroupPtr ngCAESolid; 
	WString uniqueCaeModelName;
	WString uniqueCaeModelNameHide;
	bvector<WString> vecFaceNodeStr; 
	DgnPlatform::LevelId  levelId_CaeModel;
	DgnPlatform::LevelId  levelId_CaeModel_Hide;

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	// 循环检查是否存在空命名组(命名组中没有元素)
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

	// 根据工程ID生成CAE模型命名组名以及对应的隐藏层名
	uniqueCaeModelName = makeNgName_Model(m_ProjectID);
	uniqueCaeModelNameHide.append(uniqueCaeModelName);
	uniqueCaeModelNameHide.append(L"HIDE");

	// 设置命名组的属性
	ngfCAESolid.m_selectMembers = 0;// 表示自动多选
	ngfCAESolid.m_anonymous = 1;// 该命名组不会再命名组列表中显示(即隐藏的临时命名组)	

	// 创建或取得CAE模型的显示图层
	if (SUCCESS != mdlLevel_create(&levelId_CaeModel, MASTERFILE, uniqueCaeModelName.c_str(), LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_CaeModel, MASTERFILE, LEVEL_NULL_ID, uniqueCaeModelName.c_str());

	// 创建或取得CAE模型的隐藏图层
	if (SUCCESS != mdlLevel_create(&levelId_CaeModel_Hide, MASTERFILE, uniqueCaeModelNameHide.c_str(), LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_CaeModel_Hide, MASTERFILE, LEVEL_NULL_ID, uniqueCaeModelNameHide.c_str());
	// 隐藏该层的所有元素
	mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel_Hide, false);
	
	// 将模型创建位置（相对于原点(0,0,0)的偏移量）保存到Level的Description
	WChar  basePtStr[256];
	_swprintf(basePtStr, L"%f_%f_%f_%s", basePt->x, basePt->y, basePt->z, m_ProjectUnit);
	mdlLevel_setDescription(MASTERFILE, levelId_CaeModel, basePtStr);
	mdlLevel_setDescription(MASTERFILE, levelId_CaeModel_Hide, basePtStr);
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 5);

	// 创建命名组
	if (NG_Success == NamedGroup::Create(ngCAESolid, uniqueCaeModelName.c_str(), L"CAEMODELGROUP", ngfCAESolid, ACTIVEMODEL))
	{
		// 打开CAE结果数据库
		StatusInt rc  = OpenDB(m_db);
		if (rc == BE_SQLITE_OK)
		{
			m_savept = new Savepoint(m_db, "CreateElement", true);
			m_savept->Begin();

			m_statment = new Statement();
			// 生成取得CAE单元体信息的SQL语句
			rc = GetSolidFromDatabase();

			if (rc == BE_SQLITE_OK)
			{
				// 设置查询条件
				m_statment->BindInt(1, m_ProjectID);

				// 循环CAE单元体数据，在MS中生成单元体
				MSElementDescrP m_resultEdp;
				while (m_statment->Step() == BE_SQLITE_ROW)
				{
					// 初始化变量
					vecIndices.clear();
					vecPoints.clear();
					vecFaceNodeStr.clear();
					vecNodeIds.clear();// 节点ID与顶点ID一一对应。

					// 取得CAE的Mesh体的节点和顶点坐标集合
					MakeMeshIndices(vecIndices, vecPoints, vecFaceNodeStr, vecNodeIds, basePt);

					// 当节点数>=3，生成单元面或单元体
					if (vecPoints.size() >= 3)
					{
						// 生成CAE的Mesh体
						if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
						{
							// 生成Mesh体相对应的CAE结果节点集合
							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
							MeshProperty meshPro;
							for (int i = 0; i < vecNodeIds.size(); i++)
							{
								if (i < 30)
								{
									meshPro.ID[i] = vecNodeIds.at(i);
								}
							}
							
							meshPro.ElemID = m_statment->GetValueInt(SQL_ELEMID_IDX);			//元素ID
							meshPro.MaterialNumber = m_statment->GetValueInt(SQL_MATERIAL_IDX);	// 材料类型
							meshPro.ElemType = m_statment->GetValueInt(SQL_ELEMTYPE_IDX);		// 单元类型
							meshPro.RealConstantNumber = m_statment->GetValueInt(SQL_REAL_IDX);	// 实常数

							// 生成Mesh体的ElementHandle
							EditElementHandle eeh(m_resultEdp, true, true, ACTIVEMODEL);

							// 将Mesh体相对应的CAE结果节点集合保存到ElementHandle中
							eeh.ScheduleWriteXAttribute(handlerId, XAttrID_MeshProperty, sizeof(MeshProperty), &meshPro);

							ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
							propsSetter->SetLevel(levelId_CaeModel);
							propsSetter->Apply(eeh);
							// ElementHandle添加到视图中
							eeh.AddToModel();

							// 通过元素ID向组中增加元素
							ngCAESolid->AddMember(eeh.GetElementP()->GetID(), ACTIVEMODEL, NamedGroupMemberFlags());
						}

						SaveFaceInfToDb(vecFaceNodeStr, vecNodeIds, uniqueCaeModelName, m_statment->GetValueInt(SQL_ELEMID_IDX));
						index++;
						if (index<5000)
						{
							mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 5 + (index / 5000.0)*80);
						}
					}

					// 清空数组
					bvector<int>().swap(vecNodeIds);
					bvector<int>().swap(vecIndices);
					bvector<DPoint3d>().swap(vecPoints);
				}
			}

			m_statment->Finalize();
			m_savept->Commit();

			//在CAE模型绘制到dgn之后，保存该模型的命名组以及所在model
			bvector<InstanceInfo> infos = readInstanceInfo();
			InstanceInfo newInfo;
			wcscpy(newInfo.NgName, uniqueCaeModelName.data());
			newInfo.modelId = mdlModelRef_getModelID(MASTERFILE);
			newInfo.projectId = m_ProjectID;
			infos.push_back(newInfo);
			savingInstanceInfo(infos);
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 90);
		}

		m_db.CloseDb();
		ngCAESolid->WriteToFile(true);
		mdlLevelTable_rewrite(MASTERFILE);
	}

	//BuildHiliteMeshes(basePt, levelId_CaeModel);

	return true;
} 

// 生成面节点的节点ID字符串(如"1-2-3-4")
WString CreateCAESolidTool::MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4)
{
	WString sKey;
	int iTmp;

	if (NodeId4 != -1)
	{
		if (NodeId1<NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1<NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId1<NodeId4) { iTmp = NodeId1; NodeId1 = NodeId4; NodeId4 = iTmp; }
		if (NodeId2<NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2<NodeId4) { iTmp = NodeId2; NodeId2 = NodeId4; NodeId4 = iTmp; }
		if (NodeId3<NodeId4) { iTmp = NodeId3; NodeId3 = NodeId4; NodeId4 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d-%d", NodeId1, NodeId2, NodeId3, NodeId4);
	}
	else
	{
		if (NodeId1<NodeId2) { iTmp = NodeId1; NodeId1 = NodeId2; NodeId2 = iTmp; }
		if (NodeId1<NodeId3) { iTmp = NodeId1; NodeId1 = NodeId3; NodeId3 = iTmp; }
		if (NodeId2<NodeId3) { iTmp = NodeId2; NodeId2 = NodeId3; NodeId3 = iTmp; }

		sKey.Sprintf(L"%d-%d-%d", NodeId1, NodeId2, NodeId3);
	}

	return sKey;
}

// 保存面节点信息到数据库中
bool CreateCAESolidTool::SaveFaceInfToDb(bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID)
{
	StatusInt	rc;
	AString lsSql, FaceDataSql;
	Statement *l_statment = new Statement();;

	MakeFaceDataSql(FaceDataSql, vecFaceNodeStr, vecNodeIds, MSLevelName, iElemID);

	lsSql.append("INSERT INTO FACES(ProjectId, ElemID, MSLevelName, NodeIdStr, Node1, Node2, Node3, Node4, ElemShowFlg, ContourShowFlg) ").append(FaceDataSql);

	l_statment->Prepare(m_db, lsSql.c_str());

	rc = l_statment->Step();
	l_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 生成面节点数据的SQL语句
bool CreateCAESolidTool::MakeFaceDataSql(AString& FaceDataSql, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID)
{
	WString FaceNodeStr;
	Utf8String sNodeIdStr;
	char lsFaceDataSql[100];
	int iIdx = 0;
	int iNodeId1;
	int iNodeId2;
	int iNodeId3;
	int iNodeId4;

	for (int i = 0; i < vecFaceNodeStr.size(); i++)
	{
		if (i > 0)
		{
			FaceDataSql.append("UNION ALL ");			
		}

		Utf8String sMSLevelName;
		BeStringUtilities::WCharToUtf8(sMSLevelName, MSLevelName.c_str());
		BeStringUtilities::WCharToUtf8(sNodeIdStr, vecFaceNodeStr.at(i).c_str());

		iNodeId1 = vecNodeIds.at(iIdx++);
		iNodeId2 = vecNodeIds.at(iIdx++);
		iNodeId3 = vecNodeIds.at(iIdx++);
		iNodeId4 = vecNodeIds.at(iIdx++);

		if (iNodeId4 == 0)
		{
			sprintf_s(lsFaceDataSql, "SELECT %d,%d,'%s','%s',%d,%d,%d,null,1,1 \n", m_ProjectID, iElemID, sMSLevelName, sNodeIdStr, iNodeId1, iNodeId2, iNodeId3);
		}
		else
		{
			sprintf_s(lsFaceDataSql, "SELECT %d,%d,'%s','%s',%d,%d,%d,%d,1,1 \n", m_ProjectID, iElemID, sMSLevelName, sNodeIdStr, iNodeId1, iNodeId2, iNodeId3, iNodeId4);
			iIdx++;
		}

		FaceDataSql.append(lsFaceDataSql); 

		sNodeIdStr.clear();
		FaceNodeStr.clear();
	}

	return true;
}

/**
* @brief  取得CAE的Mesh体的节点和节点坐标集合
* @param[out] vecIndices		Mesh体的节点集合
* @param[out] vecPoints			Mesh体的节点坐标集合
* @param[out] oFaceNodeStrMap	Mesh体各面的节点字符串集合(如：1-2-3-4)
* @param[in]  vecNodeId			nodeID集合
* @param[in]  basePt			CAE模型的放置坐标点(相对于原点的偏移值）
* @return 无返回值
*/
void CreateCAESolidTool::MakeMeshIndices(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt)
{
	int iNodeCount;
	int iIdx;
	int iNodeID;
	bvector<int>::iterator l_it;
	// 面的节点ID
	bvector<int> vecNodes;

	// 取得元素的节点数
	iNodeCount = m_statment->GetValueInt(SQL_NODECOUNT_IDX);

	for (int i = 0; i < iNodeCount; i++)
	{
		iIdx = i * 4 + SQL_NODE_IDX;
		iNodeID = m_statment->GetValueInt(iIdx);

		l_it = find(vecNodes.begin(), vecNodes.end(), iNodeID);
		if (l_it == vecNodes.end())
		{
			vecNodes.push_back(iNodeID);
			vecPoints.push_back(GetPointFromStatment(basePt, iIdx));
		}
	}

	// CAE元素的节点数为8时，元素为体
	if (iNodeCount == 8)
	{
		// 判断节点数量
		if (vecNodes.size() == 8)
		{
			// 数量为8时，六面体
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 2, 1);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 1, 2, 6, 5);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 2, 3, 7, 6);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 4, 8, 7);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 1, 5, 8);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 5, 6, 7, 8);
		}
		else if (vecNodes.size() == 6)
		{
			// 数量为6时，五面体
			if (m_statment->GetValueInt(SQL_NODE_IDX + 8) == m_statment->GetValueInt(SQL_NODE_IDX + 12)
				&& m_statment->GetValueInt(SQL_NODE_IDX + 24) == m_statment->GetValueInt(SQL_NODE_IDX + 28))
			{
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 2, 1, -1);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 1, 2, 5, 4);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 2, 3, 6, 5);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 1, 3, 6);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 5, 6, -1);
			}
			else
			{
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 2, 1);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 1, 2, 5, -1);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 2, 3, 6, 5);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 4, 6, -1);
				MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 1, 5, 6);
			}
		}
		else if (vecNodes.size() == 4)
		{
			// 数量为4时，四面体
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 2, 1, -1);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 1, 2, 4, -1);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 2, 3, 4, -1);
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 1, -1);
		}
		else
		{
			int k,j;
			k = 0;
			j = k;
		}
	}
	// CAE元素的节点数为4时，元素为面
	else if (iNodeCount == 4)
	{
		if (vecNodes.size() == 4)
		{
			// 数量为4时，四边矩形面
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 4, 3, 2, 1);
		}
		else if (vecNodes.size() == 3)
		{
			// 数量为3时，三角面
			MakeFaceIndices(vecIndices, vecNodeId, vecFaceNodeStr, vecNodes, 3, 2, 1, -1);
		}
	}

	bvector<int>().swap(vecNodes);
}

void CreateCAESolidTool::MakeFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4)
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

// 取得节点的坐标
DPoint3d CreateCAESolidTool::GetPointFromStatment(DPoint3dCP basePt, int iIdx)
{
	DPoint3d ptTmp;

	ptTmp.x = basePt->x + m_statment->GetValueDouble(iIdx + 1) * UOR_PER_UNIT(m_ProjectUnit);
	ptTmp.y = basePt->y + m_statment->GetValueDouble(iIdx + 2) * UOR_PER_UNIT(m_ProjectUnit);
	ptTmp.z = basePt->z + m_statment->GetValueDouble(iIdx + 3) * UOR_PER_UNIT(m_ProjectUnit);

	return ptTmp;
}

// 生成取得CAE单元体信息的SQL语句
DbResult CreateCAESolidTool::GetSolidFromDatabase(void)
{
	AString strSql;

	// 生成取得CAE单元体信息的SQL语句
	strSql.append("SELECT ELEMENTS.ElemID, \n");
	strSql.append(" ELEMENTS.MaterialNumber, ELEMENTS.ElemType, ELEMENTS.RealConstantNumber, \n");
	strSql.append(" ELEMENTS.NodeCount, \n");
	strSql.append("	node1.ID, node1.X, node1.Y, node1.Z, \n");
	strSql.append("	node2.ID, node2.X, node2.Y, node2.Z, \n");
	strSql.append("	node3.ID, node3.X, node3.Y, node3.Z, \n");
	strSql.append("	node4.ID, node4.X, node4.Y, node4.Z, \n");
	strSql.append("	node5.ID, node5.X, node5.Y, node5.Z, \n");
	strSql.append("	node6.ID, node6.X, node6.Y, node6.Z, \n");
	strSql.append("	node7.ID, node7.X, node7.Y, node7.Z, \n");
	strSql.append("	node8.ID, node8.X, node8.Y, node8.Z \n");
	strSql.append("FROM ELEMENTS \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node1 ON ELEMENTS.node1 = node1.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node2 ON ELEMENTS.node2 = node2.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node3 ON ELEMENTS.node3 = node3.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node4 ON ELEMENTS.node4 = node4.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node5 ON ELEMENTS.node5 = node5.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node6 ON ELEMENTS.node6 = node6.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node7 ON ELEMENTS.node7 = node7.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node8 ON ELEMENTS.node8 = node8.ID \n");
	strSql.append("WHERE \n");
	strSql.append("	ELEMENTS.ProjectId = @ProjectId \n");

	return m_statment->Prepare(m_db, strSql.c_str());
}

void CreateCAESolidTool::SetupAndPromptForNextAction()
{
	//WString     msgStr;

	//RmgrResource::LoadWString(msgStr, (RscFileHandle)0, COMMAND_SolidAllShow, STRINGLISTID_Prompts, GetMdlDesc());
	//NotificationManager::OutputPrompt(msgStr.c_str());
	return;
}

void CreateCAESolidTool::_OnDynamicFrame(DgnButtonEventCR ev)
{
	return;
}

bool CreateCAESolidTool::_OnDataButton(DgnButtonEventCR ev)
{
	DPoint3dCP basePt = ev.GetPoint();
	//进度条
	size_t len = strlen(TXT_CAESolidPlace) + 1;
	jdwcMessageText = (WCharP)malloc(len*sizeof(WChar));
	BeStringUtilities::CurrentLocaleCharToWChar(jdwcMessageText, TXT_CAESolidPlace, len);
	// 打开进度条
	jdoytBar = mdlDialog_completionBarOpen(jdwcMessageText);
	CreateElement(basePt);
	mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 100);
	mdlDialog_completionBarClose(jdoytBar);
	jdoytBar = NULL;

	_ExitTool();
	return false; 
}

void CreateCAESolidTool::_OnPostInstall()
{
	AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.

	__super::_OnPostInstall();
}

void CreateCAESolidTool::InstallNewInstance(int toolId, int toolPrompt, int iProjectID, WString sProjectUnit)
{
	CreateCAESolidTool* tool = new CreateCAESolidTool(toolId, toolPrompt, iProjectID, sProjectUnit);
	// 启动工具
	tool->InstallTool();
}

BENTLEY_NAMESPACE_NAME::StatusInt CreateCAESolidTool::GetHiliteSolidSQL()
{
	AString strSql;
	strSql.append("SELECT \n");
	strSql.append(" b.eElemID as ElemID,b.MaterialNumber,b.ElemType,b.RealConstantNumber,b.NodeCount, \n");
	strSql.append(" b.ID, b.X, b.Y, b.Z, \n");
	strSql.append(" b.ID1, b.X1, b.Y1, b.Z1, \n");
	strSql.append(" b.ID2, b.X2, b.Y2, b.Z2, \n");
	strSql.append(" b.ID3, b.X3, b.Y3, b.Z3, \n");
	strSql.append(" b.ID4, b.X4, b.Y4, b.Z4, \n");
	strSql.append(" b.ID5, b.X5, b.Y5, b.Z5, \n");
	strSql.append(" b.ID6, b.X6, b.Y6, b.Z6, \n");
	strSql.append(" b.ID7, b.X7, b.Y7, b.Z7  \n");
	strSql.append("FROM \n");
	strSql.append(" (SELECT FACES.ElemID as fElemID FROM FACES INNER JOIN BOUNDARY ON FACES.NodeIdStr = BOUNDARY.BoundaryIdStr WHERE FACES.ProjectId = @ProjectId) a \n");
	strSql.append("LEFT OUTER JOIN \n");
	strSql.append(" (SELECT ELEMENTS.ElemID as eElemID, \n");
	strSql.append(" ELEMENTS.MaterialNumber, ELEMENTS.ElemType, ELEMENTS.RealConstantNumber, \n");
	strSql.append(" ELEMENTS.NodeCount, \n");
	strSql.append("	node1.ID as ID,  node1.X as X,  node1.Y as Y,  node1.Z as Z,  \n");
	strSql.append("	node2.ID as ID1, node2.X as X1, node2.Y as Y1, node2.Z as Z1, \n");
	strSql.append("	node3.ID as ID2, node3.X as X2, node3.Y as Y2, node3.Z as Z2, \n");
	strSql.append("	node4.ID as ID3, node4.X as X3, node4.Y as Y3, node4.Z as Z3, \n");
	strSql.append("	node5.ID as ID4, node5.X as X4, node5.Y as Y4, node5.Z as Z4, \n");
	strSql.append("	node6.ID as ID5, node6.X as X5, node6.Y as Y5, node6.Z as Z5, \n");
	strSql.append("	node7.ID as ID6, node7.X as X6, node7.Y as Y6, node7.Z as Z6, \n");
	strSql.append("	node8.ID as ID7, node8.X as X7, node8.Y as Y7, node8.Z as Z7  \n");
	strSql.append("FROM ELEMENTS \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node1 ON ELEMENTS.node1 = node1.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node2 ON ELEMENTS.node2 = node2.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node3 ON ELEMENTS.node3 = node3.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node4 ON ELEMENTS.node4 = node4.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node5 ON ELEMENTS.node5 = node5.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node6 ON ELEMENTS.node6 = node6.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node7 ON ELEMENTS.node7 = node7.ID \n");
	strSql.append("	LEFT OUTER JOIN (SELECT ID, X, Y, Z FROM NODES WHERE ProjectId = @ProjectId) node8 ON ELEMENTS.node8 = node8.ID \n");
	strSql.append(" WHERE \n");
	strSql.append("	ELEMENTS.ProjectId = @ProjectId) b \n");
	strSql.append("ON a.fElemID=b.eElemID ");

	return m_statment->Prepare(m_db, strSql.c_str());
}

void CreateCAESolidTool::BuildHiliteMeshes(const DPoint3dCP& basePt, DgnPlatform::LevelId  levelId_CaeModel)
{
	if (BE_SQLITE_OK != OpenDB(m_db)) return;
	if (BE_SQLITE_OK != GetHiliteSolidSQL()) return;

	int index = 0;
	bvector<int> vecNodeIds;		// Mesh体相对应的CAE结果节点集合
	bvector<int> vecIndices;		// Mesh体的节点集合
	bvector<DPoint3d> vecPoints;	// Mesh体的顶点坐标集合
	bvector<WString> vecFaceNodeStr;

	SelectionSetManagerR selectSetM = SelectionSetManager::GetManager();
	DgnModelRefP target = NULL;
	if(SUCCESS != (&target, mdlDgnFileObj_getMasterFile(),m_ProjectID, TRUE, TRUE)) return;
	if (!mdlModelRef_areSame(MASTERFILE,target))
	{
		mdlModelRef_activateAndDisplay(target);
	}

	m_tempElems.SetDrawMode(DRAW_MODE_TempDraw);
	
	m_savept = new Savepoint(m_db, "asd", true);
	m_savept->Begin();

	m_statment = new Statement();

	// 设置查询条件
	m_statment->BindInt(1, m_ProjectID);

	// 循环CAE单元体数据，在MS中生成单元体
	MSElementDescrP m_resultEdp;
	while (m_statment->Step() == BE_SQLITE_ROW)
	{
		// 初始化变量
		vecIndices.clear();
		vecPoints.clear();
		vecFaceNodeStr.clear();
		vecNodeIds.clear();// 节点ID与顶点ID一一对应。

		// 取得CAE的Mesh体的节点和顶点坐标集合
		MakeMeshIndices(vecIndices, vecPoints, vecFaceNodeStr, vecNodeIds, basePt);

		// 当节点数>=3，生成单元面或单元体
		if (vecPoints.size() >= 3)
		{
			// 生成CAE的Mesh体
			if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
			{
				// 生成Mesh体相对应的CAE结果节点集合
				XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
				MeshProperty meshPro;
				for (int i = 0; i < vecNodeIds.size(); i++)
				{
					if (i < 30)
					{
						meshPro.ID[i] = vecNodeIds.at(i);
					}
				}

				meshPro.ElemID = m_statment->GetValueInt(SQL_ELEMID_IDX);			//元素ID
				meshPro.MaterialNumber = m_statment->GetValueInt(SQL_MATERIAL_IDX);	// 材料类型
				meshPro.ElemType = m_statment->GetValueInt(SQL_ELEMTYPE_IDX);		// 单元类型
				meshPro.RealConstantNumber = m_statment->GetValueInt(SQL_REAL_IDX);	// 实常数

				// 生成Mesh体的ElementHandle
				EditElementHandle eeh(m_resultEdp, true, true, ACTIVEMODEL);

				// 将Mesh体相对应的CAE结果节点集合保存到ElementHandle中
				eeh.ScheduleWriteXAttribute(handlerId, XAttrID_MeshProperty, sizeof(MeshProperty), &meshPro);

				ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
				propsSetter->SetLevel(levelId_CaeModel);
				propsSetter->Apply(eeh);

				selectSetM.AddElement(eeh.GetElementRef(), target);
				m_tempElems.DoRedraw(eeh);
			}

			index++;
			if (index < 5000)
			{
				mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 5 + (index / 5000.0) * 88);
			}
		}
		// 清空数组
		bvector<int>().swap(vecNodeIds);
		bvector<int>().swap(vecIndices);
		bvector<DPoint3d>().swap(vecPoints);
	}
	ElementAgenda eAgenda;
	selectSetM.BuildAgenda(eAgenda);
	eAgenda.Hilite();

	m_statment->Finalize();
	m_savept->Commit();
}

