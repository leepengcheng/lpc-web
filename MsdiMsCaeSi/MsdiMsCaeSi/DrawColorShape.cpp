#include "MsdiMsCaeSipch.h"
double CreateTable::tablewidth = 0.0;
void CreateTable::DrawTextInShape(DPoint3dCR origin_pos, WString drawtext, RotMatrixCR invRotation, TextStringPropertiesPtr pProp,
								IViewOutputP output, IndexedViewportR viewport)
{
	DPoint3d active_pos;
	viewport.ViewToActive(&active_pos, &origin_pos, 1);
	TextStringPtr theUText = TextString::Create(drawtext.GetWCharCP(), &active_pos, &invRotation, *pProp);
	output->SetSymbology(viewport.GetContrastToBackgroundColor(), viewport.GetContrastToBackgroundColor(), 4, 0);
	output->DrawTextString(*theUText);
}

/*--------------------------------------------------------------------------------------+
|CreateAllText：画出所有区间值
|   HeaderText   ：表头值
|   ColorNum     ：颜色块总数量
|   StartValue   ：区间起始值
|   EndValue     ：区间终止值
|   output       ：对应输出视图
|   viewport     ：对应视图指针
+--------------------------------------------------------------------------------------*/
void CreateTable::CreateAllText(WCharCP projectname,WCharCP casename,WCharCP HeaderText, WCharCP UnitText, int ColorNum, double ClrShapeVal_Legeng[],
	                           IViewOutputP output, IndexedViewportR viewport, DPoint3d tablepos)
{
	//位置定义
	DPoint3d Origin_pos = tablepos;
	DPoint3d active_pos;

	DPoint3d  viewScale;
	viewScale = *viewport.GetScale();
	DPoint2d  fontSize;
	if (viewport.IsCameraOn())
	{
		CameraParams tempCamera;
		viewport.GetCameraView(&tempCamera);
		//double theScale = viewport.GetCameraRoot()->focalLength;
		//double limit = viewport.GetCameraRoot()->limit;
		//fontSize = { 0.045, 0.075 };//表头字体大小
		fontSize = { 0.2 / tempCamera.focalLength, 0.3/ tempCamera.focalLength };//表头字体大小
	}
	else
	{
		fontSize = { 9 / viewScale.x, 10 / viewScale.y };//表头字体大小
	}


	//创建TEXT对应参数设置
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	TextStringPropertiesPtr pProp = TextStringProperties::Create(DgnFontManager::GetDecoratorFont(),
		&DgnFontManager::GetDecoratorFont(), fontSize, *pActiveModel);
	pProp->SetJustification(TextElementJustification::CenterMiddle);
	pProp->SetIs3d(true);

	RotMatrix matrix;
	matrix = *viewport.GetRotMatrix();
	RotMatrix invRotation = matrix;
	invRotation.Invert();

	//创建projecname->casename
	WString prjandcasename;
	prjandcasename.append(projectname);
	prjandcasename.append(L"(");
	prjandcasename.append(casename);
	prjandcasename.append(L")");
	Origin_pos.x = Origin_pos.x + tablewidth/9;
	Origin_pos.y = Origin_pos.y + TABLE_HEADERY;
	Origin_pos.z = 0;
	DrawTextInShape(Origin_pos, prjandcasename, invRotation, pProp, output, viewport);

	//表头位置设置(力值类型)
	Origin_pos.y = Origin_pos.y + TABLE_UNITY;
	//创建表头
	DrawTextInShape(Origin_pos, HeaderText, invRotation, pProp, output, viewport);

	//单位位置设置
	Origin_pos.y = Origin_pos.y + TABLE_UNITY;
	//创建单位
	WString allunitText;
	allunitText.append(L"Unit:");
	allunitText.append(UnitText);
	DrawTextInShape(Origin_pos, allunitText, invRotation, pProp, output, viewport);


	//区间值字体大小和区间高度获取
	int Color_height;
	Color_height = (TABLE_HEIGHT * 8 / 10) / ColorNum;
	if (ColorNum < 16)
	{
		fontSize.x = fontSize.x / 1.0;
		fontSize.y = fontSize.y / 1.5;
	}
	else
	{
		fontSize.x = fontSize.x / ColorNum * 16;
		fontSize.y = fontSize.y / ColorNum * 8;
	}
	pProp->SetFontSize(fontSize);

	WString lsClrShapeVal;

	//区间值创建
	Origin_pos.x = tablepos.x + tablewidth / 8 + tablewidth / 5 + 2;
	Origin_pos.z = 0;
	for (int i = 0; i < ColorNum; i++)
	{
		Origin_pos.y = tablepos.y + TABLE_HEIGHT / 6 + i*Color_height + TABLE_HEIGHT / 30;

		lsClrShapeVal.clear();
		lsClrShapeVal.Sprintf(L"%g", ClrShapeVal_Legeng[i]);
		viewport.ViewToActive(&active_pos, &Origin_pos, 1);
		TextStringPtr theText_i = TextString::Create(lsClrShapeVal.GetWCharCP(), &active_pos, &invRotation, *pProp);
		output->DrawTextString(*theText_i);
	}

	//区间的终止值
	Origin_pos.y = tablepos.y + TABLE_HEIGHT / 6 + ColorNum*Color_height + TABLE_HEIGHT / 30;

	lsClrShapeVal.clear();
	lsClrShapeVal.Sprintf(L"%g", ClrShapeVal_Legeng[ColorNum]);
	viewport.ViewToActive(&active_pos, &Origin_pos, 1);
	TextStringPtr theText_i = TextString::Create(lsClrShapeVal.GetWCharCP(), &active_pos, &invRotation, *pProp);
	output->DrawTextString(*theText_i);
}

