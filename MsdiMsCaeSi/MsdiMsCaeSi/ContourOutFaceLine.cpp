#include "MsdiMsCaeSipch.h"
DrawTempOutLine   s_drawtempLine;
std::string GetNormalString(DVec3d normal, bvector<std::string>& normalstd)
{
	for (size_t i = 0; i < normalstd.size();i++)//
	{
		std::string tempNormal = normalstd.at(i);
		WString tempw;
		tempw.AppendA(tempNormal.c_str());
		bvector<WString> splitResult;
		BeStringUtilities::Split(tempw.data(), L"_", splitResult);
		if (splitResult.size() != 3)
			continue;
		DVec3d tNorVec;
		tNorVec.x = BeStringUtilities::Wcstol(splitResult.at(0).GetWCharCP(),NULL,10) / 10000;
		tNorVec.y = BeStringUtilities::Wcstol(splitResult.at(1).GetWCharCP(),NULL,10) / 10000;
		tNorVec.z = BeStringUtilities::Wcstol(splitResult.at(2).GetWCharCP(),NULL,10) / 10000;
																			
		double theDot = 2;
		theDot = tNorVec.DotProduct(normal);
		if (theDot>0.1||theDot<-0.1)
		{
			return tempNormal;
		}
	}

	std::string midString;
	char tempCx[20], tempCy[20], tempCz[20];

	ltoa((long)(fabs(normal.x)*10000), tempCx, 10);
	ltoa((long)(fabs(normal.y)*10000), tempCy, 10);
	ltoa((long)(fabs(normal.z)*10000), tempCz, 10);

	midString.append(tempCx);
	midString.append("_");
	midString.append(tempCy);
	midString.append("_");
	midString.append(tempCz);

	normalstd.push_back(midString);

	return midString;
}

void DrawContourOutFaceLine(bvector<std::string>& normalstd, map<std::string, bvector<bvector<ContourPoint>>>& faceDatas,
	bvector<bvector<ContourPoint>>& PlotDatas)
{
	for (UInt32 i = 0; i < PlotDatas.size(); i++)
	{
		bvector<ContourPoint> tempface;
		tempface = PlotDatas.at(i);

		ContourPoint pt1, pt2, pt3;
		pt1 = tempface.at(0);
		pt2 = tempface.at(1);
		pt3 = tempface.at(2);

		DVec3d vec1 = DVec3d::FromStartEndNormalize(pt1.dPt, pt2.dPt);
		DVec3d vec2 = DVec3d::FromStartEndNormalize(pt1.dPt, pt3.dPt);

		DVec3d tempNormal1 = DVec3d::FromCrossProduct(vec1, vec2);
		tempNormal1.Normalize();

		std::string normalkey = GetNormalString(tempNormal1, normalstd);
		bvector<bvector<ContourPoint>>* temp = &faceDatas[normalkey];
		if (tempface.size()>2)
		{
			temp->push_back(tempface);
		}

		bvector<ContourPoint>().swap(tempface);
	}
}

std::string GetLineMidValueString(int node1, int node2)
{
	
	std::string midString;
	char tempCx[20], tempCy[20];

	if (node2<node1)
	{
		int temp = node1;
		node1 = node2;
		node2 = temp;
	}

	itoa((int)(node1), tempCx, 10);
	itoa((int)(node2), tempCy, 10);

	midString.append(tempCx);
	midString.append("-");
	midString.append(tempCy);

	return midString;
}
void DrawoutFaceLine(std::string normalKey, map<std::string, bvector<bvector<ContourPoint>>>& faceDatas, LevelId  levelId,bvector<DPoint3d>& linePoints)
{
	//MSElement tempElem;
	bvector<bvector<ContourPoint>> tfaceS = faceDatas[normalKey];
	if (normalKey=="0-0-0")
	{
		return;
	}
	ElementPropertiesSetterPtr propsSetter = ElementPropertiesSetter::Create();
	propsSetter->SetLevel(levelId);
	map<std::string, int> stringmap;
	for (size_t i = 0; i < tfaceS.size(); i++)
	{
		bvector<ContourPoint> facepts = tfaceS.at(i);
		for (size_t j = 0; j < facepts.size(); j++)
		{
			if (j < facepts.size() - 1)
			{
				stringmap[GetLineMidValueString(facepts.at(j).NodeId, facepts.at(j + 1).NodeId)] += 1;
			}
			else
			{
				stringmap[GetLineMidValueString(facepts.at(j).NodeId, facepts.at(0).NodeId)] += 1;
			}
		}
	}
	for (size_t i = 0; i < tfaceS.size(); i++)
	{
		bvector<ContourPoint> facepts = tfaceS.at(i);
		for (size_t m = 0; m < facepts.size(); m++)
		{
			if (m < facepts.size() - 1)
			{
				if (stringmap[GetLineMidValueString(facepts.at(m).NodeId, facepts.at(m + 1).NodeId)] < 2)
				{
					linePoints.push_back(facepts.at(m).dPt);
					linePoints.push_back(facepts.at(m + 1).dPt);
				}

			}
			else
			{
				if (stringmap[GetLineMidValueString(facepts.at(m).NodeId, facepts.at(0).NodeId)] < 2)
				{
					linePoints.push_back(facepts.at(m).dPt);
					linePoints.push_back(facepts.at(0).dPt);
				}
			}
		}
		bvector<ContourPoint>().swap(facepts);
	}
}

