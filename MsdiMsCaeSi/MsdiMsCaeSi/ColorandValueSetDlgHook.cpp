#include "MsdiMsCaeSipch.h"

extern BSIColorDescr*	m_pBackgroundColor;
extern BSIColorDescr*	m_pForegroundColor;
extern DgnPlatform::BSIColorPalette*  ppPalette;
extern SurfaceShowDlgInfo	g_surfaceShowDlgInfo;
extern ColorSettingDlgInfo	g_colorSettingDlgInfo;

DrawColorShape	s_DCShape_Setting;
DrawColorShape*	s_pDCSSetting;

SourceTypeDCS s_sourceDCS;

extern DPoint3d s_SecCountourBasePt;

void HookDialog_ColorSetting(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_INIT:
	{
		//根据是否automatic禁用/可用指定控件
		DialogItem* diP_Widget = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP_Widget->itemIndex, !s_DCShape_Setting.m_bAutomatic, false);
		diP_Widget = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP_Widget->itemIndex, !s_DCShape_Setting.m_bAutomatic, false);

		//将应用按钮置为禁用
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dmP->db, diP->itemIndex, true, false);

		mdlDialog_itemsSynch(dmP->db);
	}break;
	default:
		dmP->msgUnderstood = FALSE; 
		break;
	}
}

ListModel* createListModel(int nCols)
{
	//list表参数定义
	ListModel*		pListModel = NULL;
	ListRow*		pRow = NULL;
	int				rowIndex;//行
	int				colIndex;//列

	pListModel = mdlListModel_create(nCols);

	//由对应的颜色个数创建相应的表
	for (rowIndex = 0; rowIndex < s_DCShape_Setting.ColorNum_Legeng; rowIndex++)
	{
		pRow = mdlListRow_create(pListModel);
		for (colIndex = 0; colIndex < nCols; colIndex++)
		{
			ListCell*	pCell = NULL;
			WChar		w_value[32];
			char        c_value[32];
			sprintf(c_value, "%d", rowIndex);

			BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);

			pCell = mdlListRow_getCellAtIndex(pRow, colIndex);

			switch (colIndex)//根据不同的列设置不同的值
			{
			case 0://序号
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			case 1://颜色
			{
				BSIColorDescr*	m_pBackgroundColor;
				BSIColorDescr*	m_pForegroundColor;
				m_pBackgroundColor = mdlColorPal_getColorDescr(ppPalette, rowIndex);
				m_pForegroundColor = mdlColorPal_getColorDescr(ppPalette, rowIndex);
				sprintf(c_value, "(%d,%d,%d)", s_DCShape_Setting.fillColor[rowIndex].red, s_DCShape_Setting.fillColor[rowIndex].green,
					s_DCShape_Setting.fillColor[rowIndex].blue);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setIcon(pCell, ICONID_ColorPatch, RTYPE_Icon, mdlSystem_getCurrMdlDesc());

				//	RgbColorDef testColor;
				//	testColor.red = s_DCShape_Setting.fillColor[rowIndex].red;
				//	testColor.green = s_DCShape_Setting.fillColor[rowIndex].green;
				//	testColor.blue = s_DCShape_Setting.fillColor[rowIndex].blue;

				mdlColorDescr_setByRgb(m_pForegroundColor, &s_DCShape_Setting.fillColor[rowIndex], FALSE);
				mdlListCell_setIconColorOverrides(pCell, m_pBackgroundColor, m_pForegroundColor);
				mdlListCell_setEditor(pCell, RTYPE_Container, CONTAINERID_ColorChooser, mdlSystem_getCurrMdlDesc(), FALSE, TRUE);
			}
			break;
			case 2://区间最小值
			{
				double	flag = s_DCShape_Setting.ClrShapeVal_Legeng[rowIndex];
				sprintf(c_value, "%0.5f", flag);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setLongValue(pCell, flag);
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			}
			case 3://区间最大值
			{
				double	flag = s_DCShape_Setting.ClrShapeVal_Legeng[rowIndex + 1];
				sprintf(c_value, "%0.5f", flag);

				BeStringUtilities::CurrentLocaleCharToWChar(w_value, c_value, sizeof(c_value) * 2);
				mdlListCell_setLongValue(pCell, flag);
				mdlListCell_setStringValue(pCell, w_value, TRUE);
				break;
			}

			}
		}
		mdlListModel_addRow(pListModel, pRow);
	}

	return pListModel;
}

