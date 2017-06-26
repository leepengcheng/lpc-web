/**
* @file  OutColumnData.h
* @brief  主要功能用来输出柱坐标数值图
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
#include   "ClipFace.h"

/**
* @brief 用于输出柱坐标数值图的类
*/
class    OutColumnData : DgnPrimitiveTool
{
public:
	/** 截面的CAE模型ID m_ProjectID；*/
	int m_ProjectID;

	/** 截面的CAE模型单位 m_Unit；*/
	WString m_Unit;

	/** 截面的工况ID m_CaseID；*/
	int m_CaseID;

	/** 截面的命名组 m_ngName；*/
	WString m_ngName;

	/** 用来记录鼠标点击坐标点 m_points*/
	bvector<DPoint3d>   m_points;

	/** 原切面最小值坐标点 orgMinPoint*/
	DPoint3d orgMinPoint;
	/** 原切面最大值坐标点 orgMaxPoint*/
	DPoint3d orgMaxPoint;

	/** 用来记录那些数值类型是否会输出（输出的为TURE，不输出的为false按顺序存储） outColumnTypeint*/
	bvector<int> outColumnTypeint;

	/** 用来记录输出的数值类型（字符串格式） outColumnType*/
	bvector<WString> outColumnType;

	/** 用来记录输出的数值类型（字符串格式） outColumnTypeName*/
	bvector<WString> outColumnTypeName;

protected:

	OutColumnData(int toolId) : DgnPrimitiveTool(0, toolId) {}

	virtual void _OnPostInstall() override;
	virtual void _OnRestartTool() override { _ExitTool(); }
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); return true; }
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	void SetupAndPromptForNextAction();
	bool CreateLineElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);
	bool CreateArcElement(EditElementHandleR eeh, bvector<DPoint3d> const& points);

	/**
	* @brief 初始化要输出柱坐标数值图的类型
	* @param[out] outColumnTypeWString    所有要输出的柱坐标数值图的类型集合
	* @return 无返回值
	*/
	void InitColumnValueType();
public:
	static void InstallmyNewInstance(int toolId, WString ngName, int ProjectID, int caseID, WString sUnit);

	/**
	* @brief 获取中点的字符串，连接方式XYZ
	* @param[in] str     起点坐标
	* @param[in] end     终点坐标
	* @return 返回中点坐标（以字符串的形式）
	*/
	static WString GetMidValueString(DPoint3d str, DPoint3d end);

	/**
	* @brief 计算切面MESH的外轮廓线,并将外轮廓线用临时元素的方式画出来
	* @param[out] outLines   外轮廓线DESCR指针输出集合
	* @param[out] minPoint     mesh面的最小坐标位置
	* @param[out] maxPoint     mesh面的最大坐标位置
	* @param[in] NgName		   mesh面所属的组名
	* @return 无返回值
	*/
	static void GetOutLineDsc(bvector<MSElementDescrP>& outLines, DPoint3d& minPoint, DPoint3d& maxPoint, WString NgName);

	/**
	* @brief 计算划分圆弧，并将划分的圆弧点输出
	* @param[out] arcPoints   输出划分好的点坐标
	* @param[in] arcDes     输入划分直线
	* @param[int] dfNum     输入要划分的点个数
	* @return 无返回值
	*/
	void GetArcPoints(bvector<DPoint3d>& arcPoints, MSElementDescrP arcDes, double dfNum);

	/**
	* @brief 计算划分圆弧点和圆心点的连线，与所有外轮廓线的交点
	* @param[out] IntersectPoints   计算得到的交点
	* @param[in] CenterPoint     圆心点
	* @param[in] desPoint        划分的圆弧点
	* @param[in] outLines        外轮廓线DESCR指针输出集合
	* @return 无返回值
	*/
	void GetAllIntersecPoints(bvector<DPoint3d>& IntersectPoints, DPoint3d CenterPoint, DPoint3d desPoint, bvector<MSElementDescrP>& outLines);
	
	/**
	* @brief 画数值图的外方框
	* @param[in] outLines   外轮廓线DESCR指针输出集合
	* @param[in] dis_x      输出数值图时偏移量，相对于原来的切面图位置
	* @param[in] levelId_OutData     数值图输出的图层
	* @return 无返回值
	*/
	bool DrawFaceOutline(bvector<MSElementDescrP>&outLines, EditElementHandleR cellHeader);

	/**
	* @brief 生成辅助圆弧
	* @param[out] tCircle    生成的辅助圆弧
	* @param[in] m_points   保存的所有鼠标点击点
	* @param[in] minpt      切面最小值坐标点
	* @return 返回创建结果，success表示成功
	*/
	//int DrawArc(MSElement& tCircle,bvector<DPoint3d>& m_points, DPoint3d minpt);

	/**
	* @brief 计算所有圆弧划分点与外轮廓所有交点
	* @param[out] allIntersectPoints    得到的所有交点
	* @param[in] mid_point   辅助圆弧的中心点
	* @param[in] outLines    所有外轮廓线段集合
	* @param[in] arcPoints   所有圆弧划分点
	* @return 无返回值
	*/
	void GetInsectPointswithOutLines(bvector<bvector<DPoint3d>>& allIntersectPoints, DPoint3d mid_point, bvector<MSElementDescrP> outLines, bvector<DPoint3d> arcPoints);
	
	/**
	* @brief 由圆弧划分点和外轮廓的交点，求取所有圆柱数值输出坐标
	* @param[out] allOutPoints    所有要输出的数值点坐标
	* @param[in] PerDis      每一段圆弧上等分输出数值点距离
	* @param[in] allIntersectPoints    求得的与外轮廓的所有交点
	* @param[in] mid_point   辅助圆弧的中心点
	* @param[in] outLines    所有外轮廓线段集合
	* @return 无返回值
	*/
	void GetColumnoutPoints(bvector<bvector<DPoint3d>>& allOutPoints, double PerDis,
		bvector<bvector<DPoint3d>> allIntersectPoints, DPoint3d mid_point, bvector<MSElementDescrP>& outLines);
};

void OutColumnValueLists(WString ngName, int ProjectID, int caseID, WString sUnit);