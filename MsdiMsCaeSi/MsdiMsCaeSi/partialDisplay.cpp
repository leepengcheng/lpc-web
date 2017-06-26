#include "MsdiMsCaeSipch.h"

//pd:partial display
bvector<int> pd_id_Ma, pd_id_Et, pd_id_Rc;
bvector<WString> pd_val_Ma, pd_val_Et, pd_val_Rc;

extern bvector<Condition> pd_ConditionLst;

void showMeshsOfInstance(int iProjectId, WString NGName, bvector<ElementId> meshIDs, bool traversalAll)
{
	//遍历指定命名组下的所有元素，meshIDs中的元素显示(同时记录其XAttribution中保存的ElemID属性)，其余隐藏
	bvector<WString> elemIDs;
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

	//根据命名组名获取对应的显示层与隐藏层名
	WString LvName = NGName + L"HIDE";
	LevelId lvID_show = 0, lvID_hide = 0;
	mdlLevel_getIdFromName(&lvID_hide, MASTERFILE, 0, LvName.data());
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, NGName.data());

	NamedGroupPtr namedGroup = NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data());
	if (namedGroup.IsValid())
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);

		for (UInt32 i = 0; i < graphicMembers; i++)
		{
			ElementId elemid = namedGroup->GetMember(i)->GetElementId();
			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
			{
				LevelId elemlv = eeh.GetElementDescrP()->el.ehdr.level;
				//若需要遍历命名组中所有元素则不进行过滤,否则仅处理命名组中处于显示层的元素
				if (!traversalAll && elemlv != lvID_show)
					continue;

				//将该元素放到隐藏层中
				if (elemlv != lvID_hide)
				{
					eeh.GetElementDescrP()->el.ehdr.level = lvID_hide;
					eeh.ReplaceInModel(eeh.GetElementRef());
				}
			}
		}

		// 元素显示
		for (auto iter = meshIDs.begin(); iter != meshIDs.end(); iter++)
		{
			/*cout << (*iter) << endl;*/
			EditElementHandle eeh;
			eeh.FindByID(*iter, MASTERFILE);

			if (eeh.IsValid())
			{
				//获取该元素的XAttribution中数据库对应的ElemID
				XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
				MeshProperty *meshPro = NULL;

				if (xh.IsValid())
				{
					meshPro = (MeshProperty *)xh.PeekData();

					//记录数据库中对应的ElemID
					if (meshPro != NULL)
					{
						WString ElemID_DB = num2Ws(meshPro->ElemID);
						elemIDs.push_back(ElemID_DB);
					}
				}

				eeh.GetElementDescrP()->el.ehdr.level = lvID_show;
				eeh.ReplaceInModel(eeh.GetElementRef());
			}
		}
	}

	showMeshsOfInstance_DB(iProjectId, NGName, elemIDs);
}

void showAllOfInstance(int iProjectId, WString NGName)
{
	XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);

	//根据命名组名获取对应的显示层
	LevelId lvID_show = 0;
	mdlLevel_getIdFromName(&lvID_show, MASTERFILE, 0, NGName.data());

	NamedGroupPtr namedGroup = NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data());
	if (namedGroup.IsValid())
	{
		UInt32 graphicMembers;
		namedGroup->GetMemberCount(&graphicMembers, NULL);

		for (UInt32 i = 0; i < graphicMembers; i++)
		{
			ElementId elemid = namedGroup->GetMember(i)->GetElementId();

			EditElementHandle eeh;
			if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
			{
				//将该元素放到显示层中
				if (eeh.GetElementDescrP()->el.ehdr.level != lvID_show)
				{
					eeh.GetElementDescrP()->el.ehdr.level = lvID_show;
					eeh.ReplaceInModel(eeh.GetElementRef());
				}
			}
		}
	}
	showAllOfInstance_DB(iProjectId, NGName);
}

