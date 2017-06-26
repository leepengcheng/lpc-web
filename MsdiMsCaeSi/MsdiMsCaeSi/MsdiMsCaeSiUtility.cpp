#include "MsdiMsCaeSipch.h"

void MsdiMsCaeSiUtility::getBasePtAndLevelID(DPoint3dR basePt, WStringR sUnit, LevelId& levelId_CaeModel, WStringCR ngName)
{
	bvector<WString> oBasePtVec;

	if (SUCCESS == mdlLevel_getIdFromName(&levelId_CaeModel, MASTERFILE, LEVEL_NULL_ID, ngName.c_str()))
	{
		WChar  basePtStr[256];
		if (SUCCESS == mdlLevel_getDescription(basePtStr, 256, MASTERFILE, levelId_CaeModel))
		{
			BeStringUtilities::Split(basePtStr, L"_", oBasePtVec);
			// CAE模型的放置坐标
			basePt.x = BeStringUtilities::Wtof(oBasePtVec.at(0).c_str());
			basePt.y = BeStringUtilities::Wtof(oBasePtVec.at(1).c_str());
			basePt.z = BeStringUtilities::Wtof(oBasePtVec.at(2).c_str());
			// CAE模型的单位
			sUnit.append(oBasePtVec.at(3));
		}
	}
}

// 取得CAE模型的显示单位
double MsdiMsCaeSiUtility::GetUnit(WString strProjectUnit)
{
	if (strProjectUnit.Equals(L"mm"))
	{
		return mdlModelRef_getUorPerMeter(ACTIVEMODEL) / 1000;
	}
	else
	{
		return mdlModelRef_getUorPerMeter(ACTIVEMODEL);
	}
}

// 获取截面的转换矩阵(三维截面->二维平面的转换矩阵)
void MsdiMsCaeSiUtility::GetSectionTransform(TransformP *tMatrix, WString strNgName)
{
	NamedGroupCollectionPtr ngcCutFace;
	ngcCutFace = NamedGroupCollection::Create(*ACTIVEMODEL);

	for each (NamedGroupPtr namedGroup in *ngcCutFace)
	{
		if (namedGroup.IsValid())
		{
			WString ngName = namedGroup->GetName();
			if (ngName.find(strNgName) != WString::npos)
			{
				UInt32 graphicMembers;
				namedGroup->GetMemberCount(&graphicMembers, NULL);
				for (UInt32 i = 0; i < graphicMembers; i++)
				{
					ElementId elemid = namedGroup->GetMember(i)->GetElementId();
					EditElementHandle eeh;
					if (SUCCESS == eeh.FindByID(elemid, ACTIVEMODEL))//得到的每一个mesh面
					{
						//转换矩阵获取
						XAttributeHandlerId handlerMatrixId(XAttrHandleID_Matrix, XAttrHandleID_Matrix);
						XAttributeHandle Matrixh(eeh.GetElementRef(), handlerMatrixId, XAttrID_Matirx);
						if (Matrixh.IsValid())
						{
							*tMatrix = (Bentley::TransformP)Matrixh.PeekData();
							break;
						}
					}
				}
			}
		}
	}
}

// 数字格式显示（保留指定的小数点位数）
void MsdiMsCaeSiUtility::FormatValDisplay(WStringR strVal, double& dVal, int iDecimal)
{
	strVal.clear();

	wostringstream oss;
	oss << setprecision(iDecimal) << setiosflags(ios::fixed) << dVal;
	strVal.append(oss.str().data());
	oss.str(L"");
}

//刷新截面列表选中后的文本显示
WString MsdiMsCaeSiUtility::MakeSelectedTreeNodeForTxt(GuiTreeNodeP treeNode)
{
	//根据用户用户选择的结点更新
	bvector<WString> vecNodeText;
	GuiTreeNode* optNode = treeNode;

	//由选中节点一直向根遍历获取显示内容（根节点除外）
	do
	{
		WString nodeText;
		mdlTreeNode_getDisplayTextWString(optNode, nodeText);
		vecNodeText.push_back(nodeText);
	} while ((optNode = mdlTreeNode_getParent(optNode)) && !mdlTreeNode_isRoot(optNode));

	//反序显示各个节点的内容
	WString current;
	for (int i = (int)vecNodeText.size() - 1; i >= 0; i--)
	{
		current.append(vecNodeText[i]);
		if (i != 0)
			current.append(L">");
	}

	return current;
}

