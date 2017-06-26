#pragma once
void ClearClipOutData();//清除切面数值输出

void HookDialog_ClipResult(DialogMessage* dmP);

void HookTree_ClipResult_ClipList(DialogItemMessage* dimP);

void HookOptButton_ClipResult_D_ALL(DialogItemMessage* dimP);

void HookOptButton_ClipResult_Z_ALL(DialogItemMessage* dimP);

void HookButton_ClipResult_Clear(DialogItemMessage *dmP);

void HookButton_ClipResult_Submit(DialogItemMessage *dmP);

void HookButton_ClipResult_OutDGN(DialogItemMessage *dmP);

void HookTabpage_ClipResult_ResultType(DialogItemMessage* dimP);

void clipResultShowCmd(char* unparsed);

void CopyMeshToDgn(DgnModelRefP meshModel, DgnModelRefP dgnModel, WString PlotName);

void ClipResult_adjustVSashDialogItems(MSDialogP db, BSIRect *pOldContent, bool refreshItems);

void HookSash_ClipResult_vSashHook(DialogItemMessage* dimP);