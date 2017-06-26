#include "MsdiMsCaeSipch.h"

extern outRebarDlgInfo	g_outRebarDlgInfo;

DPoint3d ClipFace::minPoint;
DPoint3d ClipFace::maxPoint;
bvector<double>   ClipFace::oCaeresultsux;
bvector<double>   ClipFace::oCaeresultsuy;
bvector<double>   ClipFace::oCaeresultsuz;
bmap<UInt32, vector<vector<DPoint3d>>> ClipFace::allLinePoints;
extern void ApplyCaeDisplayStyle(WString styleName, int viewIndex);

//筛选与面相交物体的线程函数
void FilterMeshThreadFunc
(
UInt32 Starti,
UInt32 Endi,
NamedGroupPtr namedGroup,
bvector<ElementId>& meshSolids,
bvector<ElementId>& nitsmeshSolids,
DPoint3d CutFacePos,
DPoint3d CutNormal
)
{
	//筛选方法为由物体范围方框的8个值之间的连线，判断与切面有没有交，有一个有交说明面与物体有交
	for (UInt32 i = Starti; i < Endi; i++)
	{
		ElementId elemid = namedGroup->GetMember(i)->GetElementId();
		EditElementHandle eeh;
		if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
		{
			//获取物体的范围坐标
			DPoint3d pt1;
			DPoint3d pt2;
			mdlElmdscr_computeRange(&pt1, &pt2, eeh.GetElementDescrP(), NULL);

			double Xlength = pt2.x - pt1.x;
			double Ylength = pt2.y - pt1.y;
			//判断pt1与pt2之间的连线与切面是否有交，有交的话将物体的ID保存起来，并返回
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal))
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.x = pt1.x + Xlength;
			pt2.x = pt2.x - Xlength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Xlength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.y = pt1.y + Ylength;
			pt2.y = pt2.y - Ylength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Ylength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			pt1.x = pt1.x - Xlength;
			pt2.x = pt2.x + Xlength;
			if (ClipFace::IsinFacesSameSide(CutFacePos, pt1, pt2, CutNormal) && Xlength != 0)
			{
				meshSolids.push_back(elemid);
				continue;
			}
			nitsmeshSolids.push_back(elemid);
		}
	}
}

//计算线段ID，线段ID的获取方式：计算两个点的ID值，对ID值由大到小排序，生成线段ID“大ID-小ID”
std::string ClipFace::getLinekeys(CutPt& pt1, CutPt& pt2)
{
	string sKey;
	char tempC[150];
	if (pt1.StartID > pt2.StartID)
	{
		sprintf_s(tempC, "%d-%d", pt2.StartID, pt1.StartID);
	}
	else
	{
		sprintf_s(tempC, "%d-%d", pt1.StartID, pt2.StartID);
	}
	sKey.append(tempC);

	return sKey;
}

//计算直线与面的交点
bool CalPlaneLineIntersectPoint(DVec3d planeVector, DPoint3d planePoint, DVec3d lineVector, DPoint3d linePoint1, DPoint3d linePoint2, DPoint3d& returnResult)
{
	//planeVector：面法向
	//planePoint:面上一点
	//lineVector:直线矢量
	//linePoint：直线上一点
	//returnResult：计算得到的面与线的交点
	double vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t, vpt;
	vp1 = planeVector.x;
	vp2 = planeVector.y;
	vp3 = planeVector.z;
	n1 = planePoint.x;
	n2 = planePoint.y;
	n3 = planePoint.z;
	v1 = lineVector.x;
	v2 = lineVector.y;
	v3 = lineVector.z;
	m1 = linePoint1.x;
	m2 = linePoint1.y;
	m3 = linePoint1.z;
	vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;

	DVec3d lineVec1 = DVec3d::FromStartEndNormalize(planePoint, linePoint1);
	DVec3d lineVec2 = DVec3d::FromStartEndNormalize(planePoint, linePoint2);
	double theLine1 = lineVec1.DotProduct(planeVector);
	double theLine2 = lineVec2.DotProduct(planeVector);
	//首先判断直线是否与平面平行  
	if (fabs(vpt) < Tolerance)
	{
		return false;
	}
	else if (fabs(theLine1) < 0.000001) // 点Pt1在面上
	{
		returnResult = linePoint1;
	}
	else if (fabs(theLine2) < 0.000001) // 点Pt2在面上
	{
		returnResult = linePoint2;
	}
	else // 线段中间某一点在面上
	{
		t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
		returnResult.x = m1 + v1 * t;
		returnResult.y = m2 + v2 * t;
		returnResult.z = m3 + v3 * t;
	}
	return true;
}

//由面上线的点来求取体与切面的交点，原理是：遍历体上的每个面上的所有线段，首先通过线段ID值来判断此线段有没有判断过，没有判断过，就计算此线段与切面有没有
//交点，如果有交点求出交点坐标，并保存，记录此线段ID表示此线段已经判断过了。重复以上过程求出所有交点。
bool ClipFace::GetIntersectPointsbyfacePoints(bvector<bvector<CutPt>>& allfacePoints, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace)
{
	WString strVal;
	double theDot1 = 0;
	double theDot2 = 0;
	//获取切面法向以及切面上一点的坐标
	DPoint3d CutFacePos;
	DPoint3d CutNormal;
	mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, CutFace.GetElementDescrP(), NULL);

	//记录线段ID的MAP
	bmap<string, int> lines;
	//遍历每一个面
	for (size_t i = 0; i < allfacePoints.size(); i++)
	{
		bvector<CutPt> facepoints = allfacePoints.at(i);
		//遍历面上的线段
		for (int j = 0; j < facepoints.size(); j++)
		{
			CutPt pt1, pt2;
			if (j == facepoints.size() - 1)
			{
				pt1.dPt = facepoints.at(j).dPt;
				pt1.StartID = facepoints.at(j).StartID;
				pt1.EndID = facepoints.at(j).EndID;
				pt1.StartRatio = facepoints.at(j).StartRatio;
				pt2.dPt = facepoints.at(0).dPt;
				pt2.StartID = facepoints.at(0).StartID;
				pt2.EndID = facepoints.at(0).EndID;
				pt2.StartRatio = facepoints.at(0).StartRatio;
			}
			else
			{
				pt1.dPt = facepoints.at(j).dPt;
				pt1.StartID = facepoints.at(j).StartID;
				pt1.EndID = facepoints.at(j).EndID;
				pt1.StartRatio = facepoints.at(j).StartRatio;
				pt2.dPt = facepoints.at(j + 1).dPt;
				pt2.StartID = facepoints.at(j + 1).StartID;
				pt2.EndID = facepoints.at(j + 1).EndID;
				pt2.StartRatio = facepoints.at(j + 1).StartRatio;
			}
			//获取线段ID
			std::string linekeys = getLinekeys(pt1, pt2);
			if (lines[linekeys] == 0)//此线段还没有判断过
			{
				DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt1.dPt);
				DVec3d vec2 = DVec3d::FromStartEndNormalize(CutFacePos, pt2.dPt);
				DVec3d normalVec = DVec3d::From(CutNormal);

				theDot1 = vec1.DotProduct(normalVec);
				theDot2 = vec2.DotProduct(normalVec);

				strVal.Sprintf(L"%lf", fabs(theDot1));
				if (strVal.StartsWith(L"0.00000"))//直线的端点1在切面上
				{
					InterSectPoint_PushBack(InterSectPoints, pt1);
				}

				strVal.Sprintf(L"%lf", fabs(theDot2));
				if (strVal.StartsWith(L"0.00000"))//直线的端点2在切面上
				{
					InterSectPoint_PushBack(InterSectPoints, pt2);
				}

				if (theDot1*theDot2 < 0)//直线与面有交点,且交点位置不在直线的端点上。
				{
					DVec3d linevec = DVec3d::FromStartEndNormalize(pt1.dPt, pt2.dPt);
					DPoint3d intesectPoint;
					//计算线段与面的交点
					if (CalPlaneLineIntersectPoint(normalVec, CutFacePos, linevec, pt1.dPt, pt2.dPt, intesectPoint))
					{
						CutPt pt3;
						pt3.dPt = intesectPoint;
						pt3.StartID = pt1.StartID;
						pt3.EndID = pt2.EndID;

						double value_dis = mdlVec_distance(&pt1.dPt, &pt2.dPt);
						double mid_dis = mdlVec_distance(&pt1.dPt, &intesectPoint);
						pt3.StartRatio = (mid_dis / value_dis);

						InterSectPoint_PushBack(InterSectPoints, pt3);
					}
				}

				lines[linekeys] += 1;
			}
		}
	}
	return true;
}

