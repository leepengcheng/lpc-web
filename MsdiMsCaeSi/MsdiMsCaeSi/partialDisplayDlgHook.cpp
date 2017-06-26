#include "MsdiMsCaeSipch.h"

extern bvector<int> pd_id_Ma, pd_id_Et, pd_id_Rc;
extern bvector<WString> pd_val_Ma, pd_val_Et, pd_val_Rc;

extern PartialDisDlgInfo	g_partialDisDlgInfo;

bvector<Condition> pd_ConditionLst;

bool matchCondition(MeshProperty& prop, Condition& condition)
{
	//根据condition条件名和条件值下标初始化左右值
	int* leftVal, *rightVal;
	switch (condition.iName)
	{
		//单元类型
	case 0:
	{
		leftVal = &(prop.ElemType);
		rightVal = &(pd_id_Et.at(condition.iVal));
	}break;
	//材料类型
	case 1:
	{
		leftVal = &(prop.MaterialNumber);
		rightVal = &(pd_id_Ma.at(condition.iVal));
	}
	break;
	//实常数
	case 2:
	default:
	{
		leftVal = &(prop.RealConstantNumber);
		rightVal = &(pd_id_Rc.at(condition.iVal));
	}
	break;
	}

	//根据condition条件操作符进行左右值比较
	bool bEqual = *rightVal == *leftVal ? true : false;
	//==
	if (condition.iOpt == 0)
		return bEqual;
	//<>
	else
		return !bEqual;
}

