#include "MsdiMsCaeSipch.h"

USING_NAMESPACE_BENTLEY;

ImportCAEInf::ImportCAEInf(void) :m_savept(NULL), m_statment(NULL), m_ProjectId(-1), m_ProjectName("")
{
}

void ImportCAEInf::ImportCAEResult(BeFileName sModelFileName, bset<BeFileName> oAddResultFileSet, int ProjectId)
{
	mdlState_clear();
	StatusInt rc;
	bool bRet = true;

	rc = OpenDB(m_db);
	if (rc != BE_SQLITE_OK)
	{
		return;
	}

	m_savept = new Savepoint(m_db, "ImportCAEResult", true);
	m_savept->Begin();

	m_statment = new Statement();

	// 进度条
	MSDialogP oBar = NULL;
	// 进度条显示文字
	WCharP 	  wcMessageText;
	size_t len = strlen(TXT_CAEImporting) + 1;
	wcMessageText = (WCharP)malloc(len*sizeof(WChar));
	BeStringUtilities::CurrentLocaleCharToWChar(wcMessageText, TXT_CAEImporting, len);
	// 打开进度条
	oBar = mdlDialog_completionBarOpen(wcMessageText);

	if (!WString::IsNullOrEmpty(sModelFileName))
	{
		// 读取CAE网格模型信息
		bRet = ReadCAEElems(sModelFileName, oBar, wcMessageText, 50);
	}
	else
	{
		m_ProjectId = ProjectId;
		m_ProjectName = getProjectNameById(ProjectId);
	}

	if (bRet == true)
	{
		// 读取CAE计算结果信息
		bRet = ReadCAEResults(oAddResultFileSet, oBar, wcMessageText, 55);
	}

	m_statment->Finalize();
	m_statment = NULL;

	if (bRet == true)
		m_savept->Commit();
	else
		m_savept->Cancel();

	m_db.CloseDb();

	mdlDialog_completionBarUpdate(oBar, wcMessageText, 100);
	mdlDialog_completionBarClose(oBar);

	return;
}

/**
* @brief  取得的CAE工程名
* @param[in] iProjectId   CAE网格模型ID
* @return CAE工程名
*/
WString ImportCAEInf::getProjectNameById(int iProjectId)
{
	AString lsSql;
	WString lsProjectName;

	// 查询CAE工程表中是否存在不连续的工程ID
	// 如果存在，新CAE工程ID就从不连续的工程ID之间产生
	lsSql.append("SELECT ProjectName FROM PROJECTS \n");
	lsSql.append("WHERE ProjectId = @ProjectId \n");

	m_statment->Prepare(m_db, lsSql.c_str());
	// CAE工程ID
	m_statment->BindInt(1, iProjectId);
	while (m_statment->Step() == BE_SQLITE_ROW)
	{
		lsProjectName.AppendUtf8(m_statment->GetValueUtf8(0));
		break;
	}

	m_statment->Finalize();

	return lsProjectName;
}

/**
* @brief  取得新的CAE工程ID
* @return CAE工程ID
*/
void ImportCAEInf::GetProjectID(void)
{
	AString lsSql;

	// 查询CAE工程表中是否存在不连续的工程ID
	// 如果存在，新CAE工程ID就从不连续的工程ID之间产生
	lsSql.append("SELECT(SELECT COUNT(*) FROM PROJECTS a WHERE a.rowid < b.rowid) AS ids, ID FROM PROJECTS b \n");
	lsSql.append("WHERE ids <> ID \n");
	lsSql.append("ORDER BY ids LIMIT 1 \n");
	m_statment->Prepare(m_db, lsSql.c_str());
	while (m_statment->Step() == BE_SQLITE_ROW)
	{
		m_ProjectId = m_statment->GetValueInt(0);
		break;
	}

	m_statment->Finalize();

	// 如果不存在，新CAE工程ID就为CAE工程表的最大记录数
	if (m_ProjectId == -1)
	{
		lsSql.clear();
		lsSql.append("SELECT COUNT(*) FROM PROJECTS");
		m_statment->Prepare(m_db, lsSql.c_str());
		while (m_statment->Step() == BE_SQLITE_ROW)
		{
			m_ProjectId = m_statment->GetValueInt(0);
			break;
		}
		m_statment->Finalize();
	}

	return;
}