/*--------------------------------------------------------------------------------------+
|   updateColorListBox：由用户在界面输入相应数值来刷新颜色块和相应区间值
+--------------------------------------------------------------------------------------*/
void updateColorListBox(MSDialog* db)
{	
	s_DCShape_Setting.StartValue = g_colorSettingDlgInfo.dStartValue;
	s_DCShape_Setting.EndValue = g_colorSettingDlgInfo.dEndValue;
	s_DCShape_Setting.ColorNum_Legeng = g_colorSettingDlgInfo.iColorNum;

	s_DCShape_Setting.divideColorRange();
	ListModel* pListModel = createListModel(4);
	mdlDialog_listBoxSetListModelP(db->GetItemByTypeAndId(RTYPE_ListBox, LISTBOXID_ColorSet_ColorList)->rawItemP, pListModel, 0);
}

/*--------------------------------------------------------------------------------------+
|   HookPushBtn_ColorSet_ColorChooser：选中颜色后的刷新函数
+--------------------------------------------------------------------------------------*/
void HookPushBtn_ColorSet_ColorChooser(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		RscId	     	iconId = ICONID_ColorBoundary;
		RscType	  		iconType = RTYPE_Icon;
		BSIColorDescr*	pFgColorDesc = NULL;
		ListCell*		pCell = NULL;
		ValueUnion		uValue;

		mdlDialog_pushButtonSetIcon(dimP->dialogItemP->rawItemP, NULL, &iconId, &iconType, NULL);

		mdlListCell_getLongValue(pCell, &uValue.sLongFormat);

		mdlColorPal_getElemColorDescrByModelRef(&pFgColorDesc, uValue.sLongFormat, MASTERFILE, 0);
		mdlDialog_pushButtonSetIconColors(dimP->dialogItemP->rawItemP, &pFgColorDesc, NULL);

		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		mdlDialog_lastActionTypeSet(dimP->db, ACTIONBUTTON_OK);
		mdlWindow_hide((MSWindow *)dimP->db, HideReasonCode::Exiting, TRUE);
		mdlDialog_closeCommandQueue(dimP->db);

		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   TColorChooser：点击颜色块时的响应函数
+--------------------------------------------------------------------------------------*/
void TColorChooser(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		break;
	}
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		//int				format;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		//	Get a reference to the ListBox current ListCell
		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		//	Get this ColorChooser's Color index
		mdlDialog_itemGetValue(uValue, dimP->db, dimP->itemIndex);
		if (uValue.IsSet())
		{
			int theIdex = uValue.GetLong();
			m_pForegroundColor = mdlColorPal_getColorDescr(NULL, theIdex);
			RgbColorDef tRGB;
			mdlColorDescr_getRgb(&tRGB, m_pForegroundColor);
			s_DCShape_Setting.fillColor[row] = tRGB;
			s_DCShape_Setting.fillColorTBGR[row] = (tRGB.blue << 16) + (tRGB.green << 8) + tRGB.red;
			// 根据颜色RGB值，取得MS中相对应的颜色ID
			mdlColor_rawColorFromRGBColor(&s_DCShape_Setting.fillCororIndex[row], &s_DCShape_Setting.fillColor[row], ACTIVEMODEL);

			mdlListCell_setIconColorOverrides(pCell, m_pBackgroundColor, m_pForegroundColor);
			mdlDialog_itemDraw(dimP->db, pLBox->itemIndex);
			dimP->db->Refresh();
		}
		//	Inform the ListCell that we've finished editing
		mdlDialog_lastActionTypeSet(dimP->db, ACTIONBUTTON_OK);
		mdlWindow_hide((MSWindow *)dimP->db, HideReasonCode::HideWindow, TRUE);
		mdlDialog_closeCommandQueue(dimP->db);
		//printf ("ColorChooser_STATECHANGED value=%d\n", uValue.sLongFormat);
		break;
	}

	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   ListBoxCreate：颜色设置对话框钩函数
+--------------------------------------------------------------------------------------*/
void ListBoxCreate(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		RawItemHdr*	 pThis = dimP->dialogItemP->rawItemP;
		ListModel*	pListModel = createListModel(MAX_COLUMN_COUNT);
		mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}
