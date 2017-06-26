/**
* @file     DrawVectorGraph.h
* @brief    画矢量图
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
#pragma once
#include "MsdiMsCaeSipch.h"
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_SQLITE
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
/**
* @brief 临时存取NodeID结构体
*/
struct MeshSurfaceValue
{
	double NodeValue[30];
	int nodeCount;
};

/**
* @brief 临时存取NodeID结构体
*/
struct MeshProperty
{
	int ID[30];
	int ElemID;
	int MaterialNumber; // 材料类型
	int ElemType;		// 单元类型
	int RealConstantNumber;// 实常数
};
/**
* @brief 画矢量图类
*/
class DrawVectorGraph : public IViewTransients
{
public:
	/**
	* @brief 画矢量图函数
	* @param[in] isPreUpdate   是否提前刷新
	* @param[in] viewport     对应视图指针
	* @return 无返回值
	*/
	virtual void     _DrawTransients(ViewContextR context, bool isPreUpdate);

	/**
	* @brief 获取所有mesh顶点坐标和顶点坐标对应的相应应力方向
	* @return 无返回值
	*/
	void GetAllPoints();

	/**
	* @brief 从数据库中获取所有节点坐标
	* @return 获取到的节点坐标数据库结果
	*/
	DbResult GetNodeFromDatabase(void);

	/**
	* @brief 从数据库中获取所有节点对应应力方向坐标
	* @return 获取到的方向坐标数据库结果
	*/
	DbResult GetVectorFromDatabase(void);

	/** 节点坐标集合 NodePoints*/
	bvector<Dpoint3d> NodePoints;

	/** 应力坐标集合 VectorPoints*/
	bvector<Dpoint3d> VectorPoints;

	/**
	* @brief  通过view画临时线段函数（矢量箭头是由各个线段组成）
	* @param[in] iview_draw   视图画图相关指针
	* @param[in] points       线段起点和终点数组
	* @param[in] num          点的个数（默认为2）
	* @return 无返回值
	*/
	void DrawLine(IViewDrawR iview_draw, DPoint3d* points, int num);
private:
	/** 定义数据库变量 m_db*/
	Db m_db;
	/** 定义数据库状态变量 m_statment*/
	Statement *m_statment;

	/**
	* @brief  由获取到的节点数据库结果得到对应的节点坐标值
	* @param[out] vecPoints   节点坐标集合
	* @return 无返回值
	*/
	void GetNodePoints(bvector<DPoint3d>& vecPoints);

	/**
	* @brief  由对应序号从数据库结果中取出对应坐标值
	* @param[out] iIdx   序号值
	* @return 坐标值
	*/
	DPoint3d GetPointFromStatment(int iIdx);

	/**
	* @brief  由应力值从颜色表中获取此应力对应的颜色值
	* @param[in] theValue 应力数值
	* @return 颜色表中序号
	*/
	UInt32 GetTheColor(double theValue);
};