// 坐标数组排序（升序）（SortType：0：x,1:y,2:z）
void MsdiMsCaeSiUtility::SortPoints(bvector<DPoint3d>& vecPoints, int SortType)
{
	int flag;
	DPoint3d ptTmp;
	for (int i = (int)vecPoints.size() - 1; i>0; i--)
	{
		flag = 0;
		for (int j = 0; j <= i - 1; j++)
		{
			switch (SortType)
			{
			case 0:// 按X轴排序
			{
				if (vecPoints[j].x > vecPoints[j + 1].x)
				{
					ptTmp = vecPoints[j];
					vecPoints[j] = vecPoints[j + 1];
					vecPoints[j + 1] = ptTmp;
					flag = 1;
				}
				break;
			}
			case 1:// 按y轴排序
			{
				if (vecPoints[j].y > vecPoints[j + 1].y)
				{
					ptTmp = vecPoints[j];
					vecPoints[j] = vecPoints[j + 1];
					vecPoints[j + 1] = ptTmp;
					flag = 1;
				}
				break;
			}
			case 2:// 按z轴排序
			{
				if (vecPoints[j].z > vecPoints[j + 1].z)
				{
					ptTmp = vecPoints[j];
					vecPoints[j] = vecPoints[j + 1];
					vecPoints[j + 1] = ptTmp;
					flag = 1;
				}
				break;
			}
			}
		}
		if (flag == 0)break;
	}
}

//求两条直线的交点
DPoint3d MsdiMsCaeSiUtility::GetTwoLineIntersect(DPoint3d str1, DPoint3d end1, DPoint3d str2, DPoint3d end2)
{
	DPoint3d intersectP;

	DSegment3d line_Fir = DSegment3d::From(str1, end1);
	DSegment3d line_Sec = DSegment3d::From(str2, end2);
	mdlVec_intersect(&intersectP, &line_Fir, &line_Sec);
	intersectP.z = str1.z;

	return intersectP;
}

//创建字体函数
bool MsdiMsCaeSiUtility::CreateText(EditElementHandleR eehText, DgnModelRefP ModelRef, DPoint3d tPos, WString tText, double fwidth, double fheight, double iSlant, UInt32 iJustification)
{
	if (ModelRef == NULL)
	{
		ModelRef = MASTERFILE;
	}
	// 字体样式
	DgnTextStylePtr myTextStyle = DgnTextStyle::GetActive();
	// 字体宽度
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_Width, fwidth);
	// 字体高度
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_Height, fheight);
	// 字体倾斜弧度
	myTextStyle->SetProperty(TextStyleProperty::TextStyle_Slant, iSlant);
	// 字体对齐方式
	if (iJustification != -1)
		myTextStyle->SetProperty(TextStyleProperty::TextStyle_Justification, iJustification);

	// 字体设置
	UInt32 foundNum;
	DgnFileP modelref = ISessionMgr::GetActiveDgnFile();
	DgnFontP pDGNFont = DgnFontManager::GetDgnFontMapP(ModelRef)->FindFontByName(&foundNum, L"宋体");
	if (!pDGNFont)
	{
		pDGNFont = DgnFontManager::GetDgnFontMapP(ModelRef)->FindFontByName(&foundNum, L"Batang");
	}
	if (!pDGNFont)
	{
		pDGNFont = &DgnFontManager::GetDgnFontMapP(ModelRef)->ResolveFont(-1);
	}

	// 设置文字样式
	myTextStyle->SetProperty(TextStyle_Font, pDGNFont);
	TextBlockPtr textBlock = TextBlock::Create(*myTextStyle, *ModelRef->GetDgnModelP());

	textBlock->SetUserOrigin(tPos);
	textBlock->AppendText(tText.GetWCharCP());
	if (TEXTBLOCK_TO_ELEMENT_RESULT_Success == TextHandlerBase::CreateElement(eehText, NULL, *textBlock))
	{
		return true;
	}

	return false;
}

bool MsdiMsCaeSiUtility::GetPosition(ContourPoint& ptResult, DPoint3d ptStart, DPoint3d ptEnd, double dStartVal, double dEndVal, double dVal)
{
	double dVal1;
	double dVal2;
	double index;
	bool bRet;

	dVal1 = dStartVal - dVal;
	dVal2 = dEndVal - dVal;
	index = dVal1 * dVal2;

	if (index > 0)
	{
		bRet = false;
	}
	else if (dStartVal == dVal || dEndVal == dVal)
	{
		bRet = false;
	}
	else
	{
		// 加上0.0000000001防止分母为0
		ptResult.dPt.x = ptStart.x + (dVal - dStartVal) * (ptEnd.x - ptStart.x) / (dEndVal - dStartVal + 0.0000000001);
		ptResult.dPt.y = ptStart.y + (dVal - dStartVal) * (ptEnd.y - ptStart.y) / (dEndVal - dStartVal + 0.0000000001);
		ptResult.dPt.z = ptStart.z + (dVal - dStartVal) * (ptEnd.z - ptStart.z) / (dEndVal - dStartVal + 0.0000000001);

		ptResult.dVal = dVal;

		bRet = true;
	}

	return bRet;
}