void HookComboBox_PartDis_CondName(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		g_partialDisDlgInfo.indexCName = 0;
		g_partialDisDlgInfo.indexCOpt = 0;
		g_partialDisDlgInfo.indexCVal = 0;

		pd_ConditionLst.clear();
	}
	break;
	case DITEM_MESSAGE_STATECHANGED:
	{
		DialogItem* dip_cValue = NULL;
		ListModel*	pListModel = NULL;

		dip_cValue = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ComboBox, COMBOBOXID_ConditionVal, 0);

		if (dip_cValue == NULL)
			return;
			
		//根据当前选择的筛选条件初始化筛选值,初始显示筛选条件0（单元类型）
		RawItemHdr*	rItem_cValue = dip_cValue->rawItemP;
		pListModel = mdlDialog_comboBoxGetListModelP(rItem_cValue);

		mdlListModel_empty(pListModel, true);

		bvector<WString>* pCValues = NULL;
		switch (g_partialDisDlgInfo.indexCName)
		{
			//单元类型
		case 0:
			pCValues = &pd_val_Et;
			break;
			//材料类型
		case 1:
			pCValues = &pd_val_Ma;
			break;
			//实常数
		case 2:
		default:
			pCValues = &pd_val_Rc;
			break;
		}

		ListRowP	pRow = NULL;
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		for (int i = 0; i < (int)(*pCValues).size(); i++)
		{
			pRow = mdlListRow_create(pListModel);
			cell = mdlListRow_getCellAtIndex(pRow, 0);
			cellValue.SetWChar((*pCValues).at(i).data());
			mdlListCell_setValue(cell, cellValue, TRUE);
			mdlListModel_addRow(pListModel, pRow);
		}

		mdlDialog_comboBoxSetListModelP(rItem_cValue, pListModel);

		g_partialDisDlgInfo.indexCVal = 0;

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_PartDis_CondVal(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	break;
	case DITEM_MESSAGE_CREATE:
	{
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
		mdlDialog_comboBoxSetListModelP(pThis, pListModel);
	}
	case DITEM_MESSAGE_INIT:
	{
		ListModel*	pListModel = NULL;

		//根据当前选择的筛选条件初始化筛选值,初始显示筛选条件0（单元类型）
		RawItemHdr*	rItem_cValue = dimP->dialogItemP->rawItemP;
		pListModel = mdlDialog_comboBoxGetListModelP(rItem_cValue);
		mdlListModel_empty(pListModel, true);

		ListRowP	pRow = NULL;
		ListCellP	cell = NULL;
		MSValueDescr cellValue;

		for (int i = 0; i < (int)pd_val_Et.size(); i++)
		{
			pRow = mdlListRow_create(pListModel);
			cell = mdlListRow_getCellAtIndex(pRow, 0);
			cellValue.SetWChar(pd_val_Et.at(i).data());
			mdlListCell_setValue(cell, cellValue, TRUE);
			mdlListModel_addRow(pListModel, pRow);
		}

		mdlDialog_comboBoxSetListModelP(rItem_cValue, pListModel);

		mdlDialog_itemsSynch(dimP->db);
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_PartDis_Add(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
		{
			DialogItem* dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ConditionList, 0);
			if (dip_cList == NULL)
				return;

			ListModel*	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);

			ListRowP	pRow = mdlListRow_create(pListModel);
			ListCellP	cell = NULL;
			WString cellWs;
			MSValueDescr cellValue;

			RscFileHandle   rfHandle;
			mdlResource_openFile(&rfHandle, L"partialDisplayDlg.r", RSC_READONLY);
			StringListP sl = NULL;

			//筛选条件
			cell = mdlListRow_getCellAtIndex(pRow, 0);
			sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_ConditionType);
			mdlStringList_getMemberString(cellWs, NULL, sl, (long)g_partialDisDlgInfo.indexCName);
			cellValue.SetWChar(cellWs.data());
			mdlListCell_setValue(cell, cellValue, TRUE);
			mdlStringList_destroy(sl);

			//筛选操作符
			cell = mdlListRow_getCellAtIndex(pRow, 1);
			sl = mdlStringList_loadResource(rfHandle, STRINGLISTID_Condition);
			mdlStringList_getMemberString(cellWs, NULL, sl, (long)g_partialDisDlgInfo.indexCOpt);
			cellValue.SetWChar(cellWs.data());
			mdlListCell_setValue(cell, cellValue, TRUE);
			mdlStringList_destroy(sl);

			//筛选值
			cell = mdlListRow_getCellAtIndex(pRow, 2);
			bvector<WString>* pCValues = NULL;
			switch (g_partialDisDlgInfo.indexCName)
			{
				//单元类型
			case 0:
				pCValues = &pd_val_Et;
				break;
				//材料类型
			case 1:
				pCValues = &pd_val_Ma;
				break;
				//实常数
			case 2:
			default:
				pCValues = &pd_val_Rc;
				break;
			}

			cellValue.SetWChar(pCValues->at(g_partialDisDlgInfo.indexCVal).data());
			mdlListCell_setValue(cell, cellValue, TRUE);

			mdlListModel_addRow(pListModel, pRow);

			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);

			Condition newCon;
			newCon.iName = g_partialDisDlgInfo.indexCName;
			newCon.iOpt = g_partialDisDlgInfo.indexCOpt;
			newCon.iVal = g_partialDisDlgInfo.indexCVal;
			pd_ConditionLst.push_back(newCon);

			mdlDialog_itemsSynch(dimP->db);
		}
		break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_PartDis_Del(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//获取用户选择的行数，删除指定筛选条件
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ConditionList, 0);
		ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
		int row = -1, col = -1;
		mdlDialog_listBoxGetLocationCursor(&row, &col, dip_cList->rawItemP);
		
		if (row != -1)
		{
			mdlListModel_removeRowAtIndex(pListModel, row, true);
			mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
			mdlDialog_itemsSynch(dimP->db);

			bvector<Condition>::iterator iter = pd_ConditionLst.begin();
			advance(iter, row);
			pd_ConditionLst.erase(iter);
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookButton_PartDis_Clear(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//清空列表中的所有筛选条件
		DialogItem	*dip_cList = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_ConditionList, 0);
		ListModelP	pListModel = mdlDialog_listBoxGetListModelP(dip_cList->rawItemP);
		
		mdlListModel_empty(pListModel, true);
		mdlDialog_listBoxSetListModelP(dip_cList->rawItemP, pListModel, 0);
		mdlDialog_itemsSynch(dimP->db);

		pd_ConditionLst.clear();
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_PartDis_Conditions(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}