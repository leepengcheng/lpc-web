#pragma once

#include "BentlyCommonfile.h"
//输出位移数值图
extern void OutValueLists(WString ngName, int ProjectID, int caseID, WString sUnit);

extern void CreateOutFrame(DataListParamas dataparams, OutLineData& outLine, DPoint3d orgminpt, DPoint3d orgmaxpt, EditElementHandleR cellHeader, WString sUnit);

extern void CreateTextExt
(
EditElementHandleR cellHeader, 
DPoint3d tPos, 
double fwidth, 
double fheight, 
WString tText, 
DgnModelRefP ModelRef, 
double iSlant, 
UInt32 iJustification
);

// 生成笛卡尔坐标的划分坐标点
extern void CreateCartesianPoints(
	DPoint3d minPos, 
	DPoint3d maxPos, 
	DataListParamas& dataparams,
	bvector<bvector<DPoint3d>>& alloutPoints, 
	bvector<MSElementDescrP>& outLines
	);

extern void CaculateAndDisplayValue
(
bvector<bvector<DPoint3d>>&alloutPoints, 
DataListParamas& dataparams, 
DPoint3d orgminpt, 
DPoint3d orgmaxpt,	
bvector<bvector<ContourPoint>>& MeshPoints,
bvector<bvector<ContourPoint>>& MeshPoints_sx,
bvector<bvector<ContourPoint>>& MeshPoints_sy,
bvector<bvector<ContourPoint>>& MeshPoints_sxy,
EditElementHandleR cellHeader
);

extern void DisplayOutPointValue(
	bvector<bvector<PointOutData>>& outPointAndValues,
	const bvector<bvector<PointOutData>>& vecSectionMeshDats,
	DataListParamas& dataparams,
	DPoint3d orgminpt,
	DPoint3d orgmaxpt,
	EditElementHandleR cellHeader);

extern void CalcOutPointValues
(
bvector<bvector<PointOutData>>&  outPointAndValues,
const bvector<bvector<DPoint3d>>& alloutPoints,
const bvector<bvector<PointOutData>>& vecSectionMeshDats,
TransformCP tMatrix,
RotMatrixCR rotMatrix
);

extern void CreateOutLine(DataListParamas datalistpraramas, OutLineData&  outLine, 
	DPoint3d orgminpt, DPoint3d orgmaxpt, EditElementHandleR cellHeader, WString sUnit);

extern bool PtInRange(DPoint3d p, DPoint3d ptmin, DPoint3d ptmax);

extern bool IsInSameLineAndSetSingleValue//判断是否共线并计算中间点value值
(
PointOutData& mid_Point,
const bvector<PointOutData> vecMeshPoints
);

extern bool PtInPolygon(DPoint3d p, const bvector<PointOutData>& ptPolygon, size_t nCount);

/**
* @brief  读取截面各Mesh元素值参数
* @param[out] vecSectionMeshDat		截面各Mesh元素值参数
* @param[in] allNineDatas			模型各结果类型的CAE计算结果
*/
extern void GetSectionMeshDatas
(
bvector<bvector<PointOutData>>& vecSectionMeshDat,
const bmap<WString, bvector<double>>& allNineDatas
);

extern void GetMeshMinMaxPoints(DPoint3d& minpoint, DPoint3d& maxpoint, WString& ngName);

extern void InitOutValueType(bvector<WString>& outValueTypeWString);

// 创建截面的轮廓线
void CreateFaceOutLine(bvector<DPoint3d>& linePoints, EditElementHandleR cellHeader);

extern WString O_CaseName;
extern WString O_tableTitle;//表头名称
extern double  O_titleFont;//表头字体大小
extern double  O_contentFont;//工况字体大小
extern double  O_heightCoe;//高度比例
extern int O_XSpaceCnt;//X坐标划分个数
extern int O_YSpaceCnt;//Y坐标划分个数
extern double O_coorFont;//坐标数字字体大小
extern int O_XValueCnt;//X方向数值个数
extern int O_YValueCnt;//Y方向间隔长
extern double O_valueFont;//数值字体大小
extern int O_decimal;//小数点位数

extern bvector<int>  outValueTypeint;
extern bvector<WString> outValueTypeWString;
extern DgnModelRefP OutDataModelRef;