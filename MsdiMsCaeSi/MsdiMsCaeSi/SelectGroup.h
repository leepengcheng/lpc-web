
#pragma once
#include "BentlyCommonfile.h"
#include "CommonStructFile.h"


struct SelectGroup :public  DgnElementSetTool
{

	SelectGroup(int toolID) : DgnElementSetTool(toolID)
	{

	}

	virtual bool WantDynamics()  { return false; }

	virtual bool NeedAcceptPoint()  { return false; }

	virtual bool IsModifyOriginal()  { return false; }

	virtual bool WantAccuSnap() { return false; }


	EditElementHandleP  _BuildLocateAgenda(HitPathCP path, DgnButtonEventCP ev);//override

	StatusInt       _OnElementModify(EditElementHandleR  el) override          { return ERROR; }

	virtual void _OnPostInstall();

	virtual bool _OnResetButton(DgnButtonEventCR ev) override { _OnRestartTool(); /*_ExitTool();*/ return true; }


	virtual bool	_OnModifyComplete(DgnButtonEventCR ev);

	bool          _WantAdditionalLocate(DgnButtonEventCP ev); //override

	static void InstallNewInstance(int toolId);
	//void SetupForLocate(int msgId);

	void   _OnRestartTool();
};