/*--------------------------------------------------------------------------------------+
|CreateAllColorTable：画出所有颜色块
|   ColorNum        ：颜色块总数量
|   lineColorTBGR   ：线型颜色
|   fillColorTBGR   ：所有颜色块对应的填充颜色
|   output          ：对应输出视图
|   viewport        ：对应视图指针
+--------------------------------------------------------------------------------------*/
void CreateTable::CreateAllColorTable(int ColorNum, UInt32 lineColorTBGR, UInt32 fillColorTBGR[], IViewOutputP output, IndexedViewportR viewport,DPoint3d tablepos)
{
	//颜色块起始位置获取
	DPoint3d Origin_pos = tablepos;
	if (fillColorTBGR == NULL)
	{
		return;
	}

	DPoint3d v_test[5], v_active[5];
	v_test[0] = Origin_pos;
	v_test[0].z = 0;
	v_test[1].x = Origin_pos.x + tablewidth;
	v_test[1].y = Origin_pos.y;
	v_test[1].z = 0;
	v_test[2].x = Origin_pos.x + tablewidth;
	v_test[2].y = Origin_pos.y + TABLE_HEIGHT;
	v_test[2].z = 0;
	v_test[3].x = Origin_pos.x;
	v_test[3].y = Origin_pos.y + TABLE_HEIGHT;
	v_test[3].z = 0;
	v_test[4] = Origin_pos;
	v_test[4].z = 0;

	//画大的方框
	viewport.ViewToActive(v_active, v_test, 5);
	output->SetSymbology(viewport.GetBackgroundColor(), viewport.GetBackgroundColor(), 0, 0);
	output->DrawShape3d(5, v_active, true, NULL);
	output->SetSymbology(viewport.GetContrastToBackgroundColor(), viewport.GetContrastToBackgroundColor(), 0, 0);
	output->DrawShape3d(5, v_active, false, NULL);

	//画其他的颜色块
	double Color_height;
	Color_height = (TABLE_HEIGHT*8.0 / 10.0) / (double)ColorNum;
	for (int i = 0; i < ColorNum; i++)
	{
		DPoint3d temp_pos[4], temp_active[4];
		temp_pos[0] = Origin_pos;
		temp_pos[0].x = temp_pos[0].x + tablewidth / 8;
		temp_pos[0].y = temp_pos[0].y + TABLE_HEIGHT / 6 + i*Color_height;
		temp_pos[0].z = 0;
		temp_pos[1].x = temp_pos[0].x + tablewidth / 5;
		temp_pos[1].y = temp_pos[0].y;
		temp_pos[1].z = 0;
		temp_pos[2].x = temp_pos[0].x + tablewidth / 5;
		temp_pos[2].y = temp_pos[0].y + Color_height;
		temp_pos[2].z = 0;
		temp_pos[3].x = temp_pos[0].x;
		temp_pos[3].y = temp_pos[0].y + Color_height;
		temp_pos[3].z = 0;
		viewport.ViewToActive(temp_active, temp_pos, 4);
		output->SetSymbology(lineColorTBGR, fillColorTBGR[i], 4, 0);
		output->DrawShape3d(4, temp_active, true, NULL);
	}
}

