#ifndef COMMONSTRUCT
#define COMMONSTRUCT
#include "BentlyCommonfile.h"

/*************************
* @brief    云图结构体
*************************/
// 云图节点
typedef struct ContourPoint
{
	int iIdx;			// 面内节点编号
	int NodeId;			// 节点ID
	int NodeType;		// 节点类型
	DPoint3d dPt;		// 节点座标
	double dVal;		// 等值点的值
	DPoint3d dVectorPt;	// 矢量方向，用来画矢量图
} ContourPoint;

struct  PlotThread
{
	bvector<ContourPoint> pMeshFaces;
	UInt32 theColor;
};

struct  LineThread
{
	DSegment3d LinePt;
	UInt32 theColor;
};

struct  BorderLine
{
	DSegment3d LinePt;
	int Count;
};


/*************************
* @brief    切图用结构体
*************************/
typedef struct CutPt
{
	int StartID;			// 起始节点编号
	int EndID;			   // 终点点编号
	double StartRatio;    //距离起始点比例（StartRatio = dis(dpt,startPt)/dis(startPt,EndPt)）
	DPoint3d dPt;		// 节点座标
} CutPt;

struct OutLineData
{
	DPoint3d EleMinPoint;// 截面的左下角坐标
	DPoint3d EleMaxPoint;// 截面的右上角坐标
	DPoint3d OutLineMinPoint;// 数值图外框左下角坐标
	DPoint3d OutLineMaxPoint;// 数值图外框右上角坐标
};

struct PointOutData
{
	DPoint3d dPt;// 节点座标
	bmap<WString, double> ResultVals;
	double dMaxS;// 最大正应力
	double dMinS;// 最小正应力
	double dAngleS;// 最大正应力与X轴的夹角
};

struct  DataListParamas//数值表参数结构
{
	double cordFontSize;//坐标字体大小
	double DataFontSize;//数值字体大小
	double  CasFontSize;//工况字体大小
	double  TitFontSize;//表名字体大小
	int     Decimal; //小数点位数

	WString OutTypeName;//输出类型
	WString ListTitle;//数值表名
	WString TypeName;//值类型名	
	WString CutFaceName;//断面名称
	WString CaseName;//工况名
	WString UnitName;//单位名

	int PerXLen;//X向划分步长
	int PerYLen;//Y向划分步长

	bool isS1;
};

/*************************
* @brief    导入相关结构体
*************************/
/**
* @brief 材料类型
*/
struct MATERIAL
{
	/** 材料ID ID*/
	int ID;

	/** 材料名称 Name*/
	WString Name;

	/** 材料类型 Type*/
	WString Type;

	/** EX EX*/
	double EX;

	/** NUXY NUXY*/
	double NUXY;

	/** DENS DENS*/
	double DENS;

	/** PRXY PRXY*/
	double PRXY;
};

/**
* @brief 单元类型
*/
struct ET
{
	/** 单元类型 Elemtype*/
	int ElemType;

	/** 单元类型名称 ElemTypeName*/
	WString ElemTypeName;
};

/**
* @brief 实常数
*/
struct RLBLOCK
{
	/** 实常数号 RealConstantNumber*/
	int RealConstantNumber;

	/** 实常数属性 Property*/
	WString Property;
};

/**
* @brief 保存节点信息的结构体
*/
struct NBLOCK
{
	/** 节点号 NodeNumber*/
	int NodeNumber;

	/** 节点的面号 FaceNumber*/
	int FaceNumber;

	/** 节点类型 NodeType*/
	int NodeType;

	/** 相对线的节点位置 LineLocation*/
	int LineLocation;

	/** 节点的X坐标 XPoint*/
	double XPoint;

	/** 节点的Y坐标 YPoint*/
	double YPoint;

	/** 节点的Z坐标 YPoint*/
	double ZPoint;
};

/**
* @brief 单元块信息
*/
struct EBLOCK
{
	/** 材料号 MaterialNumber*/
	int MaterialNumber;

	/** 单元类型号 ElemType*/
	int ElemType;

	/** 实常数号 RealConstantNumber*/
	int RealConstantNumber;

	/** 截面号 SectionIDAttrNumber*/
	int SectionIDAttrNumber;

	/** 坐标号 ElemCoordSysNumber*/
	int ElemCoordSysNumber;

	/** 单元生死 BirthDeathFlag*/
	int BirthDeathFlag;

	/** 实体引用号 SolidModelRefNumber*/
	int SolidModelRefNumber;

	/** 单元形状 ElemShapeFlg*/
	int ElemShapeFlg;

	/** 节点数目 NodeCount*/
	int NodeCount;

	/** 保留位 FieldTmp*/
	int FieldTmp;

	/** 单元号 ElemNumber*/
	int ElemNumber;

	/** 节点1 Node1*/
	int Node1;

	/** 节点2 Node2*/
	int Node2;

	/** 节点3 Node3*/
	int Node3;

	/** 节点4 Node4*/
	int Node4;

	/** 节点5 Node5*/
	int Node5;

	/** 节点6 Node6*/
	int Node6;

	/** 节点7 Node7*/
	int Node7;

	/** 节点8 Node8*/
	int Node8;
};

struct BBLOCK
{
	/** 节点号 NodeNumber*/
	int Id;
	WString NodeIdStr;
	int  BCType;
};

/**
* @brief 网格面片信息
*/
struct FACETS
{
	/** 单元号 ElemID*/
	int ElemID;

	/** 单元内的面号 FaceNumber*/
	int FaceNumber;

	/** 面号 FaceID*/
	int FaceID;

	/** 节点1 Node1*/
	int Node1;

	/** 节点2 Node2*/
	int Node2;

	/** 节点3 Node3*/
	int Node3;

	/** 节点3 Node3*/
	int Node4;
};

struct ListBoxDatas
{
	bool needFresh;
	bvector<bvector<WString>> rows;
};

struct CutPoints
{
	int Num;
	int StartID[10];			// 起始节点编号
	int EndID[10];			   // 终点点编号
	double StartRatio[10];    //距离起始点比例（StartRatio = dis(dpt,startPt)/dis(startPt,EndPt)）
};


#endif // !1
