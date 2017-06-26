#include "MsdiMsCaeSipch.h"

bvector<int> OutColumnData::outColumnTypeint;
bvector<WString> OutColumnData::outColumnTypeWString;
extern DgnModelRefP OutDataModelRef;
void GetAllNineDatas(bmap<WString, bvector<double>>& allDatas, int caseID, int ProjecID);
extern double move_dis;
void OutColumnData::InitColumnValueType(bvector<WString>& outColumnTypeWString)
{
	bvector<WString>().swap(outColumnTypeWString);
	outColumnTypeWString.push_back(L"");
	//径向正应力
	outColumnTypeWString.push_back(L"SX");
	//环向正应力
	outColumnTypeWString.push_back(L"SY");
	//轴向正应力
	outColumnTypeWString.push_back(L"SZ");
	//径轴向剪应力
	outColumnTypeWString.push_back(L"SXZ");
	//轴环向剪力
	outColumnTypeWString.push_back(L"SYZ");
	//径环向剪应力
	outColumnTypeWString.push_back(L"SXY");
	//次主应力
	outColumnTypeWString.push_back(L"S1");
	//X向位移
	outColumnTypeWString.push_back(L"UX");
	//Y向位移
	outColumnTypeWString.push_back(L"UY");
	//Z向位移
	outColumnTypeWString.push_back(L"UZ");
}
std::string OutColumnData::GetMidValueString(DPoint3d str, DPoint3d end)
{
	DPoint3d midPoint;
	midPoint.x = (str.x + end.x) / 2;
	midPoint.y = (str.y + end.y) / 2;
	midPoint.z = (str.z + end.z) / 2;

	std::string midString;
	char tempCx[20], tempCy[20], tempCz[20];

	itoa((int)(midPoint.x), tempCx, 10);
	itoa((int)(midPoint.y), tempCy, 10);
	itoa((int)(midPoint.z), tempCz, 10);

	midString.append(tempCx);
	midString.append(tempCy);
	midString.append(tempCz);

	return midString;
}

