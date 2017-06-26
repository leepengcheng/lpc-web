#include "MsdiMsCaeSipch.h"
bool MoveClipPlot::_OnModelStartDrag(DgnButtonEventCR ev)
{

	IsStartDrag = TRUE;
	startPoint = *ev.GetPoint();
	_BeginDynamics();
	return false;
}
void MoveClipPlot::_OnDynamicFrame(DgnButtonEventCR ev)
{
	if (IsStartDrag == TRUE)
	{
		temppoint = *ev.GetPoint();

		ElementId elemid = ClipPlotFace::CutFaceId;
		EditElementHandle eeh;
		if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
		{
			EditElementHandle testHandle(eeh.GetElementDescrP(), true, FALSE);
			//将与物体相交的面隐藏

			DPoint3d CutNormal;
			mdlElmdscr_extractNormal(&CutNormal, NULL, testHandle.GetElementDescrP(), NULL);
			
			DVec3d vec1 = DVec3d::FromStartEndNormalize(startPoint, temppoint);
			DVec3d vec2 = DVec3d::From(CutNormal);
			double theDot = 0;
			theDot = vec1.DotProduct(vec2);
			double dis = theDot*(mdlVec_distance(&startPoint,&temppoint));

			CutNormal.x = dis*CutNormal.x;
			CutNormal.y = dis*CutNormal.y;
			CutNormal.z = dis*CutNormal.z;

			Transform       m_transform;
			m_transform.InitFrom(CutNormal);
			TransformInfo tInfo(m_transform);
			testHandle.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(testHandle, tInfo);

			RedrawElems redrawElems;
			redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
			redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
			redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
			redrawElems.DoRedraw(testHandle);
		}
		
	}

}
bool MoveClipPlot::_OnModelEndDrag(DgnButtonEventCR ev)
{
	if (IsStartDrag == TRUE)
	{
		ElementId elemid = ClipPlotFace::CutFaceId;
		EditElementHandle eeh;
		if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
		{
			DPoint3d CutNormal;
			mdlElmdscr_extractNormal(&CutNormal, NULL, eeh.GetElementDescrP(), NULL);

			ElementRefP oldRef = eeh.GetElementRef();
			

			DVec3d vec1 = DVec3d::FromStartEndNormalize(startPoint, temppoint);
			DVec3d vec2 = DVec3d::From(CutNormal);
			double theDot = 0;
			theDot = vec1.DotProduct(vec2);
			double dis = theDot*(mdlVec_distance(&startPoint, &temppoint));

			CutNormal.x = dis*CutNormal.x;
			CutNormal.y = dis*CutNormal.y;
			CutNormal.z = dis*CutNormal.z;

			Transform       m_transform;
			m_transform.InitFrom(CutNormal);
			TransformInfo tInfo(m_transform);
			eeh.GetHandler(MISSING_HANDLER_PERMISSION_Transform).ApplyTransform(eeh, tInfo);
			eeh.ReplaceInModel(oldRef);
			IsStartDrag = FALSE;

			 ClipPlotFace::ClearPlotCutFace();
			 bvector<ElementId> nitsmeshsolids;
			 ClipFace::FliterMeshSolids(eeh.GetElementDescrP(), ClipPlotFace::meshSolids, nitsmeshsolids,s_DrawColorShape.NgName.GetWCharCP());
			 //ClipPlotFace::GetAllCutPlotFace(eeh);
			
		}
	}
	return FALSE;
}
bool	MoveClipPlot::_OnModifyComplete(DgnButtonEventCR ev)
{
	return true;
}
void   MoveClipPlot::_OnRestartTool()//重新启动工具
{
	ElementId elemid = ClipPlotFace::CutFaceId;
	EditElementHandle eeh;
	if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
	{
		//将与物体相交的面隐藏
		ElementRefP oldRef = eeh.GetElementRef();
		mdlElmdscr_setVisible(eeh.GetElementDescrP(), false);
		eeh.ReplaceInModel(oldRef);
	}
	_ExitTool();
}
void MoveClipPlot::_OnPostInstall()
{
	__super::_OnPostInstall();
	mdlAccuDraw_setEnabledState(false); // Don't enable AccuDraw w/Dynamics...
}
bool MoveClipPlot::_OnDataButton(DgnButtonEventCR ev)
{

	return false; // Tool should exit after creating a single line if started in single-shot mode.
}
void MoveClipPlot::InstallNewInstance(int toolId)//工具开始运行安装
{
	MoveClipPlot* tool = new MoveClipPlot(toolId);
	tool->InstallTool();
}