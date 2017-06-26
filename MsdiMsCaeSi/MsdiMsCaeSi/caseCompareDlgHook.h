#pragma once

void caseCompareCmd(char* unparsed);

void HookComboBox_CaseComp_InstanceL(DialogItemMessage* dimP);

void HookComboBox_CaseComp_InstanceR(DialogItemMessage* dimP);

void HookButton_CaseComp_SettingL(DialogItemMessage* dimP);

void HookButton_CaseComp_SettingR(DialogItemMessage* dimP);

void HookButton_CaseComp_Compare(DialogItemMessage* dimP);

void HookTree_CaseComp_CaseList(DialogItemMessage* dimP);

void HookDialog_CaseCompare(DialogMessage* dmP);

void HookComboBox_CaseComp_ResultType(DialogItemMessage* dimP);

void HookComboBox_CaseComp_ShowType(DialogItemMessage* dimP);

void HookComboBox_CaseComp_ShowModel(DialogItemMessage* dimP);

void initCaseListTree(RscId treeID);

void checkCompareEnable(MSDialogP db);

struct placeCompareContoursTool : public DgnPrimitiveTool
{
protected:
	placeCompareContoursTool(int toolId, int toolPrompt);

	virtual void _OnPostInstall() override;

	virtual void _OnRestartTool() override { InstallNewInstance(GetToolId(), GetToolPrompt()); }

	virtual bool _OnDataButton(DgnButtonEventCR ev) override;

	virtual bool _OnResetButton(DgnButtonEventCR ev) override { return true; }

public:
	static void InstallNewInstance(int toolId, int toolPrompt);

};