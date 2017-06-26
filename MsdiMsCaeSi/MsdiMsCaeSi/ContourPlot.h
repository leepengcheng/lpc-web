#pragma once

void MakeContourPlotDat(bvector<ContourPoint>& vecFacePtAndVal, bvector<PlotThread>& PlotDatas, DrawColorShape* pDCShape);

void DrawContourPlot(bvector<PlotThread>& PlotDatas, DgnPlatform::LevelId levelId);

LevelId showContourPlot(WString ngName, int caseID, WString type, DrawColorShape* pDCShape, DPoint3dCP pBasePt = NULL);
