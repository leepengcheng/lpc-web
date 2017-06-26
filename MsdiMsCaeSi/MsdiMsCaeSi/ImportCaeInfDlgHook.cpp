#include "MsdiMsCaeSipch.h"

extern ImpInfDlgInfo  g_ImpDlgInfo;
extern ClipResultDlgInfo g_clipResultDlgInfo;
bset<BeFileName> oAddResultFileSet;
BeFileName sModelFileName;

/*----------------------------------------------------------------------+
* The command entry point for user function.				            *
* @param        unparsed     unused unparsed argument to the command    *
+----------------------------------------------------------------------*/
void ImportCAEInfCmd(WCharCP unparsed)
{
	int lastAction = -1;

	g_ImpDlgInfo.MeshModelVal = 1;
	g_ImpDlgInfo.ResultVal = 1;

	/* open child modal dialog box */
	if (mdlDialog_openModal(&lastAction, NULL, DIALOGID_ImportCaeInf))
	{
		//mdlDialog_dmsgsPrint("Unable to open modal");
	}

	// 当点击OK时，处理
	if (lastAction == ACTIONBUTTON_OK)
	{
		ImportCAEInf *oImpCAEInf = new ImportCAEInf();
		oImpCAEInf->ImportCAEResult(sModelFileName, oAddResultFileSet, g_ImpDlgInfo.ProjectId);
	}
}

bool SelectFilePath(BeFileNameR File_path, int Title_ID, int FilterString_ID)
{
	WChar dialogTitle[128], filterString[256];
	bool bRet = false;
	int result;

	mdlResource_loadFromStringList(dialogTitle, NULL, STRINGLISTID_Prompts, Title_ID);
	mdlResource_loadFromStringList(filterString, NULL, STRINGLISTID_Prompts, FilterString_ID);

	// 打开文件选取对话框
	result = mdlDialog_fileOpen(File_path, 0, 0, g_clipResultDlgInfo.fileOpenPath, filterString, L"", dialogTitle);
	if (result == SUCCESS)
	{
		// 保存文件的打开路径，下次打开文件时，这个路径为默认路径
		swprintf(g_clipResultDlgInfo.fileOpenPath, L"%s", BeFileName::GetDirectoryName(File_path.data()));

		bRet = true;
	}

	return bRet;
}

void SelectCaeModelPath(DialogItemMessage *dimP)
{
	DialogItem     *diPTxtCaeModel;

	dimP->msgUnderstood = TRUE;

	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		if (true == SelectFilePath(sModelFileName, MESSAGEID_ModelDataDialogTitle, MESSAGEID_ModelDataFilterString))
		{
			diPTxtCaeModel = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ImportCaeModel, 0);
			diPTxtCaeModel->SetValue(NULL, sModelFileName);
		}

		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ImportCaeInfDialog_AddResultFileHook(DialogItemMessage *dimP)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	ListRow		*pRow = NULL;
	ListCell	*pCell = NULL;
	BeFileName sResultFileName;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		if (true == SelectFilePath(sResultFileName, MESSAGEID_ResultDataDialogTitle, MESSAGEID_ResultDataFilterString))
		{
			bset<BeFileName>::iterator iter;
			iter = oAddResultFileSet.find(sResultFileName);
			if (oAddResultFileSet.size() > 0 && iter != oAddResultFileSet.end())
			{
				WString sMsg;
				sMsg.append(L"[");
				sMsg.append(sResultFileName.c_str());
				sMsg.append(L"]\n该计算结果文件已存在列表中！");
				//mdlDialog_dmsgsPrint(sMsg.c_str());
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, sMsg.c_str(), MessageBoxIconType::Warning);
			}
			else
			{
				diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_CaeResultFile, 0);
				pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
				if (pListModel == NULL)
					pListModel = mdlListModel_create(1);

				// 生成ListBox的行
				pRow = mdlListRow_create(pListModel);
				// 生成ListBox的单元格
				pCell = mdlListRow_getCellAtIndex(pRow, 0);
				// 向ListBox的单元格赋值
				mdlListCell_setStringValue(pCell, sResultFileName.c_str(), TRUE);
				// 向ListBox中增加行
				mdlListModel_addRow(pListModel, pRow);
				// 刷新ListBox控件
				mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
				// 选中新增行
				mdlDialog_listBoxSelectCells(diP->rawItemP, mdlListModel_getRowCount(pListModel) - 1, mdlListModel_getRowCount(pListModel) - 1, -1, -1, TRUE, TRUE);

				oAddResultFileSet.insert(sResultFileName);
			}
		}

		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ImportCaeInfDialog_DelResultFileHook