void ClipFace::InterSectPoint_PushBack(bvector<CutPt>& InterSectPoints, CutPt intesectPoint)
{
	//如果vector中有此交点，就不再添加
	bool bExist = false;
	for (int k = 0; k < InterSectPoints.size(); k++)
	{
		CutPt tpt = InterSectPoints.at(k);
		double dis = mdlVec_distanceSquared(&intesectPoint.dPt, &tpt.dPt);
		if (dis <= (0.001*UOR_PER_MASTER))
			//if (dis<=0.1)
		{
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		InterSectPoints.push_back(intesectPoint);
	}
}

//获取一个mesh体与切面的所有点坐标
bool ClipFace::GetMeshNodeIntersectwithcutfacePoints(const EditElementHandleP eh, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace)
{
	bvector<int>					pIndices;
	bvector<DPoint3d>				pXYZ;
	int								iNumIndexPerFace;
	int								iNumFace;
	bool							bRet = false;
	//存取所有面上的节点及节点上的ID值
	bvector<bvector<CutPt>> allfacePoints;
	if (SUCCESS == mdlMesh_getPolyfaceArrays(eh->GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		XAttributeHandle xh(eh->GetElementRef(), handlerId, XAttrID_MeshProperty);
		MeshProperty *meshPro = NULL;

		if (xh.IsValid())
		{
			meshPro = (MeshProperty *)xh.PeekData();
		}

		bvector<CutPt> facepoints;
		for (size_t i = 0; i < pIndices.size(); i++)
		{
			CutPt tempPoint;
			int k = pIndices.at(i);
			if (k == 0)
			{
				allfacePoints.push_back(facepoints);
				bvector<CutPt>().swap(facepoints);
			}
			else
			{
				tempPoint.dPt = pXYZ.at(k - 1);// 节点座标
				// 由于云图对象没有MeshProperty的XAttribute，
				// 所以云图剖切时xh对象没有被实例化
				if (xh.IsValid())
				{
					tempPoint.StartID = meshPro->ID[i];// 起始节点编号
					tempPoint.EndID = meshPro->ID[i];// 终点点编号
				}
				else
				{
					// 云图剖切时,节点编号直接用Mesh自身的编号
					tempPoint.StartID = k;// 起始节点编号
					tempPoint.EndID = k;// 终点点编号
				}
				tempPoint.StartRatio = 0.5;//距离起始点比例(初始为0.5)
				facepoints.push_back(tempPoint);
			}
		}

		//由面上的点得到交点
		GetIntersectPointsbyfacePoints(allfacePoints, InterSectPoints, CutFace);
	}
	return false;
}

//用切面来切所有实体
void ClipFace::CutSolidsByFace(EditElementHandle& CutFace)
{
	//先清空之前存储的ID
	bvector<ElementId>().swap(meshSolids);
	DgnPlatform::LevelId levelId;
	// 创建或取得CAE云图的图层
	if (SUCCESS != mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, L"CAE云图"))//不是云图模式
	{
		//存取所有与切面没有交的实体ID
		bvector<ElementId> NitsmeshSolids;
		//筛选与实体相交与否的实体
		FliterMeshSolids(CutFace.GetElementDescrP(), meshSolids, NitsmeshSolids, s_DrawColorShape.NgName.GetWCharCP());
		//获取并生成所有切面与实体相交的MESH面
		GetAllCutResultFace(CutFace);
		bvector<ElementId>().swap(NitsmeshSolids);
	}
}

bool ClipFace::_OnDataButton(DgnButtonEventCR ev)
{
	if (g_outRebarDlgInfo.chooseBtnIndex < 3)
	{
		CreateClipFace(ev, false);
		m_points.clear();
	}
	else
	{
		m_points.push_back(*ev.GetPoint());
		if (m_points.size() < 2)
		{
			return false;
		}
		else
		{
			CreateClipFace(ev, false);
			m_points.clear();
		}
	}

	return true; // Tool should exit after creating a single line if started in single-shot mode.
}
bool ClipFace::IsClipFaceModel()//判断当前是否在切面model中
{
	WChar             name[MAX_MODEL_NAME_LENGTH];
	mdlModelRef_getModelName(ACTIVEMODEL, name);
	WString modelName;
	modelName.append(name);
	if (splitNgName_Clip(modelName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING, UNUSE_WSTRING))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//创建直线元素
bool ClipFace::CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points)
{
	if (2 != points.size())
		return false;

	// NOTE: Easier to always work with CurveVector/CurvePrimitive and not worry about element specific create methods, ex. LineHandler::CreateLineElement.
	if (SUCCESS != DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLine(DSegment3d::From(points.front(), points.back())), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
		return false;

	ElementPropertyUtils::ApplyActiveSettings(eeh);

	return true;
}

//动态显示，生成拉伸面
void ClipFace::_OnDynamicFrame(DgnButtonEventCR ev)
{
	CreateClipFace(ev, true);
}

//获取并生成所有切面与实体相交的MESH面
void ClipFace::GetAllCutResultFace(EditElementHandle& CutFace)
{
	//根据命名组名获取对应的显示层与隐藏层名
	LevelId lvID_show = 0;
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, s_pDCShape->NgName.data());

	DgnModelRefP modelRef3d;

	NamedGroupFlags ngfCAESolid;
	NamedGroupPtr ngCAESolid;
	ngfCAESolid.m_selectMembers = 0;// 表示自动多选
	ngfCAESolid.m_anonymous = 1;// 该命名组不会再命名组列表中显示(即隐藏的临时命名组)

	//根据CAE模型的ngName和截面名生成对应的截面model名
	int proID, indexID;
	splitNgName_Model(s_DrawColorShape.NgName, UNUSE_WSTRING, proID, indexID, UNUSE_WSTRING, UNUSE_WSTRING);
	WString clipNgName = makeNgName_Clip(proID, indexID, g_outRebarDlgInfo.clipName);
	DgnPlatform::LevelId  levelId_CutfaceModel;

	MakeDgnModel(modelRef3d, clipNgName);

	// 创建或取得CAE模型的显示图层
	if (SUCCESS != mdlLevel_create(&levelId_CutfaceModel, modelRef3d, clipNgName.c_str(), LEVEL_NULL_CODE))
		mdlLevel_getIdFromName(&levelId_CutfaceModel, modelRef3d, LEVEL_NULL_ID, clipNgName.c_str());
	// 创建命名组
	if (NG_Success == NamedGroup::Create(ngCAESolid, clipNgName.data(), L"CAE模型组", ngfCAESolid, modelRef3d))
	{
		Transform	tMatrix;
		size_t Index;
		Index = 0;
		DPoint3d PlacePosPoint;
		DPoint3d FaceNormal;
		mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, CutFace.GetElementDescrP(), NULL);
		{//求得旋转矩阵,可以将所有生成的切面转到XY平面上

			//获取切面上的一个点以及切面的法向
			DPoint3d PlacePosPoint;
			DPoint3d FaceNormal;
			mdlElmdscr_extractNormal(&FaceNormal, &PlacePosPoint, CutFace.GetElementDescrP(), NULL);
			DVec3d vec1, vec2, vec3;
			vec1.x = vec1.y = 0.0;
			vec1.z = 1.0;

			//XZ平面的法向
			vec2.x = 0.0;
			vec2.y = 1.0;
			vec2.z = 0.0;

			//XZ平面的法向反方向
			vec3.x = 0.0;
			vec3.y = -1.0;
			vec3.z = 0.0;

			double dot2 = FaceNormal.DotProduct(vec1);
			double dot3 = FaceNormal.DotProduct(vec3);
			if (dot3 > dot2)
			{
				vec2.y = -1.0;
				vec1.z = 1.0;
			}
			else
			{
				vec2.y = 1.0;
				vec1.z = -1.0;
			}

			RotMatrix  rMatrix, rMatrix1, rMatrix2;
			//将平面旋转到XZ平面
			mdlRMatrix_fromVectorToVector(&rMatrix1, &FaceNormal, &vec2);
			//将XZ平面旋转到xy平面
			mdlRMatrix_fromVectorToVector(&rMatrix2, &vec2, &vec1);
			mdlRMatrix_multiply(&rMatrix, &rMatrix2, &rMatrix1);

			mdlTMatrix_fromRMatrix(&tMatrix, &rMatrix);
			mdlTMatrix_setOrigin(&tMatrix, &PlacePosPoint);
		}
		FaceNormal.Normalize();

		DVec3d lineFromX = DVec3d::FromStartEnd(prjstrPoint, prjendPoint);
		lineFromX.Normalize();
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

				//获取切面与实体的所有交点，交点保存在InterSectPoints中
				GetMeshNodeIntersectwithcutfacePoints(&eeh, CutFacePoints, CutFace);
				if (CutFacePoints.size() < 3)
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

				CutPt tempLineX;
				tempLineX.dPt.x = midPos.x + 100 * lineFromX.x*UOR_PER_UNIT(m_Unit);
				tempLineX.dPt.y = midPos.y + 100 * lineFromX.y*UOR_PER_UNIT(m_Unit);
				tempLineX.dPt.z = midPos.z + 100 * lineFromX.z*UOR_PER_UNIT(m_Unit);
				CutFacePoints.insert(CutFacePoints.begin(), tempLineX);
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
				CutFacePoints.erase(CutFacePoints.begin());

				//将切面点信息保存在结构体中
				CutPoints cutpts;
				memset(&cutpts, 0, sizeof(CutPoints));
				cutpts.Num = (int)CutFacePoints.size();
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					cutpts.StartID[i] = CutFacePoints.at(i).StartID;
					cutpts.EndID[i] = CutFacePoints.at(i).EndID;
					cutpts.StartRatio[i] = CutFacePoints.at(i).StartRatio;
				}

#pragma region 在另一个model中生成Mesh,并将相关数据写入到mesh中	
				MSElementDescrP m_resultEdp = NULL;//mesh面指针
				MSElementDescrP m_resultEdp2 = NULL;//mesh面指针
				bvector<int> vecIndices; // Mesh体的节点集合
				bvector<DPoint3d> vecPoints; // Mesh体的顶点坐标集合
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					vecIndices.push_back((int)(CutFacePoints.size() - i));
				}
				vecIndices.push_back(0);
				for (int i = 0; i < CutFacePoints.size(); i++)
				{
					vecPoints.push_back(CutFacePoints.at(i).dPt);
				}

				// 生成CAE的Mesh体
				if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp, NULL, vecIndices.data(), (int)vecIndices.size(), vecPoints.data(), (int)vecPoints.size()))
				{
					// 生成Mesh体的ElementHandle
					EditElementHandle tmpeeh(m_resultEdp, true, true, modelRef3d);

					//旋转mesh面
					TransformInfo tInfo(tMatrix);
					tmpeeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(tmpeeh, tInfo);

					// 为了消除旋转后mesh面节点的Z坐标不在一个平面的问题
					// 所以取出所有节点的坐标，将z置为0.0
					bvector<int>		pIndices;
					bvector<DPoint3d>	pXYZ;
					if (SUCCESS == mdlMesh_getPolyfaceArrays(tmpeeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
					{
						bvector<DPoint3d>::iterator iterNow = pXYZ.begin();
						bvector<DPoint3d>::iterator iterend = pXYZ.end();
						// 将所有节点坐标的z置为0.0
						for (; iterNow != iterend; iterNow++)
						{
							iterNow->z = 0.0;
						}

						// 重新生成最终的截面Mesh
						if (SUCCESS == mdlMesh_newVariablePolyface(&m_resultEdp2, NULL, pIndices.data(), (int)pIndices.size(), pXYZ.data(), (int)pXYZ.size()))
						{
							// 生成Mesh体的ElementHandle
							EditElementHandle tempeeh(m_resultEdp2, true, true, modelRef3d);

							// 生成Mesh体相对应的CAE结果节点集合
							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

							// 将Mesh体相对应的CAE结果节点集合保存到ElementHandle中
							tempeeh.ScheduleWriteXAttribute(handlerId, XAttrID_CutFace, sizeof(CutPoints), &cutpts);

							//转换矩阵保存到xattribute中
							XAttributeHandlerId handlerMatrixId(XAttrHandleID_Matrix, XAttrHandleID_Matrix);
							// 将Mesh体相对应的转换矩阵保存到ElementHandle中
							tempeeh.ScheduleWriteXAttribute(handlerMatrixId, XAttrID_Matirx, sizeof(Bentley::Transform), &tMatrix);

							// ElementHandle添加到视图中
							ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
							propsSetter->SetLevel(levelId_CutfaceModel);
							propsSetter->Apply(tempeeh);

							// ElementHandle添加到视图中
							tempeeh.AddToModel();

							// 通过元素ID向组中增加元素
							ngCAESolid->AddMember(tempeeh.GetElementP()->GetID(), modelRef3d, NamedGroupMemberFlags());
						}
					}
				}
#pragma endregion
			}
		}

		ngCAESolid->WriteToFile(true);
		mdlModelRef_activateAndDisplay(modelRef3d);//激活并显示创建的Model
		//切完后显示光滑模式
		ApplyCaeDisplayStyle(L"CAEMesh:NoEdge", tcb->lstvw);
		MsdiMsCaeSiUtility::fitView(0);
	}
	mdlLevelTable_rewrite(modelRef3d);
	mdlModelRef_freeWorking(modelRef3d);

	//DPoint3d GrpTextPos;//切面组名称
	//GrpTextPos = maxPoint;
	//GrpTextPos.x = GrpTextPos.x - (maxPoint.x - minPoint.x)/2;
	//GrpTextPos.y = GrpTextPos.y - 1 * UOR_PER_UNIT(m_Unit);
	//CreateText(GrpTextPos, UOR_PER_UNIT(m_Unit) / 2, UOR_PER_UNIT(m_Unit) / 2, clipName);
}