void deleteCAEInstance(WString NGName)
{
	//删除该实例对应的显示层/隐藏层
	WString levelNameDeleted = NGName;
	DgnPlatform::LevelId levelId;

	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, levelNameDeleted.data()))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}
	levelNameDeleted.append(L"HIDE");
	if (SUCCESS == mdlLevel_getIdFromName(&levelId, MASTERFILE, LEVEL_NULL_ID, levelNameDeleted.data()))
	{
		mdlLevel_deleteElement(NULL, MASTERFILE, levelId, FALSE, TRUE);
		mdlLevel_delete(MASTERFILE, levelId);
	}

	//删除该实例对应的命名组
	NamedGroupCollection::Create(*MASTERFILE)->FindByName(NGName.data())->DeleteFromFile();

	//删除数据库中的FACES表对应记录
	deleteAllFaceOfInstance(NGName);

	//删除所有附属的剖面model
	NGName.ReplaceAll(NGNAME_PREFIX_CAEMODEL, NGNAME_PREFIX_CAECLIP);

	DgnIndexIteratorP   cellIterator = mdlModelIterator_create(mdlDgnFileObj_getMasterFile());
	DgnIndexItemP       IndexItem;
	mdlModelIterator_getFirst(cellIterator);
	mdlModelIterator_setAcceptCellsOnly(cellIterator, FALSE);
	while (NULL != mdlModelIterator_getNext(cellIterator))
	{
		IndexItem = mdlModelIterator_getCurrent(cellIterator);
		WChar wModelName[MAX_CELLNAME_LENGTH];
		//获取model名
		mdlModelItem_getName(IndexItem, wModelName, MAX_CELLNAME_LENGTH);
		WString wsModelName;
		wsModelName.append(wModelName);

		if (wsModelName.FindI(NGName.data()) != string::npos)
		{
			DgnModelRefP model = NULL;
			mdlModelRef_createWorkingByName(&model, mdlDgnFileObj_getMasterFile(), wModelName, TRUE, TRUE);
			mdlModelRef_deleteModel(model, false);
			mdlModelRef_freeWorking(model);
		}
	}
	mdlModelIterator_free(cellIterator);

	//删除XAttribution中的相关记录
	bvector<InstanceInfo> infos = readInstanceInfo();
	for (bvector<InstanceInfo>::iterator iter = infos.begin(); iter != infos.end(); iter++)
	{
		if (NGName.CompareTo((*iter).NgName) == 0)
		{
			infos.erase(iter);
			savingInstanceInfo(infos);
			break;;
		}
	}
	
}

bool partialDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	//获取用户点击的元素
	EditElementHandleP eeh = GetElementAgenda().GetFirstP();

	//获取元素所在命名组名/层名
	WString NGName = getNGNameOfElem(*eeh);

	//根据命名组名获取工程ID
	int iProjectId;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjectId, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//根据工程ID获取材料名、元素类型和实常数的可能值
	//ID值
	getMaterialIDByProjectid(iProjectId, pd_id_Ma);
	getElemTypeIDByProjectid(iProjectId, pd_id_Et);
	getRealConsIDByProjectid(iProjectId, pd_id_Rc);
	//显示值
	pd_val_Ma.clear();
	for (int i = 0; i < (int)pd_id_Ma.size(); i++)
	{
		WString mName, mType, mEX, mNUXY, mDENS, mPRXY;
		getMaterialAllByPK(iProjectId, pd_id_Ma[i], mName, mType, mEX, mNUXY, mDENS, mPRXY);

		//Type-Name拼成材料属性
		if (mType.length() > 0)
			mType.append(L"-");
		mType.append(mName);

		pd_val_Ma.push_back(mType);
	}
	pd_val_Et.clear();
	for (int i = 0; i < (int)pd_id_Et.size(); i++)
	{
		WString elemTypeName;
		getElemNameByPK(iProjectId, pd_id_Et[i], elemTypeName);
		pd_val_Et.push_back(elemTypeName);
	}
	pd_val_Rc.clear();
	for (int i = 0; i < (int)pd_id_Rc.size(); i++)
	{
		WString realConsProp;
		getRealConsPropByPK(iProjectId, pd_id_Rc[i], realConsProp);
		pd_val_Rc.push_back(realConsProp);
	}

	//打开局部显示筛选对话框
	int lastAction = -1;
	mdlDialog_openModal(&lastAction, NULL, DIALOGID_SolidPartShow);

	//进行局部显示,若没有筛选条件则不进行任何操作
	if (lastAction == ACTIONBUTTON_OK && !pd_ConditionLst.empty())
	{
		//命名组与层同名
		NamedGroupPtr namedGroup = NamedGroupCollection::Create(*ACTIVEMODEL)->FindByName(NGName.data());

		if (namedGroup.IsValid())
		{
			UInt32 graphicMembers;
			namedGroup->GetMemberCount(&graphicMembers, NULL);

			bvector<ElementId> shows;
			for (UInt32 i = 0; i < graphicMembers; i++)
			{
				ElementId elemid = namedGroup->GetMember(i)->GetElementId();

				EditElementHandle eeh;
				if (SUCCESS == eeh.FindByID(elemid, MASTERFILE))
				{
					XAttributeHandlerId handlerId(XAttrHandleID_Major, XAttrHandleID_Minor);
					XAttributeHandle xh(eeh.GetElementRef(), handlerId, XAttrID_MeshProperty);
					MeshProperty *meshPro = NULL;

					if (xh.IsValid())
						meshPro = (MeshProperty *)xh.PeekData();

					//根据筛选条件进行分类
					bool needShow = true;
					for (int j = 0; j < (int)pd_ConditionLst.size(); j++)
					{
						Condition& condition = pd_ConditionLst[j];
						if (!matchCondition(*meshPro, condition))
						{
							needShow = false;
							break;
						}
					}

					if (needShow)
						shows.push_back(elemid);
				}
			}

			showMeshsOfInstance(iProjectId, NGName, shows, true);
		}	
	}
	mdlState_startDefaultCommand();
	return true;
}