bool ImportCAEInf::SaveProjectNameToDb(void)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO PROJECTS(ProjectId, ProjectName, Description) VALUES (@ProjectId, @ProjectName, @Description)");

	// ID
	m_statment->BindInt(1, m_ProjectId);
	// CAE工程名
	Utf8String sProjectName;
	BeStringUtilities::WCharToUtf8(sProjectName, m_ProjectName.c_str());
	m_statment->BindUtf8CP(2, sProjectName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
	// CAE工程描述
	m_statment->BindNull(3);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ImportCAEInf::SaveProjectDescriptionToDb(WString& sProjectDescription)
{
	StatusInt	rc;
 
	m_statment->Prepare(m_db, "UPDATE PROJECTS SET Description = @Description WHERE ProjectId = @ProjectId");
	
	// CAE工程描述
	Utf8String sDescription;
	BeStringUtilities::WCharToUtf8(sDescription, sProjectDescription.c_str());
	m_statment->BindUtf8CP(1, sDescription.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
	// CAE工程ID
	m_statment->BindInt(2, m_ProjectId);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ImportCAEInf::SaveProjectUnitToDb(WString& sProjectUnit)
{
	StatusInt	rc;

	// 更新SQL
	m_statment->Prepare(m_db, "UPDATE PROJECTS SET ProjectUnit = @ProjectUnit WHERE ProjectId = @ProjectId");

	// CAE模型单位
	Utf8String sUnit;
	BeStringUtilities::WCharToUtf8(sUnit, sProjectUnit.c_str());
	// 导入单位"m-Mpa" 转换为 "m"
	bvector<Utf8String> vecUnit;
	BeStringUtilities::Split(sUnit.data(), "-", vecUnit);
	m_statment->BindUtf8CP(1, vecUnit[0].c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
	// CAE工程ID
	m_statment->BindInt(2, m_ProjectId);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 保存单元类型信息
bool ImportCAEInf::SaveElemTypeToDb(bvector<WString> oSplitVec)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO ELEMTYPES(ProjectId, ElemType, ElemName) VALUES (@ProjectId, @ElemType, @ElemName)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);
	// 单元类型
	m_statment->BindInt(2, BeStringUtilities::Wtoi(oSplitVec.at(0).c_str()));
	// 单元名称
	m_statment->BindInt(3, BeStringUtilities::Wtoi(oSplitVec.at(1).c_str()));

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
* @brief  判断DB中是否相同CAE工程名的数据
* @param[out] iOldProjectId   已存在的CAE网格模型ID
* @return 判断结果(true:存在，false:不存在)
*/
bool ImportCAEInf::ChkProjectExist(int& iOldProjectId)
{
	AString lsSql;
	bool bRet = false;

	Utf8String sProjectName;
	BeStringUtilities::WCharToUtf8(sProjectName, m_ProjectName.c_str());

	// 生成查询SQL语句
	lsSql.append("SELECT ProjectId FROM PROJECTS \n");
	lsSql.append("WHERE \n");
	lsSql.append("ProjectName = '");
	lsSql.append(sProjectName.c_str());
	lsSql.append("'");

	// 执行SQL语句
	m_statment->Prepare(m_db, lsSql.c_str());
	while (m_statment->Step() == BE_SQLITE_ROW)
	{
		// 数据存在时，取出已存在的CAE网格模型ID
		iOldProjectId = m_statment->GetValueInt(0);
		// 判断结果设为True
		bRet = true;
		break;
	}
	m_statment->Finalize();

	return bRet;
}

/**
* @brief  根据网格模型ID，删除相关的CAE网格模型数据
* @param[in] iOldProjectId   CAE网格模型ID
*/
void ImportCAEInf::DelCAEElemsByProjectId(int iOldProjectId)
{
	StatusInt	rc;
	AString lsWhere;
	AString lsSql;
	AString asOldProjectId;

	// int转AString
	wstringstream ssOldProjectId;
	ssOldProjectId << iOldProjectId;
	BeStringUtilities::WCharToCurrentLocaleChar(asOldProjectId, ssOldProjectId.str().c_str());

	// 生成Where语句
	lsWhere.append("WHERE \n");
	lsWhere.append("ProjectId = ");
	lsWhere.append(asOldProjectId);

	// 生成删除PROJECTS表的SQL语句
	lsSql.append("DELETE FROM PROJECTS \n");
	lsSql.append(lsWhere.c_str());
	// 执行SQL语句
	m_statment->Prepare(m_db, lsSql.c_str());
	rc = m_statment->Step();
	if (rc == BE_SQLITE_DONE)
	{
		m_statment->Finalize();
	}
}

/**
* @brief  读取CAE元素值参数
* @param[in] sModelFileName   CAE网格模型文件
* @param[in] oBar			  进度条对象
* @param[in] wcMessageText    进度条显示字符
* @param[in] iPercent         进度条显示百分比
* @param[out] 处理结果
*/
bool ImportCAEInf::ReadCAEElems(BeFileName sModelFileName, MSDialogP oBar, WCharP wcMessageText, int iPercent)
{
	WString				sProjectName;// CAE工程名
	WString				sProjectDesp;// CAE工程描述
	NBLOCK				oNBlock;// 节点信息
	EBLOCK				oEBlock;// 单元信息
	BBLOCK				oBBlock;// 边界信息
	RLBLOCK				oRLBlock;// 实常数信息
	bmap<WString, MATERIAL> oMaterialMap;
	bvector<WString>	oSplitVec;
	WString 			tmpStr;
	int					iDataSum;
	AString				lsFaceDataSql;
	bool				bRealExistFlg = false;

	string line;
	ifstream in(sModelFileName.c_str());
	if (in)
	{
		while (getline(in, line))
		{
			tmpStr.clear();
			oSplitVec.clear();
			tmpStr.AppendUtf8(line.c_str());

			if (tmpStr.StartsWith(L"*SET,_PROJECT_NAME,") == true)
			{
				// 取得CAE工程名 
				tmpStr.ReplaceAll(L"*SET,_PROJECT_NAME,", L"");
				tmpStr.ReplaceAll(L"'", L"");
				tmpStr.Trim();
				m_ProjectName.append(tmpStr.c_str());

				int iOldProjectId;
				if (ChkProjectExist(iOldProjectId) == true)
				{
					WString sMsg;
					int actionButton;
					sMsg.append(L"在数据库中已存在该网格模型的信息！\n是否需要重新导入？");
					actionButton = mdlDialog_openMessageBox(DIALOGID_MsgBoxYesNo, sMsg.c_str(), MessageBoxIconType::Question);

					if (ACTIONBUTTON_YES == actionButton)
					{
						// 根据网格模型ID，删除相关的CAE网格模型数据
						DelCAEElemsByProjectId(iOldProjectId);
					}
					else
					{
						return false;
					}
				}
				// 取得新的CAE工程ID
				GetProjectID();
				SaveProjectNameToDb();
			}
			else if (tmpStr.StartsWith(L"*SET,_PROJECT_PROPERTY,"))
			{
				// 取得CAE工程描述
				tmpStr.ReplaceAll(L"*SET,_PROJECT_PROPERTY,", L"");
				tmpStr.ReplaceAll(L"'", L"");
				tmpStr.Trim();

				SaveProjectDescriptionToDb(tmpStr);
			}
			else if (tmpStr.StartsWith(L"*SET,_PROJECT_UNITS,"))
			{
				// 取得CAE工程描述
				tmpStr.ReplaceAll(L"*SET,_PROJECT_UNITS,", L"");
				tmpStr.ReplaceAll(L"'", L"");
				tmpStr.Trim();

				SaveProjectUnitToDb(tmpStr);
			}
			else if (tmpStr.StartsWith(L"*SET,_MAT_"))
			{
				MATERIAL oMATERIAL;// 材料信息
				tmpStr.ReplaceAll(L"*SET,_MAT_", L"");

				size_t iPos = tmpStr.FindI(L"_NAME,");
				if (std::string::npos != iPos)
				{
					tmpStr.ReplaceAll(L"_NAME", L"");
					BeStringUtilities::Split(tmpStr.c_str(), L",", oSplitVec);

					// 判断材料数组中是否存在该材料
					if (oMaterialMap.count(oSplitVec.at(0)) > 0)
					{
						oMATERIAL = oMaterialMap[oSplitVec.at(0)];
					}

					oMATERIAL.ID = BeStringUtilities::Wtoi(oSplitVec.at(0).c_str());

					// 取得材料名称
					oMATERIAL.Name = oSplitVec.at(1);
					// 删除多余字符
					oMATERIAL.Name.ReplaceAll(L"'", L"");
					oMATERIAL.Name.Trim();

					oMaterialMap[oSplitVec.at(0)] = oMATERIAL;
				}
				else
				{
					iPos = tmpStr.FindI(L"_TYPE,");
					if (std::string::npos != iPos)
					{
						tmpStr.ReplaceAll(L"_TYPE", L"");
						BeStringUtilities::Split(tmpStr.c_str(), L",", oSplitVec);
						// 判断材料数组中是否存在该材料
						if (oMaterialMap.count(oSplitVec.at(0)) > 0)
						{
							oMATERIAL = oMaterialMap[oSplitVec.at(0)];
							// 取得材料类型
							oMATERIAL.Type = oSplitVec.at(1);
							// 删除多余字符
							oMATERIAL.Type.ReplaceAll(L"'", L"");
							oMATERIAL.Type.Trim();

							oMaterialMap[oSplitVec.at(0)] = oMATERIAL;
						}
					}
				}
			}
			else if (tmpStr.StartsWith(L"ET,"))
			{
				tmpStr.ReplaceAll(L"ET,", L"");
				BeStringUtilities::Split(tmpStr.c_str(), L",", oSplitVec);

				SaveElemTypeToDb(oSplitVec);
			}
			else if (tmpStr.StartsWith(L"MPDATA,"))
			{
				WString sKey;
				MATERIAL oMATERIAL;// 材料信息

				BeStringUtilities::Split(tmpStr.c_str(), L",", oSplitVec);
				sKey = oSplitVec.at(4);
				sKey.Trim();

				if (std::string::npos != tmpStr.FindI(L",EX"))
				{
					if (oMaterialMap.count(sKey) > 0)
					{
						oMATERIAL = oMaterialMap[sKey];
					}
					oMATERIAL.ID = BeStringUtilities::Wtoi(sKey.c_str());
					oMATERIAL.EX = BeStringUtilities::Wtof(oSplitVec.at(6).c_str());

					oMaterialMap[sKey] = oMATERIAL;
				}
				else if (std::string::npos != tmpStr.FindI(L",NUXY"))
				{
					if (oMaterialMap.count(sKey) > 0)
					{
						oMATERIAL = oMaterialMap[sKey];
					}
					oMATERIAL.ID = BeStringUtilities::Wtoi(sKey.c_str());
					oMATERIAL.NUXY = BeStringUtilities::Wtof(oSplitVec.at(6).c_str());

					oMaterialMap[sKey] = oMATERIAL;
				}
				else if (std::string::npos != tmpStr.FindI(L",DENS"))
				{
					if (oMaterialMap.count(sKey) > 0)
					{
						oMATERIAL = oMaterialMap[sKey];
					}
					oMATERIAL.ID = BeStringUtilities::Wtoi(sKey.c_str());
					oMATERIAL.DENS = BeStringUtilities::Wtof(oSplitVec.at(6).c_str());

					oMaterialMap[sKey] = oMATERIAL;
				}
				else if (std::string::npos != tmpStr.FindI(L",PRXY"))
				{
					if (oMaterialMap.count(sKey) > 0)
					{
						oMATERIAL = oMaterialMap[sKey];
					}
					oMATERIAL.ID = BeStringUtilities::Wtoi(sKey.c_str());
					oMATERIAL.PRXY = BeStringUtilities::Wtof(oSplitVec.at(6).c_str());

					oMaterialMap[sKey] = oMATERIAL;
				}
			}
			else if (tmpStr.StartsWith(L"RLBLOCK,"))
			{
				// 取得实常数信息
				BeStringUtilities::Split(tmpStr.c_str(), L",", oSplitVec);
				iDataSum = BeStringUtilities::Wtoi(oSplitVec.at(1).c_str());

				getline(in, line);
				getline(in, line);

				for (int iDat = 1; iDat <= iDataSum; iDat++)
				{
					tmpStr.clear();
					if (getline(in, line))
					{
						tmpStr.AppendUtf8(line.c_str());

						bRealExistFlg = true;
						// 解析RLBLOCK信息
						oRLBlock = ParseRLBLOCKInf(tmpStr);
						// 保存RLBLOCK信息到DB
						SaveRLBLOCKInfToDb(oRLBlock);
					}
				}
			}
			else if (tmpStr.StartsWith(L"NBLOCK,"))
			{
				// 取得节点信息
				bvector<WString> vecNBLOCK;
				BeStringUtilities::Split(tmpStr.data(), L",", vecNBLOCK);
				iDataSum = BeStringUtilities::Wtoi(vecNBLOCK.at(4).c_str());

				tmpStr.clear();
				if (getline(in, line))
				{
					for (int iDat = 1; iDat <= iDataSum; iDat++)
					{
						mdlDialog_completionBarUpdate(oBar, wcMessageText, iDat * iPercent / iDataSum);

						tmpStr.clear();
						if (getline(in, line))
						{
							tmpStr.AppendUtf8(line.c_str());

							// 解析NBLOCK信息
							oNBlock = ParseNBLOCKInf(tmpStr);
							// 保存NBLOCK信息到DB
							SaveNBLOCKInfToDb(oNBlock);
						}
					}
				}
			}
			else if (tmpStr.StartsWith(L"EBLOCK,"))
			{
				// 取得单元信息
				bvector<WString> vecEBLOCK;
				BeStringUtilities::Split(tmpStr.data(), L",", vecEBLOCK);
				iDataSum = BeStringUtilities::Wtoi(vecEBLOCK.at(4).c_str());

				tmpStr.clear();
				if (getline(in, line))
				{
					for (int iDat = 1; iDat <= iDataSum; iDat++)
					{
						mdlDialog_completionBarUpdate(oBar, wcMessageText, iPercent / 2 + iDat * iPercent / iDataSum);

						tmpStr.clear();
						if (getline(in, line))
						{
							tmpStr.AppendUtf8(line.c_str());

							// 解析EBLOCK信息
							oEBlock = ParseEBLOCKInf(tmpStr);
							// 保存EBLOCK信息到DB
							SaveEBLOCKInfToDb(oEBlock);
						}
					}
				}
			}
			else if (tmpStr.StartsWith(L"BBLOCK,"))
			{
				// 取得单元信息
				iDataSum = BeStringUtilities::Wtoi(tmpStr.substr(27, 10).c_str());

				tmpStr.clear();
				if (getline(in, line))
				{
					for (int iDat = 1; iDat <= iDataSum; iDat++)
					{
						mdlDialog_completionBarUpdate(oBar, wcMessageText, iPercent / 2 + iDat * iPercent / iDataSum);

						tmpStr.clear();
						if (getline(in, line))
						{
							tmpStr.AppendUtf8(line.c_str());

							// 解析BBLOCK信息
							oBBlock = ParseBBLOCKInf(tmpStr);
							// 保存BBLOCK信息到DB
							SaveBBLOCKToDb(oBBlock);
						}
					}
				}
			}
		}
	}

	bvector<WString>().swap(oSplitVec);

	// 保存材料信息
	if (oMaterialMap.size() > 0)
		SaveMaterialInfToDb(oMaterialMap);

	if (bRealExistFlg == false)
	{
		// RLBLOCK信息
		oRLBlock.RealConstantNumber = 1;
		oRLBlock.Property.append(L"0.00000000");
		// 保存RLBLOCK信息到DB
		SaveRLBLOCKInfToDb(oRLBlock);
	}

	return true;
}

/**
* @brief  解析RLBLOCK信息
* @param[in] sRLBlock   所有实常数信息字符串集合
* @return 分割后的实常数信息
*/
RLBLOCK ImportCAEInf::ParseRLBLOCKInf(Bentley::WString sRLBlock)
{
	RLBLOCK		oRLBLOCK;
	int			iCount;
	WString		sProperty;
	WString		sTmp;

	// 实常数
	oRLBLOCK.RealConstantNumber = BeStringUtilities::Wtoi(sRLBlock.substr(0, 8).c_str());
	// 实常数值的个数
	iCount = BeStringUtilities::Wtoi(sRLBlock.substr(8, 8).c_str());

	if (iCount > 0)
	{
		for (int i = 0; i < iCount; i++)
		{
			sTmp = sRLBlock.substr(18 + 16 * i, 16);
			sTmp.Trim();

			if (BeStringUtilities::Wtof(sTmp.c_str()) > 0)
			{
				if (sProperty.length() > 0)
					sProperty.append(L",");

				sProperty.append(sTmp);
			}
		}
	}

	if (sProperty.length() == 0)
		sProperty.append(L"0.00000000");

	// 实常数属性
	oRLBLOCK.Property = sProperty;

	return oRLBLOCK;
}

/**
* @brief  解析RLBLOCK信息
* @param[in] sRLBlock   所有实常数信息字符串集合
* @return 分割后的实常数信息
*/
bool ImportCAEInf::SaveRLBLOCKInfToDb(RLBLOCK oRLBlock)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO REALCONSTANTS(ProjectId, RealConstantNumber, Property) VALUES (@ProjectId, @RealConstantNumber, @Property)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);
	// 实常数ID
	m_statment->BindInt(2, oRLBlock.RealConstantNumber);
	// 实常数名称
	Utf8String sProperty;
	BeStringUtilities::WCharToUtf8(sProperty, oRLBlock.Property.c_str());

	m_statment->BindUtf8String(3, sProperty, m_statment->BindMakeCopy::MAKE_COPY_No);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 保存材料信息
bool ImportCAEInf::SaveMaterialInfToDb(bmap<WString, MATERIAL> oMaterialMap)
{
	StatusInt	rc;
	AString sSql;
	Utf8String sName;
	Utf8String sType;

	bmap<WString, MATERIAL>::iterator iter;
	for (iter = oMaterialMap.begin(); iter != oMaterialMap.end(); iter++)
	{
		MATERIAL oMATERIAL = iter->second;// 材料信息

		sSql.clear();
		sSql.append("INSERT INTO MATERIALS(ProjectId, ID, Name, Type, EX, NUXY, DENS, PRXY)  VALUES (@ProjectId, @ID, @Name, @Type, @EX, @NUXY, @DENS, @PRXY)\n");

		m_statment->Prepare(m_db, sSql.c_str());

		// CAE工程名
		m_statment->BindInt(1, m_ProjectId);
		// 材料ID
		m_statment->BindInt(2, oMATERIAL.ID);
		// 材料名
		sName.clear();
		if (oMATERIAL.Name.size() == 0)
		{
			char cTmp[30];
			sprintf_s(cTmp, "Material%d", oMATERIAL.ID);
			sName.append(cTmp);
		}
		else
		{
			BeStringUtilities::WCharToUtf8(sName, oMATERIAL.Name.c_str());
		}
		m_statment->BindUtf8CP(3, sName.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
		// 材料类型
		sType.clear();
		BeStringUtilities::WCharToUtf8(sType, oMATERIAL.Type.c_str());
		m_statment->BindUtf8CP(4, sType.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
		// EX
		m_statment->BindDouble(5, oMATERIAL.EX);
		// NUXY
		m_statment->BindDouble(6, oMATERIAL.NUXY);
		// DENS
		m_statment->BindDouble(7, oMATERIAL.DENS);
		// PRXY
		m_statment->BindDouble(8, oMATERIAL.PRXY);

		rc = m_statment->Step();
		m_statment->Finalize();
	}

	return true;
}

/**
* @brief  读取CAE计算结果信息
* @param[in] oAddResultFileSet   CAE计算结果文件集合
* @param[in] oBar			  进度条对象
* @param[in] wcMessageText    进度条显示字符
* @param[in] iPercent         进度条显示百分比
* @param[out] 处理结果
*/
bool ImportCAEInf::ReadCAEResults(bset<BeFileName> oAddResultFileSet, MSDialogP oBar, WCharP wcMessageText, int iPercent)
{
	BeFileName sResultFileName;// CAE计算结果文件
	bset<BeFileName>::iterator it;//定义前向迭代器
	int iDat = 1;
	int iDataSum = oAddResultFileSet.size();

	//遍历集合中的所有元素
	for (it = oAddResultFileSet.begin(); it != oAddResultFileSet.end(); it++, iDat++)
	{
		mdlDialog_completionBarUpdate(oBar, wcMessageText, iPercent + iDat * iPercent / iDataSum);

		sResultFileName = *it;
		WString 	tmpStr;

		xml_document l_Doc;
		xml_parse_result result = l_Doc.load_file(sResultFileName.c_str());
		if (result.status != xml_parse_status::status_ok)
		{
			return false;
		}
		
		// 读取项目节点
		xpath_node project_node = l_Doc.select_single_node("/Results/Project");
		if (!project_node.node().empty())
		{			
			WString sProjectName;// CAE工程名
			// 取得CAE工程名
			BeStringUtilities::CurrentLocaleCharToWChar(sProjectName, project_node.node().attribute("name").as_string());

			// 检查结果文件中CAE工程名与网格模型的CAE工程名是否相同
			if (m_ProjectName.CompareTo(sProjectName.c_str()) == 0)
			{
				int iCaseId; // 工况ID
				WString wsName; // 工况名称
				WString wsProperty; // 工况描述

				// 读取工况节点
				xpath_node case_node = l_Doc.select_single_node("/Results/Project/Case");
				 
				if (!case_node.node().empty())
				{
					// 取得工况ID
					iCaseId = case_node.node().attribute("id").as_int();
					// 取得工况名称
					BeStringUtilities::CurrentLocaleCharToWChar(wsName, case_node.node().attribute("name").as_string());
					// 取得工况描述
					BeStringUtilities::CurrentLocaleCharToWChar(wsProperty, case_node.node().attribute("property").as_string());

					// 删除计算结果信息
					DelRESULTSInfByCaseId(iCaseId);

					// 解析工况信息并存入DB中
					SaveCASESInfToDb(iCaseId, wsName, wsProperty);

					int iNodeId;

					//所有的node节点
					xpath_node_set nodes = l_Doc.select_nodes("//Node");
					for (xpath_node_set::const_iterator node = nodes.begin(); node != nodes.end(); node++)
					{
						iNodeId = node->node().attribute("id").as_int();//节点号

						// 解析RESULTS信息并存入DB中
						SaveRESULTSInfToDb(iCaseId, iNodeId, node->node());

					}
				}

				// 读取破坏信息节点
				xpath_node Damage_node = l_Doc.select_single_node("/Results/Project/Damages");

				if (!Damage_node.node().empty())
				{
					xpath_node_set ElementNodes = l_Doc.select_nodes("//Element");
					for (pugi::xpath_node_set::const_iterator node = ElementNodes.begin(); node != ElementNodes.end(); node++)
					{
						int	ElementId = node->node().attribute("id").as_int(); // CAE元素ID
						const pugi::char_t*  ch = node->node().first_child().value();// 破坏信息

						// 解析破坏信息并存入DB中
						SaveDamageInfToDb(ElementId, ch);
					}
				}
			}
		}
	}

	return true;
}

// 解析破坏信息并存入DB中
bool ImportCAEInf::SaveDamageInfToDb(int ElemId, const char *data_value)
{
	StatusInt	rc; // 处理结果
	AString strDamage;// 破坏信息
	int damage_data[8];// 各节点的破坏信息
	int iCnt = 0; 
	int tatus = 0; // CAE元素破坏标识（1：有破坏，0：无破坏）

	strDamage.append(data_value);
	bvector<Utf8String> vecDamage;// 各节点的破坏信息
	// 以半角空格分割字符串
	BeStringUtilities::Split(strDamage.data(), " ", vecDamage);

	// 循环取得8个节点的破坏信息
	for (auto iter = vecDamage.begin(); iter != vecDamage.end(); iter++)
	{
		damage_data[iCnt] = atoi(iter->data());
		if ((damage_data[iCnt] != 0 && damage_data[iCnt] != 16))
			tatus = 1;
		iCnt++;
	}

	// SQL生成
	m_statment->Prepare(m_db, "INSERT INTO DAMAGE(ProjectId, ElemId,Node1,Node2,Node3,Node4,Node5,Node6,Node7,Node8,Status) VALUES (@ProjectId, @ElemId,@Node1,@Node2,@Node3,@Node4,@Node5,@Node6,@Node7,@Node8,@Status)");
	m_statment->BindInt(1, m_ProjectId);
	m_statment->BindInt(2, ElemId);
	m_statment->BindInt(3, damage_data[0]);
	m_statment->BindInt(4, damage_data[1]);
	m_statment->BindInt(5, damage_data[2]);
	m_statment->BindInt(6, damage_data[3]);
	m_statment->BindInt(7, damage_data[4]);
	m_statment->BindInt(8, damage_data[5]);
	m_statment->BindInt(9, damage_data[6]);
	m_statment->BindInt(10, damage_data[7]);
	m_statment->BindInt(11, tatus);

	rc = m_statment->Step();
	m_statment->Finalize();
	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// 删除计算结果信息
bool ImportCAEInf::DelRESULTSInfByCaseId(int iCaseId)
{
	StatusInt	rc;
	AString lsWhere;
	AString lsSql;

	lsWhere.append("WHERE \n");
	lsWhere.append("ProjectId = @ProjectId AND CaseId = @CaseId \n");

	lsSql.append("DELETE FROM CASES \n");
	lsSql.append(lsWhere);
	m_statment->Prepare(m_db, lsSql.c_str());
	// 设置删除条件
	m_statment->BindInt(1, m_ProjectId);
	m_statment->BindInt(2, iCaseId);

	rc = m_statment->Step();
	if (rc == BE_SQLITE_DONE)
	{
		m_statment->Finalize();
	}

	lsSql.clear();
	lsSql.append("DELETE FROM RESULTS \n");
	lsSql.append(lsWhere);
	m_statment->Prepare(m_db, lsSql.c_str());
	// 设置删除条件
	m_statment->BindInt(1, m_ProjectId);
	m_statment->BindInt(2, iCaseId);

	rc = m_statment->Step();
	if (rc == BE_SQLITE_DONE)
	{
		m_statment->Finalize();
	}

	return true;
}

// 解析工况信息并存入DB中
bool ImportCAEInf::SaveCASESInfToDb(int iCaseId, WStringR wsName, WStringR wsProperty)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO CASES(ProjectId, CaseId, CaseName, Property) VALUES (@ProjectId, @CaseId, @CaseName, @Property)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);
	// 工况ID
	m_statment->BindInt(2, iCaseId);
	// 工况名称
	Utf8String asName;
	BeStringUtilities::WCharToUtf8(asName, wsName.c_str());
	m_statment->BindUtf8String(3, asName, m_statment->BindMakeCopy::MAKE_COPY_No);

	// 工况描述
	Utf8String asProperty;
	BeStringUtilities::WCharToUtf8(asProperty, wsProperty.c_str());
	m_statment->BindUtf8String(4, asProperty, m_statment->BindMakeCopy::MAKE_COPY_No);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ImportCAEInf::SaveRESULTSInfToDb(int iCaseId, int iNodeId, xml_node ValueNode)
{
	StatusInt	rc;
	AString sSql = "";
	WString sField = L"ProjectId,CaseId, ID ";
	WString sField_ValNam = L"@ProjectId,@CaseId, @ID ";
	bvector<double> bcValue;

	WString strNodeName;
	// CAE工程ID
	bcValue.push_back(m_ProjectId);
	// CaseId
	bcValue.push_back(iCaseId);
	// ID
	bcValue.push_back(iNodeId);

	for (auto input : ValueNode.children())
	{
		strNodeName.clear();
		strNodeName.AppendA(input.name());
		strNodeName.ToUpper();

		sField.append(L", ");
		sField_ValNam.append(L", @");
		if (strNodeName.Equals(RESULTTYPE_SZ) == true)
		{	
			//T_Sy = Sz
			sField.append(RESULTTYPE_SY);			
			sField_ValNam.append(RESULTTYPE_SY);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_SY) == true)
		{
			//T_Sz = Sy
			sField.append(RESULTTYPE_SZ);
			sField_ValNam.append(RESULTTYPE_SZ);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_SXZ) == true)
		{
			//T_Sxy = -Sxz
			sField.append(RESULTTYPE_SXY);
			sField_ValNam.append(RESULTTYPE_SXY);
			bcValue.push_back(-1*input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_SYZ) == true)
		{
			//T_Syz = -Syz
			sField.append(RESULTTYPE_SYZ);
			sField_ValNam.append(RESULTTYPE_SYZ);
			bcValue.push_back(-1 * input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_SXY) == true)
		{
			//T_Sxz = Sxy
			sField.append(RESULTTYPE_SXZ);
			sField_ValNam.append(RESULTTYPE_SXZ);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_UZ) == true)
		{
			//T_Uy = -Uz
			sField.append(RESULTTYPE_UY);
			sField_ValNam.append(RESULTTYPE_UY);
			bcValue.push_back(-1 * input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_UY) == true)
		{
			//T_Uz = Uy
			sField.append(RESULTTYPE_UZ);
			sField_ValNam.append(RESULTTYPE_UZ);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_EPTOZ) == true)
		{
			//T_EPTOy = EPTOz
			sField.append(RESULTTYPE_EPTOY);
			sField_ValNam.append(RESULTTYPE_EPTOY);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_EPTOY) == true)
		{
			//T_EPTOz = EPTOy
			sField.append(RESULTTYPE_EPTOZ);
			sField_ValNam.append(RESULTTYPE_EPTOZ);
			bcValue.push_back(input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_EPTOXZ) == true)
		{
			//T_EPTOxy = -EPTOxz
			sField.append(RESULTTYPE_EPTOXY);
			sField_ValNam.append(RESULTTYPE_EPTOXY);
			bcValue.push_back(-1 * input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_EPTOYZ) == true)
		{
			//T_EPTOyz = -EPTOyz
			sField.append(RESULTTYPE_EPTOYZ);
			sField_ValNam.append(RESULTTYPE_EPTOYZ);
			bcValue.push_back(-1 * input.text().as_double());
		}
		else if (strNodeName.Equals(RESULTTYPE_EPTOXY) == true)
		{
			//T_EPTOxz = EPTOxy
			sField.append(RESULTTYPE_EPTOXZ);
			sField_ValNam.append(RESULTTYPE_EPTOXZ);
			bcValue.push_back(input.text().as_double());
		}
		else
		{
			//T_Sx = Sx
			//T_Ux = Ux
			//T_EPTOx = EPTOX
			sField.append(strNodeName);
			sField_ValNam.append(strNodeName);
			bcValue.push_back(input.text().as_double());
		}
	}

	sSql.append("INSERT INTO RESULTS(");
	// 字段名
	Utf8String asField;
	BeStringUtilities::WCharToUtf8(asField, sField.c_str());
	sSql.append(asField);
	sSql.append(") VALUES (");
	Utf8String asField_ValNam;
	BeStringUtilities::WCharToUtf8(asField_ValNam, sField_ValNam.c_str());
	sSql.append(asField_ValNam);
	sSql.append(")");

	m_statment->Prepare(m_db, sSql.c_str());

	bvector<double> ::iterator iter = bcValue.begin();
	bvector<double> ::iterator end = bcValue.end();
	for (int iValIdx = 1; iter != end; iter++, iValIdx++)
	{
		if (iValIdx <= 3)
		{
			m_statment->BindInt(iValIdx, *iter);
		}
		else
		{
			m_statment->BindDouble(iValIdx, *iter);
		}
	}

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

NBLOCK ImportCAEInf::ParseNBLOCKInf(Bentley::WString sNBlock)
{
	size_t		iLen;
	NBLOCK		oNBLOCK;

	iLen = sNBlock.length();

	// 节点号
	oNBLOCK.NodeNumber = BeStringUtilities::Wtoi(sNBlock.substr(0, 9).c_str());

	// 节点的面号
	oNBLOCK.FaceNumber = BeStringUtilities::Wtoi(sNBlock.substr(9, 8).c_str());

	// 节点类型
	oNBLOCK.NodeType = BeStringUtilities::Wtoi(sNBlock.substr(17, 1).c_str());

	// 相对线的位置
	oNBLOCK.LineLocation = BeStringUtilities::Wtoi(sNBlock.substr(18, 9).c_str());

	// 节点的X坐标
	if (iLen >= 48)
	{
		oNBLOCK.XPoint = BeStringUtilities::Wtof(sNBlock.substr(27, 21).c_str()); // x = x
	}
	else
	{
		oNBLOCK.XPoint = 0.0;
	}

	// 节点的Z坐标
	if (iLen >= 69)
	{
		oNBLOCK.ZPoint = BeStringUtilities::Wtof(sNBlock.substr(48, 21).c_str()); // z = y
	}
	else
	{
		oNBLOCK.ZPoint = 0.0;
	}

	// 节点的Y坐标
	if (iLen >= 90)
	{
		oNBLOCK.YPoint = -1 * BeStringUtilities::Wtof(sNBlock.substr(69, 21).c_str()); // y = -z;
	}
	else
	{
		oNBLOCK.YPoint = 0.0;
	}

	return oNBLOCK;
}

bool ImportCAEInf::SaveNBLOCKInfToDb(NBLOCK oNBlock)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO NODES(ProjectId, ID, FaceNumber, NodeType, LineLocation, X, Y, Z) VALUES (@ProjectId, @ID, @FaceNumber, @NodeType, @LineLocation, @X, @Y, @Z)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);
	// 节点号
	m_statment->BindInt(2, oNBlock.NodeNumber);
	// 节点的面号
	m_statment->BindInt(3, oNBlock.FaceNumber);
	// 节点类型
	m_statment->BindInt(4, oNBlock.NodeType);
	// 相对线的位置
	m_statment->BindInt(5, oNBlock.LineLocation);
	// 节点的X坐标
	m_statment->BindDouble(6, oNBlock.XPoint);
	// 节点的Y坐标
	m_statment->BindDouble(7, oNBlock.YPoint);
	// 节点的Z坐标
	m_statment->BindDouble(8, oNBlock.ZPoint);

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