/*--------------------------------------------------------------------------------------+
|   Updownbutton：刷新键响应钩函数
+--------------------------------------------------------------------------------------*/
void Updownbutton(DialogItemMessage *dimP)
{
	RawItemHdr*		pThis;
	pThis = dimP->dialogItemP->rawItemP;
	MSValueDescr uvalue;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_INIT:
	{
		uvalue.SetDouble(g_colorSettingDlgInfo.iColorNum);
		mdlDialog_itemSetValue(NULL, uvalue, dimP->db, dimP->itemIndex);
		double minValue, maxValue, incAmount;
		bool isVertical;
		minValue = 1;
		maxValue = 50;
		incAmount = 1;
		isVertical = true;
		mdlDialog_upDnBtnsSetInfo(&minValue, &maxValue, &incAmount, &isVertical, TRUE, pThis);
		break;
	}
	case DITEM_MESSAGE_BUTTON:
	{
		mdlDialog_itemGetValue(uvalue, dimP->db, dimP->itemIndex);
		g_colorSettingDlgInfo.iColorNum = (int)uvalue.GetDouble();
		updateColorListBox(dimP->db);

		//将应用按钮置为可用
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookText_CellOfColor(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		WCharCP textValue = NULL;
		mdlListCell_getStringValue(pCell, &textValue);

		bool theValue = true;
		dimP->dialogItemP->SetValue(&theValue, textValue);

		break;
	};
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem*		pLBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ColorSet_ColorList, 0);
		int		    	row = -1, col = -1;
		MSValueDescr		uValue;
		ListModel*		pModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
		ListCell*		pCell = NULL;
		ListRow*		pRow = NULL;

		mdlDialog_listBoxGetEditCell(&row, &col, pLBox->rawItemP);
		pRow = mdlListModel_getRowAtIndex(pModel, row);
		pCell = mdlListRow_getCellAtIndex(pRow, col);

		//mdlDialog_itemSetStringValue(&theValue, textValue, dbP, oDialogItemPath->GetItemIndex());
		WString setValue;
		dimP->dialogItemP->GetValue(setValue);

		double t_value = BeStringUtilities::Wtof(setValue.GetWCharCP());

#pragma region 判断列表CELL修改值是否符合要求

		if (row == 0)//第一行
		{
			if (col == 2)//最小值
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//第一行最小值不能比最大值大
				{
					mdlDialog_dmsgsPrint(L"不能大于行最大值！");
					break;
				}
			}
			else
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 2])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能大于行最大值！");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能小于行最小值！");
					break;
				}

			}
		}
		else if (row == s_DCShape_Setting.ColorNum_Legeng - 1)//最后一行
		{
			if (col == 2)//最小值
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//第一行最小值不能比最大值大
				{
					mdlDialog_dmsgsPrint(L"不能大于行最大值！");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row - 1])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能小于行最小值！");
					break;
				}
			}
			else
			{
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能小于行最小值！");
					break;
				}
			}
		}
		else
		{
			if (col == 2)//最小值
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 1])//第一行最小值不能比最大值大
				{
					mdlDialog_dmsgsPrint(L"不能大于行最大值！");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row - 1])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能小于行最小值！");
					break;
				}
			}
			else
			{
				if (t_value > s_DCShape_Setting.ClrShapeVal_Legeng[row + 2])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能大于行最大值！");
					break;
				}
				if (t_value < s_DCShape_Setting.ClrShapeVal_Legeng[row])//第一行最大值不能比第二行最大值大
				{
					mdlDialog_dmsgsPrint(L"不能小于行最小值！");
					break;
				}
			}
		}

#pragma endregion