(
DialogItemMessage *dimP         /* => a	ptr to a dialog	item message */
)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	ListRow		*pRow = NULL;
	ListCell	*pCell = NULL;
	int iRow = -1;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_QUEUECOMMAND:
	{
		diP = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_CaeResultFile, 0);
		pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
		if (pListModel != NULL)
		{
			// 取得当前的行号
			mdlDialog_listBoxGetLocationCursor(&iRow, NULL, diP->rawItemP);
			pRow = mdlListModel_getRowAtIndex(pListModel, iRow);
			pCell = mdlListRow_getCellAtIndex(pRow, 0);

			WString sResultFileName;
			mdlListCell_getStringValueWString(sResultFileName, pCell);

			bset<BeFileName>::iterator iter;//定义前向迭代器
			//遍历集合中的所有元素
			for (iter = oAddResultFileSet.begin(); iter != oAddResultFileSet.end(); iter++)
			{
				if (sResultFileName.CompareTo((*iter).c_str()) == 0)
				{
					oAddResultFileSet.erase(iter);
					break;
				}
			}

			// 删除当前行
			mdlListModel_removeRowAtIndex(pListModel, iRow, true);
			// 刷新ListBox控件
			mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
			// 选中新增行
			mdlDialog_listBoxSelectCells(diP->rawItemP, iRow - 1, iRow - 1, -1, -1, TRUE, TRUE);
		}

		break;
	};
	default:
		dimP->msgUnderstood = FALSE;
		break;
	}
}

void ImportCaeInfDialog_TBSelImpDatHook
(
DialogItemMessage *dimP         /* => a	ptr to a dialog	item message */
)
{
	DialogItem     *diPImportCaeModel;
	DialogItem     *diPTxtCaeModel;
	DialogItem     *diPSelectCaeModelFile;
	DialogItem     *diPRstFileLst;
	DialogItem     *diPAddRst;
	DialogItem     *diPDelRst;
	DialogItem     *diPImpRst;

	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_STATECHANGED:
	{
		MSValueDescr    value;
		mdlDialog_itemGetValue(value, dimP->db, dimP->itemIndex);

		// 取得各控件的对象
		diPTxtCaeModel = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_Text, TEXTID_ImportCaeModel, 0); // 网格模型文件路径框
		diPImportCaeModel = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ComboBox, COMBOBOXID_ImportCaeModel, 0);// 网格模型选择框
		diPSelectCaeModelFile = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_SelectCaeModelFile, 0);// 网格模型选择控钮

		diPRstFileLst = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_ListBox, LISTBOXID_CaeResultFile, 0);// 计算结果文件列表
		diPAddRst = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_AddResultFile, 0);// 计算结果添加控钮
		diPDelRst = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, BUTTONID_DelResultFile, 0);// 计算结果删除控钮

		diPImpRst = mdlDialog_itemGetByTypeAndId(dimP->db, RTYPE_PushButton, PUSHBUTTONID_OK, 0);// 导入控钮

		// 是否选中网格模型
		if (g_ImpDlgInfo.MeshModelVal == 0)
		{
			diPTxtCaeModel->Hide();// 网格模型文件路径框
			diPImportCaeModel->Show();// 网格模型选择框
			diPSelectCaeModelFile->Hide();// 网格模型选择控钮
		}
		else
		{
			diPTxtCaeModel->Show();// 网格模型文件路径框
			diPImportCaeModel->Hide();// 网格模型选择框
			diPSelectCaeModelFile->Show();// 网格模型选择控钮
		}

		// 是否选中计算结果
		if (g_ImpDlgInfo.ResultVal == 0)
		{
			diPRstFileLst->SetEnabled(false);// 计算结果文件列表
			diPAddRst->SetEnabled(false);// 计算结果添加控钮
			diPDelRst->SetEnabled(false);// 计算结果删除控钮
		}
		else
		{
			diPRstFileLst->SetEnabled(true);// 计算结果文件列表
			diPAddRst->SetEnabled(true);// 计算结果添加控钮
			diPDelRst->SetEnabled(true);// 计算结果删除控钮
		}

		if (g_ImpDlgInfo.MeshModelVal == 0 && g_ImpDlgInfo.ResultVal == 0)
		{
			diPImpRst->SetEnabled(false);
		}
		else
		{
			diPImpRst->SetEnabled(true);
		}

		// 画面刷新
		dimP->db->Refresh();

		break;
	}


	default:
	{
		dimP->msgUnderstood = FALSE;
		break;
	}
	}
}