//从MESH中的XAttribute读取数据（主要用来给数值输出使用）
void ClipFace::GetMeshPointsAndValueForOutDatalist(bvector<PointOutData>& MeshPoints, const bmap<WString, bvector<double>>& allNineDatas, EditElementHandleCR eeh)
{
	bvector<int>					pIndices;
	bvector<DPoint3d>				pXYZ;

	if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_CutFace);
		CutPoints *cutPoints = NULL;

		if (xh.IsValid())
		{
			cutPoints = (CutPoints *)xh.PeekData();
		}
		else
			return;

		for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
		{
			int iIndices = pIndices.at(iIdx);
			if (iIndices != 0)
			{
				CutPt cutPoint;
				cutPoint.dPt = pXYZ.at(iIndices - 1);
				cutPoint.StartID = cutPoints->StartID[iIndices - 1];
				cutPoint.EndID = cutPoints->EndID[iIndices - 1];
				cutPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];

				PointOutData tempPoint;
				tempPoint.dPt = pXYZ.at(iIndices - 1);
				double dVal = 0.0;
				for (auto iter = allNineDatas.begin(); iter != allNineDatas.end(); iter++)
				{
					dVal = 0.0;
					GetCutPointValueByeType(dVal, iter->second, cutPoint);
					tempPoint.ResultVals[iter->first] = dVal;
				}

				MeshPoints.push_back(tempPoint);
			}
		}
	}
	if (!MeshPoints.empty())
		MeshPoints.push_back(MeshPoints.at(0));
}

