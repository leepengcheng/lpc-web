#pragma once
/**************************************************
* @file     DrawColorShape.h
* @brief    画临时颜色块和区间值的相关类、函数和变量
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*************************************************/
/** 表宽度常量值 TABLE_WIDTH*/
#define TABLE_WIDTH   143
/** 表高度常量值 TABLE_WIDTH*/
#define TABLE_HEIGHT  160*2
/** 颜色块宽度常量值 TABLE_WIDTH*/
#define Color_WIDTH   60*0.4

#define TABLE_OFFSETX  -200  

#define TABLE_OFFSETY   50

#define TABLE_LINCOLOR  0x00000000

#define TABLE_HEADERX   TABLE_WIDTH / 5 
#define TABLE_HEADERY   15 

#define TABLE_UNITY     14

/**
* @brief 画图表综合类，保存了画图所需的相关参数，并调用CreateTable进行画图表
*/
struct DrawColorShape : IViewDecoration
{
public:
	/** 颜色表颜色总数量 ColorNum_Legeng*/
	int ColorNum_Legeng;
	/** 颜色表颜色总数量 Color_num*/
	int Color_num;
	/** 颜色表所有填充颜色序号 fillColorTBGR*/
	UInt32 fillColorTBGR[100];
	/** 颜色表所有填充颜色 fillColor*/
	RgbColorDef  fillColor[100];
	/** rgb颜色对应的颜色序号 fillCororIndex*/
	UInt32    fillCororIndex[100];
	/** 起始区间值 StartValue*/
	double StartValue;
	/** 结束区间值 EndValue*/
	double EndValue;
	/** 起始区间值（DB） StartValue*/
	double DbStartValue;
	/** 结束区间值(DB) EndValue*/
	double DbEndValue;
	/** 所有区间值(图例显示) ClrShapeVal_Legeng*/
	double ClrShapeVal_Legeng[100];
	/** 所有区间值 ClrShapeVal*/
	double ClrShapeVal[100];

	/** 数据库中结果类型Code*/
	WString ResultTypeCode;
	/** 表头 Header*/
	WString Header;
	/** 对应单位 Unit*/
	WString Unit;

	Point2d DrawPos;

	int showType;

	/**
	* @brief 画临时元素函数
	* @param[in] viewport     对应视图指针
	* @return 返回画图成功与否
	*/
	virtual bool _DrawDecoration(IndexedViewportR viewport);

	/** 对应CAE模型ID ProjectId*/
	int ProjectId;

	/** 对应CAE模型单位 ProjectUnit*/
	WString ProjectUnit;

	/** 对应CAE模型名 ProjectName*/
	WString ProjectName;

	/** 对应工况 CaseID*/
	int CaseID;

	/** 对应工况名 CaseName*/
	WString CaseName;

	/** 对应CAE模型的命名组 NgName*/
	WString NgName;
	
	/** 极值标识相关变量 **/
	//是否绘制极值坐标点标识符
	bool m_bExtremum;
	//最大值绘制坐标点
	bvector<DPoint3d> m_vecMaxPts;
	//最小值绘制坐标点
	bvector<DPoint3d> m_vecMinPts;

	/** 节点标识相关变量 **/
	//是否绘制节点信息标识符
	bool m_bNodeView;
	bool m_bNodeView_Temp;
	//绘制节点坐标点-临时点
	DPoint3d m_NodePt_Temp;
	//绘制节点信息-临时点
	double m_dNodeInfo_Temp;
	//绘制节点坐标点-保存点
	vector<DPoint3d> m_vecNodePts;
	//绘制节点信息-保存点
	vector<double> m_vecNodeInfos;

	//标志位，是否进行自动设置，自动设置则根据数据库的数值范围自动进行计算
	bool m_bAutomatic;
	bool m_Gray;//标志位是否设置灰度显示云图
	//是否绘制色带标识符
	bool m_bDisplayed;

	void initialize();

	//开始绘制色带
	void StartDrawFunction();

	//关闭绘制色带
	void CloseDrawFunction();

	void divideColorRange();

	void GetContourLineColorValue(int colornum, double out_value[], double in_value[]);

	void initColorRangeFromDB();
};

/** 定义画颜色块类静态变量 s_DrawColorShape*/
extern DrawColorShape   s_DrawColorShape;
extern DrawColorShape	s_DrawColorShape_temp;
extern DrawColorShape*	s_pDCShape;

/** 创建的颜色表指针 ppPalette*/
extern DgnPlatform::BSIColorPalette*  ppPalette;

/**
* @brief 临时颜色表创建类
*/
class CreateTable
{
public:
	static void DrawTextInShape(DPoint3dCR origin_pos, WString drawtext, RotMatrixCR invRotation, TextStringPropertiesPtr pProp,
		IViewOutputP output, IndexedViewportR viewport);
	
	/**
	* @brief 将区间值画到表中的函数
	* @param[in] HeaderText   表头名称
	* @param[in] ColorNum     颜色总数量
	* @param[in] StartValue   起始数值
	* @param[in] EndValue     终止数值
	* @param[in] output       对应视图画图的指针
	* @param[in] viewport     对应视图指针
	* @return 无返回值
	*/
	static void CreateAllText(WCharCP projectname, WCharCP casename, WCharCP HeaderText, WCharCP UnitText, int ColorNum, double ClrShapeVal_Legeng[],
		IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos);
	/**
	* @brief 将颜色块画到表中的函数
	* @param[in] ColorNum     颜色总数量
	* @param[in] lineColorTBGR   颜色块边缘线的颜色
	* @param[in] fillColorTBGR     所有颜色块颜色数组指针
	* @param[in] output       对应视图画图的指针
	* @param[in] viewport     对应视图指针
	* @return 无返回值
	*/
	static void CreateAllColorTable(int ColorNum, UInt32 lineColorTBGR, UInt32 fillColorTBGR[],
		IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos);

	static double tablewidth;
};