void	ImportCaeInfDialog_mdlDialogHook
(
DialogMessage   *dmP
)
{
	DialogItem     *diPImportCaeModel;

	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_ACTIONBUTTON:
	{
		DialogItem*	pLBox = NULL;
		DialogItem*	diPTxtCaeModel = NULL;
		ListModel   *pListModel = NULL;
		WString sMsg;

		/* if the user pressed the cancel button then break */
		if (ACTIONBUTTON_OK != dmP->u.actionButton.actionType)
			break;

		if (g_ImpDlgInfo.MeshModelVal != 0)
		{
			BeFileName sModelFileName;

			if (NULL == (diPTxtCaeModel = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_Text, TEXTID_ImportCaeModel, 0)))
				break;

			diPTxtCaeModel->GetValue(sModelFileName);
			sModelFileName.Trim();

			if (sModelFileName.length() == 0)
			{
				sMsg.clear();
				sMsg.append(L"请输入网格模型的导入路径！");
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, sMsg.c_str(), MessageBoxIconType::Critical);

				dmP->u.actionButton.abortAction = TRUE;
				break;
			}
			else if (!BeFileName::DoesPathExist(sModelFileName.c_str()))
			{

				sMsg.clear();
				sMsg.append(L"您输入的网格模型的导入路径不正确！");
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, sMsg.c_str(), MessageBoxIconType::Critical);

				dmP->u.actionButton.abortAction = TRUE;
				break;
			}
		}
		else
		{

		}

		if (g_ImpDlgInfo.ResultVal != 0)
		{
			/* get a pointer to the tag names list box */
			if (NULL == (pLBox = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ListBox, LISTBOXID_CaeResultFile, 0)))
				break;

			pListModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
			if (mdlListModel_getRowCount(pListModel) <= 0)
			{
				sMsg.clear();
				sMsg.append(L"请输入计算结果的导入路径！！");
				mdlDialog_openMessageBox(DIALOGID_MsgBoxOK, sMsg.c_str(), MessageBoxIconType::Critical);
				dmP->u.actionButton.abortAction = TRUE;
			}
		}
		break;
	}
	case DIALOG_MESSAGE_CREATE:
	{
		dmP->u.create.interests.windowMoving = TRUE;
		dmP->u.create.interests.resizes = FALSE;
		dmP->u.create.interests.updates = TRUE;
		dmP->u.create.interests.dialogFocuses = TRUE;

		break;
	}
	case DIALOG_MESSAGE_INIT:
	{
		// 初始化
		sModelFileName.Clear();
		oAddResultFileSet.clear();
		if ((diPImportCaeModel = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ComboBox, COMBOBOXID_ImportCaeModel, 0)))
		{
			if (g_ImpDlgInfo.MeshModelVal == 1)  /* process only if iteme is enabled */
			{
				mdlDialog_itemHide(dmP->db, diPImportCaeModel->itemIndex, FALSE);
			}
			else
			{
				mdlDialog_itemShow(dmP->db, diPImportCaeModel->itemIndex);
			}
		}
		break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING:
	{
		dmP->u.windowMoving.handled = FALSE;

		/* Don't process if only moving dialog box */
		if (dmP->u.windowMoving.whichCorners == CORNER_ALL ||
			dmP->u.windowMoving.whichCorners == CORNER_ALL_RESIZED)
			break;
		dmP->u.windowMoving.handled = TRUE;
		break;
	}

	case DIALOG_MESSAGE_RESIZE:
	{
		/* Don't process if only moving dialog box */
		if (dmP->u.resize.whichCorners == CORNER_ALL)
			break;

		dmP->u.resize.forceCompleteRedraw = TRUE;
		break;
	}
	case DIALOG_MESSAGE_DESTROY:
	{
		mdlState_startDefaultCommand();
		break;
	}
	default:
		dmP->msgUnderstood = FALSE;
		break;
	}
}

