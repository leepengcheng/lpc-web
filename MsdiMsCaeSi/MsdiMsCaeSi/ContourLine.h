#pragma once

void MakeColorLine(DSegment3dCR segment, UInt32 color, DgnPlatform::LevelId levelId);

void DrawContourLine(bvector<LineThread>& LineDatas, DgnPlatform::LevelId levelId);

void MakeContourLineDat(bvector<ContourPoint>& vecFacePtAndVal, bvector<LineThread>& LineDatas, DrawColorShape* pDCShape);

LevelId showContourLine(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt = NULL);

vector<deque<DPoint3d>> connectLineString(vector<vector<DPoint3d>> allPartLineString);

bool point3d_equals(const DPoint3d pt1, const DPoint3d pt2);
void GetDisPointsandTange(bvector<DPoint3d>& arcPoints, bvector<DPoint3d>& tangPoints, MSElementDescrP Des, double dis);
void CreateLineText(DPoint3d tPos, double fwidth, double fheight, WString tText, DgnPlatform::LevelId levelId_OutData, DgnModelRefP ModelRef, RotMatrix rot);