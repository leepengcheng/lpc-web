#include "MsdiMsCaeSipch.h"
extern MSDialogP jdoytBar;
extern WCharP 	  jdwcMessageText;
extern int       jdPercent;
void ThreadFunc(size_t TSttPost, size_t TEndPost, bstdmap<int, ContourPoint>& oCaeNodeMap, double& dVectorLinePerLen, bvector<LineThread>& VectorDatas, DrawColorShape* pDCShape)
{
	int iIdx;
	for (size_t i = TSttPost; i < TEndPost; i++)
	{
		iIdx = (int)i;
		ContourPoint oTmp = oCaeNodeMap[iIdx];
		MakeContourVectorDat(oTmp, dVectorLinePerLen, VectorDatas, pDCShape);
	}
}

//获取矢量图箭头的相关点坐标以及颜色信息
void MakeContourVectorDat(ContourPoint oVertorPt, double& dVectorLinePerLen, bvector<LineThread>& VertorDatas, DrawColorShape* pDCShape)
{
	DPoint3d dPt = oVertorPt.dPt;
	DPoint3d dVecPt = oVertorPt.dVectorPt;
	double dVecterLen = -1;// 矢量相对长度
	int iClrNum = 1;
	double Lenth = 1;// 矢量线长度
	DPoint3d tempPont;
	tempPont.x = 0;
	tempPont.y = 0;
	tempPont.z = 0;
	if (dVecPt.x==0&&dVecPt.y==0&&dVecPt.z==0)
	{
		return;
	}

	if (!(pDCShape->StartValue <= oVertorPt.dVal && oVertorPt.dVal <= pDCShape->EndValue))
	{
		return;
	}

	//通过值获取颜色值
	UInt32 theColor = GetTheColor(oVertorPt.dVal, iClrNum, pDCShape);

	DPoint3d pts[2];
	pts[0] = dPt;
	pts[1] = pts[0];

	// 矢量线长度=数值*单位矢量线长度
	Lenth = oVertorPt.dVal * dVectorLinePerLen;

	//主线段坐标
	DSegment3d segment;
	segment.SetStartPoint(pts[0]);
	segment.SetEndPoint(pts[1]);

	// 生成矢量图展示数据
	LineThread tempLine1;
	tempLine1.LinePt = segment;
	tempLine1.theColor = theColor;
	VertorDatas.push_back(tempLine1);

	DPoint3d tempPoints[2];
	tempPoints[1] = pts[1];
	tempPoints[0].x = pts[0].x + (pts[1].x - pts[0].x) * 9 / 10;
	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;

	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);

	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	// 生成矢量图展示数据
	LineThread tempLine2;
	tempLine2.LinePt = segment;
	tempLine2.theColor = theColor;
	VertorDatas.push_back(tempLine2);

	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	// 生成矢量图展示数据
	LineThread tempLine3;
	tempLine3.LinePt = segment;
	tempLine3.theColor = theColor;
	VertorDatas.push_back(tempLine3);

	tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	// 生成矢量图展示数据
	LineThread tempLine4;
	tempLine4.LinePt = segment;
	tempLine4.theColor = theColor;
	VertorDatas.push_back(tempLine4);

	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
	tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
	segment.SetStartPoint(tempPoints[0]);
	segment.SetEndPoint(tempPoints[1]);

	// 生成矢量图展示数据
	LineThread tempLine5;
	tempLine5.LinePt = segment;
	tempLine5.theColor = theColor;
	VertorDatas.push_back(tempLine5);
}
bool getCaefacePointInfos(WString ngName, WString type, int iProID, int caseID, bstdmap<int, ContourPoint>& CaeMeshNodeMap, bvector<Dpoint3d> VectorPoints, DPoint3dCR basePt, WStringCR sUnit)
{
	Db m_db;
	Statement *l_statment;
	WString strSql;
	StatusInt rc;
	if (type.CompareTo(L"USUM") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, r1.UX,r1.UY,r1.UZ,r1.USUM");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, r2.UX,r2.UY,r2.UZ,r2.USUM");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, r3.UX,r3.UY,r3.UZ,r3.USUM");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, r4.UX,r4.UY,r4.UZ,r4.USUM");
	}
	else if (type.CompareTo(L"UX") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, r1.UX,0.0,0.0,r1.UX");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, r2.UX,0.0,0.0,r2.UX");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, r3.UX,0.0,0.0,r3.UX");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, r4.UX,0.0,0.0,r4.UX");
	}
	else if (type.CompareTo(L"UY") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,r1.UY,0.0,r1.UY");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,r2.UY,0.0,r2.UY");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,r3.UY,0.0,r3.UY");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,r4.UY,0.0,r4.UY");
	}
	else if (type.CompareTo(L"UZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.UZ,r1.UZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.UZ,r2.UZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.UZ,r3.UZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.UZ,r4.UZ");

	}
	else if (type.CompareTo(L"SXY") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,r1.SXY,0.0,r1.SXY");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,r2.SXY,0.0,r2.SXY");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,r3.SXY,0.0,r3.SXY");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,r4.SXY,0.0,r4.SXY");
	}
	else if (type.CompareTo(L"SYZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.SYZ,r1.SYZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.SYZ,r2.SYZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.SYZ,r3.SYZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.SYZ,r4.SYZ");
	}
	else if (type.CompareTo(L"SXZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.SXZ,r1.SXZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.SXZ,r2.SXZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.SXZ,r3.SXZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.SXZ,r4.SXZ");

	}
	else if (type.CompareTo(L"EPTOX") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, r1.EPTOX,0.0,0.0, r1.EPTOX");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, r2.EPTOX,0.0,0.0, r2.EPTOX");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, r3.EPTOX,0.0,0.0, r3.EPTOX");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, r4.EPTOX,0.0,0.0, r4.EPTOX");

	}
	else if (type.CompareTo(L"EPTOY") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,r1.EPTOY,0.0,r1.EPTOY");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,r2.EPTOY,0.0,r2.EPTOY");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,r3.EPTOY,0.0,r3.EPTOY");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,r4.EPTOY,0.0,r4.EPTOY");
	}
	else if (type.CompareTo(L"EPTOZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.EPTOZ,r1.EPTOZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.EPTOZ,r2.EPTOZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.EPTOZ,r3.EPTOZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.EPTOZ,r4.EPTOZ");
	}
	else if (type.CompareTo(L"EPTOEQV") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, r1.EPTOX,r1.EPTOY,r1.EPTOZ,r1.EPTOEQV");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, r2.EPTOX,r2.EPTOY,r2.EPTOZ,r2.EPTOEQV");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, r3.EPTOX,r3.EPTOY,r3.EPTOZ,r3.EPTOEQV");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, r4.EPTOX,r4.EPTOY,r4.EPTOZ,r4.EPTOEQV");
	}
	else if (type.CompareTo(L"EPTOXY") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,r1.EPTOXY,0.0,r1.EPTOXY");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,r2.EPTOXY,0.0,r2.EPTOXY");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,r3.EPTOXY,0.0,r3.EPTOXY");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,r4.EPTOXY,0.0,r4.EPTOXY");
	}
	else if (type.CompareTo(L"EPTOYZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.EPTOYZ,r1.EPTOYZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.EPTOYZ,r2.EPTOYZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.EPTOYZ,r3.EPTOYZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.EPTOYZ,r4.EPTOYZ");

	}
	else if (type.CompareTo(L"EPTOEXZ") == 0)
	{
		strSql.append(L"SELECT a.Node1, n1.X, n1.Y, n1.Z, 0.0,0.0,r1.EPTOEXZ,r1.EPTOEXZ");
		strSql.append(L"\n,a.Node2, n2.X, n2.Y, n2.Z, 0.0,0.0,r2.EPTOEXZ,r2.EPTOEXZ");
		strSql.append(L"\n,a.Node3, n3.X, n3.Y, n3.Z, 0.0,0.0,r3.EPTOEXZ,r3.EPTOEXZ");
		strSql.append(L"\n,a.Node4, n4.X, n4.Y, n4.Z, 0.0,0.0,r4.EPTOEXZ,r4.EPTOEXZ");
	}
	strSql.append(L"\nFROM (SELECT ProjectId,Node1,Node2,Node3,Node4 FROM FACES WHERE ProjectId = @ProjectId AND MSLevelName = @MSLevelName AND ElemShowFlg = 1) a \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n1 ON a.ProjectId = n1.ProjectId AND a.Node1 = n1.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n2 ON a.ProjectId = n2.ProjectId AND a.Node2 = n2.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n3 ON a.ProjectId = n3.ProjectId AND a.Node3 = n3.ID \n");
	strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,X,Y,Z FROM NODES WHERE ProjectId = @ProjectId) n4 ON a.ProjectId = n4.ProjectId AND a.Node4 = n4.ID \n");
	if (type.CompareTo(L"USUM") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UY,UZ,USUM FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UY,UZ,USUM FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UY,UZ,USUM FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UY,UZ,USUM FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"UX") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UX,UX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"UY") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UY,UY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UY,UY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UY,UY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UY,UY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"UZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UZ,UZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UZ,UZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UZ,UZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,UZ,UZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"SXY") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXY,SXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXY,SXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXY,SXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXY,SXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"SYZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SYZ,SYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SYZ,SYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SYZ,SYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SYZ,SYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"SXZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXZ,SXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXZ,SXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXZ,SXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,SXZ,SXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");

	}
	else if (type.CompareTo(L"EPTOX") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOX FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"EPTOY") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOY,EPTOY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOY,EPTOY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOY,EPTOY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOY,EPTOY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"EPTOZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOZ,EPTOZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOZ,EPTOZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOZ,EPTOZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOZ,EPTOZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"EPTOEQV") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOY,EPTOZ,EPTOEQV FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOY,EPTOZ,EPTOEQV FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOY,EPTOZ,EPTOEQV FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOX,EPTOY,EPTOZ,EPTOEQV FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"EPTOXY") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOXY,EPTOXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOXY,EPTOXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOXY,EPTOXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOXY,EPTOXY FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
	else if (type.CompareTo(L"EPTOYZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOYZ,EPTOYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOYZ,EPTOYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOYZ,EPTOYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOYZ,EPTOYZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");

	}
	else if (type.CompareTo(L"EPTOEXZ") == 0)
	{
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOEXZ,EPTOEXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r1 ON a.ProjectId = r1.ProjectId AND a.Node1 = r1.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOEXZ,EPTOEXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r2 ON a.ProjectId = r2.ProjectId AND a.Node2 = r2.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOEXZ,EPTOEXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r3 ON a.ProjectId = r3.ProjectId AND a.Node3 = r3.ID \n");
		strSql.append(L"\n	LEFT OUTER JOIN (select ProjectId,ID,EPTOEXZ,EPTOEXZ FROM RESULTS WHERE ProjectId = @ProjectId AND CaseId = @CaseId) r4 ON a.ProjectId = r4.ProjectId AND a.Node4 = r4.ID \n");
	}
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

		l_statment->BindInt(3, caseID);

		if (rc == BE_SQLITE_OK)
		{
			bvector<ContourPoint>	FacePoints;

			while (l_statment->Step() == BE_SQLITE_ROW)
			{
				ContourPoint oTmp;

				for (int i = 0; i < 4; i++)
				{
					if (l_statment->IsColumnNull(8 * i) == true) continue;
					oTmp.iIdx = i;
					oTmp.NodeId = l_statment->GetValueInt(8 * i);
					oTmp.dPt.x = l_statment->GetValueDouble(8 * i + 1) * UOR_PER_UNIT(sUnit) + basePt.x;
					oTmp.dPt.y = l_statment->GetValueDouble(8 * i + 2) * UOR_PER_UNIT(sUnit) + basePt.y;
					oTmp.dPt.z = l_statment->GetValueDouble(8 * i + 3) * UOR_PER_UNIT(sUnit) + basePt.z;
					oTmp.dVectorPt.x = l_statment->GetValueDouble(8 * i + 4);
					oTmp.dVectorPt.y = l_statment->GetValueDouble(8 * i + 5);
					oTmp.dVectorPt.z = l_statment->GetValueDouble(8 * i + 6);
					oTmp.dVal = l_statment->GetValueDouble(8 * i + 7);
					CaeMeshNodeMap[oTmp.NodeId] = oTmp;
				}
			}

			bvector<ContourPoint>().swap(FacePoints);
		}

		l_statment->Finalize();

		return true;
	}
	else
		return false;
}
bool GetCaeMeshNodeInfo(bstdmap<int, ContourPoint>& CaeMeshNodeMap, double& dMeshDistance, WString ngName, bvector<double> oCaeResultVec,
	bvector<Dpoint3d> VectorPoints, UInt32& NodeNum)
{
	NamedGroupCollectionPtr ngcCAESolid;
	NamedGroupPtr			namedGroup;
	bvector<int>			pIndices;
	bvector<DPoint3d>		pXYZ;
	bvector<int>			nodeIds;
	int CountTime = 0;

	ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
	namedGroup = ngcCAESolid->FindByName(ngName.data());


	DgnPlatform::LevelId  levelId_CAEModel;
	mdlLevel_getIdFromName(&levelId_CAEModel, ACTIVEMODEL, LEVEL_NULL_ID, ngName.c_str());

	if (namedGroup.IsValid())
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		NodeNum = graphicMembers*8;
		for (UInt32 i = 0; i < graphicMembers; i++)
		{
			ElementId elemid = namedGroup->GetMember(i)->GetElementId();
			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
			{
				
				ElementPropertiesGetterPtr propsGetter = ElementPropertiesGetter::Create(eeh);				
				DgnPlatform::LevelId  levelId_Temp;
				levelId_Temp = propsGetter->GetLevel();
				if (levelId_Temp != levelId_CAEModel)
				{
					continue;
				}
				// 变量初始化
				pIndices.clear();
				pXYZ.clear();
				nodeIds.clear();

				// 取得Mesh信息
				if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, NULL, NULL))
				{
					// 取得Mesh单元体的大小
					if (CountTime == 0)
					{
						CountTime++;
						DPoint3d thePtmin;
						DPoint3d thePtmax;
						mdlElmdscr_computeRange(&thePtmin, &thePtmax, eeh.GetElementDescrCP(), NULL);
						dMeshDistance = mdlVec_distance(&thePtmin, &thePtmax);
					}

					// 取得Mesh附加信息
					XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
					XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
					MeshProperty *meshPro = NULL;

					if (xh.IsValid())
					{
						meshPro = (MeshProperty *)xh.PeekData();
					}

					for (int iIdx = 0; iIdx < pIndices.size(); iIdx++)
					{
						int iIndices = pIndices.at(iIdx);
						// 判断Mesh节点编号的下标是否为分隔符('0')
						if (iIndices != 0)
						{
							ContourPoint oTmp;
							// 取得顶点坐标
							oTmp.dPt = pXYZ.at(iIndices - 1);
							// 取得顶点的CAE计算结果
							oTmp.dVal = oCaeResultVec.at(meshPro->ID[iIdx] - 1);
							// 取得顶点的方向矢量
							oTmp.dVectorPt = VectorPoints.at(meshPro->ID[iIdx] - 1);

							CaeMeshNodeMap[meshPro->ID[iIdx]] = oTmp;
						}
					}
				}

				
			}
		}
	}
	
	// 释放变量
	bvector<int>().swap(pIndices);
	bvector<DPoint3d>().swap(pXYZ);
	bvector<int>().swap(nodeIds);

	return true;
}