BBLOCK ImportCAEInf::ParseBBLOCKInf(Bentley::WString sBBlock)
{
	BBLOCK oBBlock;

	// CAE工程ID
	oBBlock.Id = BeStringUtilities::Wtoi(sBBlock.substr(0, 10).c_str());

	// 边界面节点的拓扑关系
	oBBlock.NodeIdStr = sBBlock.substr(10, 40).c_str();

	// 边界类型
	oBBlock.BCType = BeStringUtilities::Wtoi(sBBlock.substr(50, 10).c_str());

	return oBBlock;
}

bool  ImportCAEInf::SaveBBLOCKToDb(BBLOCK oBBlock)
{
	StatusInt	rc;
	Utf8String str;

	m_statment->Prepare(m_db, "INSERT INTO BOUNDARY(ProjectId, NodeIdStr, BCType) VALUES (@ProjectId, @NodeIdStr, @BCType)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);
	// 边界面节点的拓扑关系
	str.clear();
	BeStringUtilities::WCharToUtf8(str, oBBlock.NodeIdStr.c_str());
	str.Trim();
	m_statment->BindUtf8CP(2, str.c_str(), m_statment->BindMakeCopy::MAKE_COPY_No);
	// 边界类型
	m_statment->BindInt(3, oBBlock.BCType);
	  
	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

EBLOCK ImportCAEInf::ParseEBLOCKInf(Bentley::WString sEBlock)
{
	EBLOCK		oEBLOCK;

	// 材料号
	oEBLOCK.MaterialNumber = BeStringUtilities::Wtoi(sEBlock.substr(0, 9).c_str());

	// 单元类型号
	oEBLOCK.ElemType = BeStringUtilities::Wtoi(sEBlock.substr(9, 9).c_str());

	// 实常数号
	oEBLOCK.RealConstantNumber = BeStringUtilities::Wtoi(sEBlock.substr(18, 9).c_str());

	// 截面号
	oEBLOCK.SectionIDAttrNumber = BeStringUtilities::Wtoi(sEBlock.substr(27, 9).c_str());

	// 坐标号
	oEBLOCK.ElemCoordSysNumber = BeStringUtilities::Wtoi(sEBlock.substr(36, 9).c_str());

	// 单元生死
	oEBLOCK.BirthDeathFlag = BeStringUtilities::Wtoi(sEBlock.substr(45, 9).c_str());

	// 实体引用号
	oEBLOCK.SolidModelRefNumber = BeStringUtilities::Wtoi(sEBlock.substr(54, 9).c_str());

	// 单元形状
	oEBLOCK.ElemShapeFlg = BeStringUtilities::Wtoi(sEBlock.substr(63, 9).c_str());

	// 节点数目
	oEBLOCK.NodeCount = BeStringUtilities::Wtoi(sEBlock.substr(72, 9).c_str());

	// 保留位
	oEBLOCK.FieldTmp = BeStringUtilities::Wtoi(sEBlock.substr(81, 9).c_str());

	// 单元号
	oEBLOCK.ElemNumber = BeStringUtilities::Wtoi(sEBlock.substr(90, 9).c_str());

	// 节点1
	oEBLOCK.Node1 = BeStringUtilities::Wtoi(sEBlock.substr(99, 9).c_str());

	if (oEBLOCK.NodeCount > 1)
	{
		// 节点2
		oEBLOCK.Node2 = BeStringUtilities::Wtoi(sEBlock.substr(108, 9).c_str());

		if (oEBLOCK.NodeCount > 2)
		{
			// 节点3
			oEBLOCK.Node3 = BeStringUtilities::Wtoi(sEBlock.substr(117, 9).c_str());

			// 节点4
			oEBLOCK.Node4 = BeStringUtilities::Wtoi(sEBlock.substr(126, 9).c_str());

			// 当前元素为实体元素时
			if (oEBLOCK.NodeCount > 4)
			{
				// 节点5
				oEBLOCK.Node5 = BeStringUtilities::Wtoi(sEBlock.substr(135, 9).c_str());

				// 节点6
				oEBLOCK.Node6 = BeStringUtilities::Wtoi(sEBlock.substr(144, 9).c_str());

				// 节点7
				oEBLOCK.Node7 = BeStringUtilities::Wtoi(sEBlock.substr(153, 9).c_str());

				// 节点8
				oEBLOCK.Node8 = BeStringUtilities::Wtoi(sEBlock.substr(162, 9).c_str());
			}
		}
	}

	return oEBLOCK;
}

bool ImportCAEInf::SaveEBLOCKInfToDb(EBLOCK oEBlock)
{
	StatusInt	rc;

	m_statment->Prepare(m_db, "INSERT INTO ELEMENTS(ProjectId, MaterialNumber, ElemType, RealConstantNumber, SectionIDAttrNumber, ElemCoordSysNumber, BirthDeathFlag, SolidModelRefNumber, ElemShapeFlg, NodeCount, FieldTmp, ElemID, Node1, Node2, Node3, Node4, Node5, Node6, Node7, Node8) VALUES (@ProjectId, @MaterialNumber, @ElemType, @RealConstantNumber, @SectionIDAttrNumber, @ElemCoordSysNumber, @BirthDeathFlag, @SolidModelRefNumber, @ElemShapeFlg, @NodeCount, @FieldTmp, @ElemID, @Node1, @Node2, @Node3, @Node4, @Node5, @Node6, @Node7, @Node8)");

	// CAE工程ID
	m_statment->BindInt(1, m_ProjectId);

	// 材料号
	m_statment->BindInt(2, oEBlock.MaterialNumber);

	// 单元类型号
	m_statment->BindInt(3, oEBlock.ElemType);

	// 实常数号
	m_statment->BindInt(4, oEBlock.RealConstantNumber);

	// 截面号
	m_statment->BindInt(5, oEBlock.SectionIDAttrNumber);

	// 坐标号
	m_statment->BindInt(6, oEBlock.ElemCoordSysNumber);

	// 单元生死
	m_statment->BindInt(7, oEBlock.BirthDeathFlag);

	// 实体引用号
	m_statment->BindInt(8, oEBlock.SolidModelRefNumber);

	// 单元形状
	m_statment->BindInt(9, oEBlock.ElemShapeFlg);

	// 节点数目
	m_statment->BindInt(10, oEBlock.NodeCount);

	// 保留位
	m_statment->BindInt(11, oEBlock.FieldTmp);

	// 单元号
	m_statment->BindInt(12, oEBlock.ElemNumber);

	// 节点1
	m_statment->BindInt(13, oEBlock.Node1);

	if (oEBlock.NodeCount > 1)
	{
		// 节点2
		m_statment->BindInt(14, oEBlock.Node2);

		if (oEBlock.NodeCount > 2)
		{
			// 节点3
			m_statment->BindInt(15, oEBlock.Node3);

			// 节点4
			m_statment->BindInt(16, oEBlock.Node4);

			// 当前元素为实体元素时
			if (oEBlock.NodeCount > 4)
			{
				// 节点5
				m_statment->BindInt(17, oEBlock.Node5);

				// 节点6
				m_statment->BindInt(18, oEBlock.Node6);

				// 节点7
				m_statment->BindInt(19, oEBlock.Node7);

				// 节点8
				m_statment->BindInt(20, oEBlock.Node8);
			}
		}
	}

	rc = m_statment->Step();
	m_statment->Finalize();

	if (rc == BE_SQLITE_DONE)
	{
		return true;
	}
	else
	{
		return false;
	}
}
