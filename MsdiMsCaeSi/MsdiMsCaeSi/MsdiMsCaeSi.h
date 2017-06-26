/*--------------------------------------------------------------------------------------+
|   MsdiMsCaeSi.h
|
+--------------------------------------------------------------------------------------*/
#pragma once

#include <Mstn\MicroStation.r.h>

//TODO: header file
#if !defined (resource)

typedef struct impInfDlgInfo
{
	int		MeshModelVal;
	int		ResultVal;
	int		ProjectId;
} ImpInfDlgInfo;

typedef struct solidDispDlgInfo
{
	WChar	current[256];	//当前工况名
	int		ResultType;		//需要显示的计算结果类型
	int		ShowType;		//显示类型：（云图，等值线图，矢量图）
	int		ShowModel;		//显示模式：（光滑、网格）
	int		bShowExtremum;	//是否显示最大/最小值:0-不显示,-1-显示
	int		indexCType;		//部分展示条件名下标
	int		indexCOpt;		//部分展示条件操作下标
	int		indexCValue;	//部分展示条件值下标
} SolidDispDlgInfo;

typedef struct surfaceShowDlgInfo
{
	WChar	current[256];	//当前工况名
	int		indexReType;	//需要显示的计算结果类型
	int		indexShowType;	//显示类型：（云图，等值线图，矢量图）
	int		indexShowModel;	//显示模式：（光滑、网格）
	int		bShowExtremum;	//是否显示最大/最小值:0-不显示,-1-显示
}SurfaceShowDlgInfo;

typedef struct expCalculationDlgInfo
{
	int		proIndex;		//工程模型comboBox下标
	char	proDescr[4096];	//工程概况
	int		nCellCnt;		//总单元数
	int		nNodeCnt;		//节点数

}ExpCalculationDlgInfo;

typedef struct reBarCalDlgInfo
{
	WChar	strCurrentKey[256];			//当前节点Key
	WChar	rebarName[256];			    //配筋线名称（配筋方向）
	WChar	strCurrentSel[256];			//当前选择
	double  dStruFt;					//结构系数
	double  dStruSignifyFt;				//结构重要系数
	double  dDesignFt;					//设计状况系数
	int		dDefPointSpan;				//默认点距(mm)
	int		iRebarType;					//钢筋种类下标，用于获取fy
	double  dRebarTesion;				//钢筋抗拉强度
	int		iConcrType;					//混凝土种类下标，用于获取ft
	double  dConcrTesion;				//混凝土轴心抗拉强度
	int		dSectionHeight;				//截面高度
	double	dDiameter;					//钢筋直径
	int     iDameterIndex;				//钢筋listbox中第0行1列所用的commbox下标
	int     iRebarCount;				//钢筋根数
	bool    bCalcStress;				//是否计算弯矩、轴力、剪力
} ReBarCalDlgInfo;

typedef struct partialDisDlgInfo
{
	int		indexCName;		//条件名comboBox下标
	int		indexCOpt;		//条件操作符comboBox下标
	int		indexCVal;		//条件值comboBox下标
}PartialDisDlgInfo;

typedef struct caseCompDlgInfo
{
	int		indexInstanL;	//左侧实例comboBox下标
	int		indexInstanR;	//右侧实例comboBox下标
	int		indexReType;	//结果类型comboBox下标
	int		indexShowType;	//显示类型comboBox下标
	int		ShowModel;		// 显示模式
}CaseCompDlgInfo;