// 判断点在多边形内(包括边线)
int MsdiMsCaeSiUtility::PointInPolygon(DPoint3d p, const bvector<PointOutData>& ptPolygon)
{
	int count = 0;
	size_t nCount = ptPolygon.size();

	DSegment3d line;
	line.point[0] = p;
	line.point[1].y = p.y;
	line.point[1].x = -INFINIT;
	line.point[1].z = p.z;

	for (int i = 0; i < nCount - 1; i++)
	{
		// 得到多边形的一条边
		DSegment3d side;
		side.point[0] = ptPolygon.at(i).dPt;
		side.point[1] = ptPolygon.at(i + 1).dPt;

		if (PtIsOnline(p, side.point[0], side.point[1]))
		{
			return 1; // 点在多边形上
		}

		// 如果side平行x轴则不作考虑
		if (fabs(side.point[0].y - side.point[1].y) < Tolerance)
		{
			continue;
		}

		if (PtIsOnline(side.point[0], line.point[0], line.point[1]))
		{
			if (side.point[0].y > side.point[1].y)
				count++;
		}
		else if (PtIsOnline(side.point[1], line.point[0], line.point[1]))
		{
			if (side.point[1].y > side.point[0].y)
				count++;
		}
		else if (Intersect(line, side))
		{
			count++;
		}
	}

	if (count % 2 == 1)
	{
		return 2;  // 点在多边形内
	}
	return 0;   // 点在多边形外
}

// 判断线段相交
bool MsdiMsCaeSiUtility::Intersect(DSegment3d L1, DSegment3d L2)
{
	return((max(L1.point[0].x, L1.point[1].x) >= min(L2.point[0].x, L2.point[1].x)) &&
		(max(L2.point[0].x, L2.point[1].x) >= min(L1.point[0].x, L1.point[1].x)) &&
		(max(L1.point[0].y, L1.point[1].y) >= min(L2.point[0].y, L2.point[1].y)) &&
		(max(L2.point[0].y, L2.point[1].y) >= min(L1.point[0].y, L1.point[1].y)) &&
		(Multiply(L2.point[0], L1.point[1], L1.point[0]) * Multiply(L1.point[1], L2.point[1], L1.point[0]) >= 0) &&
		(Multiply(L1.point[0], L2.point[1], L2.point[0]) * Multiply(L2.point[1], L1.point[1], L2.point[0]) >= 0)
		);
}

double MsdiMsCaeSiUtility::Multiply(DPoint3d p1, DPoint3d p2, DPoint3d p0)
{
	return ((p1.x - p0.x) * (p2.y - p0.y) - (p2.x - p0.x) * (p1.y - p0.y));
}

// 判断线段是否包含点p0(p0任意一点)
bool MsdiMsCaeSiUtility::PtIsOnline(DPoint3d p0, DPoint3d p1, DPoint3d p2)
{
	double dis_P01 = mdlVec_distance(&p0, &p1);
	double dis_P02 = mdlVec_distance(&p0, &p2);
	double dis_P12 = mdlVec_distance(&p1, &p2);
	double dis_P0 = fabs(dis_P12 - dis_P01 - dis_P02);

	return(((fabs(Multiply(p1, p2, p0)) < Tolerance) && ((p0.x - p1.x) * (p0.x - p2.x) <= Tolerance) && ((p0.y - p1.y) * (p0.y - p2.y) <= Tolerance))
		|| dis_P01 < Tolerance || dis_P02 < Tolerance || dis_P0 < Tolerance);
}

void MsdiMsCaeSiUtility::fitView(int viewIndex)
{
	for (int i = 0; i < 7; i++)
	{
		if (i == viewIndex)
		{
			if (!mdlView_isVisible(i))
				mdlView_turnOn(i);
		}
		else
			mdlView_turnOff(i);
	}

	mdlInput_sendSynchronizedKeyin(L"window tile", 0, INPUTQ_EOQ, NULL);


	mdlView_fit(viewIndex, NULL);
	mdlView_updateSingle(viewIndex);
}

