#pragma once
bool GetCaeMeshNodeInfo(bstdmap<int, ContourPoint>& CaeMeshNodeMap, double& dMeshDistance, WString ngName, bvector<double> oCaeResultVec,
							bvector<Dpoint3d> VectorPoints, UInt32& NodeNum);

bool getCaefacePointInfos(WString ngName, WString type, int iProID, int caseID, bstdmap<int, ContourPoint>& CaeMeshNodeMap, bvector<Dpoint3d> VectorPoints, DPoint3dCR basePt, WStringCR sUnit);

void DrawContourVecter(bvector<LineThread>& LineDatas, DgnPlatform::LevelId levelId);

void MakeContourVectorDat(ContourPoint oVertorPt, double& dMeshDistance, bvector<LineThread>& VertorDatas, DrawColorShape* pDCShape);

UInt32 GetTheColor(double& dVecterLen, int& dClrNum, DrawColorShape* pDCShape = &s_DrawColorShape);

void GetVectorFromDb(bvector<Dpoint3d>& VectorPoints, WString type, int proID, int caseID);

void GetNodePoints(bvector<DPoint3d>& vecPoints, Statement *m_statment);

LevelId showContourVector(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt = NULL);