void ClipFace::GetMeshPointsAndValue(bvector<ContourPoint>& MeshPoints, EditElementHandle& eeh, bvector<double>& oCaeresults)
{
	bvector<int>					pIndices;
	bvector<DPoint3d>				pXYZ;
	bvector<bvector<ContourPoint>>	vecSolidPtAndValTmp;
	bvector<ContourPoint>			vecFacePtAndVal;
	int								iNumIndexPerFace;
	int								iNumFace;

	if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
	{
		XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
		XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_CutFace);
		CutPoints *cutPoints = NULL;

		if (xh.IsValid())
		{
			cutPoints = (CutPoints *)xh.PeekData();
		}
		else
			return;

		for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
		{
			int iIndices = pIndices.at(iIdx);
			if (iIndices != 0)
			{
				CutPt cutPoint;
				cutPoint.dPt = pXYZ.at(iIndices - 1);
				cutPoint.StartID = cutPoints->StartID[iIndices - 1];
				cutPoint.EndID = cutPoints->EndID[iIndices - 1];
				cutPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];

				ContourPoint tempPoint;
				tempPoint.dPt = pXYZ.at(iIndices - 1);
				GetCutPointValueByeType(tempPoint.dVal, oCaeresults, cutPoint);
				//tempPoint.dVectorPt = cutPoints->vecTorPoints[iIndices - 1];
				MeshPoints.push_back(tempPoint);

			}

		}
	}
	if (!MeshPoints.empty())
		MeshPoints.push_back(MeshPoints.at(0));
}
void ClipFace::MakeColorLine(DSegment3dCR segment, UInt32 color, DgnPlatform::LevelId levelId)
{
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	UInt32 newColor = (color == 256) ? 0 : color;
	propsSetter->SetColor(newColor);
	propsSetter->SetLevel(levelId);

	EditElementHandle eeh;
	if (SUCCESS == LineHandler::CreateLineElement(eeh, NULL, segment, true, *MASTERFILE))
	{
		propsSetter->SetLocked(eeh, true);
		propsSetter->Apply(eeh);
		eeh.AddToModel();
	}
}
void ClipFace::DrawFaceVector(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
	DPoint3d thePtmin;
	DPoint3d thePtmax;
	thePtmin = CutFacePoints.at(0).dPt;
	thePtmax = CutFacePoints.at(2).dPt;
	double theDistance = mdlVec_distanceXY(&thePtmin, &thePtmax);

	//循环计算矩形面上所有节点之间的等值点
	for (int i = 0; i < CutFacePoints.size(); i++)
	{
		CutPt oCp = CutFacePoints.at(i);
		DPoint3d dPt = oCp.dPt;
		DPoint3d dVecPt;
		GetCutPointValueByeType(dVecPt.x, oCaeresultsux, oCp);
		GetCutPointValueByeType(dVecPt.y, oCaeresultsuy, oCp);
		GetCutPointValueByeType(dVecPt.z, oCaeresultsuz, oCp);

		double dVecterLen = -1;// 矢量相对长度
		int iClrNum = 1;
		double Lenth = 1;// 矢量线长度
		DPoint3d tempPont;
		tempPont.x = 0;
		tempPont.y = 0;
		tempPont.z = 0;
		if (dVecPt.x == 0 && dVecPt.y == 0 && dVecPt.z == 0)
		{
			return;
		}

		double tValue;
		GetCutPointValueByeType(tValue, oCaeresults, oCp);
		UInt32 theColor = GetTheColor(tValue, iClrNum);

		DPoint3d pts[2];
		pts[0] = dPt;
		pts[1] = pts[0];
		double tempLenth;
		tempLenth = iClrNum*theDistance / 8;
		Lenth = (abs(dVecPt.x) > abs(dVecPt.y)) ? abs(dVecPt.x) : abs(dVecPt.y);
		Lenth = (Lenth > abs(dVecPt.z)) ? Lenth : abs(dVecPt.z);
		Lenth = tempLenth / Lenth;

		DSegment3d segment;
		segment.SetStartPoint(pts[0]);
		segment.SetEndPoint(pts[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		DPoint3d tempPoints[2];
		tempPoints[1] = pts[1];
		tempPoints[0].x = pts[0].x + (pts[1].x - pts[0].x) * 9 / 10;
		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);

		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);

		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
		tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
		segment.SetStartPoint(tempPoints[0]);
		segment.SetEndPoint(tempPoints[1]);
		MakeColorLine(segment, theColor, levelId_ContourPlot);
	}

}

