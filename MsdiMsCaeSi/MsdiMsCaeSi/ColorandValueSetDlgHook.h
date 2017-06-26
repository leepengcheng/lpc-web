#pragma once
#include "BentlyCommonfile.h"

enum SourceTypeDCS
{
	SolidDisplay,
	SurfaceDisplay,
	CaseCompare,
};

void HookDialog_ColorSetting(DialogMessage* dmP);

ListModel* createListModel(int	nCols);

void updateColorListBox(MSDialog* db);

void HookPushBtn_ColorSet_ColorChooser(DialogItemMessage *dimP);

void TColorChooser(DialogItemMessage *dimP);

void ListBoxCreate(DialogItemMessage *dimP);

void Updownbutton(DialogItemMessage *dimP);

void HookText_CellOfColor(DialogItemMessage *dimP);

void HookToggle_ColorSet_Automatic(DialogItemMessage* dimP);

void HookText_ColorSet_ColorValue(DialogItemMessage* dimP);

void HookPushBtn_ColorSet_Apply(DialogItemMessage* dimP);

void HookPushBtn_ColorSet_Gray(DialogItemMessage* dimP);

void openColorShapeSetting(DrawColorShape* optDCS, SourceTypeDCS source);