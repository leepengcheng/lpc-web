#include "MsdiMsCaeSipch.h"

#define PROJECT_ID_SELECTED g_expCalculationInfo.proIndex == -1 ? -1 : readInstanceInfo().at(g_expCalculationInfo.proIndex).projectId

extern bool writeCalculationDocument(const char path[MAXFILELENGTH]);

//计算书生成窗口相关控件对应结构体
extern ExpCalculationDlgInfo g_expCalculationInfo;

//储存不同类别的图片路径名链表
list<wstring> lstModelPaths;			//计算网格模型图
list<wstring> lstWYCloudPaths;			//位移云图&等值线图
list<wstring> lstYLCloudPaths;			//应力云图&等值线图
list<wstring> lstYLPaths;				//应力数值图

//参数表ListBox对应数据
ListBoxDatas	lbd_CaseList;
ListBoxDatas	lbd_ModelParamList;
ListBoxDatas	lbd_WYParamList;
ListBoxDatas	lbd_YLParamList;
ListBoxDatas	lbd_PJParamList;

//计算书输出对话框的钩子函数
void HookDialog_expCalc(DialogMessage* dmP)
{
	dmP->msgUnderstood = TRUE;

	switch (dmP->messageType)
	{
		case DIALOG_MESSAGE_INIT:
		{
			//清空内存中残留的相关数据
			lstModelPaths.clear();
			lstWYCloudPaths.clear();
			lstYLCloudPaths.clear();
			lstYLPaths.clear();

			strcpy(g_expCalculationInfo.proDescr, "");

			mdlDialog_itemsSynch(dmP->db);
		}
		break;
		default:
			dmP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExpCalc_Import(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		BeFileName	path;
		if (SUCCESS == mdlDialog_fileOpen(path, NULL, 0, L"", L"*.docx", L"d:\\", L"请选择工程概述文档"))
		{			
			ifstream in(ws2s(path.GetName()).data());
			string buffer;
			int index = 0;
			bool outOfRange = false;
			while (!in.eof())
			{
				in.get(g_expCalculationInfo.proDescr[index++]);				
				if (index == 4090)
					break;			
			}

			g_expCalculationInfo.proDescr[index++] = '\0';
			WString wsProDescr;
			wsProDescr.AppendA(g_expCalculationInfo.proDescr);
			DialogItem	*dip = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_MultilineText, MLTEXTID_ExportCalc_ProjectDescr, 0);
			MSValueDescr value;
			value.SetWChar(wsProDescr.data());
			mdlDialog_rItemValueSet(NULL, value, dip->rawItemP);
			mdlDialog_itemsSynch(dimP->db);
		}		
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

//插入图片路径按钮的钩子函数
void HookPushBtn_ExpCalc_AddPath(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON:
		{
			//打开文件选择对话框供用户选择
			//用户选取Excel文件路径
			BeFileName    path;

			if (SUCCESS == mdlDialog_fileOpen(path, NULL, 0, L"", L"*.bmp", L"d:\\", L"请选择图片来源导入"))
			{
				//根据钩子函数的来源判断对哪一个LISTBOX进行操作
				list<wstring>* optLstR = NULL;
				RscId btnID = dimP->dialogItemP->GetId();
				RscId lBoxID = 0;
				switch (btnID)
				{
				case PUSHBTNID_ExportCalc_AddModelPath:
					optLstR = &lstModelPaths;
					lBoxID = LISTBOXID_ExportCalc_ModelPaths;
					break;
				case PUSHBTNID_ExportCalc_AddWYCloudPath:
					optLstR = &lstWYCloudPaths;
					lBoxID = LISTBOXID_ExportCalc_WYCloudPaths;
					break;
				case PUSHBTNID_ExportCalc_AddYLCloudPath:
					optLstR = &lstYLCloudPaths;
					lBoxID = LISTBOXID_ExportCalc_YLCloudPaths;
					break;
				case PUSHBTNID_ExportCalc_AddYLPath:
					optLstR = &lstYLPaths;
					lBoxID = LISTBOXID_ExportCalc_YLPaths;
					break;
				default:
					break;
				}

				//获取对应listBox的listModel
				Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lBoxID, 0);
				if (listBox != NULL)
				{
					ListModel*	pListModel = mdlDialog_listBoxGetListModelP(listBox->GetRawItem());
					if (pListModel == NULL)
						pListModel = mdlListModel_create(MAX_COLUMN_COUNT);

					//需要得到listBox的行数来确定第一列序号的值
					int rowCnt = mdlListModel_getRowCount(pListModel);

					//创建新行，第一列为序号，第二列为图片路径
					MSValueDescr cellValue;
					ListRowP row = mdlListRow_create(pListModel);
					ListCellP cell = mdlListRow_getCellAtIndex(row, 0);
					wstring rowNO = num2Ws(rowCnt + 1).data();
					cellValue.SetWChar(rowNO.data());
					mdlListCell_setValue(cell, cellValue, TRUE);

					cell = mdlListRow_getCellAtIndex(row, 1);
					cellValue.SetWChar(path.GetName());
					mdlListCell_setValue(cell, cellValue, TRUE);

					mdlListModel_addRow(pListModel, row);

					//更新ListBox的显示
					mdlDialog_listBoxSetListModelP(listBox->GetRawItem(), pListModel, 0);

					//同时更新内存中对应的链表
					wstring str_path = path.GetName();
					optLstR->push_back(str_path);
				}
			}
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExcCalc_DelPath(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_BUTTON:
	{
		//根据钩子函数的来源判断对哪一个LISTBOX进行操作
		list<wstring>* optLstR = NULL;
		RscId btnID = dimP->dialogItemP->GetId();
		RscId lBoxID = 0;
		switch (btnID)
		{
		case PUSHBTNID_ExportCalc_DelModelPath:
			optLstR = &lstModelPaths;
			lBoxID = LISTBOXID_ExportCalc_ModelPaths;
			break;
		case PUSHBTNID_ExportCalc_DelWYCloudPath:
			optLstR = &lstWYCloudPaths;
			lBoxID = LISTBOXID_ExportCalc_WYCloudPaths;
			break;
		case PUSHBTNID_ExportCalc_DelYLCloudPath:
			optLstR = &lstYLCloudPaths;
			lBoxID = LISTBOXID_ExportCalc_YLCloudPaths;
			break;
		case PUSHBTNID_ExportCalc_DelYLPath:
			optLstR = &lstYLPaths;
			lBoxID = LISTBOXID_ExportCalc_YLPaths;
			break;
		default:
			break;
		}

		//获取对应listBox的listModel
		Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lBoxID, 0);
		if (listBox != NULL)
		{
			ListModel*	pListModel = mdlDialog_listBoxGetListModelP(listBox->GetRawItem());
			if (pListModel != NULL)
			{
				//获取指定listBox当前选择的行数
				int row = -1, col = -1;
				mdlDialog_listBoxGetLocationCursor(&row, &col, listBox->rawItemP);

				if (row != -1)
				{
					mdlListModel_removeRowAtIndex(pListModel, row, true);
					mdlDialog_listBoxSetListModelP(listBox->rawItemP, pListModel, 0);
					mdlDialog_itemsSynch(dimP->db);

					list<wstring>::iterator iter = (*optLstR).begin();
					advance(iter, row);
					(*optLstR).erase(iter);
				}
			}
		}
	}
	break;
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookPushBtn_ExpCalc_Submit(DialogItemMessage *dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_BUTTON:
		{
			//生成计算文档
			BeFileName    path;

			if (SUCCESS == mdlDialog_fileCreate(path, NULL, 0, L"cal.docx", L"*.docx", L"d:\\", L"请选择计算书输出路径"))
			{
				wstring wsPath = path.GetName();
				
				if (writeCalculationDocument(ws2s(wsPath).data()))
				{
					mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"计算文档生成完毕！", MessageBoxIconType::Information);
				}
				else
				{
					mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, L"计算文档生成失败！", MessageBoxIconType::Critical);
				}
			}

		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}


void HookListBox_ExpCalc_CaseList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->GetRawItem();
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//工况及载荷数据初始化
		case DITEM_MESSAGE_INIT:
		{
			//当前工程ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_CaseList);
		}
		break;
		//将数据显示到ListBox中
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_CaseList.needFresh)
				initDatasIntoListBox(pThis, lbd_CaseList);
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_ModelParamList(DialogItemMessage* dimP)
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
		}
		break;
		//计算模型参数表数据初始化
		case DITEM_MESSAGE_INIT:
		{
			//当前工程ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_ModelParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_ModelParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_ModelParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_WYParamList(DialogItemMessage* dimP)
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
		}
		break;
		//位移数据初始化
		case DITEM_MESSAGE_INIT:
		{
			//当前工程ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_WYParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_WYParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_WYParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_YLParamList(DialogItemMessage* dimP)
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
		}
		break;
		//应力数据初始化
		case DITEM_MESSAGE_INIT:
		{		
			//当前工程ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_YLParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_YLParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_YLParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookListBox_ExpCalc_PJParamList(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT_REBAR);
			mdlDialog_listBoxSetListModelP(pThis, pListModel, 0);
		}
		break;
		//配筋参数表数据初始化
		case DITEM_MESSAGE_INIT:
		{			
			//当前工程ID
			initDatasFromDB(PROJECT_ID_SELECTED, LISTBOXID_ExportCalc_PJParamList);
		}
		break;
		case DITEM_MESSAGE_DRAW:
		{
			if (lbd_PJParamList.needFresh)
				initDatasIntoListBox(pThis, lbd_PJParamList);
		}break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookMlText_ExpCalc_ProDescr(DialogItemMessage* dimP)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_FOCUSOUT:
		{
			MSValueDescr value;
			mdlDialog_rItemValueGet(value, dimP->dialogItemP->rawItemP);
			wstring ws = value.GetWCharCP();
		
			strcpy(g_expCalculationInfo.proDescr, ws2s(ws).data());
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void HookComboBox_ExpCalc_ProjectLists(DialogItemMessage* dimP)
{
	RawItemHdr*	pThis = dimP->dialogItemP->rawItemP;
	ListModel*	pListModel = NULL;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
		case DITEM_MESSAGE_CREATE:
		{
			pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
			mdlDialog_comboBoxSetListModelP(pThis, pListModel);
		}
		break;
		case DITEM_MESSAGE_INIT:
		{

			pListModel = mdlDialog_comboBoxGetListModelP(pThis);

			ListRowP	pRow = NULL;
			ListCellP	cell = NULL;
			MSValueDescr cellValue;
		
			//实例列表comboBox中显示实例名，并保存其对应的工程ID
			bvector<InstanceInfo> instanceInfos = readInstanceInfo();
			for (int i = 0; i < (int)instanceInfos.size(); i++)
			{
				pRow = mdlListRow_create(pListModel);
				cell = mdlListRow_getCellAtIndex(pRow, 0);

				//设置对应的实例名作为显示
				WString ngName = instanceInfos[i].NgName;
				WString instanceName;
				int projectID;
				splitNgName_Model(ngName, UNUSE_WSTRING, projectID, UNUSE_INT, instanceName, UNUSE_WSTRING);
				mdlListCell_setDisplayText(cell, instanceName.data());

				//对应的工程ID保存在cell中
				cellValue.SetInt32(projectID);
				mdlListCell_setValue(cell, cellValue, FALSE);

				mdlListModel_addRow(pListModel, pRow);
				g_expCalculationInfo.proIndex = 0;
			}

			mdlDialog_comboBoxSetListModelP(pThis, pListModel);

			//**此处编码计算书输出对话框中Text控件的初始化
			if (instanceInfos.empty())
				g_expCalculationInfo.proIndex = -1;

			//节点数
			g_expCalculationInfo.nNodeCnt = getNodeCntByProjectid(PROJECT_ID_SELECTED);

			//总单元数
			g_expCalculationInfo.nCellCnt = getElemCntByProjectid(PROJECT_ID_SELECTED);

			mdlDialog_itemsSynch(dimP->db);
		}
		break;
		case DITEM_MESSAGE_STATECHANGED:
		{
			//文本控件刷新
			//节点数
			g_expCalculationInfo.nNodeCnt = getNodeCntByProjectid(PROJECT_ID_SELECTED);

			//总单元数
			g_expCalculationInfo.nCellCnt = getElemCntByProjectid(PROJECT_ID_SELECTED);

			//重新初始化ListBoxData数据并强制draw
			for (RscId lb = LISTBOXID_ExportCalc_CaseList; lb <= LISTBOXID_ExportCalc_PJParamList; lb++)
			{
				initDatasFromDB(PROJECT_ID_SELECTED, lb);
				Bentley::DialogItemP listBox = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, lb, 0);
				if (listBox != NULL)
					mdlDialog_itemDraw(dimP->db, listBox->itemIndex);
			}

			mdlDialog_itemsSynch(dimP->db);
		}
		break;
		default:
			dimP->msgUnderstood = FALSE;
		break;
	}
}