void ClipFace::DrawFaceLine(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
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
		GetCutPointValueByeType(TcpStart.dVal, oCaeresults, oCpStart);

		oStart.dVal = TcpStart.dVal;
		oStart.dPt = TcpStart.dPt;
		oEnd.dPt = oCpEnd.dPt;
		GetCutPointValueByeType(oEnd.dVal, oCaeresults, oCpEnd);
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

	// 根据云图的颜色区间信息，生成等值线
	for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
	{
		double dContourVal = s_DrawColorShape.ClrShapeVal[iVal];

		int iContourPtNow = 0;
		int iContourPtEnd = (int)contourlist.size() - 1;
		ContourPoint ContourPt;
		ContourPoint ContourPtEnd;

		// 根据矩形面上的等值点，生成各区域的云图
		for (; iContourPtNow <= iContourPtEnd; iContourPtNow++)
		{
			DSegment3d segment;
			ContourPt = contourlist.at(iContourPtNow);

			segment.SetStartPoint(ContourPt.dPt);

			if (abs(dContourVal - ContourPt.dVal) < 0.0000001)
			{
				for (; iContourPtEnd > iContourPtNow; iContourPtEnd--)
				{
					ContourPtEnd = contourlist.at(iContourPtEnd);
					if (abs(dContourVal - ContourPtEnd.dVal) < 0.0000001)
					{
						segment.SetEndPoint(ContourPtEnd.dPt);

						//记录等值线的坐标用来输出等值线数值
						vector<DPoint3d> tempLinePoints;
						tempLinePoints.push_back(ContourPt.dPt);
						tempLinePoints.push_back(ContourPtEnd.dPt);
						allLinePoints[s_DrawColorShape.fillCororIndex[iVal]].push_back(tempLinePoints);

						break;
					}
				}
			}
		}
	}

}
void ClipFace::_OnPostInstall()
{
	//获取basePt和显示层ID
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(m_basePt, m_Unit, levelId_CaeModel, s_pDCShape->NgName);

	getMaxMinXYZPtValOfProject(s_pDCShape->ProjectId, m_minx, m_maxx, m_miny, m_maxy, m_minz, m_maxz);

	m_minx = m_minx*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.x;
	m_maxx = m_maxx*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.x;

	m_miny = m_miny*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.y;
	m_maxy = m_maxy*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.y;

	m_minz = m_minz*UOR_PER_UNIT(m_Unit) - 10 * UOR_PER_UNIT(m_Unit) + m_basePt.z;
	m_maxz = m_maxz*UOR_PER_UNIT(m_Unit) + 10 * UOR_PER_UNIT(m_Unit) + m_basePt.z;

	AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.
	_BeginDynamics();
	__super::_OnPostInstall();
}

//获取切点的Value值，通过type类型
void ClipFace::GetCutPointValueByeType(double& cutValue, const bvector<double>& oCaeresults, CutPt cutpt)
{
	int startID = cutpt.StartID;
	int endID = cutpt.EndID;
	double startRatio = cutpt.StartRatio;
	double startValue, endValue;
	startValue = 0;
	endValue = 0;
	if (oCaeresults.size() < 1)
	{
		return;
	}
	startValue = oCaeresults.at(startID - 1);
	endValue = oCaeresults.at(endID - 1);
	cutValue = startValue + startRatio*(endValue - startValue);
}

void ClipFace::DrawFacePlot(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults)
{
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
		GetCutPointValueByeType(TcpStart.dVal, oCaeresults, oCpStart);

		oStart.dVal = TcpStart.dVal;
		oStart.dPt = TcpStart.dPt;
		oEnd.dPt = oCpEnd.dPt;
		GetCutPointValueByeType(oEnd.dVal, oCaeresults, oCpEnd);
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
}

