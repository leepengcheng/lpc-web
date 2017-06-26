
class MsdiMsCaeSiUtility
{
public:
	MsdiMsCaeSiUtility(void);
	~MsdiMsCaeSiUtility();
public:
	// 取得CAE模型的显示单位
	static double GetUnit(WString strProjectUnit);

	static void getBasePtAndLevelID(DPoint3dR basePt, WStringR sUnit, LevelId& levelId_CaeModel, WStringCR ngName);

	// 获取截面的转换矩阵(三维截面->二维平面的转换矩阵)
	static void GetSectionTransform(TransformP *tMatrix, WString strNgName);

	//刷新截面列表选中后的文本显示
	static WString MakeSelectedTreeNodeForTxt(GuiTreeNodeP treeNode);

	// 数字格式显示
	static void FormatValDisplay(WStringR strVal, double& dVal, int iDecimal);

	// 坐标数组排序（升序）（SortType：0：x,1:y,2:z）
	static void SortPoints(bvector<DPoint3d>& vecPoints, int SortType);

	static bool CreateText(EditElementHandleR eehText, DgnModelRefP ModelRef, DPoint3d tPos, WString tText, double fwidth, double fheight, double iSlant = 0.0, UInt32 iJustification = -1);

	static bool GetPosition(ContourPoint& ptResult, DPoint3d ptStart, DPoint3d ptEnd, double dStartVal, double dEndVal, double dVal);

	static int PointInPolygon(DPoint3d p, const bvector<PointOutData>& ptPolygon);

	// 判断线段是否包含点p0(p0任意一点)
	static bool PtIsOnline(DPoint3d p0, DPoint3d p1, DPoint3d p2);

	static void fitView(int viewIndex);

	//求两条直线的交点
	static DPoint3d GetTwoLineIntersect(DPoint3d str1, DPoint3d end1, DPoint3d str2, DPoint3d end2);

	//// 有限元形函数计算应力值
	//static double UseBilinear_interpolationGetVlue(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1 = false);//有限元形函数
	// 反距离加权法计算应力值
	static void UseIDW_interpolationGetVlue(PointOutData& mid_Point, const bvector<PointOutData>&  MeshPoints);
private:
	static bool Intersect(DSegment3d L1, DSegment3d L2);
	static double Multiply(DPoint3d p1, DPoint3d p2, DPoint3d p0);

	//static double UseBilinear_interpolationGetVlue_56(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//有限元形函数（5/6个点的情况）

	//static double UseBilinear_interpolationGetVlue_4(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//有限元形函数（4个点的情况）

	//static double UseBilinear_interpolationGetVlue_3(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1);//有限元形函数（3个点的情况）
};