void initDatasIntoListBox(RawItemHdrP listBox, ListBoxDatas& datas)
{
	if (listBox == NULL)
		return;

	//执行更新ListBox操作，将对应lbd更新位置为false防止无限递归
	datas.needFresh = false;

	//根据RawItem获取对应的ListModel同时清空其数据
	ListModelP pListModel = mdlDialog_listBoxGetListModelP(listBox);
	if (pListModel == NULL)
		pListModel = mdlListModel_create(MAX_COLUMN_COUNT);
	else
		mdlListModel_empty(pListModel, true);

	if (datas.rows.empty())
		return;

	//遍历datas中的数据填充到ListModel中
	ListRowP	row = NULL;
	ListCellP	cell = NULL;
	MSValueDescr cellValue;

	for (int i = 0; i < (int)datas.rows.size(); i++)
	{
		bvector<WString>& rowData = datas.rows[i];

		row = mdlListRow_create(pListModel);

		for (int j = 0; j < (int)rowData.size(); j++)
		{
			cell = mdlListRow_getCellAtIndex(row, j);
			cellValue.SetWChar(rowData[j].data());
			mdlListCell_setValue(cell, cellValue, TRUE);
		}

		mdlListModel_addRow(pListModel, row);
	}

	//更新ListBox的显示
	mdlDialog_listBoxSetListModelP(listBox, pListModel, 0);

}