//// 有限元形函数计算应力值
//double MsdiMsCaeSiUtility::UseBilinear_interpolationGetVlue(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1)
//{
//	double dRet;
//	//if (MeshPoints.size() > 4)
//	//{
//	dRet = UseBilinear_interpolationGetVlue_56(MeshPoints, tempPoint, isS1);
//	//}
//	//else if (MeshPoints.size() > 3)
//	//{
//	//	dRet = UseBilinear_interpolationGetVlue_4(MeshPoints, tempPoint, isS1);
//	//}
//	//else
//	//{
//	//	dRet = UseBilinear_interpolationGetVlue_3(MeshPoints, tempPoint, isS1);
//	//}
//
//	return dRet;
//}

// 反距离加权法计算应力值
void MsdiMsCaeSiUtility::UseIDW_interpolationGetVlue(PointOutData& mid_Point, const bvector<PointOutData>&  MeshPoints)
{
	double tValue = 0, all_W = 0, dis, ratio;
	DPoint3d meshpoint;

	for (UINT32 i = 0; i < MeshPoints.size(); i++)
	{
		meshpoint = MeshPoints.at(i).dPt;
		dis = mdlVec_distance(&mid_Point.dPt, &meshpoint);
		if (dis < Tolerance)
		{
			for (auto iter1 = MeshPoints.at(i).ResultVals.begin(); iter1 != MeshPoints.at(i).ResultVals.end(); iter1++)
			{
				mid_Point.ResultVals[iter1->first] = iter1->second;
			}				
			return;
		}
		ratio = pow(1 / dis, 2);
		all_W += ratio;

		for (auto iter2 = MeshPoints.at(i).ResultVals.begin(); iter2 != MeshPoints.at(i).ResultVals.end(); iter2++)
		{
			mid_Point.ResultVals[iter2->first] += iter2->second * ratio;
		}			
	}

	for (auto iter3 = mid_Point.ResultVals.begin(); iter3 != mid_Point.ResultVals.end(); iter3++)
	{
		iter3->second /= all_W;
	}		
}

