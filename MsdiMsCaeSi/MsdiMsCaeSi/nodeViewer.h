#pragma once

struct nodeViewer : public DgnElementSetTool
{
	nodeViewer(int toolID, bool plotModel);

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt _OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		__super::_OnPostInstall();

		mdlAccuSnap_enableLocate(FALSE);
		mdlAccuDraw_setEnabledState(FALSE);

		mdlLocate_allowLocked();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnPostLocate(HitPathCP path, WStringR cantAcceptReason);

	virtual bool _OnDataButton(DgnButtonEventCR ev);

	virtual bool _OnResetButton(DgnButtonEventCR ev) override
	{
		_ExitTool();
		return true;
	}

	virtual void _OnCleanup();

	//virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId, bool plotModel)
	{
		nodeViewer* tool = new nodeViewer(toolId, plotModel);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId(), m_bPlotModel); }

	DPoint3d m_CurNodePt;

	double	 m_dCurNodeInfo;

	//表示加载工具时是否为云图模式，若不是则需要根据所选工况从数据库中读取数据
	bool m_bPlotModel;

	//从数据中读取的指定结果，其下标+1即为对应的nodeID
	vector<double> m_vecResults;
};

void nodeViewerCmd(WCharP unparsed);