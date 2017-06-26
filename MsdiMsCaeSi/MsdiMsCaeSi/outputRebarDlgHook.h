#pragma once

void outputRebarCmd(char* unparsed);

// 保存配筋数据
void writeRebarResultsToX(bmap<WString, ReBarInfo>& mapRebarResults);
// 读取配筋数据
void readRebarResultsFromX(bmap<WString, ReBarInfo>& mapRebarResults);
void CalRebar_adjustVSashDialogItems(MSDialogP  db, BSIRect *pOldContent, bool refreshItems);
void HookDialog_CalRebar_InterfaceDynamic(DialogMessage *dmP);	
void HookPushBtn_CalRebar_Refresh(DialogItemMessage* dimP);				//刷新
void HookPushBtn_CalRebar_Calculate(DialogItemMessage* dimP);			//配筋计算
void HookPushBtn_CalRebar_OutPut(DialogItemMessage* dimP);				//输出

void HookSash_CalRebar_vSashHook(DialogItemMessage* dimP);				//分隔条拉动事件	
void HookGeneric_CalRebar_Canva(DialogItemMessage* dimP);
void HookTree_CalRebar_SectionTree(DialogItemMessage* dimP);			//截面树
void HookListBox_CalRebar_CalcResult(DialogItemMessage* dimP);			//结果listbox
void HookListBox_CalRebar_RebarInfo(DialogItemMessage* dimP);			//钢筋listbox,添加一行,绑定txt控件到0行第1列
void HookItem_CalRebar_CellCombbox_Diameter(DialogItemMessage* dimP);	//钢筋listbox的直径cell所用txt的事件
void HookItem_CalRebar_CellTxt_RebarNumber(DialogItemMessage* dmp);		//钢筋listbox的根数cell所用txt的事件
void HookComboBox_CalRebar_RebarType(DialogItemMessageP dimP);			//钢筋类型下拉框			
void HookComboBox_CalRebar_ConcrType(DialogItemMessageP dimP);			//混泥土类型下拉框
void HOOKText_CalRebar_RebarTesion(DialogItemMessageP dimP);			//钢筋抗拉强度值变动时关联combbox
void HOOKText_CalRebar_ConcrTesion(DialogItemMessageP dimP);			//混泥土抗拉强度值变动时关联combbox
void HookPdm_CalRebar_AddRebarLine(DialogItemMessage* dimP);			//创建配筋线右键菜单
void HookPdm_CalRebar_DelRebarLine(DialogItemMessage* dimP);			//删除配筋线	
void HookDialog_CalRebar_AddRebarLine(DialogMessage *dmP);				//配筋线命名对话框


// 初始化树显示(截面信息)
void InitSectionTreeDisp(DialogItemP dimp);
// 初始化窗体数据
bool InitFormData();									
// 初始化所有ListBox的数据
void InitListBoxData(MSDialogP dbP);
//初始化控件与变量关联的结构体ReBarCalDlgInfo
void InitReBarCalDlgInfo();
//获取控件值
void GetDataFromControls(rebarInfo& orebarInfo);
//获取listbox中EditCell值
void GetListBoxEditCellStringValue(DialogItemMessage* dimP, RscId listboxId, WCharCP* textValue);	
//初始化绘制云图的颜色参数
void InitDrawColorShape(DialogItemMessage* dimP, GuiTreeNode* pRebarLineNode, const bvector<WString>& vecData);

void getStressList(const WChar* field, bvector<double>& tesionList);
double GetTrapezoidArea(double dTop, double bBottom, double dHeigh);	//通过应力和点距计算梯形面积
double GetAreaByDiameter(int& number, double dAs, double diameter=6.0);	//根据直径和配筋面积计算:参考配筋面积，根数
double GetAreaByRebarCount(double& diameter, double dAs, int number);	//根据根数和配筋面积计算:参考配筋面积，直径
int checkTesionValid(const bvector<double>& tesionList, double dFt);
void CountOfZeroPoint(const bvector<double>& tesionList, int& nZero);
void GetValidTesionHeadAndTail(const bvector<double>& tesionList, double Ft, int& iStart, int& iEnd);
void GetAAndAct(double dPrev, double dNext, double Ft, double span, double& dA, double& dAct);
void GetConcretePositions(const bvector<double>& tesionList, double iTail, double span, double Ft, double dLineLength, double dEndSpan, int& iConcr_No, double& dConcrP);

double GetAPeriodBendingStress(double x1, double y1, double x2, double y2, double dD);
double GetBendingStress(const bvector<double>& tesionList, double span, double dLineLength);

double GetAPeriodAxialStress(double x1, double y1, double x2, double y2);
double GetAxialStress(const bvector<double>& tesionList, double span, double dLineLength);
/************************************************************************/
/*	dLineLength 配筋线长度
/*  ReBarInfo	界面参数和部分计算结果的结构体
/*	@brief		配筋计算
/************************************************************************/
bool calculateRebarArea(double dLineLength, ReBarInfo& info);

/*-----------------------------------------------------------------
*  功能描述: 在指定的Dialog指定的Item中绘制应力图
*	@param	dbP	IN	指定的Dialog指针
*	@param	diP	IN	指定的DialogItem指针
*
*  @return	无
-----------------------------------------------------------------*/
void drawStressView(MSDialogP dbP, DialogItemP diP);
bool createLineString(EditElementHandleR eeh, bvector<DPoint3d> &points, DgnModelRefP modelRef);
bool createLine(EditElementHandleR eeh, DSegment3dCR segment, DgnModelRefP modelRef);

void CalRebar_setupPopupMenu(DialogMessageP dmP);

// 取得配筋线指定间距位置上的应力值
bool getRebarLineStressbyDistances(ReBarInfo& orebarInfo, const bvector<bvector<PointOutData>>& m_FacetVec, CurveVectorPtr curve, double dLineLength, TransformCP tMatrix, RotMatrixCR rotMatrix);

void SetStressValToListBox(MSDialogP dbP, const ReBarInfo& info);	//写入应力Listbox
void SetResultValToListBox(MSDialogP dbP, const ReBarInfo& info);	//计算结果写入结果列表和钢筋列表
void SetRebarInfoToListBox(MSDialogP dbP, const ReBarInfo& info);	//获取listbox第二列cell的钢筋根数,计算结果写入钢筋信息listbox

void     mdlDialog_rectInset
(
BSIRect        *rP,
int            deltaX,        /* positive moves in */
int            deltaY
);

void Draw3DCell
(
MSElementDescr      *cellP,
MSDialogP           dbP,
BSIRect             *localRectP,
ViewFlags           *viewflagsP,
bool                nDices
);