#pragma region 将修改后的CELL值反写到list中
		if (row == 0)
		{
			if (col == 3)//最大列
			{
				ListCell*		npCell = NULL;
				ListRow*		npRow = NULL;
				npRow = mdlListModel_getRowAtIndex(pModel, row + 1);
				npCell = mdlListRow_getCellAtIndex(npRow, col - 1);
				mdlListCell_setStringValue(npCell, setValue.GetWCharCP(), TRUE);
			}
		}
		else if (row == s_DCShape_Setting.ColorNum_Legeng - 1)
		{
			if (col == 2)//最小列
			{
				ListCell*		tpCell = NULL;
				ListRow*		tpRow = NULL;
				tpRow = mdlListModel_getRowAtIndex(pModel, row - 1);
				tpCell = mdlListRow_getCellAtIndex(tpRow, col + 1);
				mdlListCell_setStringValue(tpCell, setValue.GetWCharCP(), TRUE);
			}
		}
		else
		{
			if (col == 2)//最小列
			{
				ListCell*		tpCell = NULL;
				ListRow*		tpRow = NULL;
				tpRow = mdlListModel_getRowAtIndex(pModel, row - 1);
				tpCell = mdlListRow_getCellAtIndex(tpRow, col + 1);
				mdlListCell_setStringValue(tpCell, setValue.GetWCharCP(), TRUE);
			}
			if (col == 3)//最大列
			{
				ListCell*		npCell = NULL;
				ListRow*		npRow = NULL;
				npRow = mdlListModel_getRowAtIndex(pModel, row + 1);
				npCell = mdlListRow_getCellAtIndex(npRow, col - 1);
				mdlListCell_setStringValue(npCell, setValue.GetWCharCP(), TRUE);
			}
		}
		mdlListCell_setStringValue(pCell, setValue.GetWCharCP(), TRUE);