void PointPushback(bvector<ContourPoint> & vecFace, Statement *l_statment, DPoint3d basePt, WString sProUnit)
{
	ContourPoint oTmp;

	for (int i = 0; i < 4; i++)
	{
		if (l_statment->IsColumnNull(4 * i) == true) continue;

		oTmp.iIdx = i;
		oTmp.NodeId = l_statment->GetValueInt(4 * i);
		oTmp.dPt.x = l_statment->GetValueDouble(4 * i + 1) * UOR_PER_UNIT(sProUnit) + basePt.x;
		oTmp.dPt.y = l_statment->GetValueDouble(4 * i + 2) * UOR_PER_UNIT(sProUnit) + basePt.y;
		oTmp.dPt.z = l_statment->GetValueDouble(4 * i + 3) * UOR_PER_UNIT(sProUnit) + basePt.z;

		vecFace.push_back(oTmp);
	}
}

bool getCaeFaceInfos(WString ngName, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt)
{
	//根据命名组名获取工程ID
	int iProID;
	WString sProUnit;
	splitNgName_Model(ngName, UNUSE_WSTRING, iProID, UNUSE_INT, UNUSE_WSTRING, sProUnit);

	Db m_db;
	Statement *l_statment;
	WString strSql;
	StatusInt rc;

	strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z");
	strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z");
	strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z");
	strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z");
	strSql.append(L"\nFROM (SELECT ProjectId,Node1,Node2,Node3,Node4 FROM FACES WHERE ProjectId = @ProjectId AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 GROUP BY NodeIdStr HAVING COUNT(NodeIdStr) < 2) a \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n1 ON a.ProjectId = n1.ProjectId AND a.Node1 = n1.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n2 ON a.ProjectId = n2.ProjectId AND a.Node2 = n2.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n3 ON a.ProjectId = n3.ProjectId AND a.Node3 = n3.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n4 ON a.ProjectId = n4.ProjectId AND a.Node4 = n4.ID \n");

	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		l_statment = new Statement();

		size_t len = strSql.length() + 1;
		CharP cSQL = (CharP)malloc(len*sizeof(char));
		strSql.ConvertToLocaleChars(cSQL);

		rc = l_statment->Prepare(m_db, cSQL);

		l_statment->BindInt(1, iProID);

		Utf8String sTmp;
		BeStringUtilities::WCharToUtf8(sTmp, ngName.c_str());
		l_statment->BindUtf8String(2, sTmp, l_statment->BindMakeCopy::MAKE_COPY_No);

		if (rc == BE_SQLITE_OK)
		{
			bvector<ContourPoint>	FacePoints;

			while (l_statment->Step() == BE_SQLITE_ROW)
			{
				FacePoints.clear();
				PointPushback(FacePoints, l_statment, basePt, sProUnit);

				vecFaces.push_back(FacePoints);
			}

			bvector<ContourPoint>().swap(FacePoints);
		}

		l_statment->Finalize();

		return true;
	}
	else
		return false;
}

LevelId showContourOutFaceLine(WString ngName, DrawColorShape* pDCShape, LevelId  levelId, DPoint3dCP pBasePt /*= NULL*/)
{
	//获取basePt和显示层ID
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);
	if (pBasePt != NULL)
		basePt = *pBasePt;

	bvector<bvector<ContourPoint>> vecFaces;

	// 取得CAE的所有Mesh面集合和面节点集合
	if (true == getCaeFaceInfos(ngName, vecFaces, basePt))
	{
		map<std::string, bvector<bvector<ContourPoint>>> faceDatasMap;
		bvector<std::string> normalstring;
		if (vecFaces.size()<1)
		{
			return 0;
		}
		DrawContourOutFaceLine(normalstring, faceDatasMap, vecFaces);
	
		bvector<DPoint3d>().swap(s_drawtempLine.linePoints);
		for (size_t i = 0; i < normalstring.size();i++)
		{
			std::string normalKey = normalstring.at(i);
			DrawoutFaceLine(normalKey, faceDatasMap,levelId,s_drawtempLine.linePoints);
		}
		if (s_drawtempLine.linePoints.size()>1)
		{
			s_drawtempLine.StartDrawFunction();
		}
		mdlLevel_setDisplay(MASTERFILE, levelId_CaeModel, false);
	}

	return levelId;
}
