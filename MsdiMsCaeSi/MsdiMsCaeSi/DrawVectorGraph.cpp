#include "MsdiMsCaeSipch.h"

void  DrawVectorGraph::_DrawTransients(ViewContextR context, bool isPreUpdate)
{
	if (NULL == context.GetViewport())
	{
		//	Don't complain: there are reasons why a ViewContext may be NULL
		return;
	}

	if (isPreUpdate)
	{
		//	Draw after real elements
		return;
	}

	if (context.CheckStop())
	{
		//	Stop drawing
		return;
	}
	//	DrawPurpose enum in <mstypes.h>
	const	DrawPurpose	drawPurpose = context.GetDrawPurpose();
	//	Uncomment the following to see draw events in the Message Center (enable debug in that window)

	switch (drawPurpose)
	{
		//	FitView doesn't seem to make any difference
	case DrawPurpose::FitView:
		//	When user updates a view
	case DrawPurpose::Update:
		//	When user dynamically pans in a view
	case DrawPurpose::UpdateDynamic:
		//	When something is drawn or erased
	case DrawPurpose::UpdateHealing:
		//	When a transient object (geometry or transient element) is added or removed
	case DrawPurpose::TransientChanged:
	{
		IViewDrawR	output = context.GetIViewDraw();

		NamedGroupCollectionPtr ngcCAESolid;
		WString uniqueName;

		ngcCAESolid = NamedGroupCollection::Create(*ACTIVEMODEL);
		NamedGroupPtr namedGroup = ngcCAESolid->FindByName(L"CAEMODEL");
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);
		// 循环检查是否存在空命名组(命名组中没有元素)
		if (graphicMembers > 0)
		{
			for (UInt32 i = 0; i < graphicMembers; i++)
			{
				ElementId elemid = namedGroup->GetMember(i)->GetElementId();
				EditElementHandle eeh;
				if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))
				{
					//UInt32 color;
					bvector<int>   pIndices; //
					bvector<DPoint3d>  pXYZ;

					int   pNumIndexPerFace;
					int   pNumFace;

					if (SUCCESS == mdlMesh_getPolyfaceArrays(eeh.GetElementDescrCP(), &pIndices, &pXYZ, &pNumIndexPerFace, &pNumFace))
					{
						XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
						XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
						MeshProperty *meshPro = NULL;

						if (xh.IsValid())
						{
							meshPro = (MeshProperty *)xh.PeekData();
						}

						bvector<DPoint3d> vecPoints;

						for (size_t j = 0; j < pXYZ.size(); j++)
						{
							for (size_t i = 0; i < pIndices.size(); i++)
							{
								int k = pIndices.at(i);

								if (k == j + 1)
								{
									int nodeIndex = meshPro->ID[i] - 1;
									vecPoints.push_back(VectorPoints.at(nodeIndex));

									DPoint3d pts[2];
									pts[0] = pXYZ.at(j);
									pts[1] = pts[0];

									DPoint3d tempPont;
									tempPont.x = 0;
									tempPont.y = 0;
									tempPont.z = 0;
									if (mdlVec_distance(&tempPont, &VectorPoints.at(nodeIndex)) == 0)
									{
										continue;
									}
									UInt32 theColor = GetTheColor(mdlVec_distance(&tempPont, &VectorPoints.at(nodeIndex)));
									output.SetSymbology(theColor, 0, 2, 0); // opaque blue (color is TBGR packed int), weight 1...

									DrawLine(output, pts, 2);

									DPoint3d tempPoints[2];
									tempPoints[1] = pts[1];
									tempPoints[0].x = pts[0].x + (pts[1].x - pts[0].x) * 9 / 10;
									tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
									tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;

									tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);

									output.SetSymbology(theColor, 0, 2, 0); // opaque blue (color is TBGR packed int), weight 1...

									DrawLine(output, tempPoints, 2);

									tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
									tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
									DrawLine(output, tempPoints, 2);

									tempPoints[0].y = pts[0].y + (pts[1].y - pts[0].y) * 9 / 10;
									tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 + mdlVec_distance(&tempPoints[0], &tempPoints[1]);
									DrawLine(output, tempPoints, 2);

									tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10;
									tempPoints[0].z = pts[0].z + (pts[1].z - pts[0].z) * 9 / 10 - mdlVec_distance(&tempPoints[0], &tempPoints[1]);
									DrawLine(output, tempPoints, 2);
									break;
								}

							}
						}

					}
				}
			}
		}
		break;
	}
	//	When user needs to pick something
	case DrawPurpose::Pick:
	{
		break;
	}
	}
}
UInt32 DrawVectorGraph::GetTheColor(double theValue)
{
	UInt32 theColor;
	if (theValue < s_pDCShape->StartValue)
	{
		theColor = s_pDCShape->fillColorTBGR[0];
		return theColor;
	}
	else if (theValue > s_pDCShape->EndValue)
	{
		theColor = s_pDCShape->fillColorTBGR[s_pDCShape->ColorNum_Legeng - 1];
		return theColor;
	}
	else
	{
		int theNumber = (theValue - s_pDCShape->StartValue) / ((s_pDCShape->EndValue - s_pDCShape->StartValue) / s_pDCShape->ColorNum_Legeng);
		theColor = s_pDCShape->fillColorTBGR[theNumber];
		return theColor;
	}
}
void DrawVectorGraph::DrawLine(IViewDrawR iview_draw, DPoint3d* points, int num)
{
	iview_draw.DrawLineString3d(num, points, NULL);
}