bool DrawColorShape::_DrawDecoration(IndexedViewportR viewport)//画临时元素函数
{
	IViewOutputP output = viewport.GetIViewOutput();

	//极值标识的绘制，由于色带需要显示再最顶层，因此先绘制极值标识
	if (m_bExtremum)
	{
		UInt32     clr = viewport.GetContrastToBackgroundColor();
		output->SetSymbology(clr, clr, 0, 0);

		RotMatrix matrix;
		matrix = *viewport.GetRotMatrix();
		matrix.Invert();
		DgnFontR  font = DgnFontManager::GetDecoratorFont();
		DPoint3d  viewScale;
		viewScale = *viewport.GetScale();
		DPoint2d  fontSize = { 30 / viewScale.x, 24 / viewScale.y };
		DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
		TextStringPropertiesPtr pProp = TextStringProperties::Create(font, &font, fontSize, *pActiveModel);
		pProp->SetIs3d(true);
		for (int i = 0; i < (int)m_vecMaxPts.size(); i++)
		{
			WString ws;
			ws.AppendA("↙MX");
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecMaxPts[i], &matrix, *pProp);
			output->DrawTextString(*pText);
		}
		for (int i = 0; i < (int)m_vecMinPts.size(); i++)
		{
			WString ws;
			ws.AppendA("↙MN");
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecMinPts[i], &matrix, *pProp);
			output->DrawTextString(*pText);
		}
	}

	//节点查看结果的绘制
	if (m_bNodeView)
	{
		UInt32     clr = viewport.GetContrastToBackgroundColor();
		output->SetSymbology(clr, clr, 0, 0);

		RotMatrix matrix;
		matrix = *viewport.GetRotMatrix();
		matrix.Invert();
		DgnFontR  font = DgnFontManager::GetDecoratorFont();
		DPoint3d  viewScale;
		viewScale = *viewport.GetScale();
		DPoint2d  fontSize = { 15 / viewScale.x, 12 / viewScale.y };
		DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
		TextStringPropertiesPtr pProp = TextStringProperties::Create(font, &font, fontSize, *pActiveModel);
		pProp->SetIs3d(true);
	
		//临时节点查看
		if (m_bNodeView_Temp)
		{
			WString ws;
			char nodeMsg[64];
			sprintf_s(nodeMsg, "↙%.6g", m_dNodeInfo_Temp);
			ws.AppendA(nodeMsg);
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_NodePt_Temp, &matrix, *pProp);
			output->DrawTextString(*pText);
		}

		//已保存节点查看
		for (int i = 0; i < (int)m_vecNodePts.size(); i++)
		{
			WString ws;
			char nodeMsg[64];
			sprintf_s(nodeMsg, "↙%.6g", m_vecNodeInfos.at(i));
			ws.AppendA(nodeMsg);
			TextStringPtr pText = TextString::Create(ws.GetWCharCP(), &m_vecNodePts.at(i), &matrix, *pProp);
			output->DrawTextString(*pText);
		}
	}

	//色带盘的绘制
	if (m_bDisplayed)
	{
		//获取视图相关参数
		BSIRect theRect;
		viewport.GetViewRect(theRect);
		Point2d origin = theRect.origin;//屏幕左上角，x往左，y往下
		Point2d corner = theRect.corner;//屏幕右下角

		//设置画临时框在屏幕上对应位置
		DPoint3d v_tabPos;
		v_tabPos.x = corner.x + DrawPos.x;
		v_tabPos.y = origin.y + DrawPos.y;
		v_tabPos.z = 0;

		char prj[20];
		char casn[20];
		ProjectName.ConvertToLocaleChars(prj);
		CaseName.ConvertToLocaleChars(casn);
		CreateTable::tablewidth = (strlen(prj) + strlen(casn)) * 143 / 12;
		//画颜色表
		CreateTable::CreateAllColorTable(ColorNum_Legeng, TABLE_LINCOLOR, fillColorTBGR, output, viewport, v_tabPos);
		//颜色表标注
		if (showType == 1)//画等值线
		{
			double dTmp[100];// 取两个等值点之间的平均值
			GetContourLineColorValue(ColorNum_Legeng, dTmp, ClrShapeVal_Legeng);
			CreateTable::CreateAllText(ProjectName.GetWCharCP(), CaseName.GetWCharCP(), Header.GetWCharCP(),
				Unit.GetWCharCP(), ColorNum_Legeng, dTmp, output, viewport, v_tabPos);
		}
		else
		{
			CreateTable::CreateAllText(ProjectName.GetWCharCP(), CaseName.GetWCharCP(), Header.GetWCharCP(),
				Unit.GetWCharCP(), ColorNum_Legeng, ClrShapeVal_Legeng, output, viewport, v_tabPos);
		}
	}

	return false;
}