void ImportCaeInfDialog_SelectCaeModelHook
(
DialogItemMessage *dimP         /* => a	ptr to a dialog	item message */
)
{
	dimP->msgUnderstood = TRUE;
	switch (dimP->messageType)
	{
	case DITEM_MESSAGE_CREATE:
	{
		RawItemHdr     *riP;
		ListModel       *pListModel;

		riP = dimP->dialogItemP->rawItemP;
		if (riP)
		{
			pListModel = mdlListModel_create(1);
			mdlDialog_comboBoxSetListModelP(riP, pListModel);
			if (pListModel)
			{
				ListRow *pRow;
				ListCell *pCell;
				/** 定义数据库变量 m_db*/
				Db m_db;
				/** 定义数据库状态变量 m_statment*/
				Statement *m_statment;

				// 打开CAE结果数据库
				StatusInt rc = OpenDB(m_db);
				if (rc == BE_SQLITE_OK)
				{
					m_statment = new Statement();
					rc = m_statment->Prepare(m_db, "SELECT ProjectId,ProjectName,Description FROM PROJECTS");

					if (rc == BE_SQLITE_OK)
					{
						while (m_statment->Step() == BE_SQLITE_ROW)
						{
							//WString lsVal;
							WString lsDisplay;
							//BeStringUtilities::Utf8ToWChar(lsVal, m_statment->GetValueUtf8(0));
							BeStringUtilities::Utf8ToWChar(lsDisplay, m_statment->GetValueUtf8(1));

							pRow = mdlListRow_create(pListModel);
							pCell = mdlListRow_getCellAtIndex(pRow, 0);
							mdlListCell_setUInt32Value(pCell, m_statment->GetValueInt(0));
							mdlListCell_setDisplayText(pCell, lsDisplay.c_str());

							mdlListModel_addRow(pListModel, pRow);
						}
					}

					m_statment->Finalize();
					m_statment = NULL;
				}

				m_db.CloseDb();
			}
		}

		break;
	}

	case DITEM_MESSAGE_GETSTATE:
	{
		dimP->u.value.msValueDescrP->SetInt32(0);
		dimP->u.value.hookHandled = true;
		break;
	}

	case DITEM_MESSAGE_STATECHANGED:
	{
		MSValueDescr    value;
		mdlDialog_itemGetValue(value, dimP->db, dimP->itemIndex);
		break;
	}
	case DITEM_MESSAGE_DESTROY:
	{
		ListModel   *pListModel = NULL;
		pListModel = mdlDialog_comboBoxGetListModelP(dimP->dialogItemP->rawItemP);
		mdlListModel_destroy(pListModel, TRUE);
		break;
	}
	default:
		dimP->msgUnderstood = FALSE;

		break;
	}
}