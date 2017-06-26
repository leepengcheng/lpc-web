/**
* @file  CreateCAESolid.h
* @brief  关于CAE实体创建相关类和函数
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/
/**
* @brief 用于创建CAE模型的类
*/
struct CreateCAESolidTool : public DgnPrimitiveTool
{
	const double SQL_ELEMID_IDX = 0;
	const double SQL_MATERIAL_IDX = 1;
	const double SQL_ELEMTYPE_IDX = 2;
	const double SQL_REAL_IDX = 3;
	const double SQL_NODECOUNT_IDX = 4;
	const double SQL_NODE_IDX = 5;
protected:
	/**
	* @brief 构造函数
	* @param[in] toolName 工具名称
	* @return 无返回值
	*/
	CreateCAESolidTool(int toolName, int toolPrompt, int iProjectID, WString sProjectUnit) : DgnPrimitiveTool(toolName, toolPrompt)
	{
		// CAE模型ID
		m_ProjectID = iProjectID;
		// CAE模型的单位
		m_ProjectUnit = sProjectUnit;
	}

	/**
	* @brief 工具继承函数
	* @return 无返回值
	*/
	virtual void _OnPostInstall() override;

	/**
	* @brief 工具重启函数
	* @return 无返回值
	*/
	virtual void _OnRestartTool() override { InstallNewInstance(GetToolId(), GetToolPrompt(), m_ProjectID, m_ProjectUnit); }

	/**
	* @brief 鼠标点击后的响应函数
	* @return 无返回值
	*/
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;

	/**
	* @brief 工具重启函数
	*/
	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _ExitTool(); return true; }

	/**
	* @brief 自动定位函数
	*/
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	/**
	* @brief 创建CAE模型函数
	*/
	bool CreateElement(DPoint3dCP basePt);

	void SetupAndPromptForNextAction();
public:
	/**
	* @brief 工具启动函数
	*/
	static void InstallNewInstance(int toolId, int toolPrompt, int iProjectID, WString sProjectUnit);

private:
	RedrawElems m_tempElems;
	/** 定义数据库变量 m_db*/
	Db m_db;

	/** 定义保存数据点 m_savept*/
	Savepoint *m_savept;

	/** 定义数据库状态变量 m_statment*/
	Statement *m_statment;

	/** 定义CAE网格工程ID m_ProjectID*/
	int m_ProjectID;

	/** 定义CAE模型的单位 m_ProjectUnit*/
	WString m_ProjectUnit;

	/**
	* @brief 从数据库中获取表ELEMENTS所有node数据
	* @return 获取到的数据库结果
	*/
	DbResult GetSolidFromDatabase(void);

	/**
	* @brief 由数据库状态变量获取对应序号的位置坐标
	* @param[in] iIdx node序号
	* @return 序号对应的点坐标
	*/
	DPoint3d GetPointFromStatment(DPoint3dCP basePt, int iIdx);

	WString MakeNodeIdKey(int& NodeId1, int& NodeId2, int& NodeId3, int& NodeId4);

	bool SaveFaceInfToDb(bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID);
	bool MakeFaceDataSql(AString& FaceDataSql, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeIds, WString MSLevelName, int iElemID);

	/**
	* @brief  取得CAE的Mesh体的节点和节点坐标集合
	* @param[out] vecIndices		Mesh体的节点集合
	* @param[out] vecPoints			Mesh体的节点坐标集合
	* @param[out] oFaceNodeStrMap	Mesh体各面的节点字符串集合(如：1-2-3-4)
	* @param[in]  vecNodeId			nodeID集合
	* @param[in]  basePt			CAE模型的放置坐标点(相对于原点的偏移值）
	* @return 无返回值
	*/
	void MakeMeshIndices(bvector<int>& vecIndices, bvector<DPoint3d>& vecPoints, bvector<WString>& vecFaceNodeStr, bvector<int>& vecNodeId, DPoint3dCP basePt);
	void MakeFaceIndices(bvector<int>& vecIndices, bvector<int>& vecNodeId, bvector<WString>& vecFaceNodeStr, bvector<int> vecNodes, int node1, int node2, int node3, int node4);

	//获取CAE高亮单元体信息的sql语句
	StatusInt GetHiliteSolidSQL();

	//创建高亮的mesh面
	void BuildHiliteMeshes(const DPoint3dCP& basePt, DgnPlatform::LevelId  levelId_CaeModel);
};