typedef struct clipResultDlgInfo
{
	WChar	current[256];
	WChar	tableTitle[256];
	double	titleFont;
	double	contentFont;
	double	heightCoe;
	int		XSpaceCnt;
	int		YSpaceCnt;
	double	coorFont;
	int		XValueCnt;
	int		YValueCnt;
	double	valueFont;
	int     decimal;

	//笛卡尔积包含参数,0代表非选中, -1代表被选中
	int		D_SX;
	int		D_SY;
	int		D_SZ;
	int		D_SC;
	int		D_SXY;
	int		D_SYZ;
	int		D_SXZ;
	int		D_UX;
	int		D_UY;
	int		D_UZ;
	int		D_All;

	//柱坐标包含参数,0代表非选中, -1代表被选中
	int		Z_SR;
	int		Z_SA;
	int		Z_SH;
	int		Z_SRA;
	int		Z_SAH;
	int     Z_SRH;
	int		Z_SC;
	int		Z_UX;
	int		Z_UY;
	int		Z_UZ;
	int		Z_All;

	// 其它画面用参数
	WChar	fileOpenPath[256]; // 文件打开画面的默认路径
}ClipResultDlgInfo;

typedef struct outRebarDlgInfo
{
	//由于该界面控件绑定变量较少，因此把多个界面的绑定变量都定义在此
	int		indexInstance;

	bool     clipPlotNormalSide;
	//clipFace截面名
	WChar	clipName[256];
	int		chooseBtnIndex;
}OutRebarDlgInfo;

typedef struct colorSettingDlgInfo
{
	double	dStartValue;
	double	dEndValue;
	int		iColorNum;
	int		bAutomatic;
	int		Gray;//灰度显示
}ColorSettingDlgInfo;

typedef	struct treexmplinfo
{
	int         containerId;
	void        *pSingleLM;
	void        *pMultiLM;
	void        *pSingleLBRiP;
	void        *pMultiLBRiP;
	void        *pTreeModel;
	int         minSashX;
	int         maxSashX;
	double      range1;
	int         range2;
	double      comboBoxValue;
	int         radioButtonValue;
	double      spinBoxValue;
	//  long        treeComboBoxValue;
	WChar       treeComboBoxValue[256];

	unsigned long   toggle1 : 1;
	unsigned long   toggle2 : 1;
	unsigned long   toggle3 : 1;
	unsigned long   resBits : 29;

} TreeXmplInfo;

typedef struct exportinfo
{
	int             version;
	double          numerator;                      /* Units per meter fraction numerator */
	double          denominator;                    /* Units per meter fraction denominator */
}ExportInfo;

typedef struct instanceInfo
{
	WChar	NgName[256];
	ModelId	modelId;
	int		projectId;
}InstanceInfo;

typedef struct rebarInfo
{
	int		projectID;					//项目ID
	int		caseID;						//工况ID
	int		SectionID;					//截面ModelID
	WChar   NgName[1024];				//截面Model名
	ElementId ElemID;					//元素ID
	WChar   caseName[1024];				//工况名称WChar
	WChar	sectionName[1024];			//截面名称WChar
	WChar	rebarName[1024];			//配筋线名称（配筋方向）WChar
	double	dLineLength;				//配筋线长度
	double  dStruSignifyFt;				//结构重要系数
	double  dDesignFt;					//设计状况系数
	double  dStruFt;					//结构系数
	double  dDefPointSpan;				//默认点距(mm)
	int		iRebarType;					//钢筋种类下标，用于获取fy
	double  dRebarTesion;				//钢筋抗拉强度
	int		iConcrType;					//混凝土种类下标，用于获取ft
	double  dConcrTesion;				//混凝土轴心抗拉强度
	int		dSectionHeight;				//截面高度
	bool    bCalcStress;				//是否计算弯矩、轴力、剪力
	WChar	strStress[2048];			//SY,以&分隔,用于配筋计算

	double  bendingStress;				//弯矩
	double  axialStress;				//轴力
	WChar   shearSXY[2048];				//SXY,以&分隔,用于剪力计算
	WChar	shearSYZ[2048];				//SYZ,以&分隔
	WChar	shearSXZ[2048];				//SXZ,以&分隔

	WChar	strResult[2048];			//计算结果,以&分隔
	int		iRebarCount;				//钢筋根数
	double	dRebarDiameter;				//钢筋直径
	double	dRebarArea;					//钢筋面积
} ReBarInfo;

#endif