// 绘制矢量线
void DrawContourVecter(bvector<LineThread>& LineDatas, DgnPlatform::LevelId levelId)
{
	for (UInt32 i = 0; i < LineDatas.size(); i++)
	{
		LineThread TempLine = LineDatas.at(i);
		MakeColorLine(TempLine.LinePt, TempLine.theColor, levelId);
	}
}


void GetVectorFromDb(bvector<Dpoint3d>& VectorPoints, WString type, int proID, int caseID)
{
	/** 定义数据库变量 m_db*/
	Db m_db;
	/** 定义数据库状态变量 m_statment*/
	Statement *m_statment;
	StatusInt rc;
	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		m_statment = new Statement();
		//获取所有Vector坐标
		AString strSql;
		if (type.CompareTo(L"USUM") == 0)
		{
			strSql.append("SELECT UX, UY, UZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"UX") == 0)
		{
			strSql.append("SELECT UX, 0.0, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"UY") == 0)
		{
			strSql.append("SELECT 0.0, UY, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"UZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, UZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"SXY") == 0)
		{
			strSql.append("SELECT 0.0, SXY, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"SYZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, SYZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"SXZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, SXZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOX") == 0)
		{
			strSql.append("SELECT EPTOX, 0.0, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOY") == 0)
		{
			strSql.append("SELECT 0.0, EPTOY, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, EPTOZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOEQV") == 0)
		{
			strSql.append("SELECT EPTOX, EPTOY, EPTOZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOXY") == 0)
		{
			strSql.append("SELECT 0.0, EPTOXY, 0.0 FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOYZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, EPTOYZ FROM RESULTS \n");
		}
		else if (type.CompareTo(L"EPTOEXZ") == 0)
		{
			strSql.append("SELECT 0.0, 0.0, EPTOEXZ FROM RESULTS \n");
		}
		strSql.append("WHERE ProjectId = @ProjectId AND CaseId = @CaseId \n");

		rc = m_statment->Prepare(m_db, strSql.c_str());

		if (rc == BE_SQLITE_OK)
		{
			m_statment->BindInt(1, proID);
			m_statment->BindInt(2, caseID);

			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				GetNodePoints(VectorPoints,m_statment);
			}
		}
		m_statment->Finalize();
	}

	m_db.CloseDb();	
}

void GetNodePoints(bvector<DPoint3d>& vecPoints, Statement *m_statment)
{
	DPoint3d ptTmp;

	ptTmp.x = m_statment->GetValueDouble(0);
	ptTmp.y = m_statment->GetValueDouble(1);
	ptTmp.z = m_statment->GetValueDouble(2);

	vecPoints.push_back(ptTmp);
}

// 取得数值对应的颜色ID
UInt32 GetTheColor(double& dVal, int& dClrNum, DrawColorShape* pDCShape /*= &s_DrawColorShape*/)
{
	UInt32 theColor;

	// 根据色域的设置，查找数值对应的颜色ID
	for (dClrNum = 1; dClrNum < pDCShape->Color_num; dClrNum++)
	{
		if (dVal < pDCShape->ClrShapeVal[dClrNum])
		{
			// 找到后赋值并退出循环
			theColor = pDCShape->fillCororIndex[dClrNum - 1];
			break;
		}
	}

	// 返回颜色ID
	return theColor;
}

LevelId showContourVector(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt /* = NULL*/)
{
	int percent = 70;
	double idex = 0.0;

	//获取basePt和显示层ID
	DPoint3d basePt;
	WString sUnit;
	LevelId  levelId_CaeModel;
	MsdiMsCaeSiUtility::getBasePtAndLevelID(basePt, sUnit, levelId_CaeModel, ngName);
	if (pBasePt != NULL)
		basePt = *pBasePt;

	bstdmap<int, ContourPoint> oCaeMeshNodeMap;
	bvector<double> oCaeResultVec;
	bvector<Dpoint3d> VectorPoints;
	double dVectorLinePerLen; // 矢量线的单位长度

	//根据命名组名获取工程ID
	int proID;
	splitNgName_Model(ngName, UNUSE_WSTRING, proID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//获取实体层名
	LevelId  levelId_CAEModel, levelId_ContourVector;
	mdlLevel_getIdFromName(&levelId_CAEModel, ACTIVEMODEL, LEVEL_NULL_ID, ngName.c_str());

	//通过projectID和caseid以及选择的TYPE类型，获取所有节点值
	GetCaeResult(oCaeResultVec, proID, caseID, type);

	//从数据库中获取节点坐标值
	GetVectorFromDb(VectorPoints, type, proID, caseID);

	if (oCaeResultVec.size() > 0)
	{
		// 创建或取得CAE矢量图的图层
		if (SUCCESS != mdlLevel_create(&levelId_ContourVector, MASTERFILE, L"CAE矢量图", LEVEL_NULL_CODE))
			mdlLevel_getIdFromName(&levelId_ContourVector, MASTERFILE, LEVEL_NULL_ID, L"CAE矢量图");

		size_t NodeNum = 0;
		// 取得CAE的所有Mesh面集合和面节点集合
		if (true == getCaefacePointInfos(ngName, type, proID, caseID, oCaeMeshNodeMap, VectorPoints, basePt, sUnit))
		{
			NodeNum = 0;
			double dTmpVal, dMaxAbsVal = 0.0, dMaxZ = oCaeMeshNodeMap.begin()->second.dPt.z, dMinZ = oCaeMeshNodeMap.begin()->second.dPt.z;
			for (bstdmap<int, ContourPoint>::iterator mapit = oCaeMeshNodeMap.begin(); mapit != oCaeMeshNodeMap.end();mapit++)
			{
				if (NodeNum < mapit->first)
				{
					NodeNum = mapit->first;
				}

				// 取得当前矢量图中最大绝对值的数值
				dTmpVal = fabs(mapit->second.dVal);
				if (dMaxAbsVal < dTmpVal)
				{
					dMaxAbsVal = dTmpVal;
				}

				if (dMinZ > mapit->second.dPt.z)
				{
					dMinZ = mapit->second.dPt.z;
				}
				if (dMaxZ < mapit->second.dPt.z)
				{
					dMaxZ = mapit->second.dPt.z;
				}
			}

			// 取得模型的高度
			Dpoint3d pt1, pt2;
			pt1.Init(0.0, 0.0, dMinZ);
			pt2.Init(0.0, 0.0, dMaxZ);
			// 矢量线的单元长度=(模型的高度/20)/矢量图中最大绝对值的数值
			dVectorLinePerLen = (mdlVec_distance(&pt1, &pt2) / 20) / dMaxAbsVal;

			// 利用多线程技术计算出CAE的所有节点
			bvector<LineThread> VectorDatas1; // 线程1的计算结果
			bvector<LineThread> VectorDatas2; // 线程2的计算结果
			bvector<LineThread> VectorDatas3; // 线程3的计算结果
			bvector<LineThread> VectorDatas4; // 线程4的计算结果
			bvector<LineThread> VectorDatas5; // 线程5的计算结果
			bvector<LineThread> VectorDatas6; // 线程6的计算结果
			bvector<LineThread> VectorDatas7; // 线程7的计算结果
			bvector<LineThread> VectorDatas8; // 线程8的计算结果

			// 线程1(处理量为总Mesh节点数的1/8)
			std::thread t1(ThreadFunc, 0, NodeNum / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas1), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程2(处理量为总Mesh节点数的1/8)
			std::thread t2(ThreadFunc, NodeNum / 8, NodeNum * 2 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas2), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程3(处理量为总Mesh节点数的1/8)
			std::thread t3(ThreadFunc, NodeNum * 2 / 8, NodeNum * 3 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas3), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程4(处理量为总Mesh节点数的1/8)
			std::thread t4(ThreadFunc, NodeNum * 3 / 8, NodeNum * 4 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas4), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程5(处理量为总Mesh节点数的1/8)
			std::thread t5(ThreadFunc, NodeNum * 4 / 8, NodeNum * 5 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas5), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程6(处理量为总Mesh节点数的1/8)
			std::thread t6(ThreadFunc, NodeNum * 5 / 8, NodeNum * 6 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas6), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程7(处理量为总Mesh节点数的1/8)
			std::thread t7(ThreadFunc, NodeNum * 6 / 8, NodeNum * 7 / 8, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas7), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));
			// 线程8(处理量为总Mesh节点数的1/8)
			std::thread t8(ThreadFunc, NodeNum * 7 / 8, NodeNum, oCaeMeshNodeMap, dVectorLinePerLen, ref(VectorDatas8), pDCShape);
			idex = idex + NodeNum / 8;
			mdlDialog_completionBarUpdate(jdoytBar, jdwcMessageText, 15 + (int)(idex / NodeNum*percent));

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
			bvector<double>().swap(oCaeResultVec);
			bstdmap<int, ContourPoint>().swap(oCaeMeshNodeMap);
			bvector<Dpoint3d>().swap(VectorPoints);

			// 绘制矢量图
			DrawContourVecter(VectorDatas1, levelId_ContourVector);
			DrawContourVecter(VectorDatas2, levelId_ContourVector);
			DrawContourVecter(VectorDatas3, levelId_ContourVector);
			DrawContourVecter(VectorDatas4, levelId_ContourVector);
			DrawContourVecter(VectorDatas5, levelId_ContourVector);
			DrawContourVecter(VectorDatas6, levelId_ContourVector);
			DrawContourVecter(VectorDatas7, levelId_ContourVector);
			DrawContourVecter(VectorDatas8, levelId_ContourVector);

			// 释放变量
			bvector<LineThread>().swap(VectorDatas1);
			bvector<LineThread>().swap(VectorDatas2);
			bvector<LineThread>().swap(VectorDatas3);
			bvector<LineThread>().swap(VectorDatas4);
			bvector<LineThread>().swap(VectorDatas5);
			bvector<LineThread>().swap(VectorDatas6);
			bvector<LineThread>().swap(VectorDatas7);
			bvector<LineThread>().swap(VectorDatas8);

			mdlLevel_setDisplay(MASTERFILE, levelId_CAEModel, false);
		}
	}

	return levelId_ContourVector;
}