void initDatasFromDB(int iProjectID, RscId listBoxID)
{
	switch (listBoxID)
	{
		case LISTBOXID_ExportCalc_CaseList:
		{
			lbd_CaseList.rows.clear();

			//查询工程ID下的工况名和主要载荷（Property）
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			//将数据填充到结构体中
			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.工况名
				rowData.push_back(vecCaseNames[i].data());

				//2.主要载荷
				rowData.push_back(vecCaseProps[i].data());

				lbd_CaseList.rows.push_back(rowData);
			}

			//初始进行一次刷新
			lbd_CaseList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_ModelParamList:
		{
			lbd_ModelParamList.rows.clear();

			//查询工程ID下的所有材料ID
			bvector<int> vecMaterialIDs;
			getMaterialIDByProjectid(iProjectID, vecMaterialIDs);

			//遍历所有材料ID，填充其对应数据
			for (int i = 0; i < (int)vecMaterialIDs.size(); i++)
			{
				int mID = vecMaterialIDs[i];

				//根据材料ID与工程ID获取对应材料的属性
				WString mName, mType, mEX, mNUXY, mDENS, mPRXY;
				getMaterialAllByPK(iProjectID, mID, mName, mType, mEX, mNUXY, mDENS, mPRXY);

				//计算该工程中使用该材料ID的单元数
				int ElemCnt = getElemCntUsingMaterialByProjectid(iProjectID, mID);

				bvector<WString> rowData;

				//1.材料号
				WString mNumber = num2Ws(mID);
				rowData.push_back(mNumber);

				//2.材料属性
				//Type-Name拼成材料属性
				if (mType.length() > 0)
					mType.append(L"-");
				mType.append(mName);
				rowData.push_back(mType);

				//3.单元数
				WString wsElemCnt = num2Ws(ElemCnt);
				rowData.push_back(wsElemCnt);

				//4.弹性模量EX
				rowData.push_back(mEX);

				//5.泊松比NUXY
				rowData.push_back(mNUXY);

				//6.密度DENS
				rowData.push_back(mDENS);

				lbd_ModelParamList.rows.push_back(rowData);
			}

			//初始进行一次刷新
			lbd_ModelParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_WYParamList:
		{
			lbd_WYParamList.rows.clear();

			//查询工程ID下的工况名和主要载荷（Property）
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			bvector<int> nodeLst;
			double maxValue;
			wostringstream oss;

			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.工况名
				rowData.push_back(vecCaseNames[i].data());
				
				//2.最大合位移(数值&节点号)
				WString maxWY_SUM;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "USUM");
				if (nodeLst.front() == -1)
					maxWY_SUM = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxWY_SUM.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_SUM);

				//3.最大X向位移
				WString maxWY_X;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UX");
				if (nodeLst.front() == -1)
					maxWY_X = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxWY_X.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_X);

				//4.最大Y向位移
				WString maxWY_Y;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UY");
				if (nodeLst.front() == -1)
					maxWY_Y = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxWY_Y.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_Y);

				//5.最大Z向位移
				WString maxWY_Z;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "UZ");
				if (nodeLst.front() == -1)
					maxWY_Z = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxWY_Z.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxWY_Z);

				lbd_WYParamList.rows.push_back(rowData);
			}

			//初始进行一次刷新
			lbd_WYParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_YLParamList:
		{
			lbd_YLParamList.rows.clear();

			//查询工程ID下的工况名和主要载荷（Property）
			bvector<int> vecCaseIDs;
			bvector<WString> vecCaseNames;
			bvector<WString> vecCaseProps;
			getCaseInfoByProjectid(iProjectID, vecCaseIDs, vecCaseNames, vecCaseProps);

			bvector<int> nodeLst;
			double maxValue;
			wostringstream oss;

			for (int i = 0; i < (int)vecCaseNames.size(); i++)
			{
				bvector<WString> rowData;
				//1.工况名
				rowData.push_back(vecCaseNames[i].data());

				//2.X向最大应力(数值&节点号)
				WString maxYL_X;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SX");
				if (nodeLst.front() == -1)
					maxYL_X = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxYL_X.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_X);

				//3.Y向最大应力
				WString maxYL_Y;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SY");
				if (nodeLst.front() == -1)
					maxYL_Y = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxYL_Y.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_Y);

				//4.Z向最大应力
				WString maxYL_Z;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "SZ");
				if (nodeLst.front() == -1)
					maxYL_Z = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxYL_Z.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_Z);

				//5.第一主应力
				WString maxYL_1;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "S1");
				if (nodeLst.front() == -1)
					maxYL_1 = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxYL_1.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_1);

				//6.第三主应力
				WString maxYL_3;
				getExtremumResultAndNode(iProjectID, vecCaseIDs[i], nodeLst, maxValue, "S3");
				if (nodeLst.front() == -1)
					maxYL_3 = L" ";
				else
				{
					oss << scientific << maxValue << L"\n节点号：" << nodeLst.front();
					maxYL_3.append(oss.str().data());
					oss.str(L"");
				}
				rowData.push_back(maxYL_3);

				lbd_YLParamList.rows.push_back(rowData);
			}
			//初始进行一次刷新
			lbd_YLParamList.needFresh = true;
		}
		break;
		case LISTBOXID_ExportCalc_PJParamList:
		{
			lbd_PJParamList.rows.clear();

			//取得所有的配筋结果
			bmap<WString, ReBarInfo> allRebarResults;
			readRebarResultsFromX(allRebarResults);

			//根据iProjectID查找对应结果
			bmap<WString, ReBarInfo> currentResults;
			for (auto it = allRebarResults.begin(); it != allRebarResults.end();++it)
			{
				if (iProjectID == it->second.projectID)
				{
					currentResults[it->first] = it->second;
				}
			}
			allRebarResults.empty();

			for (auto it = currentResults.begin(); it != currentResults.end(); ++it)
			{
				ReBarInfo info = it->second;
				WString value;
				bvector<WString> rowData;

				//1 工况名
				rowData.push_back(info.caseName);
				//2 截面名
				rowData.push_back(info.sectionName);
				//3 配筋线名
				rowData.push_back(info.rebarName);
				//4 结构系数
				value.Sprintf(L"%.2g", info.dStruFt);
				rowData.push_back(value);
				//5 结构重要性系数
				value.Sprintf(L"%.2g", info.dStruSignifyFt);
				rowData.push_back(value);
				//6 设计状况系数
				value.Sprintf(L"%.2g", info.dDesignFt);
				rowData.push_back(value);
				//7 钢筋抗拉强度 
				value.Sprintf(L"%.3g", info.dRebarTesion);
				rowData.push_back(value);
				//8 混凝土抗拉强度
				value.Sprintf(L"%.3g", info.dConcrTesion);
				rowData.push_back(value);
		
				bvector<WString> vResult;
				BeStringUtilities::Split(it->second.strResult, SeparatorAnd, vResult);
				size_t resultNumber = vResult.size();
				if (resultNumber > 13)
				{
					//9 总拉应力面积 
					rowData.push_back(vResult[CALCRESULT_IDX_A]);
					//10 混凝土拉应力面积
					rowData.push_back(vResult[CALCRESULT_IDX_ACT]);
					//11 计算配筋面积
					rowData.push_back(vResult[CALCRESULT_IDX_AS]);
					//12 参考配筋
					value.Sprintf(L"%d根%g", info.iRebarCount, info.dRebarDiameter);
					rowData.push_back(value);
					//13 弯矩
					rowData.push_back(vResult[CALCRESULT_IDX_BENDING]);
					//14 轴力
					rowData.push_back(vResult[CALCRESULT_IDX_AXIAL]);
					//15 XY方向剪力
					rowData.push_back(vResult[CALCRESULT_IDX_XYSHEAR]);
					//16 YZ方向剪力
					rowData.push_back(vResult[CALCRESULT_IDX_YZSHEAR]);
					//17 XZ方向剪力
					rowData.push_back(vResult[CALCRESULT_IDX_XZSHEAR]);
				}
				else
				{
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
					rowData.push_back(L"");
				}

				lbd_PJParamList.rows.push_back(rowData);
			}

			//初始进行一次刷新
			lbd_PJParamList.needFresh = true;

			break;
		}
		default:
		break;
	}
}

void exportCalDocCmd(char* unparsed)
{
	MSDialog *Dlialog = mdlDialog_find(DIALOGID_ExportCalculation, NULL);
	if (Dlialog)
		mdlDialog_show(Dlialog);
	else
		mdlDialog_open(NULL, DIALOGID_ExportCalculation);
}