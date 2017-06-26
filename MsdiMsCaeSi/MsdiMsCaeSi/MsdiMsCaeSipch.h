#pragma once
#include    "BentlyCommonfile.h"
#include    "CommonStructFile.h"
#include	"MsdiMsCaeSiids.h"
#include	"MsdiMsCaeSiCmd.h"
#include	"transkit\MsdiMsCaeSitxt.h"
#include	"MsdiMsCaeSi.h"

#include	"pugixml\pugixml.hpp"
#include	"pugixml\pugiconfig.hpp"

#include	"ImportCAEInf.h"
#include	"CreateCAESolid.h"
#include    "DrawVectorGraph.h"
#include    "DrawColorShape.h"
#include    "ContourUtility.h"
#include    "ContourVector.h"
#include    "ClipFace.h"

#include    "exportTool.h"
#include    "log.h"
#include	"ImportCaeInfDlgHook.h"
#include	"SolidContourDispDlgHook.h"
#include	"SolidMeshShowDlgHook.h"
#include	"ColorandValueSetDlgHook.h"

#include    "ContourLine.h"
#include    "ContourPlot.h"
#include    "ClipPlotFace.h"
#include    "MoveClipPlot.h"
#include    "OutColumnData.h"
#include    "OutDataList.h"

#include    "SelectGroup.h"

#include	"nodeViewer.h"
#include	"partialDisplay.h"
#include	"dbFunction.h"
#include    "calculateRebar.h"
#include    "ExportCalcDlgHook.h"
#include	"partialDisplayDlgHook.h"
#include	"caseCompareDlgHook.h"
#include	"clipResultShowDlgHook.h"
#include	"surfaceShowDlgHook.h"
#include	"outputRebarDlgHook.h"
#include    "ContourOutFaceLine.h"
#include	"MsdiMsCaeSiUtility.h"

#define UOR_PER_METER	(mdlModelRef_getUorPerMeter(ACTIVEMODEL))
#define UOR_PER_MM		(mdlModelRef_getUorPerMeter(ACTIVEMODEL)/1000)
#define UOR_PER_MASTER	(mdlModelRef_getUorPerMaster(ACTIVEMODEL))
#define UOR_PER_UNIT(x)	MsdiMsCaeSiUtility::GetUnit(x)

#define Tolerance (1e-4)
#define INFINIT  (1e10)

//命名组名称
#define NGNAME_PREFIX_CAEMODEL L"CAEMODEL"
#define NGNAME_PREFIX_CAEMODEL_DOLLAR L"CAEMODEL$"
#define NGNAME_PREFIX_CAECLIP L"CAECLIP"
#define NGNAME_PREFIX_CAECLIP_DOLLAR L"CAECLIP$"

//分割符
#define SeparatorDollar L"$"
#define SeparatorAnd L"&"

#define XAttrHandleID_Major		800		//XAttributionHandle的MajorID
#define XAttrHandleID_Minor		900		//XAttributionHandle的MinorID
#define XAttrHandleID_Matrix    1000	//XAttributionHandle的Matirx

#define XAttrID_RebarResults	125		//配筋结果XAttribution的ID
#define	XAttrID_InstanceInfo	225		//所有CAE模型实例XAttribution的ID
#define XAttrID_MeshProperty	1		//单元体或单元面属性XAttribution的ID
#define XAttrID_PlotMeshProperty 20		//单元体或单元面属性XAttribution的ID
#define XAttrID_CutSurfaceMeshProperty 22	//单元体或单元面属性XAttribution的ID
#define XAttrID_CutFace			10		//切图节点数据XAttribution的ID
#define	XAttrID_DBFlag			333		//数据库标识符XAttribution的ID

#define XAttrID_Matirx	 666		//XAttributionHandle的MajorID


#define MAX_COLUMN_COUNT 10
#define MAX_COLUMN_COUNT_REBAR 17

#define DB_NAME L"MsdiMsCaeSi.db"
#define DOCUMENT_NAME L"CAE计算文档模板.docx"

// CAE计算结果类型
#define RESULTTYPE_USUM L"USUM"
#define RESULTTYPE_UX L"UX"
#define RESULTTYPE_UY L"UY"
#define RESULTTYPE_UZ L"UZ"
#define RESULTTYPE_S1 L"S1"
#define RESULTTYPE_S2 L"S2"
#define RESULTTYPE_S3 L"S3"
#define RESULTTYPE_SX L"SX"
#define RESULTTYPE_SY L"SY"
#define RESULTTYPE_SZ L"SZ"
#define RESULTTYPE_EPTOEQV L"EPTOEQV"
#define RESULTTYPE_EPTOX L"EPTOX"
#define RESULTTYPE_EPTOY L"EPTOY"
#define RESULTTYPE_EPTOZ L"EPTOZ"
#define RESULTTYPE_SEQV L"SEQV"
#define RESULTTYPE_EPTOXY L"EPTOXY"
#define RESULTTYPE_EPTOYZ L"EPTOYZ"
#define RESULTTYPE_EPTOXZ L"EPTOXZ"
#define RESULTTYPE_SXY L"SXY"
#define RESULTTYPE_SYZ L"SYZ"
#define RESULTTYPE_SXZ L"SXZ"

// 配筋计算结果= 最大有效点号&0应力点号&0应力对应位置&总拉应力面积&混凝土强度对应点号&混凝土强度对应位置&混凝土拉应力面积&配筋面积&参考配筋面积
const int CALCRESULT_IDX_MAXTVALIDNO = 0;//最大有效点号
const int CALCRESULT_IDX_ZEROTNO = 1;//0应力点号
const int CALCRESULT_IDX_ZEROP = 2;//0应力对应位置
const int CALCRESULT_IDX_A = 3;//总拉应力面积
const int CALCRESULT_IDX_CONCRNO = 4;//混凝土强度对应点号
const int CALCRESULT_IDX_CONCRP = 5;//混凝土强度对应位置
const int CALCRESULT_IDX_ACT = 6;//混凝土拉应力面积
const int CALCRESULT_IDX_AS = 7;//配筋面积
const int CALCRESULT_IDX_REFERAREA = 8;//参考配筋
const int CALCRESULT_IDX_BENDING = 9;//弯矩
const int CALCRESULT_IDX_AXIAL = 10; //轴力
const int CALCRESULT_IDX_XYSHEAR = 11;//XY方向剪力
const int CALCRESULT_IDX_YZSHEAR = 12;//YZ方向剪力
const int CALCRESULT_IDX_XZSHEAR = 13;//XZ方向剪力

#pragma warning(disable:4189)