void DrawColorShape::initialize()
{
	/** 颜色表所有填充颜色序号 fillColorTBGR*/
	memset(fillColorTBGR, 0, 100 * sizeof(UInt32));
	/** 颜色表所有填充颜色 fillColor*/
	memset(fillColor, 0, 100 * sizeof(RgbColorDef));
	/** rgb颜色对应的颜色序号 fillCororIndex*/
	memset(fillCororIndex, 0, 100 * sizeof(UInt32));
	/** 起始区间值 StartValue*/
	StartValue = 0.0;
	/** 结束区间值 EndValue*/
	EndValue = 0.0;
	/** 起始区间值(DB) DbStartValue*/
	DbStartValue = 0.0;
	/** 结束区间值(DB) DbEndValue*/
	DbEndValue = 0.0;
	/** 所有区间值(图例显示) ClrShapeVal_Legeng*/
	memset(ClrShapeVal_Legeng, 0, 100 * sizeof(double));
	/** 所有区间值 ClrShapeVal*/
	memset(ClrShapeVal, 0, 100 * sizeof(double));
	/** 显示类型 DrawType*/
	ResultTypeCode.clear();
	/** 表头 Header*/
	Header.clear();
	/** 对应单位 Unit*/
	Unit.clear();

	ProjectName.clear();

	CaseName.clear();

	showType = 0;
	/** 对应CAE模型的命名组 NgName*/
	NgName.clear();

	//缺省置为automtic
	m_bAutomatic = true;
	//缺省不显示极值
	m_bExtremum = false;
	//缺省不显示色带
	m_bDisplayed = false;
	//缺省不显示节点信息
	m_bNodeView = false;
	m_bNodeView_Temp = false;

	ColorNum_Legeng = 10;	
}