DbResult DrawVectorGraph::GetNodeFromDatabase(void)
{
	WString strSql;
	CharP cSQL = "";

	strSql.append(L"SELECT NODES.X, NODES.Y, \n");
	strSql.append(L"NODES.Z \n");
	strSql.append(L"FROM NODES \n");

	size_t len = strSql.length() + 1;
	cSQL = (CharP)malloc(len*sizeof(char));
	strSql.ConvertToLocaleChars(cSQL);

	return m_statment->Prepare(m_db, cSQL);
}
DbResult DrawVectorGraph::GetVectorFromDatabase(void)
{
	WString strSql;
	CharP cSQL = "";

	strSql.append(L"SELECT UX, UY, UZ FROM RESULTS \n");
	strSql.append(L" WHERE  ProjectId = @ProjectId AND CaseId = @CaseId \n");
	size_t len = strSql.length() + 1;
	cSQL = (CharP)malloc(len*sizeof(char));
	strSql.ConvertToLocaleChars(cSQL);
	DbResult theState = m_statment->Prepare(m_db, cSQL);

	m_statment->BindInt(1, s_pDCShape->ProjectId);
	m_statment->BindInt(2, s_pDCShape->CaseID);

	return theState;
}
void DrawVectorGraph::GetNodePoints(bvector<DPoint3d>& vecPoints)
{
	int iIdx = 0;
	vecPoints.push_back(GetPointFromStatment(iIdx));
}

DPoint3d DrawVectorGraph::GetPointFromStatment(int iIdx)
{
	DPoint3d ptTmp;

	ptTmp.x = m_statment->GetValueDouble(iIdx + 0);
	ptTmp.y = m_statment->GetValueDouble(iIdx + 1);
	ptTmp.z = m_statment->GetValueDouble(iIdx + 2);

	return ptTmp;
}
void DrawVectorGraph::GetAllPoints()
{
	bvector<Dpoint3d>().swap(NodePoints);
	bvector<Dpoint3d>().swap(VectorPoints);
	StatusInt rc;
	rc = OpenDB(m_db);
	if (rc == BE_SQLITE_OK)
	{
		m_statment = new Statement();

		//获取所有NODE坐标
		rc = GetNodeFromDatabase();

		if (rc == BE_SQLITE_OK)
		{
			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				GetNodePoints(NodePoints);
			}
		}
		m_statment->Finalize();

		//获取所有Vector坐标
		rc = GetVectorFromDatabase();

		if (rc == BE_SQLITE_OK)
		{
			while (m_statment->Step() == BE_SQLITE_ROW)
			{
				GetNodePoints(VectorPoints);
			}
		}

		m_statment->Finalize();
	}

	m_db.CloseDb();

}