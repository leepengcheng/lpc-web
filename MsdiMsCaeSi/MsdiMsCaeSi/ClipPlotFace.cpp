#include "MsdiMsCaeSipch.h"

extern WString ngName, caseName, insName, proID;
ElementId ClipPlotFace::CutFaceId = 0;
bvector<ElementId>  ClipPlotFace::meshSolids;
bvector<ElementId>  ClipPlotFace::NitsmeshSolids;
extern outRebarDlgInfo	g_outRebarDlgInfo;

bool ClipPlotFace::_OnDataButton(DgnButtonEventCR ev)
{
	if (0 == m_points.size())
		_BeginDynamics(); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.

	m_points.push_back(*ev.GetPoint()); // Save current data point location.


	if (m_points.size() < 2)
	{
		return false;
	}
	else
	{
		if (!ClipFace::IsClipFaceModel() && IsHaveCutFace == FALSE)
		{
			bvector<DPoint3d> tempLinePts;
			tempLinePts.push_back(tPlineOrg);
			tempLinePts.push_back(tPlineEnd);
			EditElementHandle tempLine;
			CreateElement(tempLine, tempLinePts);

			MSElementDescrP tempCutFace = NULL;
			mdlCurrTrans_begin();
			mdlCurrTrans_identity();
			mdlSurface_project(&tempCutFace, tempLine.GetElementDescrP(), &prjstrPoint, &prjendPoint, NULL);//向下拉伸投影线段，得到切面
			mdlCurrTrans_end();
			EditElementHandle CutFace(tempCutFace, TRUE, TRUE);
			
			m_points.clear();
			bvector<ElementId>().swap(meshSolids);
			bvector<ElementId>().swap(NitsmeshSolids);
			
			DgnPlatform::LevelId levelId;
			// 创建或取得CAE云图的图层
			if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE云图"))//不是云图模式
			{
				s_DrawColorShape.NgName = ngName;
				ClipFace::FliterMeshSolids(CutFace.GetElementDescrP(), meshSolids, NitsmeshSolids, s_DrawColorShape.NgName.GetWCharCP());
				ClipPlotFace::GetAllCutPlotFace(CutFace);
				IsHaveCutFace = TRUE;
			}
			_ExitTool();
		}

	}
	return false; // Tool should exit after creating a single line if started in single-shot mode.
}
bool ClipPlotFace::CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points)
{
	if (2 != points.size())
		return false;

	// NOTE: Easier to always work with CurveVector/CurvePrimitive and not worry about element specific create methods, ex. LineHandler::CreateLineElement.
	if (SUCCESS != DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLine(DSegment3d::From(points.front(), points.back())), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
		return false;

	ElementPropertyUtils::ApplyActiveSettings(eeh);

	return true;
}

void ClipPlotFace::DrawTempVector(DPoint3d strPos, DPoint3d endPos, UInt32 tColor, RedrawElems& redrawElems)
{
	EditElementHandle templine;
	DSegment3d segment;
	segment.SetStartPoint(strPos);
	segment.SetEndPoint(endPos);


	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	propsSetter->SetColor(tColor);
	propsSetter->SetWeight(5);

	DraftingElementSchema::ToElement(templine, *ICurvePrimitive::CreateLine(segment), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	ElementPropertyUtils::ApplyActiveSettings(templine);
	propsSetter->Apply(templine);
	redrawElems.DoRedraw(templine);
	// 生成矢量图展示数据
	DPoint3d pts[2];
	pts[0] = strPos;
	pts[1] = endPos;

	DPoint3d tempPoints[2];
	tempPoints[1] = pts[1];
	tempPoints[0].x = pts[0].x + (pts[1].x - pts[0].x) * 9 / 10;
	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);

	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	DraftingElementSchema::ToElement(templine, *ICurvePrimitive::CreateLine(segment), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	ElementPropertyUtils::ApplyActiveSettings(templine);
	propsSetter->Apply(templine);
	redrawElems.DoRedraw(templine);


	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	DraftingElementSchema::ToElement(templine, *ICurvePrimitive::CreateLine(segment), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	ElementPropertyUtils::ApplyActiveSettings(templine);
	propsSetter->Apply(templine);
	redrawElems.DoRedraw(templine);

	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	DraftingElementSchema::ToElement(templine, *ICurvePrimitive::CreateLine(segment), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	ElementPropertyUtils::ApplyActiveSettings(templine);
	propsSetter->Apply(templine);
	redrawElems.DoRedraw(templine);

	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	DraftingElementSchema::ToElement(templine, *ICurvePrimitive::CreateLine(segment), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	ElementPropertyUtils::ApplyActiveSettings(templine);
	propsSetter->Apply(templine);
	redrawElems.DoRedraw(templine);
}
void ClipPlotFace::_OnDynamicFrame(DgnButtonEventCR ev)
{
	bvector<DPoint3d>   tmpPts = m_points;
	EditElementHandle   eeh;

	tmpPts.push_back(*ev.GetPoint()); // Use current button location as end point.
	if (m_points.size() < 2 && m_points.size()>0)
	{
		double dLen2 = mdlVec_distance(&tmpPts[0], &tmpPts[1]);
		DPoint3d vec1, vec2;
		vec1.x = 0;
		vec1.y = 0;
		vec1.z = 1;

		RotMatrix rotmatrix;
		AccuDraw::GetInstance().GetRotation(rotmatrix);
		mdlVec_multiplyRotMatrixTransposeDVec3dArray(&vec2, &rotmatrix, &vec1, 1);
		vec2.Normalize();
		vec2.Scale(dLen2);
		mdlVec_add(&tmpPts.at(0), &tmpPts.at(0), &vec2);
		vec2.Normalize();
		vec2.Scale(-2 * dLen2);
		mdlVec_add(&tmpPts.at(1), &tmpPts.at(0), &vec2);

		tPlineOrg = tmpPts[0];
		tPlineEnd = tmpPts[1];
		if (!CreateElement(eeh, tmpPts))
			return;
		prjendPoint = *ev.GetPoint();
		RedrawElems redrawElems;
		redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
		redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
		redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
		 
		mdlVec_projectPointToLine(&prjstrPoint, NULL, &prjendPoint, &tmpPts[0], &tmpPts[1]);
		MSElementDescrP tempCutFace = NULL;
		mdlCurrTrans_begin();
		mdlCurrTrans_identity();
		mdlSurface_project(&tempCutFace, eeh.GetElementDescrP(), &prjstrPoint, &prjendPoint, NULL);//向下拉伸投影线段，得到切面
		mdlCurrTrans_end();

		EditElementHandle testedt(tempCutFace, TRUE, TRUE);
		ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
		propsSetter->SetTransparency(90);
		propsSetter->Apply(testedt);
		redrawElems.DoRedraw(testedt);

		DPoint3d PlacePosPoint, pt1, pt2;
		DPoint3d FaceNormal;
		mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, testedt.GetElementDescrP(), NULL);
		EditElementHandle   templine;
		pt1 = m_points.at(0);
		pt2 = m_points.at(0);
		pt2.Add(DPoint3d::FromScale(FaceNormal, 5 * UOR_PER_MM));

		DrawTempVector(pt1, pt2, 3, redrawElems);
	}

}
void ClipPlotFace::GetAllCutResultFace(EditElementHandle& CutFace, bool isWriteMesh, bool isShowYt)
{
	//根据命名组名获取对应的显示层与隐藏层名
	LevelId lvID_show = 0;
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, ngName.data());


	bvector<double>   oCaeresults;
	GetCaeResult(oCaeresults, s_DrawColorShape.ProjectId, s_DrawColorShape.CaseID, s_DrawColorShape.ResultTypeCode);
	DgnPlatform::LevelId levelId_ContourPlot;
	if (SUCCESS != mdlLevel_create(&levelId_ContourPlot, MASTERFILE, L"CAE云图切面", LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_ContourPlot, MASTERFILE, LEVEL_NULL_ID, L"CAE云图切面");

	DPoint3d PlacePosPoint;
	DPoint3d FaceNormal;
	mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, CutFace.GetElementDescrP(), NULL);
	FaceNormal.Normalize();
	for (size_t i = 0; i < meshSolids.size(); i++)//每一个mesh体遍历
	{
		ElementId elemid = meshSolids.at(i);
		EditElementHandle eeh;
		bvector<CutPt> CutFacePoints;
		if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
		{
			LevelId elemlv = eeh.GetElementDescrP()->el.ehdr.level;
			//将该元素放到显示层中
			if (elemlv != lvID_show)
			{
				continue;
			}
			ClipFace::GetMeshNodeIntersectwithcutfacePoints(&eeh, CutFacePoints, CutFace);
			if (CutFacePoints.size()<3)
			{
				continue;
			}

			// 得到与切面的交点后，对交点进行排序，排序后用来生成MESH面。
			// 排序方法：由中心点与交点之间的方向矢量之间的夹角大小来排序
			Dpoint3d midPos;
			midPos.x = midPos.y = midPos.z = 0;
			CutPt cutpt;
			for (int i = 0; i < CutFacePoints.size(); i++)
			{
				cutpt = CutFacePoints.at(i);
				midPos.x += cutpt.dPt.x;
				midPos.y += cutpt.dPt.y;
				midPos.z += cutpt.dPt.z;
			}
			midPos.x = midPos.x / CutFacePoints.size();
			midPos.y = midPos.y / CutFacePoints.size();
			midPos.z = midPos.z / CutFacePoints.size();
			double theDot = -1;

			for (int i = 0; i < CutFacePoints.size(); i++)
			{
				DVec3d vec1 = DVec3d::FromStartEndNormalize(midPos, CutFacePoints.at(i).dPt);
				double theDot = -1;
				for (int j = i + 1; j < CutFacePoints.size(); j++)
				{
					DVec3d vec2 = DVec3d::FromStartEndNormalize(midPos, CutFacePoints.at(j).dPt);
					DVec3d tempNormal = DVec3d::FromCrossProduct(vec1, vec2);
					tempNormal.Normalize();
					double tempDot = tempNormal.DotProduct(FaceNormal);
					if (tempDot > 0.99)
					{
						double theDot2 = vec1.DotProduct(vec2);
						if (theDot2 > theDot)
						{
							CutPt tempPoint = CutFacePoints.at(j);
							CutFacePoints.at(j) = CutFacePoints.at(i + 1);
							CutFacePoints.at(i + 1) = tempPoint;
							theDot = theDot2;
						}
					}

				}
			}


#pragma region 画云图的切面
			CutFacePoints.push_back(CutFacePoints.at(0));
			bvector<ContourPoint> contourlist;
			bvector<ContourPoint> pMeshFace;
			// 循环计算矩形面上所有节点之间的等值点
			for (int i = 0; i < CutFacePoints.size() - 1; i++)
			{
				CutPt oCpStart = CutFacePoints.at(i);
				CutPt oCpEnd = CutFacePoints.at(i + 1);

				ContourPoint oStart;
				ContourPoint oEnd;
				ContourPoint oCpTemp;
				ContourPoint TcpStart;

				TcpStart.dPt = oCpStart.dPt;
				ClipFace::GetCutPointValueByeType(TcpStart.dVal, oCaeresults, oCpStart);

				oStart.dVal = TcpStart.dVal;
				oStart.dPt = TcpStart.dPt;
				oEnd.dPt = oCpEnd.dPt;
				ClipFace::GetCutPointValueByeType(oEnd.dVal, oCaeresults, oCpEnd);
				contourlist.push_back(TcpStart);

				if (oStart.dVal <= oEnd.dVal)
				{
					for (int iVal = 0; iVal <= s_DrawColorShape.Color_num; ++iVal)
					{
						// 计算节点之间的等值点
						if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
				else
				{
					for (int iVal = s_DrawColorShape.Color_num; iVal >= 0; --iVal)
					{
						// 计算节点之间的等值点
						if (true == MsdiMsCaeSiUtility::GetPosition(oCpTemp, oStart.dPt, oEnd.dPt, oStart.dVal, oEnd.dVal, s_DrawColorShape.ClrShapeVal[iVal]))
						{
							contourlist.push_back(oCpTemp);
						}
					}
				}
			}

			// 根据云图的颜色区间信息，生成云图
			for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
			{
				double dValStt = s_DrawColorShape.ClrShapeVal[iVal];
				double dValEnd = s_DrawColorShape.ClrShapeVal[iVal + 1];

				int iContourPtNow = 0;
				int iContourPtEnd = (int)contourlist.size() - 1;
				ContourPoint ContourPt;
				ContourPoint ContourPtNxt;
				ContourPoint ContourPtEnd;

				// 根据矩形面上的等值点，生成各区域的云图
				for (; iContourPtNow <= iContourPtEnd; iContourPtNow++)
				{
					ContourPt = contourlist.at(iContourPtNow);
					if (dValStt <= ContourPt.dVal && ContourPt.dVal <= dValEnd)
					{
						if (pMeshFace.size() == 0 && ContourPt.dVal < dValEnd)
						{
							for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
							{
								ContourPtEnd = contourlist.at(iContourPtEnd);
								if (dValStt <= ContourPtEnd.dVal && ContourPtEnd.dVal < dValEnd)
								{
									pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
								}
								else if (ContourPtEnd.dVal >= dValEnd)
								{
									pMeshFace.insert(pMeshFace.begin(), ContourPtEnd);
									break;
								}
							}
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
									// 生成云图的区域
									ClipFace::MakeColorMesh(pMeshFace, s_DrawColorShape.fillCororIndex[iVal], levelId_ContourPlot);
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

				// 生成云图的区域
				ClipFace::MakeColorMesh(pMeshFace, s_DrawColorShape.fillCororIndex[iVal], levelId_ContourPlot);
				pMeshFace.clear();
			}
#pragma endregion
		}
	}
}
StatusInt ClipPlotFace::trimSurfaceBySurface(EditElementHandle &outEdPP, EditElementHandle& trimEdP, EditElementHandle& eehCutFace, bool bKeep, StatusInt HALF, int isInNormalSide)
{
	// 取得切面的法向和面上的坐标
	DPoint3d CutFacePos;
	DPoint3d CutNormal;
	mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, eehCutFace.GetElementDescrP(), NULL);

	// 取得面与切面的交点
	bvector<CutPt> InterSectPoints;
	ClipFace::GetMeshNodeIntersectwithcutfacePoints(&trimEdP, InterSectPoints, eehCutFace);

	bvector<DPoint3d>	allfacePoints;// 面的所有节点
	bvector<DPoint3d>	resultPoints;// 切除后的面的所有节点
	// 取得面的所有节点
	bvector<int>		pIndices;
	bvector<DPoint3d>	pXYZ;
	if (SUCCESS == mdlMesh_getPolyfaceArrays(trimEdP.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
	{
		for (size_t i = 0; i < pIndices.size(); i++)
		{
			int k = pIndices.at(i);
			if (k != 0)
			{
				allfacePoints.push_back(pXYZ.at(k - 1));
			}
		}
	}
	// 面与切面的交点数一定为2，否则为错误
	if (InterSectPoints.size() != 2)
	{
		return ERROR;
	}
	else
	{
		// 先将面与切面的交点加入到结果数组中
		for (bvector<CutPt>::const_iterator iter = InterSectPoints.begin(); iter != InterSectPoints.end(); iter++)
		{
			resultPoints.push_back(iter->dPt);
		}

		// 取得切线的中间点
		//DPoint3d mid_point;
		//mid_point.x = (InterSectPoints.at(0).dPt.x + InterSectPoints.at(1).dPt.x) / 2;
		//mid_point.y = (InterSectPoints.at(0).dPt.y + InterSectPoints.at(1).dPt.y) / 2;
		//mid_point.z = (InterSectPoints.at(0).dPt.z + InterSectPoints.at(1).dPt.z) / 2;
		for (int i = 0; i < allfacePoints.size(); i++)
		{
			DPoint3d pt = allfacePoints.at(i);
			if (!(pt.AlmostEqual(InterSectPoints.at(0).dPt) || pt.AlmostEqual(InterSectPoints.at(1).dPt)))
			{
				DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt);
				DVec3d vec2 = DVec3d::From(CutNormal);
				double theDot = 0;
				theDot = vec1.DotProduct(vec2);
				if (isInNormalSide == 0)
				{
					if (theDot < 0.0001)
					{
						resultPoints.push_back(pt);
					}
				}
				else
				{
					if (theDot > -0.0001)
					{
						resultPoints.push_back(pt);
					}
				}
			}
		}

		if (resultPoints.size() > 3)
		{
			Dpoint3d midPos;
			midPos.x = midPos.y = midPos.z = 0;
			for (int i = 0; i < resultPoints.size(); i++)
			{
				midPos.x += resultPoints.at(i).x;
				midPos.y += resultPoints.at(i).y;
				midPos.z += resultPoints.at(i).z;
			}
			midPos.x = midPos.x / resultPoints.size();
			midPos.y = midPos.y / resultPoints.size();
			midPos.z = midPos.z / resultPoints.size();

			DVec3d vec1 = DVec3d::FromStartEndNormalize(midPos, resultPoints.at(0));
			DVec3d vec2 = DVec3d::FromStartEndNormalize(midPos, resultPoints.at(1));
			DVec3d faceNormal = DVec3d::FromCrossProduct(vec1, vec2);
			faceNormal.Normalize();

			for (int i = 0; i < resultPoints.size(); i++)
			{
				vec1 = DVec3d::FromStartEndNormalize(midPos, resultPoints.at(i));
				double theDot = -1;
				for (int j = i + 1; j < resultPoints.size(); j++)
				{
					vec2 = DVec3d::FromStartEndNormalize(midPos, resultPoints.at(j));
					DVec3d tempNormal = DVec3d::FromCrossProduct(vec1, vec2);
					tempNormal.Normalize();
					double tempDot = tempNormal.DotProduct(faceNormal);
					if (tempDot > 0.99)
					{
						double theDot2 = vec1.DotProduct(vec2);
						if (theDot2 > theDot)
						{
							Dpoint3d tempPoint = resultPoints.at(j);
							resultPoints.at(j) = resultPoints.at(i + 1);
							resultPoints.at(i + 1) = tempPoint;
							theDot = theDot2;
						}
					}
				}
			}
		}
	}

	MSElementDescrP m_resultEdp;//mesh面指针
	bvector<int> vecIndices; // Mesh体的节点集合
	bvector<DPoint3d> vecPoints; // Mesh体的顶点坐标集合
	for (int i = 0; i < resultPoints.size(); i++)
	{
		vecIndices.push_back((int)(resultPoints.size() - i));
	}
	vecIndices.push_back(0);
	for (int i = 0; i < resultPoints.size(); i++)
	{
		vecPoints.push_back(resultPoints.at(i));
	}
	// 生成CAE的Mesh体
	if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
	{
		outEdPP.SetElementDescr(m_resultEdp, true, true, ACTIVEMODEL);
		return SUCCESS;
	}
	return ERROR;
}
void ClipPlotFace::GetAllCutPlotFace(EditElementHandle& CutFace)
{
	bool isInNormalSide = g_outRebarDlgInfo.clipPlotNormalSide;//和法相同一边
	DgnPlatform::LevelId levelId;
	DgnPlatform::LevelId levelId_CutfaceModel;
	bvector<ElementId> ytmeshsolids;
	bvector<ElementId> nitsytmeshsolids;
	WString GroupNameYT;

	DPoint3d CutFacePos;
	DPoint3d CutNormal;
	mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, CutFace.GetElementDescrP(), NULL);
	CutNormal.Normalize();

	GroupNameYT.append(L"CAE云图");
	ClipFace::FliterMeshSolids(CutFace.GetElementDescrP(), ytmeshsolids, nitsytmeshsolids, GroupNameYT.GetWCharCP());
	if (ytmeshsolids.size()>0)
	{
		// 创建或取得CAE云图的图层
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE云图切面", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE云图切面");
		ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
		propsSetter->SetLevel(levelId);

		//ElementCopyContextP ccP = mdlCopyContext_create();
		for (UInt32 i = 0; i < ytmeshsolids.size(); i++)
		{
			ElementId elemid = ytmeshsolids.at(i);
			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
			{
				MSElementDescrP edP = eeh.GetElementDescrP();
				EditElementHandle outEdP;
				if (!trimSurfaceBySurface(outEdP, eeh, CutFace, false, 1, isInNormalSide))//切云图面成功
				{
					if (outEdP.IsValid())
					{
						ElementPropertiesGetterPtr propGetter = ElementPropertiesGetter::Create(eeh);
						UInt32 tColor = propGetter->GetColor();
						propsSetter->SetColor(tColor);

						//EditElementHandle tempHandle(outEdP, true, false, ACTIVEMODEL);
						propsSetter->Apply(outEdP);
						outEdP.AddToModel();
					}
				}
				else
				{
					if (IsInCutFaceVectorSide(edP, CutNormal, CutFacePos) == isInNormalSide)//切云图不成功且在切面法相的同一侧
					{

						EditElementHandle tempHandle;
						tempHandle.Duplicate(eeh);

						ElementPropertiesGetterPtr propGetter = ElementPropertiesGetter::Create(eeh);
						propsSetter->SetColor(propGetter->GetColor());
						propsSetter->SetFillColor(propGetter->GetColor());
						propsSetter->SetDisplayPriority(TRUE);
						propsSetter->Apply(tempHandle);

						tempHandle.AddToModel();
					}
				}
				mdlElmdscr_freeAll(&edP);
			}

		}
		for (UInt32 i = 0; i < nitsytmeshsolids.size(); i++)
		{
			ElementId elemid = nitsytmeshsolids.at(i);
			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
			{
				MSElementDescrP edP = eeh.GetElementDescrP();
				if (IsInCutFaceVectorSide(edP, CutNormal, CutFacePos) == isInNormalSide)//在切面法相的同一侧的云图面
				{
					EditElementHandle tempHandle;
					tempHandle.Duplicate(eeh);

					ElementPropertiesGetterPtr propGetter = ElementPropertiesGetter::Create(eeh);
					propsSetter->SetColor(propGetter->GetColor());
					propsSetter->SetFillColor(propGetter->GetColor());
					propsSetter->Apply(tempHandle);
					tempHandle.AddToModel();
				}
				mdlElmdscr_freeAll(&edP);
			}

		}
		//mdlCopyContext_free(&ccP);

		bool isWriteMesh = FALSE;
		bool isShowYt = TRUE;
		GetAllCutResultFace(CutFace, isWriteMesh, isShowYt);

		mdlLevel_getIdFromName(&levelId_CutfaceModel, ACTIVEMODEL, LEVEL_NULL_ID, L"CAE云图");

		mdlLevel_setDisplay(MASTERFILE, levelId_CutfaceModel, false);
	}
}
bool ClipPlotFace::IsInCutFaceVectorSide(MSElementDescrP edp, DPoint3dCR CutNormal, DPoint3dCR CutFacePos)
{
	bvector<int>		pIndices;
	bvector<DPoint3d>	pXYZ;
	bvector<DPoint3d> allfacePoints;
	bvector<DPoint3d> resultPoints;
	bool				bRet = false;
	
	if (SUCCESS == mdlMesh_getPolyfaceArrays(edp, &pIndices, &pXYZ, NULL, NULL))
	{
		for (size_t i = 0; i < pIndices.size(); i++)
		{
			int k = pIndices.at(i);
			if (k != 0)
			{
				allfacePoints.push_back(pXYZ.at(k - 1));
			}
		}
	}

	int numSide = 0;
	double theDot = 0;

	for (int i = 0; i < allfacePoints.size(); i++)
	{
		DPoint3d pt = allfacePoints.at(i);
		DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt);

		theDot = vec1.DotProduct(CutNormal);

		//WString myString;
		//myString.Sprintf(L"theDot: %lf", theDot);
		//mdlDialog_dmsgsPrint(myString.GetWCharCP());

		if (theDot > -0.0001)
		{
			numSide++;
		}
	}
	if (numSide == allfacePoints.size())
	{
		bRet = true;
	}
	return bRet;
}
void ClipPlotFace::_OnPostInstall()
{
	AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.

	__super::_OnPostInstall();
}

void ClipPlotFace::MakeColorMesh(bvector<ContourPoint> pMeshFace, UInt32 color, DgnPlatform::LevelId levelId)
{
	if (pMeshFace.size() < 3) return;

	MSElementDescrP     m_resultEdp;
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	UInt32 newColor = (color == 256) ? 0 : color;
	propsSetter->SetColor(newColor);
	propsSetter->SetFillColor(newColor);
	propsSetter->SetLevel(levelId);

	bvector<int> vecIndices;
	bvector<DPoint3d> vecPoints;

	bvector<ContourPoint>::iterator iterNow = pMeshFace.begin();
	bvector<ContourPoint>::iterator iterend = pMeshFace.end();
	for (; iterNow != iterend; iterNow++)
	{
		vecPoints.push_back((*iterNow).dPt);
		vecIndices.push_back((int)vecPoints.size());
	}
	vecIndices.push_back(0);

	if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
	{
		EditElementHandle eeh(m_resultEdp, true, false, ACTIVEMODEL);
		propsSetter->SetLocked(eeh, true);
		propsSetter->Apply(eeh);

		eeh.AddToModel();
	}
	return;
}

void ClipPlotFace::ClearPlotCutFace()//将整个云图切面消除
{
	if (!ClipFace::IsClipFaceModel())
	{
		ShowHidePlot();
		DgnPlatform::LevelId levelId;
		// 创建或取得CAE云图的图层
		if (SUCCESS != mdlLevel_create(&levelId, MASTERFILE, L"CAE云图切面", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE云图切面");
		{
			mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
			mdlLevel_delete(MASTERFILE, levelId);
			mdlLevelTable_rewrite(MASTERFILE);
		}
	}
}
void ClipPlotFace::ShowHidePlot()//显示隐藏云图
{
	DgnPlatform::LevelId levelId_CutfaceModel;
	if (SUCCESS == mdlLevel_getIdFromName(&levelId_CutfaceModel, ACTIVEMODEL, LEVEL_NULL_ID, L"CAE云图"))
	{
		mdlLevel_setDisplay(MASTERFILE, levelId_CutfaceModel, TRUE);
		mdlLevelTable_rewrite(MASTERFILE);
	}
}
void ClipPlotFace::InstallmyNewInstance(int toolId)//工具开始运行安装
{
	ClearPlotCutFace();
	ClipPlotFace* tool = new ClipPlotFace(toolId);
	tool->InstallTool();
}

void ClipPlotFaceCmd(WCharCP unparsed)
{
	if (!ClipFace::IsClipFaceModel())
	{
		ClipPlotFace::InstallmyNewInstance(COMMAND_CLIPPLOTEFACE);
	}
}