void ClipFace::ShowByType(WString PlotName, void(*Showtype)(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults), DgnPlatform::LevelId levelId, int typeID, WString typeName, WString sUnit)
{
	EditElementHandle eh;
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr namedGroup;
	bvector<double>   oCaeresults;
	GetCaeResult(oCaeresults, s_DrawColorShape.ProjectId, typeID, typeName);
	bvector<double>().swap(oCaeresultsux);
	bvector<double>().swap(oCaeresultsuy);
	bvector<double>().swap(oCaeresultsuz);
	GetCaeResult(oCaeresultsux, s_DrawColorShape.ProjectId, typeID, L"UX");
	GetCaeResult(oCaeresultsuy, s_DrawColorShape.ProjectId, typeID, L"UY");
	GetCaeResult(oCaeresultsuz, s_DrawColorShape.ProjectId, typeID, L"UZ");

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);

	// 循环检查是否存在空命名组(命名组中没有元素)
	for each (NamedGroupPtr namedGroup in *ngcCAESolid)
	{
		if (namedGroup.IsValid())
		{
			bmap<UInt32, vector<vector<DPoint3d>>>().swap(allLinePoints);
			WString gName = namedGroup->GetName();
			if (gName.find(NGNAME_PREFIX_CAECLIP) != WString::npos)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);

				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
 
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
					{
						bvector<CutPt> CutFacePoints;

						bvector<int>					pIndices;
						bvector<DPoint3d>				pXYZ;
						bvector<bvector<ContourPoint>>	vecSolidPtAndValTmp;
						bvector<ContourPoint>			vecFacePtAndVal;
						int								iNumIndexPerFace;
						int								iNumFace;

						if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &iNumIndexPerFace, &iNumFace))
						{
							XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
							XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_CutFace);
							CutPoints *cutPoints = NULL;

							if (xh.IsValid())
							{
								cutPoints = (CutPoints *)xh.PeekData();
							}

							for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
							{
								int iIndices = pIndices.at(iIdx);
								if (iIndices != 0)
								{
									CutPt tempPoint;
									tempPoint.dPt = pXYZ.at(iIndices - 1);
									tempPoint.StartID = cutPoints->StartID[iIndices - 1];
									tempPoint.EndID = cutPoints->EndID[iIndices - 1];
									tempPoint.StartRatio = cutPoints->StartRatio[iIndices - 1];
									CutFacePoints.push_back(tempPoint);
								}

							}
						}
						if (CutFacePoints.size()>0)
							CutFacePoints.push_back(CutFacePoints.at(0));
						Showtype(CutFacePoints, levelId, typeID, typeName, oCaeresults);
					}
				}
				DgnPlatform::LevelId levelId_CutFaceModel;
				WChar             name[MAX_MODEL_NAME_LENGTH];
				mdlModelRef_getModelName(ACTIVEMODEL, name);
				WString modelName;
				modelName.append(name);
				mdlLevel_getIdFromName(&levelId_CutFaceModel, MASTERFILE, LEVEL_NULL_ID, modelName.c_str());
				mdlLevel_setDisplay(MASTERFILE, levelId_CutFaceModel, false);
			}
			//画等值线和输出等值线数值
			if (Showtype == ClipFace::DrawFaceLine)
			{
				//小数点位数
				int idec = 4;
				WString dec;
				dec.Sprintf(L"%%0.%dg", idec);
				//等值线数值文字大小
				double valueSize = 0.5 * UOR_PER_UNIT(sUnit);
				//等值线数值划分区间
				double dis = 15 * UOR_PER_UNIT(sUnit);
				// 根据云图的颜色区间信息，生成等值线
				for (int iVal = 0; iVal < s_DrawColorShape.Color_num; ++iVal)
				{
					UInt32 theColor = s_DrawColorShape.fillCororIndex[iVal];
					double dContourVal = s_DrawColorShape.ClrShapeVal[iVal];
					vector <vector<DPoint3d>> colorLinePoints = allLinePoints[theColor];
					vector<deque<DPoint3d>> result = connectLineString(colorLinePoints);

					ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
					UInt32 newColor = (theColor == 256) ? 0 : theColor;
					propsSetter->SetColor(newColor);
					propsSetter->SetLevel(levelId);

					WChar buffer[20];
					unsigned int  old_exponent_format = _set_output_format(_TWO_DIGIT_EXPONENT);
					_swprintf(buffer, dec.GetWCharCP(), dContourVal);
					_set_output_format(old_exponent_format);
					WString wValue;
					wValue.append(buffer);
					for (int i = 0; i < result.size(); i++)
					{
						deque<DPoint3d> linePoints = result.at(i);
						if (linePoints.size() < 1)
						{
							continue;
						}
						EditElementHandle eeh;
						bvector<DPoint3d> bLinePoints;
						bLinePoints.insert(bLinePoints.end(), linePoints.begin(), linePoints.end());
						DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLineString(bLinePoints)
							, NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
						propsSetter->Apply(eeh);
						eeh.AddToModel();
						bvector<DPoint3d> lPoints;
						bvector<DPoint3d> tangPoints;
						GetDisPointsandTange(lPoints, tangPoints, eeh.GetElementDescrP(), dis);
						for (int j = 0; j < lPoints.size(); j++)
						{
							DPoint3d lpoint = lPoints.at(j);
							DPoint3d tpoint = tangPoints.at(j);
							DPoint3d tpt;
							if (tpoint.x > 0)
							{
								tpt = DPoint3d::From(1.0, 0.0, 0.0);
							}
							else
							{
								tpt = DPoint3d::From(-1.0, 0.0, 0.0);
							}

							RotMatrix rotM;
							mdlRMatrix_fromVectorToVector(&rotM, &tpt, &tpoint);

							CreateLineText(lpoint, valueSize, valueSize, wValue, levelId, ACTIVEMODEL, rotM);
						}
					}
				}
			}

		}

	}
}

void ClipFace::MakeDgnModel(DgnModelRefP& modelRef3d, WString modelName)
{

	//创建model
	if (mdlDgnFileObj_findModelIDByName(mdlDgnFileObj_getMasterFile(), NULL, modelName.data()) != SUCCESS)
	{
		mdlDgnFileObj_createModel(&modelRef3d, mdlDgnFileObj_getMasterFile(), ACTIVEMODEL, modelName.data(), modelName.data(), NULL, NULL);
	}
	else
	{
		mdlModelRef_createWorkingByName(&modelRef3d, mdlDgnFileObj_getMasterFile(), modelName.data(), TRUE, TRUE);
	}
}
void ClipFace::MakeColorMesh(bvector<ContourPoint> pMeshFace, UInt32 color, DgnPlatform::LevelId levelId)
{
	MeshSurfaceValue meshValues;
	if (pMeshFace.size() < 3) return;

	MSElementDescrP     m_resultEdp;
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	UInt32 newColor = (color == 256) ? 0 : color;
	propsSetter->SetColor(newColor);
	propsSetter->SetFillColor(newColor);
	propsSetter->SetLevel(levelId);

	bvector<int> vecIndices;
	bvector<DPoint3d> vecPoints;
	int nodesCount = 0;
	bvector<ContourPoint>::iterator iterNow = pMeshFace.begin();
	bvector<ContourPoint>::iterator iterend = pMeshFace.end();
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
	}
	return;
}

