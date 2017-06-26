#pragma  once

/**
* @brief 用于创建配筋线并计算配筋面积的工具类
*/
struct ReinforceTool : public DgnElementSetTool
{
protected:
	virtual bool            _IsModifyOriginal() override { return false; }
	virtual StatusInt       _ProcessAgenda(DgnButtonEventCR ev) override { return SUCCESS; }
	virtual UsesFence       _AllowFence() override { return USES_FENCE_None; }
	virtual UsesSelection   _AllowSelection() override { return USES_SS_None; }
	virtual bool            _DoGroups() override { return false; }    // No graphics groups
	virtual ComponentMode   _GetComponentMode() override { return ComponentMode::SharedChild; }
	virtual bool			_NeedAcceptPoint() override { return true; }
	virtual bool			_WantDynamics() override { return false; }
	virtual bool			_WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }
	virtual StatusInt		_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual bool _OnPostLocate(HitPathCP path, WStringR cantAcceptReason);
protected:
	/**
	* @brief 构造函数
	* @param[in] toolName 工具名称
	* @return 无返回值
	*/
	ReinforceTool(int toolName, int toolPrompt, MSDialogP dbP, int proID, int iCaseID, WString sNgName);

	/**
	* @brief 工具继承函数
	* @return bool
	*/
	virtual bool _OnInstall() override;

	/**
	* @brief 工具继承函数
	* @return 无返回值
	*/
	virtual void _OnPostInstall() override;

	/**
	* @brief 工具重启函数
	* @return 无返回值
	*/
	virtual void _OnRestartTool() override;

	/**
	* @brief 鼠标点击后的响应函数
	* @return 无返回值
	*/
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;

	/**
	* @brief 工具重启函数
	*/
	virtual bool _OnResetButton(DgnButtonEventCR ev) override;

	/**
	* @brief 自动定位函数
	*/
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

	virtual void _OnCleanup() override;
	
	void SetupAndPromptForNextAction();

	void InsertTreeNode();
public:
	/**
	* @brief 工具安装函数
	*/
	static void InstallNewInstance(int toolId, int toolPrompt, MSDialogP dbP, int proID, int iCaseID, WString sNgName);

	//初始化XAttribute的结构体rebarInfo
	void InitRebarInfo(rebarInfo& orebarInfo);
private:
	//是否已经选择了起点
	bool	m_bFirstClicked;
	bool	m_bErrorClicked;

	//配筋线的起始点
	DSegment3d m_clickPts;

	//配筋线所在的Z平面值
	double	m_dPlaneZ;

	//本次命令操作的ModelRef
	DgnModelRefP m_modelOpt;

	ReBarInfo orebarInfo;

	MSDialogP m_dbP;

	// 取得所有节点的UX,UY,UZ,SX,SY,SZ,SXY,SXZ,SYZ
	bmap<WString, bvector<double>> allNineDatas;

	// 截面节点信息
	bvector<bvector<PointOutData>> vecSectionMeshDats;
	bvector<ElementId> m_lineList;

	TransformP tMatrix = NULL;

	bool isPointInFace(DPoint3d pt);
	void SmartLook(DPoint3d pt);
};
