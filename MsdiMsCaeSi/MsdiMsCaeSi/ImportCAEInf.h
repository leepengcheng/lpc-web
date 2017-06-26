/**
* @file     ImportCAEInf.h
* @brief    从文件中导入数据，然后保存到数据库中
* @author   d
* @date     2016-10-11
* @version  A001
* @copyright Poweritech
*/

using namespace pugi;

/**
* @brief CAE信息导入类
*/
class ImportCAEInf
{
public:
	ImportCAEInf(void);
	~ImportCAEInf();
public:

	/**
	* @brief  导入CAE计算结果
	*/
	void ImportCAEResult(BeFileName sModelFileName, bset<BeFileName> oAddResultFileSet, int ProjectId);

private:
	/** 定义数据库变量 m_db*/
	Db m_db;

	/** 定义保存数据点 m_savept*/
	Savepoint *m_savept;

	/** 定义数据库状态变量 m_statment*/
	Statement *m_statment;

	/** 定义CAE工程ID m_statment*/
	int m_ProjectId;

	/** 定义CAE工程名 m_statment*/
	WString m_ProjectName;

private:
	/**
	* @brief  取得新的CAE工程ID
	* @return CAE工程ID
	*/
	void GetProjectID(void);

	/**
	* @brief  取得的CAE工程名
	* @param[in] iProjectId   CAE网格模型ID
	* @return CAE工程名
	*/
	WString getProjectNameById(int iProjectId);

	/**
	* @brief  判断DB中是否相同CAE工程名的数据
	* @param[out] iOldProjectId   已存在的CAE网格模型ID
	* @return 判断结果(true:存在，false:不存在)
	*/
	bool ChkProjectExist(int& iOldProjectId);

	bool SaveProjectNameToDb(void);
	bool SaveProjectDescriptionToDb(WString& sProjectDescription);
	bool SaveProjectUnitToDb(WString& sProjectUnit);
	// 保存单元类型信息
	bool SaveElemTypeToDb(bvector<WString> oSplitVec);
	// 保存材料信息
	bool SaveMaterialInfToDb(bmap<WString, MATERIAL> oMaterialMap);

	/**
	* @brief  根据网格模型ID删除相关的CAE网格模型数据
	* @param[in] iOldProjectId   CAE网格模型ID
	*/
	void DelCAEElemsByProjectId(int iOldProjectId);

	/**
	* @brief  读取CAE元素值参数
	* @param[in] sModelFileName   CAE网格模型文件
	* @param[in] oBar			  进度条对象
	* @param[in] wcMessageText    进度条显示字符
	* @param[in] iPercent         进度条显示百分比
	* @param[out] 处理结果
	*/
	bool ReadCAEElems(BeFileName sModelFileName, MSDialogP oBar, WCharP wcMessageText, int iPercent);

	/**
	* @brief  读取CAE计算结果信息
	* @param[in] oAddResultFileSet   CAE计算结果文件集合
	* @param[in] oBar			  进度条对象
	* @param[in] wcMessageText    进度条显示字符
	* @param[in] iPercent         进度条显示百分比
	* @param[out] 处理结果
	*/
	bool ReadCAEResults(bset<BeFileName> oAddResultFileSet, MSDialogP oBar, WCharP wcMessageText, int iPercent);

	/**
	* @brief  解析NBLOCK信息
	* @param[in] sNBlock   所有单元块信息字符串集合
	* @return 分割后的单元块信息
	*/
	NBLOCK ParseNBLOCKInf(Bentley::WString sNBlock);

	/**
	* @brief  解析NBLOCK信息
	* @param[in] sNBlock   所有单元块信息字符串集合
	* @return 分割后的单元块信息
	*/
	bool SaveNBLOCKInfToDb(NBLOCK oNBlock);

	/**
	* @brief  解析RLBLOCK信息
	* @param[in] sRLBlock   所有实常数信息字符串集合
	* @return 分割后的实常数信息
	*/
	RLBLOCK ParseRLBLOCKInf(Bentley::WString sRLBlock);

	/**
	* @brief  解析RLBLOCK信息
	* @param[in] sRLBlock   所有实常数信息字符串集合
	* @return 分割后的实常数信息
	*/
	bool SaveRLBLOCKInfToDb(RLBLOCK oRLBlock);

	// 解析EBLOCK信息
	EBLOCK ParseEBLOCKInf(Bentley::WString sEBlock);
	bool SaveEBLOCKInfToDb(EBLOCK oEBlock);
	
	//解析BBLOCK信息
	BBLOCK ParseBBLOCKInf(Bentley::WString sBBlock);
	bool SaveBBLOCKToDb(BBLOCK oBBlock);

	// 删除计算结果信息
	bool DelRESULTSInfByCaseId(int iCaseId);

	// 解析工况信息并存入DB中
	bool SaveCASESInfToDb(int iCaseId, WStringR wsName, WStringR wsProperty);
	// 解析RESULTS信息并存入DB中
	bool SaveRESULTSInfToDb(int iCaseId, int iNodeId, xml_node ValueNode);
	// 解析破坏信息并存入DB中
	bool SaveDamageInfToDb(int id, const char *data_value);
};