/*--------------------------------------------------------------------------------------+
|   StartDrawFunction：开始画临时元素命令响应函数
+--------------------------------------------------------------------------------------*/
void DrawColorShape::StartDrawFunction()
{
	if (StartValue == DbStartValue && EndValue == DbEndValue)
	{
		for (int i = 0; i <= ColorNum_Legeng; ++i)
		{
			ClrShapeVal[i] = ClrShapeVal_Legeng[i];
			if (i < ColorNum_Legeng)
				mdlColor_rawColorFromRGBColor(&fillCororIndex[i], &fillColor[i], ACTIVEMODEL);
		}

		Color_num = ColorNum_Legeng;
	}
	else
	{
		int iStep = 0;
		if (StartValue > DbStartValue)
		{
			ClrShapeVal[0] = DbStartValue;
			fillCororIndex[0] = 112;
			iStep = 1;
		}

		for (int i = 0; i <= ColorNum_Legeng; ++i)
		{
			ClrShapeVal[iStep + i] = ClrShapeVal_Legeng[i];
			if (i < ColorNum_Legeng)
				mdlColor_rawColorFromRGBColor(&fillCororIndex[iStep + i], &fillColor[i], ACTIVEMODEL);
		}

		if (EndValue < DbEndValue)
		{
			iStep += 1;
			ClrShapeVal[iStep + ColorNum_Legeng] = DbEndValue;
			fillCororIndex[iStep + ColorNum_Legeng - 1] = 112;
		}

		Color_num = ColorNum_Legeng + iStep;
	}

	if (showType == 1)//显示等值线图
	{
		double dTmp[100];// 取两个等值点之间的平均值
		GetContourLineColorValue(Color_num, dTmp, ClrShapeVal);
		memcpy(ClrShapeVal, dTmp, sizeof(double)*100);
	}

	m_bDisplayed = true;
}
/*--------------------------------------------------------------------------------------+
|   CloseStartDrawFunction：画完临时元素后要先关闭再退出程序
+--------------------------------------------------------------------------------------*/
void DrawColorShape::CloseDrawFunction()
{
	initialize();
}

void DrawColorShape::divideColorRange()
{
	for (int i = 0; i < ColorNum_Legeng + 1; i++)//设置区间数值
	{
		ClrShapeVal_Legeng[i] = StartValue + (EndValue - StartValue) / ColorNum_Legeng * i;
	}

	for (int i = 0; i < ColorNum_Legeng; i++)//设置每个颜色块对应的颜色
	{
		if (ColorNum_Legeng <= 0 || ColorNum_Legeng > 100)
		{
			return;
		}
		RgbColorDef theColor;
		// 判断是否为灰度显示
		if (m_Gray)
		{
			// 灰度显示处理
			if (i == 0)//第1个色块
			{
				if (ColorNum_Legeng == 1)
				{
					fillColorTBGR[0] = 0x003C3C3C;
					theColor.red = 60;
					theColor.green = 60;
					theColor.blue = 60;
				}
				else
				{
					fillColorTBGR[0] = 0x00C8C8C8;
					theColor.red = 200;
					theColor.green = 200;
					theColor.blue = 200;
				}
			}
			else
			{
				theColor.red = 200 - i*(140.0 / ColorNum_Legeng);
				theColor.green = 200 - i*(140.0 / ColorNum_Legeng);
				theColor.blue = 200 - i*(140.0 / ColorNum_Legeng);
				fillColorTBGR[i] = (theColor.blue << 16) + (theColor.green << 8) + theColor.red;
			}
		}
		else
		{
			if (i == 0)//第1个色块
			{
				if (ColorNum_Legeng == 1)
				{
					fillColorTBGR[0] = 0x000000ff;
					theColor.red = 255;
					theColor.green = 0;
					theColor.blue = 0;
				}
				else
				{
					fillColorTBGR[0] = 0x00ff0000;
					theColor.red = 0;
					theColor.green = 0;
					theColor.blue = 255;
				}

			}
			else if (i != ColorNum_Legeng - 1)//不是最后一个
			{
				int TheRange = -1;//区间
				double ThePercent = 0.0;//区间百分比
				double the_Pos = 0.0;
				the_Pos = (4.0 / (double)(ColorNum_Legeng - 1))*i;

				TheRange = the_Pos;
				ThePercent = the_Pos - TheRange;
				int r = 0;
				int g = 0;
				int b = 0;
				switch (TheRange)
				{
				case 0://第一区间
				{
					r = 0;
					g = 0;
					b = 255;
					g = 255.0*ThePercent + 0.5;
					break;
				}
				case 1://第二区间
				{
					r = 0;
					g = 255;
					b = 255;
					b = 255.0*(1 - ThePercent) + 0.5;
					break;
				}
				case 2://第三区间
				{
					r = 0;
					g = 255;
					b = 0;
					r = 255.0*ThePercent + 0.5;
					break;
				}
				case 3://第四区间
				{
					r = 255;
					g = 255;
					b = 0;
					g = 255.0*(1 - ThePercent) + 0.5;
					break;
				}
				default:
					break;
				}
				fillColorTBGR[i] = (b << 16) + (g << 8) + r;
				theColor.red = r;
				theColor.green = g;
				theColor.blue = b;
			}
			else//最后一个色块
			{
				fillColorTBGR[i] = 0x000000ff;
				theColor.red = 255;
				theColor.green = 0;
				theColor.blue = 0;
			}
		}
		fillColor[i] = theColor;
	}
}