//判断直线是否有交，判断方法：直线两端点是否在面的同一侧
bool ClipFace::IsinFacesSameSide(DPoint3d CutFacePos, DPoint3d pt1, DPoint3d pt2, DPoint3d CutNormal)
{
	//求得面上一点与点PT1的方向矢量1
	DVec3d vec1 = DVec3d::FromStartEndNormalize(CutFacePos, pt1);
	//求得面上一点与点PT2的方向矢量2
	DVec3d vec2 = DVec3d::FromStartEndNormalize(CutFacePos, pt2);
	//得到面的法向
	DVec3d normalVec = DVec3d::From(CutNormal);
	double theDot1 = 0;
	double theDot2 = 0;
	//方向矢量1与法向的点乘
	theDot1 = vec1.DotProduct(normalVec);
	//方向矢量2与法向的点乘
	theDot2 = vec2.DotProduct(normalVec);
	//如果方向矢量1与法向的点乘与方向矢量2与法向的点乘的乘积为负值，说明直线与面有交
	if (theDot1*theDot2 < 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ClipFace::FliterMeshSolids//筛选实体
(
MSElementDescr* CutFace, bvector<ElementId>& meshsolids, bvector<ElementId>& nitsmeshsolids, WCharCP groupName
)
{
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr namedGroup;
	WString uniqueName;

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	namedGroup = ngcCAESolid->FindByName(groupName);


	if (namedGroup.IsValid())
	{
		DPoint3d CutFacePos;
		DPoint3d CutNormal;
		mdlElmdscr_extractNormal(&CutNormal, &CutFacePos, CutFace, NULL);

		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		bvector<ElementId> meshSolids1;
		bvector<ElementId> meshSolids2;
		bvector<ElementId> meshSolids3;
		bvector<ElementId> meshSolids4;
		bvector<ElementId> meshSolids5;
		bvector<ElementId> meshSolids6;
		bvector<ElementId> meshSolids7;
		bvector<ElementId> meshSolids8;

		bvector<ElementId> NItsmeshSolids1;
		bvector<ElementId> NItsmeshSolids2;
		bvector<ElementId> NItsmeshSolids3;
		bvector<ElementId> NItsmeshSolids4;
		bvector<ElementId> NItsmeshSolids5;
		bvector<ElementId> NItsmeshSolids6;
		bvector<ElementId> NItsmeshSolids7;
		bvector<ElementId> NItsmeshSolids8;


		// 线程1(处理量为总Mesh面数的1/8)
		std::thread t1(FilterMeshThreadFunc, 0, graphicMembers / 8, namedGroup, ref(meshSolids1), ref(NItsmeshSolids1), CutFacePos, CutNormal);
		// 线程2(处理量为总Mesh面数的1/8)
		std::thread t2(FilterMeshThreadFunc, graphicMembers / 8, graphicMembers * 2 / 8, namedGroup, ref(meshSolids2), ref(NItsmeshSolids2), CutFacePos, CutNormal);
		// 线程3(处理量为总Mesh面数的1/8)
		std::thread t3(FilterMeshThreadFunc, graphicMembers * 2 / 8, graphicMembers * 3 / 8, namedGroup, ref(meshSolids3), ref(NItsmeshSolids3), CutFacePos, CutNormal);
		// 线程4(处理量为总Mesh面数的1/8)
		std::thread t4(FilterMeshThreadFunc, graphicMembers * 3 / 8, graphicMembers * 4 / 8, namedGroup, ref(meshSolids4), ref(NItsmeshSolids4), CutFacePos, CutNormal);
		// 线程5(处理量为总Mesh面数的1/8)
		std::thread t5(FilterMeshThreadFunc, graphicMembers * 4 / 8, graphicMembers * 5 / 8, namedGroup, ref(meshSolids5), ref(NItsmeshSolids5), CutFacePos, CutNormal);
		// 线程6(处理量为总Mesh面数的1/8)
		std::thread t6(FilterMeshThreadFunc, graphicMembers * 5 / 8, graphicMembers * 6 / 8, namedGroup, ref(meshSolids6), ref(NItsmeshSolids6), CutFacePos, CutNormal);
		// 线程7(处理量为总Mesh面数的1/8)
		std::thread t7(FilterMeshThreadFunc, graphicMembers * 6 / 8, graphicMembers * 7 / 8, namedGroup, ref(meshSolids7), ref(NItsmeshSolids7), CutFacePos, CutNormal);
		// 线程8(处理量为总Mesh面数的1/8)
		std::thread t8(FilterMeshThreadFunc, graphicMembers * 7 / 8, graphicMembers, namedGroup, ref(meshSolids8), ref(NItsmeshSolids8), CutFacePos, CutNormal);

		// 启动线程
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		t7.join();
		t8.join();

		meshsolids.insert(meshsolids.end(), meshSolids1.begin(), meshSolids1.end());
		meshsolids.insert(meshsolids.end(), meshSolids2.begin(), meshSolids2.end());
		meshsolids.insert(meshsolids.end(), meshSolids3.begin(), meshSolids3.end());
		meshsolids.insert(meshsolids.end(), meshSolids4.begin(), meshSolids4.end());
		meshsolids.insert(meshsolids.end(), meshSolids5.begin(), meshSolids5.end());
		meshsolids.insert(meshsolids.end(), meshSolids6.begin(), meshSolids6.end());
		meshsolids.insert(meshsolids.end(), meshSolids7.begin(), meshSolids7.end());
		meshsolids.insert(meshsolids.end(), meshSolids8.begin(), meshSolids8.end());

		//获取所有不相交mesh体集合
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids1.begin(), NItsmeshSolids1.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids2.begin(), NItsmeshSolids2.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids3.begin(), NItsmeshSolids3.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids4.begin(), NItsmeshSolids4.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids5.begin(), NItsmeshSolids5.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids6.begin(), NItsmeshSolids6.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids7.begin(), NItsmeshSolids7.end());
		nitsmeshsolids.insert(nitsmeshsolids.end(), NItsmeshSolids8.begin(), NItsmeshSolids8.end());
		// 释放变量
		bvector<ElementId>().swap(meshSolids1);
		bvector<ElementId>().swap(meshSolids2);
		bvector<ElementId>().swap(meshSolids3);
		bvector<ElementId>().swap(meshSolids4);
		bvector<ElementId>().swap(meshSolids5);
		bvector<ElementId>().swap(meshSolids6);
		bvector<ElementId>().swap(meshSolids7);
		bvector<ElementId>().swap(meshSolids8);

		bvector<ElementId>().swap(NItsmeshSolids1);
		bvector<ElementId>().swap(NItsmeshSolids2);
		bvector<ElementId>().swap(NItsmeshSolids3);
		bvector<ElementId>().swap(NItsmeshSolids4);
		bvector<ElementId>().swap(NItsmeshSolids5);
		bvector<ElementId>().swap(NItsmeshSolids6);
		bvector<ElementId>().swap(NItsmeshSolids7);
		bvector<ElementId>().swap(NItsmeshSolids8);
	}

}

void ClipFace::InstallmyNewInstance(int toolId)//工具开始运行安装
{
	ClipFace* tool = new ClipFace(toolId);
	tool->cutType = 3;
	tool->InstallTool();
}


bool ClipFace::CreateClipFace(DgnButtonEventCR ev, bool bTempClipFace)
{
	bool bCreate = false;
	int type = g_outRebarDlgInfo.chooseBtnIndex;
	//在生成面前需要先得到一条线，线的端点由tempLinePts存取
	bvector<DPoint3d> tempLinePts;
	EditElementHandle  tempLine;

	//面拉升起点、终点坐标
	DPoint3d lstrPt, lsend;
	if (type < 3)
	{
		//辅助线终点坐标，辅助线起点坐标取lstrPt
		DPoint3d lendPt;

		if (0 == type)
		{
			//yz平面X坐标都一样
			lstrPt.x = ev.GetPoint()->x;
			lendPt.x = lstrPt.x;

			lstrPt.y = m_miny;
			lendPt.y = m_maxy;

			lstrPt.z = m_minz;
			lendPt.z = m_minz;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//面拉升终点坐标,面拉升起点取辅助线起点
			lsend.x = lstrPt.x;
			lsend.y = lstrPt.y;
			lsend.z = m_maxz;
		}
		else if (1 == type)
		{
			//xz平面y坐标都一样
			lstrPt.y = ev.GetPoint()->y;
			lendPt.y = lstrPt.y;

			lstrPt.x = m_minx;
			lendPt.x = m_maxx;

			lstrPt.z = m_minz;
			lendPt.z = m_minz;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//面拉升终点坐标,面拉升起点取辅助线起点
			lsend.x = lstrPt.x;
			lsend.y = lstrPt.y;
			lsend.z = m_maxz;
		}
		else if (2 == type)
		{
			//xy平面z坐标都一样
			lstrPt.z = ev.GetPoint()->z;
			lendPt.z = lstrPt.z;

			lstrPt.x = m_minx;
			lendPt.x = m_maxx;

			lstrPt.y = m_miny;
			lendPt.y = m_miny;
			tempLinePts.push_back(lstrPt);
			tempLinePts.push_back(lendPt);

			//面拉升终点坐标,面拉升起点取辅助线起点
			lsend.x = lstrPt.x;
			lsend.z = lstrPt.z;
			lsend.y = m_maxy;
		}
		bCreate = CreateElement(tempLine, tempLinePts);
	}
	else
	{
		//size=0时不执行create
		int size = (int)m_points.size();
		if (1 == size)
		{
			tempLinePts.push_back(m_points[0]);
			tempLinePts.push_back(*ev.GetPoint());

			double dDistance = mdlVec_distance(&tempLinePts[0], &tempLinePts[1]);
			DPoint3d vNormal, vLength;
			vNormal.x = 0;
			vNormal.y = 0;
			vNormal.z = 1;

			RotMatrix rotmatrix;
			AccuDraw::GetInstance().GetRotation(rotmatrix);
			mdlVec_multiplyRotMatrixTransposeDVec3dArray(&vLength, &rotmatrix, &vNormal, 1);
			vLength.Normalize();
			vLength.Scale(dDistance);
			mdlVec_add(&tempLinePts.at(0), &tempLinePts.at(0), &vLength);
			vLength.Normalize();
			vLength.Scale(-2 * dDistance);
			mdlVec_add(&tempLinePts.at(1), &tempLinePts.at(0), &vLength);

			tPlineOrg = tempLinePts[0];
			tPlineEnd = tempLinePts[1];

			prjendPoint = *ev.GetPoint();
			mdlVec_projectPointToLine(&prjstrPoint, NULL, &prjendPoint, &tempLinePts[0], &tempLinePts[1]);
			bCreate = CreateElement(tempLine, tempLinePts);
			lsend = prjendPoint;
			lstrPt = prjstrPoint;
		}
		else if (size > 1 && false == bTempClipFace)
		{
			if (!ClipFace::IsClipFaceModel())
			{
				//面拉升方向矢量
				DVec3d lined;
				lined = DVec3d::FromStartEnd(prjstrPoint, prjendPoint);
				lined.Normalize();
				//将辅助线起点往前拉升（防止出现不能左右切穿的情况）
				tPlineOrg.x = tPlineOrg.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				tPlineOrg.y = tPlineOrg.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				tPlineOrg.z = tPlineOrg.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);
				tempLinePts.push_back(tPlineOrg);
				tPlineEnd.x = tPlineEnd.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				tPlineEnd.y = tPlineEnd.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				tPlineEnd.z = tPlineEnd.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);

				//将辅助线终点往后拉升（防止出现不能左右切穿的情况）
				prjstrPoint.x = prjstrPoint.x - 10 * lined.x*UOR_PER_UNIT(m_Unit);
				prjstrPoint.y = prjstrPoint.y - 10 * lined.y*UOR_PER_UNIT(m_Unit);
				prjstrPoint.z = prjstrPoint.z - 10 * lined.z*UOR_PER_UNIT(m_Unit);

				prjendPoint.x = prjendPoint.x + 10 * lined.x*UOR_PER_UNIT(m_Unit);
				prjendPoint.y = prjendPoint.y + 10 * lined.y*UOR_PER_UNIT(m_Unit);
				prjendPoint.z = prjendPoint.z + 10 * lined.z*UOR_PER_UNIT(m_Unit);
				tempLinePts.push_back(tPlineEnd);

				bCreate = CreateElement(tempLine, tempLinePts);
				lsend = prjendPoint;
				lstrPt = prjstrPoint;
			}
			else
				bCreate = false;
		}
	}
	if (bCreate)
	{
		MSElementDescrP faceDescrP = nullptr;
		mdlCurrTrans_begin();
		mdlCurrTrans_identity();
		mdlSurface_project(&faceDescrP, tempLine.GetElementDescrP(), &lstrPt, &lsend, NULL);
		mdlCurrTrans_end();
		EditElementHandle faceHandle(faceDescrP, TRUE, TRUE);
		if (bTempClipFace)
		{
			RedrawElems redrawElems;
			redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
			redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
			redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
			redrawElems.DoRedraw(faceHandle);
		}
		else
		{
			CutSolidsByFace(faceHandle);
			RefreshSurfaceShowDlg();
		}
	}
	return bCreate;
}

void ClipFaceCmd(WCharCP unparsed)
{
	ClipFace::InstallmyNewInstance(COMMAND_CLIPEFACE);
}