//// 距离倒数乘方法
//double MsdiMsCaeSiUtility::UseBilinear_interpolationGetVlue_56(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1) // 有限元形函数（5个点的情况）
//{
//	double tValue = 0, all_W = 0;
//
//	for (UINT32 i = 0; i < MeshPoints.size(); i++)
//	{
//		DPoint3d meshpoint = MeshPoints.at(i).dPt;
//		double dis = mdlVec_distance(&tempPoint, &meshpoint);
//		if (dis < Tolerance)
//		{
//			if (isS1)
//				return MeshPoints.at(i).dVal2;
//			else
//			{
//				return MeshPoints.at(i).dVal;
//			}
//		}
//		double ratio = pow(1 / dis, 2);
//		all_W += ratio;
//		if (isS1)
//		{
//			tValue += MeshPoints.at(i).dVal2*ratio;
//		}
//		else
//		{
//			tValue += MeshPoints.at(i).dVal*ratio;
//		}
//	}
//	tValue /= all_W;
//
//	return tValue;
//}
//
//double MsdiMsCaeSiUtility::UseBilinear_interpolationGetVlue_4(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1)//有限元形函数（4个点的情况）
//{
//	//形函数方程：f(x,y)= A*x + B*y + C*xy +D 
//	double M43, M42, M21;
//	double N43, N42, N21;
//	double O43, O42, O21;
//
//	if (isS1)
//	{
//		M43 = (MeshPoints.at(4 - 1).dVal2 - MeshPoints.at(3 - 1).dVal2) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(3 - 1).dPt.x) + 0.0000001);
//		M21 = (MeshPoints.at(2 - 1).dVal2 - MeshPoints.at(1 - 1).dVal2) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//		M42 = (MeshPoints.at(4 - 1).dVal2 - MeshPoints.at(2 - 1).dVal2) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(2 - 1).dPt.x) + 0.0000001);
//	}
//	else
//	{
//		M43 = (MeshPoints.at(4 - 1).dVal - MeshPoints.at(3 - 1).dVal) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(3 - 1).dPt.x) + 0.0000001);
//		M21 = (MeshPoints.at(2 - 1).dVal - MeshPoints.at(1 - 1).dVal) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//		M42 = (MeshPoints.at(4 - 1).dVal - MeshPoints.at(2 - 1).dVal) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(2 - 1).dPt.x) + 0.0000001);
//	}
//
//	N43 = (MeshPoints.at(4 - 1).dPt.y - MeshPoints.at(3 - 1).dPt.y) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(3 - 1).dPt.x) + 0.0000001);
//	N42 = (MeshPoints.at(4 - 1).dPt.y - MeshPoints.at(2 - 1).dPt.y) / ((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(2 - 1).dPt.x) + 0.0000001);
//	N21 = (MeshPoints.at(2 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.y) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//
//	O43 = (MeshPoints.at(4 - 1).dPt.x*MeshPoints.at(4 - 1).dPt.y - MeshPoints.at(3 - 1).dPt.x*MeshPoints.at(3 - 1).dPt.y) /
//		((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(3 - 1).dPt.x) + 0.0000001);
//	O42 = (MeshPoints.at(4 - 1).dPt.x*MeshPoints.at(4 - 1).dPt.y - MeshPoints.at(2 - 1).dPt.x*MeshPoints.at(2 - 1).dPt.y) /
//		((MeshPoints.at(4 - 1).dPt.x - MeshPoints.at(2 - 1).dPt.x) + 0.0000001);
//	O21 = (MeshPoints.at(2 - 1).dPt.x*MeshPoints.at(2 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.x*MeshPoints.at(1 - 1).dPt.y) /
//		((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//
//	double C = ((M21 - M42) / (N21 - N42 + 0.0000001) - (M43 - M42) / (N43 - N42 + 0.0000001)) /
//		((O21 - O42) / (N21 - N42 + 0.0000001) - (O43 - O42) / (N43 - N42 + 0.0000001) + 0.0000001);
//
//	double B = (M21 - M42) / (N21 - N42 + 0.0000001) - (O21 - O42) / (N21 - N42 + 0.0000001)*C;
//
//	double A = M21 - B*N21 - C*O21;
//
//	double D;
//	if (isS1)
//	{
//		D = (MeshPoints.at(1 - 1).dVal2 - A*MeshPoints.at(1 - 1).dPt.x - B*MeshPoints.at(1 - 1).dPt.y)
//			- C*MeshPoints.at(1 - 1).dPt.x*MeshPoints.at(1 - 1).dPt.y;
//	}
//	else
//	{
//		D = (MeshPoints.at(1 - 1).dVal - A*MeshPoints.at(1 - 1).dPt.x - B*MeshPoints.at(1 - 1).dPt.y)
//			- C*MeshPoints.at(1 - 1).dPt.x*MeshPoints.at(1 - 1).dPt.y;
//	}
//
//
//	double tValue = A*tempPoint.x + B*tempPoint.y + C*tempPoint.x*tempPoint.y + D;
//
//	return tValue;
//}
//
//
//double MsdiMsCaeSiUtility::UseBilinear_interpolationGetVlue_3(const bvector<ContourPoint>&  MeshPoints, DPoint3d tempPoint, bool isS1)//有限元形函数（3个点的情况）
//{
//	//形函数方程：f(x,y)= A*x + B*y + C 
//	double M21;
//	double M31;
//	if (isS1)
//	{
//		M21 = (MeshPoints.at(2 - 1).dVal2 - MeshPoints.at(1 - 1).dVal2) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//		M31 = (MeshPoints.at(3 - 1).dVal2 - MeshPoints.at(1 - 1).dVal2) / ((MeshPoints.at(3 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//	}
//	else
//	{
//		M21 = (MeshPoints.at(2 - 1).dVal - MeshPoints.at(1 - 1).dVal) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//		M31 = (MeshPoints.at(3 - 1).dVal - MeshPoints.at(1 - 1).dVal) / ((MeshPoints.at(3 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//	}
//	double N21 = (MeshPoints.at(2 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.y) / ((MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//	double N31 = (MeshPoints.at(3 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.y) / ((MeshPoints.at(3 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x) + 0.0000001);
//
//	double B = (M21 - M31) / (N21 - N31 + 0.0000001);
//
//	double A;
//	double C;
//	if (isS1)
//	{
//		A = ((MeshPoints.at(2 - 1).dVal - MeshPoints.at(1 - 1).dVal) - B*(MeshPoints.at(2 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.y)) /
//			(MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x + 0.0000001);
//		C = MeshPoints.at(1 - 1).dVal - A*MeshPoints.at(1 - 1).dPt.x - B*MeshPoints.at(1 - 1).dPt.y;
//	}
//	else
//	{
//		A = ((MeshPoints.at(2 - 1).dVal - MeshPoints.at(1 - 1).dVal) - B*(MeshPoints.at(2 - 1).dPt.y - MeshPoints.at(1 - 1).dPt.y)) /
//			(MeshPoints.at(2 - 1).dPt.x - MeshPoints.at(1 - 1).dPt.x + 0.0000001);
//		C = MeshPoints.at(1 - 1).dVal - A*MeshPoints.at(1 - 1).dPt.x - B*MeshPoints.at(1 - 1).dPt.y;
//	}
//
//	double tValue = A*tempPoint.x + B*tempPoint.y + C;
//	return tValue;
//}