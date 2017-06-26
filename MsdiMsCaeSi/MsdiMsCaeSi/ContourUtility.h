#pragma once

bool GetCaeResult(bvector<double>& oCaeResultVec, int sProjectId, int sCaseId, WString sColNmae);

void MyPushback(bvector<ContourPoint> & vecFace, Statement *Statement, DPoint3d basePt, WString sUnit);

bool GetFaceNodePtAndVal(bvector<bvector<ContourPoint>>& vecCaeMeshPtVal, bvector<ContourPoint>& vecFacePtAndVal);

bool GetPosition(ContourPoint& ptResult, DPoint3d ptStart, DPoint3d ptEnd, double dStartVal, double dEndVal, double dVal);

ContourPoint GetFaceCenNode(bvector<ContourPoint>& pFace);

bool getCaeFaceInfos(WString ngName, WString type, int caseID, bvector<bvector<ContourPoint>>& vecFaces, DPoint3d basePt);

void ClearContour();

void ShowCAEModel();

void focusOnCAEModel(WString ngName);

bool GetOutDataCaeResult(PointOutData& oPointDat, TransformCP tMatrix, RotMatrixCR rotMatrix);