bool fenceDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();
	int cnt = (int)GetElementAgenda().GetCount();
	EditElementHandleP end = curr + GetElementAgenda().GetCount();

	//根据选择集中下标为0的元素确定实例名(命名组/层名)
	WString NGName = getNGNameOfElem(*curr);

	LevelId lvID = 0;
	mdlLevel_getIdFromName(&lvID, MASTERFILE, 0, NGName.data());

	//根据命名组名获取工程ID
	int iProjectID;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	//遍历选择集，获取选择集中所有元素ID
	bvector<ElementId> idSS;
	for (; curr < end; curr++)
	{
		//仅当该元素在NGName所在层中才需要处理
		if (curr->GetElementDescrP()->el.ehdr.level == lvID)
		{
			ElementId id = mdlElement_getID(curr->GetElementP());
			idSS.push_back(id);
		}
	}

	showMeshsOfInstance(iProjectID, NGName, idSS, false);

	mdlState_startDefaultCommand();

	return true;
}

bool recoverDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();

	//根据选择集中下标为0的元素确定实例名(命名组/层名)
	WString NGName = getNGNameOfElem(*curr);

	//根据命名组名获取工程ID
	int iProjestID;
	splitNgName_Model(NGName, UNUSE_WSTRING, iProjestID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

	showAllOfInstance(iProjestID, NGName);

	mdlState_startDefaultCommand();

	return true;
}

bool deleteDisplayTool::_OnModifyComplete(DgnButtonEventCR ev)
{
	EditElementHandleP curr = GetElementAgenda().GetFirstP();

	//根据选择集中下标为0的元素确定实例名(命名组/层名)
	WString NGName = getNGNameOfElem(*curr);
	WString instanceName;
	splitNgName_Model(NGName, UNUSE_WSTRING, UNUSE_INT, UNUSE_INT, instanceName, UNUSE_WSTRING);

	//根据命名组名获取其实例名并弹出提示信息
	WString msg;
	msg.append(L"确定要删除模型<");
	msg.append(instanceName.data());
	msg.append(L">以及其相关的内容吗?");
	if (ACTIONBUTTON_OK == mdlDialog_openMessageBox(DIALOGID_MsgBoxOKCancel, msg.data(), MessageBoxIconType::Warning))
	{
		//根据命名组名删除该实例模型
		deleteCAEInstance(NGName);
	}

	//退出工具
	mdlState_startDefaultCommand();

	return true;
}

void ShowCAESolidPartCmd(WCharCP unparsed)
{
	partialDisplayTool::InstallNewInstance(COMMAND_SHOWCAESOLIDPART);
}

void fenceDisplayCmd(WCharCP unparsed)
{
	//框选显示分为两种情况
	SelectionSetManagerR ssr = SelectionSetManager::GetManager();
	int numSelected = (int)ssr.NumSelected();
	//若选择集中已经有元素则直接操作选择集
	if (numSelected != 0)
	{
		//根据选择集中下标为0的元素确定实例名(命名组/层名)
		ElementRefP ref = NULL;
		DgnModelRefP model = NULL;
		ssr.GetElement(0, &ref, &model);
		EditElementHandle eeh(ref);
		WString NGName = getNGNameOfElem(eeh);

		LevelId lvID = 0;
		mdlLevel_getIdFromName(&lvID, MASTERFILE, 0, NGName.data());

		//根据命名组名获取工程ID
		int iProjectID;
		splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

		//遍历选择集，获取选择集中所有元素ID
		bvector<ElementId> idSS;
		for (int i = 0; i < numSelected; i++)
		{
			ssr.GetElement(i, &ref, &model);
			//仅当该元素在NGName所在层中才需要处理
			if (ref->GetLevel() == lvID)
				idSS.push_back(ref->GetElementId());
		}

		showMeshsOfInstance(iProjectID, NGName, idSS, false);
	}
	else
	{
		fenceDisplayTool::InstallNewInstance(COMMAND_FENCEDISPLAY);
	}
}

void recoverDisplayCmd(WCharCP unparsed)
{
	SelectionSetManagerR ssr = SelectionSetManager::GetManager();
	int numSelected = (int)ssr.NumSelected();
	//选择集中已经有元素则直接操作选择集
	if (numSelected != 0)
	{
		//根据选择集中下标为0的元素确定实例名(命名组/层名)
		ElementRefP ref = NULL;
		DgnModelRefP model = NULL;
		ssr.GetElement(0, &ref, &model);
		EditElementHandle eeh(ref);
		WString NGName = getNGNameOfElem(eeh);

		//根据命名组名获取工程ID
		int iProjectID;
		splitNgName_Model(NGName, UNUSE_WSTRING, iProjectID, UNUSE_INT, UNUSE_WSTRING, UNUSE_WSTRING);

		showAllOfInstance(iProjectID, NGName);
	}
	else
	{
		recoverDisplayTool::InstallNewInstance(COMMAND_RECOVERDISPLAY);
	}
}

void deleteDisplayCmd(WCharCP unparsed)
{
	deleteDisplayTool::InstallNewInstance(COMMAND_DELETEDISPLAY);
}
