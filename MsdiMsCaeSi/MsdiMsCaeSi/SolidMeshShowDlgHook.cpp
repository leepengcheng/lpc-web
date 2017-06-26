#include "SolidMeshShowDlgHook.h"

UInt32 m_ProjectID = -1;
bmap<int, WString> mapProjectUnit;
/*----------------------------------------------------------------------+
* The command entry point for user function.				            *
* @param        unparsed     unused unparsed argument to the command    *
+----------------------------------------------------------------------*/
void ShowCAESolidAllCmd(WCharCP unparsed)
{
	int lastAction = -1;

	/* open child modal dialog box */
	if (!mdlDialog_openModal(&lastAction, NULL, DIALOGID_SolidAllShow))
	{
		// 当点击OK时，显示CAE整体模型
		if (lastAction == ACTIONBUTTON_OK)
		{
			CreateCAESolidTool::InstallNewInstance(COMMAND_SHOWCAESOLIDALL, PROMPT_SolidAllShow, m_ProjectID, mapProjectUnit[m_ProjectID]);
		}
	}
}
void	SolidMeshShowDlg_SolidAllShowHook
(
DialogMessage   *dmP
)
{
	DialogItem  *diP = NULL;
	ListModel   *pListModel = NULL;
	ListRow		*pRow = NULL;
	ListCell	*pCell = NULL;

	dmP->msgUnderstood = TRUE;
	switch (dmP->messageType)
	{
	case DIALOG_MESSAGE_ACTIONBUTTON:
	{
		int			iRow = -1,
			iCol = -1;
		DialogItem*	pLBox = NULL;
		ListModel   *pListModel = NULL;
		ListRow		*pRow = NULL;
		ListCell    *pCell = NULL;

		/* if the user pressed the cancel button then break */
		if (ACTIONBUTTON_OK != dmP->u.actionButton.actionType)
			break;

		/* get a pointer to the tag names list box */
		if (NULL == (pLBox = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ListBox, LISTBOXID_CaeProject, 0)))
			break;

		mdlDialog_listBoxGetLocationCursor(&iRow, &iCol, pLBox->rawItemP);
		if (iRow >= 0)
		{
			pListModel = mdlDialog_listBoxGetListModelP(pLBox->rawItemP);
			pRow = mdlListModel_getRowAtIndex(pListModel, iRow);
			pCell = mdlListRow_getCellAtIndex(pRow, 0);

			mdlListCell_getUInt32Value(pCell, &m_ProjectID);
		}
		//dmP->u.actionButton.abortAction = TRUE;
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
		diP = mdlDialog_itemGetByTypeAndId(dmP->db, RTYPE_ListBox, LISTBOXID_CaeProject, 0);
		pListModel = mdlDialog_listBoxGetListModelP(diP->rawItemP);
		if (pListModel == NULL)
			pListModel = mdlListModel_create(1);

		/** 定义数据库变量 m_db*/
		Db m_db;
		/** 定义数据库状态变量 m_statment*/
		Statement *m_statment;

		// 打开CAE结果数据库
		StatusInt rc = OpenDB(m_db);
		if (rc == BE_SQLITE_OK)
		{
			m_statment = new Statement();
			rc = m_statment->Prepare(m_db, "SELECT ProjectId,ProjectName,ProjectUnit FROM PROJECTS");

			if (rc == BE_SQLITE_OK)
			{
				while (m_statment->Step() == BE_SQLITE_ROW)
				{
					WString lsDisplay;
					BeStringUtilities::Utf8ToWChar(lsDisplay, m_statment->GetValueUtf8(1));
					// 生成ListBox的行
					pRow = mdlListRow_create(pListModel);
					// 生成ListBox的单元格
					pCell = mdlListRow_getCellAtIndex(pRow, 0);
					// 向ListBox的单元格赋值
					mdlListCell_setUInt32Value(pCell, m_statment->GetValueInt(0));
					mdlListCell_setDisplayText(pCell, lsDisplay.c_str());
					// 向ListBox中增加行
					mdlListModel_addRow(pListModel, pRow);
					// CAE模型单位
					WString lsUnit;
					BeStringUtilities::Utf8ToWChar(lsUnit, m_statment->GetValueUtf8(2));
					mapProjectUnit[m_statment->GetValueInt(0)] = lsUnit;
				}
			}

			m_statment->Finalize();
			m_statment = NULL;
		}

		m_db.CloseDb();

		// 刷新ListBox控件
		mdlDialog_listBoxSetListModelP(diP->rawItemP, pListModel, 0);
		// 选中新增行
		mdlDialog_listBoxSelectCells(diP->rawItemP, 0, 0, -1, -1, TRUE, TRUE);

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

SolidMeshShowDlgHook::SolidMeshShowDlgHook()
{
}


SolidMeshShowDlgHook::~SolidMeshShowDlgHook()
{
}
