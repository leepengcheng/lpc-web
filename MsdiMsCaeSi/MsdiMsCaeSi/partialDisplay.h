#pragma once

void showMeshsOfInstance(int iProjectID, WString NGName, bvector<ElementId> meshIDs, bool traversalAll);

void showAllOfInstance(int iProjectID, WString NGName);

void deleteCAEInstance(WString NGName);

struct partialDisplayTool : public DgnElementSetTool
{
	partialDisplayTool(int toolID) : DgnElementSetTool(toolID){}

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt	_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		mdlAccuSnap_enableLocate(TRUE);
		mdlAccuDraw_setEnabledState(FALSE);

		__super::_OnPostInstall();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnResetButton(DgnButtonEventCR ev) override 
	{ 
		_ExitTool();
		return true;
	}
	virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId)
	{
		partialDisplayTool* tool = new partialDisplayTool(toolId);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId()); }
};

struct fenceDisplayTool : public DgnElementSetTool
{
	fenceDisplayTool(int toolID) : DgnElementSetTool(toolID){}

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual UsesDragSelect  _AllowDragSelect() { return USES_DRAGSELECT_Box; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt	_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		mdlAccuSnap_enableLocate(TRUE);
		mdlAccuDraw_setEnabledState(FALSE);

		__super::_OnPostInstall();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnInstall() override 
	{
		SetCmdName(GetToolId(), PROMPT_Fence_SelectInstance);
		return __super::_OnInstall();
	}

	virtual bool _OnResetButton(DgnButtonEventCR ev) override
	{
		_ExitTool();
		return true;
	}
	virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId)
	{
		fenceDisplayTool* tool = new fenceDisplayTool(toolId);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId()); }
};

struct recoverDisplayTool : public DgnElementSetTool
{
	recoverDisplayTool(int toolID) : DgnElementSetTool(toolID){}

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt	_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		mdlAccuSnap_enableLocate(TRUE);
		mdlAccuDraw_setEnabledState(FALSE);

		__super::_OnPostInstall();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnInstall() override
	{
		SetCmdName(GetToolId(), PROMPT_Recover_SelectInstance);
		return __super::_OnInstall();
	}

	virtual bool _OnResetButton(DgnButtonEventCR ev) override
	{
		_ExitTool();
		return true;
	}
	virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId)
	{
		recoverDisplayTool* tool = new recoverDisplayTool(toolId);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId()); }
};

struct deleteDisplayTool : public DgnElementSetTool
{
	deleteDisplayTool(int toolID) : DgnElementSetTool(toolID){}

	virtual bool _WantDynamics() override { return false; }

	virtual bool _NeedAcceptPoint() override { return false; }

	virtual bool _WantAdditionalLocate(DgnButtonEventCP ev) override { return false; }

	virtual StatusInt	_OnElementModify(EditElementHandleR  el) override { return ERROR; }

	virtual void _OnPostInstall() override
	{
		mdlAccuSnap_enableLocate(TRUE);
		mdlAccuDraw_setEnabledState(FALSE);

		__super::_OnPostInstall();
		__super::_SetLocateCursor(true);
	}

	virtual bool _OnInstall() override
	{
		SetCmdName(GetToolId(), PROMPT_Recover_SelectInstance);
		return __super::_OnInstall();
	}

	virtual bool _OnResetButton(DgnButtonEventCR ev) override
	{
		_ExitTool();
		return true;
	}
	virtual bool _OnModifyComplete(DgnButtonEventCR ev) override;

	static void InstallNewInstance(int toolId)
	{
		deleteDisplayTool* tool = new deleteDisplayTool(toolId);
		tool->InstallTool();
	}

	virtual void _OnRestartTool() override{ InstallNewInstance(GetToolId()); }
};

/*----------------------------------------------------------------------+
* The command entry point for user function.				            *
* @param        unparsed     unused unparsed argument to the command    *
+----------------------------------------------------------------------*/
void ShowCAESolidPartCmd(WCharCP unparsed);

void fenceDisplayCmd(WCharCP unparsed);

void recoverDisplayCmd(WCharCP unparsed);

void deleteDisplayCmd(WCharCP unparsed);