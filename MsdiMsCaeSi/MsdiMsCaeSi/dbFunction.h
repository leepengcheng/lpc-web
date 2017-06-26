#pragma once

string ws2s(const wstring& ws);

WString s2ws(const char* str);

WString num2Ws(int i);

WString num2Ws(double d);

int wtoi(WCharCP ws);

void getRstTypeInfFromDB(bmap<WString, WString>& RstType_Name, bmap<WString, WString>& RstType_Unit);

void SetResultType(DialogItemMessage *dimP, RscId Itemid, int iProjectId, int caseID, bvector<WString>& vecRstTypeCodes);

WString getNGNameOfElem(EditElementHandleR eeh);

StatusInt OpenDB(Db& db);

WString getDBFlag(string dbPath);

void setDBFlag(const char* dbFlag, string dbPath);

string initDBFilePath();

void getProjectInfoById(WStringR sProjectName, WStringR sProjectUnit, int iProjectId);

int getNodeCntByProjectid(int iProjectID);

int getElemCntByProjectid(int iProjectID);

void getCaseInfoByProjectid(int iProjectID, bvector<int>& vecCaseID, bvector<WString>& vecCaseName, bvector<WString>& vecCaseProp);

//材料表查询函数
void getMaterialIDByProjectid(int iProjectID, bvector<int>& materialIDs);

void getMaterialAllByPK(int iProjectID, int materialID, WString& mName, WString& mType, WString& mEx, WString& mNUXY, WString& mDENS, WString& mPRXY);

//元素类型查询函数
void getElemTypeIDByProjectid(int iProjectID, bvector<int>& ElemTypeIDs);

void getElemNameByPK(int iProjectID, int elemType, WString& elemName);

//实常数查询函数
void getRealConsIDByProjectid(int iProjectID, bvector<int>& RealConIDs);

void getRealConsPropByPK(int iProjectID, int realConstant, WString& realConstantProp);

int getElemCntUsingMaterialByProjectid(int iProjectID, int materialID);

void getExtremumResultAndNode(int iProjectID, int caseID, bvector<int>& nodeLst, double& extremum, const char* field, bool bMax = true);

void showMeshsOfInstance_DB(int iProjectID, WString NGName, bvector<WString> meshIDs);

void showAllOfInstance_DB(int iProjectID, WString NGName);

void getRstTypeByPjIdAndCaseId(int ProjectId, int CaseId, bvector<WString>& RstTypeCodes, bvector<WString>& RstTypeNames);

bool getMaxMinXYZPtValOfProject(int iProjectID, double& minx, double& maxx, double& miny, double& maxy, double& minz, double& maxz);

double getMaxCoordinateValueOfProject(int iProjectID, const char* field);

double getMinCoordinateValueOfProject(int iProjectID, const char* field);

DPoint3d getNodeCoordinate(int iProjectID, int nodeID);

DPoint3d getNodeCoordinate(int iProjectID, int caseID, const char* field, double fieldValue);

void deleteAllFaceOfInstance(WString ngName);

//有关NgName的操作函数
//由于NgName的获取类函数大多有许多参数，但不一定每次全都需要，因此定义以下全局变量用于填充不用的参数
extern WString	UNUSE_WSTRING;
extern int		UNUSE_INT;

WString makeNgName_Model(int proID, int index = -1);

bool splitNgName_Model(WString ngName, WStringR proName, int& proID, int& index, WStringR instanceName, WStringR proUnit);

WString makeNgName_Clip(int proID, int indexID, WString clipName);

bool splitNgName_Clip(WString ngName, WStringR proName, int& proID, int& index, WStringR clipName, WStringR instanceName, WStringR proUnit);

WString makeInstanceName(WString proName, int index);

void splitInstanceName(WString instanceName, WString& proName, int& index);

void savingInstanceInfo(bvector<InstanceInfo> instanceInfos);

bvector<InstanceInfo> readInstanceInfo();
