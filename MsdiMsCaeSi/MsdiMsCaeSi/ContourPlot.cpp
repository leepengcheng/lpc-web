#include "MsdiMsCaeSipch.h"
extern MSDialogP jdoytBar;
extern WCharP 	  jdwcMessageText;
extern int       jdPercent;
void ThreadFunc(size_t TSttPost, size_t TEndPost, bvector<bvector<ContourPoint>>& vecFaces, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape)
{
	for (size_t i = TSttPost; i < TEndPost; i++)
	{
		MakeContourPlotDat(vecFaces.at(i), PlotDatas, pDCShape);
	}
}

void MakeContourPlotDat(bvector<ContourPoint>& vecFacePtAndVal, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape)
{
	bvector<bvector<ContourPoint>> vecCaeMeshPtVal;
	bvector<ContourPoint> contourlist;
	bvector<ContourPoint> pMeshFace;
	bvector<ContourPoint> vecFaceAndValueTmp;
	ContourPoint oCpTemp;
	bool bContourPlotFace = false;

	if (GetFaceNodePtAndVal(vecCaeMeshPtVal, vecFacePtAndVal) == true)
	{
		for (int iFace = 0; iFace < vecCaeMeshPtVal.size(); iFace++)
		{
			contourlist.clear();
			vecFaceAndValueTmp.clear();

			vecFaceAndValueTmp = vecCaeMeshPtVal.at(iFace);

			// 循环计算矩形面上所有节点之间的等值点
			for (int i = 0; i < vecFaceAndValueTmp.size() - 1; i++)
			{
				ContourPoint oCpStart = vecFaceAndValueTmp.at(i);
				ContourPoint oCpEnd = vecFaceAndValueTmp.at(i + 1);

				contourlist.push_back(oCpStart);

				if (oCpStart.dVal <= oCpEnd.dVal)
				{
					for (int iVal = 0; iVal <= pDCShape->Color_num; ++iVal)
					{
						// 计算节点之间的等值点
						if (true == GetPosition(oCpTemp, oCpStart.dPt, oCpEnd.dPt, oCpStart.dVal, oCpEnd.dVal, pDCShape->ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
				else
				{
					for (int iVal = pDCShape->Color_num; iVal >= 0; --iVal)
					{
						// 计算节点之间的等值点
						if (true == GetPosition(oCpTemp, oCpStart.dPt, oCpEnd.dPt, oCpStart.dVal, oCpEnd.dVal, pDCShape->ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
			}

			bool bAllValSame = true;//矩形面上所有顶点的值是否一致
			int iContourPtNow = 0;
			int iContourPtEnd = (int)contourlist.size() - 1;
			
			//WString myString;

			// 判断矩形面上所有顶点的值是否一致
			for (iContourPtNow = 1; iContourPtNow <= iContourPtEnd; iContourPtNow++)
			{
				if (contourlist.at(0).dVal != contourlist.at(iContourPtNow).dVal)
				{
					bAllValSame = false;
					break;
				}
			}

			//myString.Sprintf(L"bAllValSame: %d", bAllValSame);

			// 根据云图的颜色区间信息，生成云图
			for (int iVal = 0; iVal <pDCShape->Color_num; ++iVal)
			{
				double dValStt = pDCShape->ClrShapeVal[iVal];
				double dValEnd = pDCShape->ClrShapeVal[iVal + 1];

				iContourPtNow = 0;
				iContourPtEnd = (int)contourlist.size() - 1;
				ContourPoint ContourPt;
				ContourPoint ContourPtNxt;
				ContourPoint ContourPtEnd;

				// 根据矩形面上的等值点，生成各区域的云图
				for (iContourPtNow = 0; iContourPtNow <= iContourPtEnd; iContourPtNow++)
				{
					ContourPt = contourlist.at(iContourPtNow);
					if (dValStt <= ContourPt.dVal && ContourPt.dVal <= dValEnd)
					{
						if (bAllValSame == true)
						{
							pMeshFace.push_back(ContourPt);
						}
						else
						{
							if (pMeshFace.size() == 0 && ContourPt.dVal < dValEnd)
							{
								// 当面节点的个数等于0且节点的值小于区域的最大值时，
								// 先反向查找出在当前区域的所有节点，
								// 然后将自身节点加入到pMeshFace数组中。
								for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
								{
									ContourPtEnd = contourlist.at(iContourPtEnd);
									if (dValStt <= ContourPtEnd.dVal && ContourPtEnd.dVal < dValEnd)
									{
										pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
									}
									else if (ContourPtEnd.dVal >= dValEnd)
									{
										// 当等于最大值时，反向查找节点处理结束。（虽然判断式为">="，但不会出现">dValEnd"的情况）
										pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
										break;
									}
								}

								// 将自身节点加入到pMeshFace数组中
								pMeshFace.push_back(ContourPt);
							}
							else if (ContourPt.dVal == dValEnd)
							{
								if (pMeshFace.size() <= 1)
								{
									pMeshFace.clear();
									pMeshFace.push_back(ContourPt);
								}
								else
								{
									pMeshFace.push_back(ContourPt);

									ContourPtNxt = ContourPt;
									if (iContourPtNow + 1 <= iContourPtEnd)
									{
										ContourPtNxt = contourlist.at(iContourPtNow + 1);
									}

									if (ContourPtNxt.dVal != dValStt)
									{
										// 生成云图展示数据
										PlotThread tempPlot;
										tempPlot.pMeshFaces = pMeshFace;
										tempPlot.theColor = pDCShape->fillCororIndex[iVal];
										PlotDatas.push_back(tempPlot);

										pMeshFace.clear();

										iContourPtNow--;
									}
								}
							}
							else
							{
								pMeshFace.push_back(ContourPt);
							}
						}
					}
				}

				// 生成云图展示数据
				if (pMeshFace.size() > 0)
				{
					PlotThread tempPlot2;
					tempPlot2.pMeshFaces = pMeshFace;
					tempPlot2.theColor = pDCShape->fillCororIndex[iVal];
					PlotDatas.push_back(tempPlot2);
				}

				pMeshFace.clear();
			}
		}
	}

	//// 释放变量
	//bvector<bvector<ContourPoint>>().swap(vecCaeMeshPtVal);
	//bvector<ContourPoint>().swap(contourlist);
	//bvector<ContourPoint>().swap(pMeshFace);
	//bvector<ContourPoint>().swap(vecFaceAndValueTmp);
}

// 绘制云图
void DrawContourPlot(bvector<PlotThread>& PlotDatas, DgnPlatform::LevelId levelId, NamedGroupPtr ngCAESolidYT)
{
	bvector<int> vecIndices;
	bvector<DPoint3d> vecPoints;
	MeshSurfaceValue meshValues;
	
	for (UInt32 i = 0; i < PlotDatas.size(); i++)
	{
		int nodesCount = 0;
		vecIndices.clear();
		vecPoints.clear();

		PlotThread Tempplot = PlotDatas.at(i);
		if (Tempplot.pMeshFaces.size() < 3) continue;

		MSElementDescrP     m_resultEdp;
		ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
		UInt32 newColor = (Tempplot.theColor == 256) ? 0 : Tempplot.theColor;
		propsSetter->SetColor(newColor);
		propsSetter->SetFillColor(newColor);
		propsSetter->SetLevel(levelId);

		bvector<ContourPoint>::iterator iterNow = Tempplot.pMeshFaces.begin();
		bvector<ContourPoint>::iterator iterend = Tempplot.pMeshFaces.end();
		for (; iterNow != iterend; iterNow++)
		{
			vecPoints.push_back((*iterNow).dPt);
			vecIndices.push_back((int)vecPoints.size());
			meshValues.NodeValue[nodesCount++] = (*iterNow).dVal;
		}
		vecIndices.push_back(0);
		meshValues.nodeCount = nodesCount;

		if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
		{
			XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
			EditElementHandle eeh(m_resultEdp, true, true, ACTIVEMODEL);
			// 将Mesh体相对应的CAE结果节点集合保存到ElementHandle中
			eeh.ScheduleWriteXAttribute(handlerId, XAttrID_PlotMeshProperty, sizeof(MeshSurfaceValue), &meshValues);
			propsSetter->SetLocked(eeh, true);
			propsSetter->Apply(eeh);
		
			eeh.AddToModel();
			// 通过元素ID向组中增加元素
			ngCAESolidYT->AddMember(eeh.GetElementP()->GetID(), ACTIVEMODEL, NamedGroupMemberFlags());	
		}

		bvector<ContourPoint>().swap(Tempplot.pMeshFaces);
	}

	bvector<int>().swap(vecIndices);
	bvector<int>().swap(vecIndices);
}

LevelId showContourPlot(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt /*= NULL*/)
{
	int percent = 70;
	double idex = 0.0;
	//获取basePt和显示层ID
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel, levelId_ContourPlot;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);
	if (pBasePt != NULL)
		basePt = *pBasePt;

	NamedGroupCollectionPtr ngcCAESolidYT;
	NamedGroupFlags ngfCAESolidYT;
	NamedGroupPtr ngCAESolidYT;
	WString uniqueCaeYTName;

	ngcCAESolidYT = NamedGroupCollection::Create(*ACTIVEMODEL);
	// 循环检查是否存在空命名组(命名组中没有元素)
	WString GroupNameYT;
	GroupNameYT.append(L"CAE云图");
	ngfCAESolidYT.m_selectMembers = 0;// 表示自动多选
	ngfCAESolidYT.m_anonymous = 1;// 该命名组不会再命名组列表中显示(即隐藏的临时命名组)

	// 创建命名组
	if (NG_Success != NamedGroup::Create(ngCAESolidYT, GroupNameYT.c_str(), L"CAEMODELGROUP", ngfCAESolidYT, ACTIVEMODEL))
	{
		return 0;
	}
	// 创建或取得CAE云图的图层
	if (SUCCESS != mdlLevel_create(&levelId_ContourPlot, MASTERFILE, L"CAE云图", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_ContourPlot, MASTERFILE, LEVEL_NULL_ID, L"CAE云图");

	bvector<bvector<ContourPoint>> vecFaces;

	// 取得CAE的所有Mesh面集合和面节点集合
	if (true == getCaeFaceInfos(ngName, type, caseID, vecFaces, basePt))
	{
		// 利用多线程技术计算出CAE的外表面元素
		bvector<PlotThread> PlotDatas1; // 线程1的计算结果
		bvector<PlotThread> PlotDatas2; // 线程2的计算结果
		bvector<PlotThread> PlotDatas3; // 线程3的计算结果
		bvector<PlotThread> PlotDatas4; // 线程4的计算结果
		bvector<PlotThread> PlotDatas5; // 线程5的计算结果
		bvector<PlotThread> PlotDatas6; // 线程6的计算结果
		bvector<PlotThread> PlotDatas7; // 线程7的计算结果
		bvector<PlotThread> PlotDatas8; // 线程8的计算结果

		// 线程1(处理量为总Mesh面数的1/8)
		std::thread t1(ThreadFunc, 0, vecFaces.size() / 8, vecFaces, ref(PlotDatas1), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程2(处理量为总Mesh面数的1/8)
		std::thread t2(ThreadFunc, vecFaces.size() / 8, vecFaces.size() * 2 / 8, vecFaces, ref(PlotDatas2), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程3(处理量为总Mesh面数的1/8)
		std::thread t3(ThreadFunc, vecFaces.size() * 2 / 8, vecFaces.size() * 3 / 8, vecFaces, ref(PlotDatas3), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程4(处理量为总Mesh面数的1/8)
		std::thread t4(ThreadFunc, vecFaces.size() * 3 / 8, vecFaces.size() * 4 / 8, vecFaces, ref(PlotDatas4), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程5(处理量为总Mesh面数的1/8)
		std::thread t5(ThreadFunc, vecFaces.size() * 4 / 8, vecFaces.size() * 5 / 8, vecFaces, ref(PlotDatas5), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程6(处理量为总Mesh面数的1/8)
		std::thread t6(ThreadFunc, vecFaces.size() * 5 / 8, vecFaces.size() * 6 / 8, vecFaces, ref(PlotDatas6), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程7(处理量为总Mesh面数的1/8)
		std::thread t7(ThreadFunc, vecFaces.size() * 6 / 8, vecFaces.size() * 7 / 8, vecFaces, ref(PlotDatas7), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));
		// 线程8(处理量为总Mesh面数的1/8)
		std::thread t8(ThreadFunc, vecFaces.size() * 7 / 8, vecFaces.size(), vecFaces, ref(PlotDatas8), pDCShape);
		idex = idex + vecFaces.size() / 8;
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / vecFaces.size()*percent));

		// 启动线程
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		t7.join();
		t8.join();

		// 释放变量
		bvector<bvector<ContourPoint>>().swap(vecFaces);

		// 绘制云图
		DrawContourPlot(PlotDatas1, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas2, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas3, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas4, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas5, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas6, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas7, levelId_ContourPlot, ngCAESolidYT);
		DrawContourPlot(PlotDatas8, levelId_ContourPlot, ngCAESolidYT);
		mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 90);
		// 释放变量
		bvector<PlotThread>().swap(PlotDatas1);
		bvector<PlotThread>().swap(PlotDatas2);
		bvector<PlotThread>().swap(PlotDatas3);
		bvector<PlotThread>().swap(PlotDatas4);
		bvector<PlotThread>().swap(PlotDatas5);
		bvector<PlotThread>().swap(PlotDatas6);
		bvector<PlotThread>().swap(PlotDatas7);
		bvector<PlotThread>().swap(PlotDatas8);
		ngCAESolidYT->WriteToFile(true);
		mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);
	}

	return levelId_ContourPlot;
}