void DrawColorShape::GetContourLineColorValue(int colornum, double out_value[], double in_value[])
{
	for (int i = 0; i < colornum; i++)//设置区间数值
	{
		out_value[i] = (in_value[i] + in_value[i + 1]) / 2;
	}
	out_value[colornum] = in_value[colornum];
}
void DrawColorShape::initColorRangeFromDB()
{
	StatusInt rc;
	Db m_db;
	Statement *m_statment = new Statement();

	rc = OpenDB(m_db);

	if (rc == BE_SQLITE_OK)
	{
		AString strSql;
		
		//根据结果类型code值和工程ID、工况ID获取最大值最小值(仅考虑云图显示标识为1的Face节点)
		strSql.append("SELECT MIN(");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append("), MAX(");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append(") \n");
		strSql.append(" FROM (SELECT ID, ");
		strSql.append(ws2s(ResultTypeCode.data()).data());
		strSql.append(" FROM RESULTS WHERE ProjectId = @ProID AND CaseID = @CaseID) rst \n");
		strSql.append(" INNER JOIN \n");
		strSql.append(" ( \n");
		strSql.append(" select Node1 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node2 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node3 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" UNION \n");
		strSql.append(" select Node4 NodeID FROM FACES WHERE ProjectId = @ProID AND MSLevelName = @MSLevelName AND ElemShowFlg = 1 AND ContourShowFlg = 1 \n");
		strSql.append(" ) fn \n");
		strSql.append(" ON (rst.ID = fn.NodeID) ");

		rc = m_statment->Prepare(m_db, strSql.c_str());
		// 项目ID
		m_statment->BindInt(1, ProjectId);
		// 对应工况ID
		m_statment->BindInt(2, CaseID);
		// 对应CAE模型的命名组
		Utf8String sNgName;
		BeStringUtilities::WCharToUtf8(sNgName, NgName.c_str());
		m_statment->BindUtf8String(3, sNgName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);

		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			StartValue = m_statment->GetValueDouble(0);
			EndValue = m_statment->GetValueDouble(1);
			DbStartValue = StartValue;
			DbEndValue = EndValue;// +0.000001;
		}
	}

	delete m_statment;
	m_db.CloseDb();
	
	//手动刷新视图
	mdlView_updateSingle(tcb->lstvw);
}
