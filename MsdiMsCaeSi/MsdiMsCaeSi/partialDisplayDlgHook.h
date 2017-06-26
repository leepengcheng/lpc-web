#pragma once

struct Condition
{
	int iName;
	int iOpt;
	int iVal;
};
bool matchCondition(MeshProperty& prop, Condition& condition);

void HookComboBox_PartDis_CondName(DialogItemMessage* dimP);

void HookComboBox_PartDis_CondVal(DialogItemMessage* dimP);

void HookButton_PartDis_Add(DialogItemMessage* dimP);

void HookButton_PartDis_Del(DialogItemMessage* dimP);

void HookButton_PartDis_Clear(DialogItemMessage* dimP);

void HookListBox_PartDis_Conditions(DialogItemMessage* dimP);