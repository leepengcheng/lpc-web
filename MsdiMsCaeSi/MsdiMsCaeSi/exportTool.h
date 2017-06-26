/**
* @file     exportTool.h
* @brief    将ms模型导出到parasolid格式文件
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/


/**
* @brief parasolid文件导出类
*/
struct exportTool :public  DgnElementSetTool
{
private:
	/** 存取导出物体的中心位置 thePoint*/
	static DPoint3d thePoint;
protected:

	/**
	* @brief  构造函数
	*/
	exportTool(int toolID) : DgnElementSetTool(toolID)
	{

	}
	virtual UsesDragSelect  _AllowDragSelect() { return USES_DRAGSELECT_Box; }

	virtual bool WantDynamics()  { return false; }

	virtual bool NeedAcceptPoint()  { return true; }

	virtual bool IsModifyOriginal()  { return false; }

	virtual bool WantAccuSnap() { return false; }




	EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override


	virtual void _OnPostInstall();

	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _ExitTool(); return true; }


	virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	bool          _WantAdditionalLocate(DgnButtonEventCP ev); //override


	//void SetupForLocate(int msgId);

	void   _OnRestartTool();

	StatusInt       _OnElementModify(EditElementHandleR  el) override          { return ERROR; }
	///**
	//* @brief  重启工具按钮
	//* @param[in] ev   按钮指针
	//*/
	//bool            _OnResetButton(DgnButtonEventCR ev) override                { _OnRestartTool(); return true; }
	//
	///**
	//* @brief  是否需要添加组别
	//*/
	//bool            _DoGroups() override                                       { return false; }
	//
	///**
	//* @brief  是否需要更多的操作点
	//*/
	//bool            _NeedAcceptPoint() override                                { return true; }
	//
	///**
	//* @brief  是否需要点选物体
	//*/
	//bool            _NeedPointForSelection() override                          { return true; }
	//
	///**
	//* @brief  改变元素函数
	//*/
	//StatusInt       _OnElementModify(EditElementHandleR  el) override          { return ERROR; }

	//virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	///**
	//* @brief  是否允许选择物体
	//*/
	//virtual UsesSelection   _AllowSelection() { return USES_SS_Required; }
	///**
	//* @brief  是否允许框选物体
	//*/
	//virtual UsesDragSelect  _AllowDragSelect() { return USES_DRAGSELECT_Box; }
	///**
	//* @brief  筛选处理函数
	//*/
	//void     SetupForLocate();

	///**
	//* @brief  鼠标左键响应函数
	//*/
	//bool            _OnDataButton(DgnButtonEventCR ev);
	///**
	//* @brief  功能键按下或者弹起时
	//*/
	//bool            _OnModifierKeyTransition(bool wentDown, int key); //override
	//
	///**
	//* @brief  是否需要选择更多的物体
	//*/
	//bool            _WantAdditionalLocate(DgnButtonEventCP ev); //override

	///**
	//* @brief  由点选路径添加选中物体
	//*/
	//   EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override

	///**
	//* @brief  安装工具
	//*/
	//   bool            _OnInstall();
	///**
	//* @brief  重启工具
	//*/
	//void   _OnRestartTool();	

public:
	/**
	* @brief  安装一个新的工具
	*/
	static void InstallNewInstance(int toolId);

	/**
	* @brief  导出选中的物体到Parasolid
	*/
	static void ExportSelect_toParasolid(ElementAgendaR theAgenda);

	/**
	* @brief  导出对话框函数
	* @param[in] xmtVersionP    导出版本号
	* @param[in] masterUnitsP   导出的单位
	* @param[in] fileNameP      导出的路径
	*/
	static int  xmtWrite_fileCreateDialog(int *xmtVersionP, int *masterUnitsP, BeFileNameR  fileNameP);

	/**
	* @brief  由选中的导出单位得到实际参数
	* @param[in] masterUnits        导出的单位
	* @param[out] Lastmasterunits   导出的单位对应使用参数
	*/
	static void GetTheUnit(int masterUnits, DgnPlatform::StandardUnit& Lastmasterunits);

	/**
	* @brief  将单独三维模型添加到entitylist中
	* @param[in] edP        要添加的三维模型
	* @param[in] modelRef   模型所在的model
	* @param[in out] bodyListP   三维模型集合
	* @param[in out] templateListP   三维模型转换为ENTITY的模板矩阵集合
	* @param[in out] transListP      三维模型转换为ENTITY的转换矩阵集合
	* @param[in out] remainingEdP     三维模型转换为ENTITY不成功的所有模型集合
	*/
	static void AllSelectTo_ListFunction(MSElementDescrP edP, DgnModelRefP modelRef,
		TAG_ENTITY_LIST  *bodyListP, NON_ENTITY_LIST *templateListP
		, NON_ENTITY_LIST *transListP, MSElementDescr *remainingEdP);
};

/**
* @brief  ms提供的封装函数，功能是将传入的所有ENTITY以parasolid格式存放到对应路径下
* @param[in] fileName        存放路径
* @param[in] ascii
* @param[in] xmtVersion     转换版本
* @param[in] listP   ENTITY集合
*/
int mdlSolid_saveParts(WCharCP  fileName, int  ascii, int xmtVersion, TAG_ENTITY_LIST *listP);
