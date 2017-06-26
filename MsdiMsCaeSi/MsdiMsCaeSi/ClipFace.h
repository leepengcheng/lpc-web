
#pragma once
#include "BentlyCommonfile.h"
#include "CommonStructFile.h"

//#pragma once

// 包含所有Node值的云图节点
class    ClipFace : DgnPrimitiveTool
{

public:
	int cutType;//切面类型：0,与X轴平行（yz方向切面）；1，与Y轴平行（xz方向平面）；2，与Z轴平行（xy方向平面）；3，任意方向平面（默认）
	DPoint3d m_basePt;
	WString m_Unit;
	double m_minx;
	double m_maxx;
	double m_miny;
	double m_maxy;
	double m_minz;
	double m_maxz;

	//存储与切面有交的实体ID
	bvector<ElementId> meshSolids;
	EditElementHandle   line;
	bvector<DPoint3d>   m_points;
	DPoint3d tPlineOrg;//辅助线起点坐标
	DPoint3d tPlineEnd;//辅助线终点坐标
	DPoint3d prjstrPoint;//面拉伸方向的起点
	DPoint3d prjendPoint;//面拉伸方向的终点
	static bvector<double>   oCaeresultsux;
	static bvector<double>   oCaeresultsuy;
	static bvector<double>   oCaeresultsuz;
	//记录等值线的所有等值点
	static bmap<UInt32, vector<vector<DPoint3d>>> allLinePoints;

	static WString  CutCaseName;//当前被切模型的工况名称
	static Dpoint3d   minPoint;
	static Dpoint3d   maxPoint;
protected:

	ClipFace(int toolId) : DgnPrimitiveTool(0, toolId) { }

	virtual void _OnPostInstall() override;
	virtual void _OnRestartTool() override { _ExitTool(); }
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); return true; }
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

public:
	void CutSolidsByFace(EditElementHandle& CutFace);

	static bool CreateElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);
	static void InstallmyNewInstance(int toolId);

	//筛选实体
	static void FliterMeshSolids(MSElementDescr* CutFace, bvector<ElementId>& meshsolids, bvector<ElementId>& nitsmeshsolids, WCharCP groupName);
	static bool IsinFacesSameSide(DPoint3d CutFacePos, DPoint3d pt1, DPoint3d pt2, DPoint3d CutNormal);

	void GetAllCutResultFace(EditElementHandle& CutFace);

	static void GetCutPointValueByeType(double& cutValue, const bvector<double>& oCaeresults, CutPt cutpt);

	static void MakeColorMesh(bvector<ContourPoint> pMeshFace, UInt32 color, DgnPlatform::LevelId levelId);

	static void MakeColorLine(DSegment3dCR segment, UInt32 color, DgnPlatform::LevelId levelId);

	void MakeDgnModel(DgnModelRefP& modelRef3d, WString modelName);

	static void DrawFacePlot(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults);

	static void ShowByType(WString PlotName, 
		void(*Showtype)(bvector<CutPt>& CutFacePoints, 
		DgnPlatform::LevelId levelId_ContourPlot, 
		int typeID, 
		WString typeName, 
		bvector<double>& oCaeresults), 
		DgnPlatform::LevelId levelId, 
		int typeID, 
		WString typeName,
		WString sUnit);

	static void DrawFaceLine(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults);

	static void DrawFaceVector(bvector<CutPt>& CutFacePoints, DgnPlatform::LevelId levelId_ContourPlot, int typeID, WString typeName, bvector<double>& oCaeresults);

	static bool IsClipFaceModel();//判断当前是否在切面model中

	static void GetMeshPointsAndValue(bvector<ContourPoint>& MeshPoints, EditElementHandle& eeh, bvector<double>& oCaeresults);

	static void CreateTextExt(DPoint3d tPos, double fwidth, double fheight, WString tText, DgnPlatform::LevelId  levelId_OutData = 10000, DgnModelRefP ModelRef = NULL, double iSlant = 0.0, UInt32 iJustification = -1);

	static bool GetMeshNodeIntersectwithcutfacePoints(const EditElementHandleP eh, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace);
	static bool GetIntersectPointsbyfacePoints(bvector<bvector<CutPt>>& allfacePoints, bvector<CutPt>& InterSectPoints, EditElementHandle& CutFace);
	static std::string getLinekeys(CutPt& pt1, CutPt& pt2);

	static void GetMeshPointsAndValueForOutDatalist(bvector<PointOutData>& MeshPoints, const bmap<WString, bvector<double>>& allNineDatas, EditElementHandleCR eeh);

	//创建面 bTempClipFace: true临时层动态显示,false创建截面完成操作 faceDscrP: output
	bool CreateClipFace(DgnButtonEventCR ev, bool bTempClipFace);

	static void InterSectPoint_PushBack(bvector<CutPt>& InterSectPoints, CutPt intesectPoint);
};

void ClipFaceCmd(WCharCP unparsed);

