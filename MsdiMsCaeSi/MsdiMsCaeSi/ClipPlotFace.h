

#pragma once
#include "BentlyCommonfile.h"
#include "CommonStructFile.h"
class    ClipPlotFace : DgnPrimitiveTool
{
public:
	static bvector<ElementId>  meshSolids;
	static bvector<ElementId> NitsmeshSolids;
	EditElementHandle   line;
	bvector<DPoint3d>   m_points;
	bool                IsHaveCutFace;
	static ElementId    CutFaceId;
	bool                IsStartDrag;
	DPoint3d tPlineOrg;//辅助线起点坐标
	DPoint3d tPlineEnd;//辅助线终点坐标
	DPoint3d prjstrPoint;//面拉伸方向的起点
	DPoint3d prjendPoint;//面拉伸方向的终点
protected:
	bool IsInCutFaceVectorSide(MSElementDescrP edp, DPoint3dCR CutNormal, DPoint3dCR CutFacePos);

	ClipPlotFace(int toolId) : DgnPrimitiveTool(0, toolId) { IsHaveCutFace = false; IsStartDrag = false; }

	virtual void _OnPostInstall() override;
	virtual void _OnRestartTool() override { _ExitTool(); }
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); return true; }
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	bool CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);
public:
	static void InstallmyNewInstance(int toolId);

	void GetAllCutResultFace(EditElementHandle& CutFace, bool isWriteMesh, bool isShowYt);

	static void MakeColorMesh(bvector<ContourPoint> pMeshFace, UInt32 color, DgnPlatform::LevelId levelId);

	void GetAllCutPlotFace(EditElementHandle& CutFace);

	static StatusInt trimSurfaceBySurface(EditElementHandle& outEdPP, EditElementHandle& trimEdP, EditElementHandle& withEdP, bool bKeep, StatusInt HALF, int isInNormalSide);

	static void ShowHidePlot();

	static void ClearPlotCutFace();

	static void DrawTempVector(DPoint3d strPos, DPoint3d endPos, UInt32 tColor, RedrawElems& redrawElems);
};

void ClipPlotFaceCmd(WCharCP unparsed);