
#pragma once
#include "BentlyCommonfile.h"
#include "CommonStructFile.h"

class    MoveClipPlot : DgnPrimitiveTool
{
	
	ElementId           CutFaceId;
	bool                IsStartDrag;
	DPoint3d            startPoint;
	DPoint3d            temppoint;
	DPoint3d            endPoint;
	MoveClipPlot(int toolID) : DgnPrimitiveTool(0,toolID)
	{
		IsStartDrag = FALSE;
	}
protected:

	virtual bool WantDynamics()  { return false; }

	virtual bool NeedAcceptPoint()  { return true; }

	virtual bool IsModifyOriginal()  { return false; }

	virtual bool WantAccuSnap() { return false; }

	EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override

	virtual void _OnPostInstall();

	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); /*_ExitTool();*/ return true; }

public:
	virtual bool _OnModelStartDrag(DgnButtonEventCR ev);
	virtual bool _OnModelEndDrag(DgnButtonEventCR ev);

	static void InstallNewInstance(int toolId);
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;
	//void SetupForLocate(int msgId);

	void   _OnRestartTool();
};