void OutColumnData::GetOutLineDsc(bvector<MSElementDescrP>& outLines, DPoint3d& minPoint, DPoint3d& maxPoint)
{

	NamedGroupCollectionPtr ngcCutFace;
	NamedGroupFlags ngfCutFace;
	NamedGroupPtr ngCutFace;
	ngcCutFace = NamedGroupCollection::Create(*ACTIVEMODEL);

	bvector<DPoint3d>().swap(s_drawtempLine.linePoints);
	for each (NamedGroupPtr namedGroup in *ngcCutFace)
	{
		if (namedGroup.IsValid())
		{
			WString ngName = namedGroup->GetName();
			if (ngName.find(L"CAECLIP") != WString::npos)
			{
				size_t Index;
				Index = 0;

				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				map<std::string, int> stringmap;
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//得到的每一个mesh面
					{
						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;
						int								iNumIndexPerFace;
						int								iNumFace;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
						{
							for (size_t i = 0; i < pXYZ.size(); i++)
							{
								if (i < pXYZ.size() - 1)
								{
									stringmap[GetMidValueString(pXYZ.at(i), pXYZ.at(i + 1))] += 1;
								}
								else
								{
									stringmap[GetMidValueString(pXYZ.at(i), pXYZ.at(0))] += 1;
								}

							}

					{//获取最大最小坐标
						DPoint3d ptmin;
						DPoint3d ptmax;
						mdlElmdscr_computeRange(&ptmin, &ptmax, eeh.GetElementDescrP(), NULL);
						DRange3d meshRange = DRange3d::From(ptmin, ptmax);
						if (Index == 0)
						{
							Index++;
							minPoint.x = meshRange.low.x;
							minPoint.y = meshRange.low.y;
							minPoint.z = (meshRange.low.z + meshRange.high.z) / 2;
							maxPoint.x = meshRange.high.x;
							maxPoint.y = meshRange.high.y;
							maxPoint.z = minPoint.z;
						}
						else
						{
							if (minPoint.x > meshRange.low.x)
							{
								minPoint.x = meshRange.low.x;
							}
							if (minPoint.y > meshRange.low.y)
							{
								minPoint.y = meshRange.low.y;
							}
							if (maxPoint.x < meshRange.high.x)
							{
								maxPoint.x = meshRange.high.x;
							}
							if (maxPoint.y < meshRange.high.y)
							{
								maxPoint.y = meshRange.high.y;
							}
						}
					}
					

					bvector<int>().swap(pIndices);
					bvector<DPoint3d>().swap(pXYZ);

						}
					}
				}
				MSElement tempElem;
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//得到的每一个mesh面
					{
						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;
						int								iNumIndexPerFace;
						int								iNumFace;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
						{
							for (size_t i = 0; i < pXYZ.size(); i++)
							{
								if (i < pXYZ.size() - 1)
								{
									if (stringmap[GetMidValueString(pXYZ.at(i), pXYZ.at(i + 1))] < 2)
									{
										DPoint3d points[2];
										points[0] = pXYZ.at(i);
										points[1] = pXYZ.at(i + 1);
										mdlLine_create(&tempElem, NULL, points);
										MSElementDescrP tempDes = MSElementDescr::Allocate(tempElem);
										outLines.push_back(tempDes);
										s_drawtempLine.linePoints.push_back(points[0]);
										s_drawtempLine.linePoints.push_back(points[1]);
									}

								}
								else
								{
									if (stringmap[GetMidValueString(pXYZ.at(i), pXYZ.at(0))] < 2)
									{
										DPoint3d points[2];
										points[0] = pXYZ.at(i);
										points[1] = pXYZ.at(0);
										mdlLine_create(&tempElem, NULL, points);
										MSElementDescrP tempDes = MSElementDescr::Allocate(tempElem);
										outLines.push_back(tempDes);
										s_drawtempLine.linePoints.push_back(points[0]);
										s_drawtempLine.linePoints.push_back(points[1]);
									}
								}
							}
						}

						bvector<int>().swap(pIndices);
						bvector<DPoint3d>().swap(pXYZ);

					}
				}

			}
		}
	}

}
void OutColumnData::GetArcPoints(bvector<DPoint3d>& arcPoints, MSElementDescrP arcDes, int dfNum)
{
	DPoint3d str, end;
	mdlElmdscr_extractEndPoints(&str, NULL, &end, NULL, arcDes, ACTIVEMODEL);
	double dis_w;
	mdlElmdscr_distanceAtPoint(&dis_w, NULL, NULL, arcDes, &end, 0);
	for (int i = 0; i <= dfNum; i++)
	{
		DPoint3d arcPoint;
		mdlElmdscr_pointAtDistance(&arcPoint, NULL, dis_w / 6 * i, arcDes, 0.0001);
		arcPoints.push_back(arcPoint);
	}
}
//求两条直线的交点
DPoint3d GetTwoLineIntersect(DPoint3d str1, DPoint3d end1, DPoint3d str2, DPoint3d end2)
{
	DPoint3d intersectP;

	//double k1 = (end1.y - str1.y) / (end1.x - str1.x + 0.00001);
	//double b1 = str1.y - k1*str1.x;

	//double k2 = (end2.y - str2.y) / (end2.x - str2.x + 0.00001);
	//double b2 = str2.y - k2*str2.x;

	//intersectP.x = (b1 - b2) / (k2 - k1 + 0.00001);
	//intersectP.y = k1*intersectP.x + b1;
	//intersectP.z = str1.z; 

	DSegment3d line_Fir = DSegment3d::From(str1, end1);
	DSegment3d line_Sec = DSegment3d::From(str2, end2);
	mdlVec_intersect(&intersectP, &line_Fir, &line_Sec);
	intersectP.z = str1.z;

	return intersectP;
}
void CreateAline(MSElement& lineElement, DPoint3d str, DPoint3d end)
{
	DPoint3d points[2];
	points[0] = str;
	points[1] = end;
	mdlLine_create(&lineElement, NULL, points);
}
//划分线段上点
void GetSplitPoints(bvector<DPoint3d>& tempPoints, double PerDis, DPoint3d str, DPoint3d end, bvector<DPoint3d>& intersectCurPoint)
{
	double tempDis = mdlVec_distance(&str, &end);
	int perNum = tempDis / PerDis;

	//PerDis = tempDis; // test
	//perNum = 1;

	if (perNum == 0)
	{
		tempPoints.push_back(str);
	}
	else
	{
		MSElement  tempElem;
		DPoint3d points[2];
		DPoint3d tempPoint;
		points[0] = str;
		points[1] = end;
		mdlLine_create(&tempElem, NULL, points);
		MSElementDescrP tempDes = MSElementDescr::Allocate(tempElem);
		for (int i = 0; i <= perNum; i++)
		{
			DPoint3d tPoint;
			mdlElmdscr_pointAtDistance(&tPoint, NULL, PerDis * i, tempDes, 0.0001);
			if (i > 0)
			{
				for (int j = 0; j < intersectCurPoint.size(); j++) // 判断射线与外轮廓交点是否在当前划分线段上
				{
					DPoint3d curPoint = intersectCurPoint.at(j);
					if (PtInLineRange2(curPoint, tempPoint, tPoint))  // 若点在划分线段范围内，添加 (curPoint, tempPoint, tPoint共线)
						tempPoints.push_back(curPoint);
				}
			}
			tempPoint = tPoint;
			tempPoints.push_back(tPoint);
		}  
	}
	tempPoints.push_back(end);
}
void OutColumnData::GetAllIntersecPoints(bvector<DPoint3d>& IntersectPoints, DPoint3d CenterPoint, DPoint3d desPoint, bvector<MSElementDescrP>& outLines)
{
	for (int i = 0; i < outLines.size(); i++)
	{
		MSElementDescrP tempDes = outLines.at(i);
		DPoint3d str, end;
		mdlElmdscr_extractEndPoints(&str, NULL, &end, NULL, tempDes, ACTIVEMODEL);

		DVec3d vecCentoStr = DVec3d::FromStartEndNormalize(CenterPoint, str);
		DVec3d vecCentoEnd = DVec3d::FromStartEndNormalize(CenterPoint, end);
		DVec3d vecCentoDes = DVec3d::FromStartEndNormalize(CenterPoint, desPoint);

		double theDotSE = 0;
		theDotSE = vecCentoStr.DotProduct(vecCentoEnd);

		double theDotDS = 0;
		theDotDS = vecCentoDes.DotProduct(vecCentoStr);

		double theDotDE = 0;
		theDotDE = vecCentoDes.DotProduct(vecCentoEnd);
		if ((int(theDotDE + 0.00001) == 1 && int(theDotDS + 0.00001) == 1 && int(theDotSE + 0.00001) == 1) || (theDotDE == 0 && theDotDS == 0 && theDotSE == 0))
		{
			IntersectPoints.push_back(str);
			IntersectPoints.push_back(end);
		}
		else if (int(theDotSE - 0.00001) == -1)
		{
			if (theDotDS == 1)
			{
				IntersectPoints.push_back(CenterPoint);
				IntersectPoints.push_back(str);
			}
			else if (theDotDE == 1)
			{
				IntersectPoints.push_back(CenterPoint);
				IntersectPoints.push_back(end);
			}
		}
		else if (acos(theDotSE) >= (acos(theDotDS) + acos(theDotDE) - 0.0001))
		{//在中间有交点
			//y = kx + b
			DPoint3d tempPoint = GetTwoLineIntersect(str, end, CenterPoint, desPoint);
			IntersectPoints.push_back(tempPoint);
		}

	}
}
bool OutColumnData::DrawFaceOutline(bvector<MSElementDescrP>&outLines, double dis_x, DgnPlatform::LevelId  levelId_OutData)
{
	for (int i = 0; i < outLines.size(); i++)
	{
		MSElementDescrP tempLine = outLines.at(i);
		MSElementDescrP copyLine = NULL;
		mdlElmdscr_duplicate(&copyLine, tempLine);

		EditElementHandle eeh(copyLine, true, FALSE, OutDataModelRef);
		DPoint3d movNormal;//往X方向偏移
		movNormal.x = 1 * dis_x;
		movNormal.y = 0;
		movNormal.z = 0;
		Transform       m_transform;
		m_transform.InitFrom(movNormal);
		TransformInfo tInfo(m_transform);
		eeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(eeh, tInfo);
		ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
		propsSetter->SetLevel(levelId_OutData);
		propsSetter->Apply(eeh);
		eeh.AddToModel();

		mdlElmdscr_freeAll(&copyLine);
		copyLine = NULL;
	}
	return true;
}
int OutColumnData::DrawArc(MSElement& tCircle, bvector<DPoint3d>& m_points, DPoint3d minpt)
{
	DPoint3d pts[2];
	pts[1].x = (m_points.at(0).x + m_points.at(1).x) / 2;
	pts[1].y = (m_points.at(0).y + m_points.at(1).y) / 2;
	pts[1].z = (m_points.at(0).z + m_points.at(1).z) / 2;

	pts[0] = m_points.at(1);

	double dis = mdlVec_distance(&pts[0], &pts[1]);

	DVec3d vec1 = DVec3d::FromStartEnd(minpt, m_points[0]);
	DVec3d vec2 = DVec3d::FromStartEnd(minpt, m_points[1]);
	vec1.Normalize();
	vec2.Normalize();
	DVec3d tempNormal1 = DVec3d::FromCrossProduct(vec1, vec2);

	DVec3d vec3 = DVec3d::FromStartEnd(m_points[2], m_points[0]);
	DVec3d vec4 = DVec3d::FromStartEnd(m_points[2], m_points[1]);
	vec3.Normalize();
	vec4.Normalize();
	DVec3d tempNormal2 = DVec3d::FromCrossProduct(vec3, vec4);

	tempNormal1.Normalize();
	tempNormal2.Normalize();
	double theDot = tempNormal1.DotProduct(tempNormal2);
	int theState;
	if (int(theDot + 0.001) == 1)//和最小点在直线的同一侧
	{
		theState = mdlArc_create(&tCircle, NULL, &pts[1], dis, dis, NULL, fc_pi / 2, fc_pi);
	}
	else//和最小点在直线的不同侧
	{
		theState = mdlArc_create(&tCircle, NULL, &pts[1], dis, dis, NULL, fc_pi * 3 / 2, fc_pi);
	}
	return theState;
}
void OutColumnData::GetInsectPointswithOutLines(bvector<bvector<DPoint3d>>& allIntersectPoints, DPoint3d mid_point, bvector<MSElementDescrP> outLines, bvector<DPoint3d> arcPoints)
{
	//MSElement tempElem;
	for (int i = 0; i < arcPoints.size(); i++)
	{
		bvector<DPoint3d> intersectPoints;
		GetAllIntersecPoints(intersectPoints, mid_point, arcPoints.at(i), outLines);
		/*for (int j = 0; j < intersectPoints.size(); j++)
		{
		DPoint3d points[2];
		points[0] = mid_point;
		points[1] = intersectPoints.at(j);
		mdlLine_create(&tempElem, NULL, points);
		EditElementHandle tempedt(&tempElem, ACTIVEMODEL);
		tempedt.AddToModel();

		}*/
		allIntersectPoints.push_back(intersectPoints);
	}
}
void OutColumnData::GetColumnoutPoints(bvector<bvector<DPoint3d>>& allOutPoints, double PerDis,
	bvector<bvector<DPoint3d>> allIntersectPoints, DPoint3d mid_point)
{
	for (int i = 0; i < allIntersectPoints.size(); i++)
	{
		DPoint3d maxPoint, minPoint;
		bvector<DPoint3d> tempPoints = allIntersectPoints.at(i);
		bvector<DPoint3d> intersectCurPoint = allIntersectPoints.at(i);
		double maxdis, mindis;
		mindis = 0;
		maxdis = 0;
		if (tempPoints.size() == 0)
		{
			continue;
		}
		if (tempPoints.size() == 1)
		{
			minPoint = mid_point;
			maxPoint = tempPoints.at(0);
		}
		else
		{
			for (int j = 0; j < tempPoints.size(); j++)
			{
				if (j == 0)
				{
					mindis = mdlVec_distance(&tempPoints.at(j), &mid_point);
					maxdis = mindis;
					minPoint = tempPoints.at(j);
					maxPoint = tempPoints.at(j);
				}
				else
				{
					double tempDis = mdlVec_distance(&tempPoints.at(j), &mid_point);
					if (tempDis > maxdis)
					{
						maxdis = tempDis;
						maxPoint = tempPoints.at(j);
					}
					if (tempDis < mindis)
					{
						mindis = tempDis;
						minPoint = tempPoints.at(j);
					}
				}

			}
		}

		bvector<DPoint3d>().swap(tempPoints);
		GetSplitPoints(tempPoints, PerDis, minPoint, maxPoint, intersectCurPoint);
		allOutPoints.push_back(tempPoints);
	}
}
//圆柱坐标系
bool OutColumnData::_OnDataButton(DgnButtonEventCR ev)
{
	if (m_points.size() < 3)
	{
		_BeginDynamics(); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.

		m_points.push_back(*ev.GetPoint()); // Save current data point location.
	}

	if (m_points.size() == 3)
	{

		DgnPlatform::LevelId  levelId_OutData;

		// 创建或取得CAE模型的显示图层
		if (SUCCESS != mdlLevel_create(&levelId_OutData, ACTIVEMODEL, L"CLIPOUTDATA", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId_OutData, ACTIVEMODEL, LEVEL_NULL_ID, L"CLIPOUTDATA");

		bvector<DPoint3d> meshminpoints;
		bvector<DPoint3d> meshmaxpoints;
		DPoint3d minpoint;
		DPoint3d maxpoint;
		int ProjectID;
		GetMeshMinMaxPoints(meshminpoints, meshmaxpoints, minpoint, maxpoint, ProjectID);

		double tempz = (minpoint.z + maxpoint.z) / 2;
		m_points.at(0).z = tempz;
		m_points.at(1).z = tempz;
		m_points.at(2).z = tempz;

		bvector<MSElementDescrP> outLines;//外轮廓所有线段
		GetOutLineDsc(outLines, minpoint, maxpoint);

		MSElement tCircle;
		//圆心点
		DPoint3d mid_point = DPoint3d::FromSumOf(m_points[0], 0.5, m_points[1], 0.5);

		//mid_point = m_points[0]; // test
		
		//画辅助圆弧
		DrawArc(tCircle, m_points, minpoint);

		bvector<DPoint3d> arcPoints;
		int dfNum = 4;
		EditElementHandle tempCir(&tCircle, ACTIVEMODEL);
		//arcPoints 划分好的交点
		GetArcPoints(arcPoints, tempCir.GetElementDescrP(), dfNum);
		//tempCir.AddToModel();

		//获取圆弧划分线段与外轮廓的交点
		bvector<bvector<DPoint3d>> allIntersectPoints;
		GetInsectPointswithOutLines(allIntersectPoints, mid_point, outLines, arcPoints);

		//{            // test
		//	for (int i = 0; i < outLines.size(); i++)
		//	{
		//		bvector<DPoint3d> intersectPoints;
		//		DPoint3d str, end;
		//		MSElementDescrP tempDes = outLines.at(i);
		//		mdlElmdscr_extractEndPoints(&str, NULL, &end, NULL, tempDes, ACTIVEMODEL);
		//		intersectPoints.push_back(str);
		//		allIntersectPoints.push_back(intersectPoints);
		//	}
		//}

		double PerDis = 10.0*UOR_PER_METER;//设置每段长度
		bvector<bvector<DPoint3d>> allOutPoints;
		//计算要输出的数值点坐标和数值
		GetColumnoutPoints(allOutPoints, PerDis, allIntersectPoints, mid_point);
		{
			OutLineData       outLine;
			DataListParamas   tempoutParama;
			outLine.EleMinPoint = minpoint;
			outLine.EleMaxPoint = maxpoint;
			//int    ListIndex = 0;
			tempoutParama.left_dis = 0.067;//画外方框左偏移比例值（以左下角与右上角距离为基础值）
			tempoutParama.right_dis = 0.045; //画外方框右偏移比例值
			tempoutParama.top_dis = O_heightCoe;//画外方框上偏移比例值
			tempoutParama.bottom_dis = O_heightCoe - 0.11;//画外方框下偏移比例值
			tempoutParama.ListTitle = O_tableTitle/*L"哈吉水电站引水坝段"*/;//数值表名,

			//tempoutParama.TypeName = L"X向正应力";//值类型名
			tempoutParama.CutFaceName = L"截面Y = 9";//断面名称
			tempoutParama.CaseName = O_CaseName;//工况名
			//tempoutParama.UnitName = L"单位:Ma";//单位名
			tempoutParama.row = O_YValueCnt;
			tempoutParama.col = O_XValueCnt;
			tempoutParama.TitFontSize = O_titleFont * UOR_PER_METER;
			tempoutParama.CasFontSize = O_contentFont* UOR_PER_METER;
			tempoutParama.cordFontSize = O_coorFont* UOR_PER_METER;
			tempoutParama.DataFontSize = O_valueFont* UOR_PER_METER;
			tempoutParama.Decimal = O_decimal;

			tempoutParama.isS1 = false;//设置不是输出S1
			double PerHeight = 2 * UOR_PER_METER;//minPoint和maxPoint为偏移过的最小最大坐标点
			int caseID = s_pDCShape->CaseID;//先默认工况取1
			move_dis = (maxpoint.x - minpoint.x) + 4 * 15 * tempoutParama.DataFontSize/*(maxpoint.x - minpoint.x) * 6 / 5*/;
			int outValueNum = 0;
			bmap<WString, bvector<double>> allNineDatas;
			GetAllNineDatas(allNineDatas, caseID, ProjectID);

			{//输出数值图
				for (int j = 0; j < outColumnTypeint.size(); j++)
				{
					if (outColumnTypeint.at(j))
					{
						WString tyPeName = outValueTypeWString.at(j);

						//第一个为画网格图
						if (j != 0)
						{
							//除开UX,UY,UZ,有一个没有值就计算不了
							if (tyPeName != L"UX"&&tyPeName != L"UY"&&tyPeName != L"UZ")
							{
								if (allNineDatas[L"SX"].size() < 1 || allNineDatas[L"SY"].size() < 1 || allNineDatas[L"SZ"].size() < 1
									|| allNineDatas[L"SXY"].size() < 1 || allNineDatas[L"SXZ"].size() < 1 || allNineDatas[L"SYZ"].size() < 1)
								{
									continue;
								}
								else
								{
									if (tyPeName == L"S1")//判断此时输出的是S1
									{
										tempoutParama.isS1 = true;
									}
								}
							}
							else
							{
								if (allNineDatas[tyPeName].size() < 1)
								{
									continue;
								}
							}

						}

						outLine.EleMinPoint.x = outValueNum*move_dis + minpoint.x;
						outLine.EleMaxPoint.x = outValueNum*move_dis + maxpoint.x;

						if (j == 0)//画网格图框
						{
							tempoutParama.UnitName.clear();
							tempoutParama.TypeName = L"网格图";
							tempoutParama.UnitName.append(L"");
						}
						else
						{
							tempoutParama.UnitName.clear();
							ClipFace::SettypeWsValue();
							tempoutParama.UnitName.append(L"单位：");
							WString DanWName;
							if (!IsTypeNameHaveValue(tyPeName, ProjectID, caseID))//如果typeName中没值，此数值图不输出
							{
								continue;
							}
							GetUnitAndTypeNameByeCode(tyPeName, DanWName, tempoutParama.TypeName);
							if (DanWName.find(L"m") != WString::npos)
							{
								DanWName = L"mm";
							}
							if (DanWName.find(L"Pa") != WString::npos)
							{
								DanWName = L"MPa";
							}
							tempoutParama.UnitName.append(DanWName);
						}
						DrawFaceOutline(outLines, outValueNum*move_dis, levelId_OutData);

						for (int i = 0; i < allOutPoints.size(); i++)
						{
							bvector<DPoint3d> LoutPoints = allOutPoints.at(i);//任意角度射线的点集合
							if (LoutPoints.size()>0)//如果射线的交点数目>0
							{
								bvector<bvector<DPoint3d>> LineOutPoints;
								LineOutPoints.push_back(LoutPoints);
<<<<<<< .mine
								DPoint3d tpt = LoutPoints.at(0);//射线上第1点
								DVec3d Dvec = DVec3d::FromStartEnd(mid_point, tpt);//X轴矢量
								DPoint3d dpt = DPoint3d::From(1.0, 0.0, 0.0);
								RotMatrix rotM;
								mdlRMatrix_fromVectorToVector(&rotM, &dpt, &Dvec);
||||||| .r244
								DPoint3d tpt = LoutPoints.at(0);
								DVec3d Dvec = DVec3d::FromStartEnd(mid_point, tpt);
								DPoint3d dpt = DPoint3d::From(1.0, 0.0, 0.0);
								RotMatrix rotM;
								mdlRMatrix_fromVectorToVector(&rotM, &dpt, &Dvec);
=======
								//DPoint3d tpt = LoutPoints.at(0);
								DPoint3d tpt = LoutPoints.at(1);
>>>>>>> .r251

								DVec3d DvecX_Cur = DVec3d::FromStartEndNormalize(mid_point, tpt); // 节点局部坐标系X轴
								DVec3d dpt = DVec3d::From(1.0, 0.0, 0.0);                         // 切面局部坐标系X轴
								RotMatrix irotM;
								mdlRMatrix_fromVectorToVector(&irotM, &dpt, &DvecX_Cur);
								mdlRMatrix_transpose(&irotM, &irotM);

								bvector<bvector<ContourPoint>> meshpoints;
								if (outValueNum - 1 >= 0)
								{
									int tIndex = GetIndexByTypeName(tyPeName);
<<<<<<< .mine
									GetMeshDatas(meshpoints, tIndex, caseID, ProjectID, allNineDatas, rotM);//柱坐标
||||||| .r244
									GetMeshDatas(meshpoints, tIndex, caseID, ProjectID, allNineDatas, rotM);
=======
									GetMeshDatas(meshpoints, tIndex, caseID, ProjectID, allNineDatas, irotM);
>>>>>>> .r251

									CaculateAndDisplayValue(LineOutPoints, outValueNum - 1, tempoutParama, caseID, PerHeight, outLine.EleMinPoint,
										outLine.EleMaxPoint, meshpoints, meshminpoints, meshmaxpoints, levelId_OutData);
								}
							}
						}

						CreateOutFrame(tempoutParama, outValueNum, outLine, minpoint, maxpoint, levelId_OutData);
						outValueNum++;
						tempoutParama.isS1 = false;
					}

				}
			}//输出数值图

		}
		for (size_t i = 0; i < outLines.size(); i++)
		{
			MSElementDescrP tempDsc = outLines.at(i);
			EditElementHandle tempedt(tempDsc, true, false, ACTIVEMODEL);
			tempedt.AddToModel();
		}
		_ExitTool();

		return true;
	}

	return false; // Tool should exit after creating a single line if started in single-shot mode.
}

void OutColumnData::_OnPostInstall()
{
	AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.

	__super::_OnPostInstall();
}

void OutColumnData::InstallmyNewInstance(int toolId)//工具开始运行安装
{
	OutColumnData* tool = new OutColumnData(toolId);
	tool->InstallTool();
}