#pragma endregion

		if (col == 2)
		{
			s_DCShape_Setting.ClrShapeVal_Legeng[row] = t_value;
			if (row == 0)
			{
				s_DCShape_Setting.StartValue = t_value;
			}
		}
		else
		{
			if (row == s_DCShape_Setting.ColorNum_Legeng - 1)
			{
				s_DCShape_Setting.EndValue = t_value;
			}
			s_DCShape_Setting.ClrShapeVal_Legeng[row + 1] = t_value;
		}

		/*dbP->Refresh();
		mdlDialog_itemsSynch(dbP);*/
		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookToggle_ColorSet_Automatic(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem* diP_Widget = NULL;
		
		//当选中AutoMatic时，查询数据库进行色带范围的计算，并将范围控件设为不可用
		if (g_colorSettingDlgInfo.bAutomatic != 0)
		{
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, false, false);
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, false, false);

			s_DCShape_Setting.m_bAutomatic = true;
			
		
			switch (s_sourceDCS)
			{
				//若来源为体展示，则“自动设置”表示从数据库中计算
				case SolidDisplay:
				{
					s_DCShape_Setting.initColorRangeFromDB();
					s_DCShape_Setting.divideColorRange();
				}break;
				//若来源为面展示，则“自动设置”表示从剖面中实际的数据中计算
				case SurfaceDisplay:
				{
					double tStrValue = 0;
					double tEndValue = 0;
					//通过用户的选择获取结果类型
					s_DCShape_Setting.CaseID = s_pDCShape->CaseID;
					s_DCShape_Setting.ProjectId = s_pDCShape->ProjectId;
					DPoint3d minPos, maxPos;
					GetStrValAndEndVal(NGNAME_PREFIX_CAECLIP, tStrValue, tEndValue, s_DCShape_Setting.ProjectId, s_DCShape_Setting.CaseID, s_DCShape_Setting.ResultTypeCode, minPos, maxPos);
					//根据选择的具体工况和结果类型设置色带
					s_DCShape_Setting.StartValue = tStrValue;
					s_DCShape_Setting.EndValue = tEndValue;
					bvector<DPoint3d>().swap(s_DCShape_Setting.m_vecMinPts);
					bvector<DPoint3d>().swap(s_DCShape_Setting.m_vecMaxPts);
					s_DCShape_Setting.m_vecMinPts.push_back(minPos);
					s_DCShape_Setting.m_vecMaxPts.push_back(maxPos);
					s_DCShape_Setting.divideColorRange();
				}
				break;
				//若来源为工况对比，则“自动设置”表示从数据库中计算
				case CaseCompare:
				{
					s_DCShape_Setting.initColorRangeFromDB();
					s_DCShape_Setting.divideColorRange();
				}break;
			default:
				break;
			}

			//将计算后的色带最大值最小值同步到对话框中
			g_colorSettingDlgInfo.dStartValue = s_DCShape_Setting.StartValue;
			g_colorSettingDlgInfo.dEndValue = s_DCShape_Setting.EndValue;			
		}
		//当不选中AutoMatic时，开放范围控件
		else
		{
			s_DCShape_Setting.m_bAutomatic = false;
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_StartValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, true, false);
			diP_Widget = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ColorSet_EndValue, 0);
			mdlDialog_itemSetEnabledState(dimP->db, diP_Widget->itemIndex, true, false);
		}

		//将应用按钮置为可用
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
	}break;
	default:
		dimP->msgUnderstood = FALSE;
	}
}
void HookPushBtn_ColorSet_Gray(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		if (g_colorSettingDlgInfo.Gray != 0)
		{
			s_DCShape_Setting.m_Gray = true;
		}
		else
		{
			s_DCShape_Setting.m_Gray = false;
		}

		//当颜色范围&颜色数控件值被修改时，刷新ListBox的显示
		updateColorListBox(dimP->db);
	}
	default:
		dimP->msgUnderstood = FALSE;
	}
}
void HookText_ColorSet_ColorValue(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		//当颜色范围&颜色数控件值被修改时，刷新ListBox的显示
		updateColorListBox(dimP->db);

		//同时修改UpDownButton中保存的值
		DialogItem* diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_UpDownButtons, BUTTONID_UpDown, 0);
		if (diP == NULL)
			return;

		MSValueDescr uvalue;
		uvalue.SetDouble(g_colorSettingDlgInfo.iColorNum);
		mdlDialog_itemSetValue(NULL, uvalue, dimP->db, diP->itemIndex);
		
		//将应用按钮置为可用
		diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBTNID_ColorSet_Apply, 0);
		mdlDialog_itemSetEnabledState(dimP->db, diP->itemIndex, true, false);
		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ColorSet_Apply(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//将设置色带复制给实际的色带对象
		*s_pDCSSetting = s_DCShape_Setting;

		//根据当前是否已经打开色带，进行云图展示
		switch (s_sourceDCS)
		{
		case SolidDisplay:
			if (s_pDCSSetting->m_bDisplayed)
				startShowContour(s_pDCSSetting);
			break;
		case SurfaceDisplay:
		if (s_pDCSSetting->m_bDisplayed)
				startShowCutFaceContour(s_pDCSSetting);
			break;
		case CaseCompare:
		{
			if (s_DrawColorShape.m_bDisplayed && s_DrawColorShape_temp.m_bDisplayed)
			{
				startShowContour(&s_DrawColorShape);
				startShowContour(&s_DrawColorShape_temp, &s_SecCountourBasePt, false);
			}
		}break;
		default:
			break;
		}		

		//将应用设为不可用
		mdlDialog_itemSetEnabledState(dimP->db, dimP->itemIndex, false, false);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void openColorShapeSetting(DrawColorShape* optDCS, SourceTypeDCS source)
{
	s_pDCSSetting = optDCS;
	s_DCShape_Setting = *optDCS;
	s_sourceDCS = source;

	//使用传入的色带对象初始化“colorSetting对话框”
	g_colorSettingDlgInfo.dStartValue = s_DCShape_Setting.StartValue;
	g_colorSettingDlgInfo.dEndValue = s_DCShape_Setting.EndValue;
	g_colorSettingDlgInfo.iColorNum = s_DCShape_Setting.ColorNum_Legeng;
	g_colorSettingDlgInfo.bAutomatic = s_DCShape_Setting.m_bAutomatic ? -1 : 0;
	g_colorSettingDlgInfo.Gray= s_DCShape_Setting.m_Gray ? -1 : 0;
	int lastAction = -1;
	mdlDialog_openModal(&lastAction, NULL, DIALOGID_SetContourColor);

	//根据设置对话框的返回值进行相应的操作
	if (lastAction == ACTIONBUTTON_OK)
	{
		//将设置色带复制给实际的色带对象
		*s_pDCSSetting = s_DCShape_Setting;

		//根据当前是否已经打开色带，进行云图展示
		switch (s_sourceDCS)
		{
		case SolidDisplay:
			if (s_pDCSSetting->m_bDisplayed)
				startShowContour(s_pDCSSetting);
			break;
		case SurfaceDisplay:
		if (s_pDCSSetting->m_bDisplayed)
				startShowCutFaceContour(s_pDCSSetting);
			break;
		case CaseCompare:
		{
			if (s_DrawColorShape.m_bDisplayed && s_DrawColorShape_temp.m_bDisplayed)
			{
				startShowContour(&s_DrawColorShape);
				startShowContour(&s_DrawColorShape_temp, &s_SecCountourBasePt, false);
			}
		}break;
		default:
			break;
		}
	}
}
