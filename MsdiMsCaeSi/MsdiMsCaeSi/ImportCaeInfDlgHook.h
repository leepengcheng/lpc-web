#pragma once
bool SelectFilePath(BeFileNameR File_path, int Title_ID, int FilterString_ID);

void ImportCAEInfCmd(WCharCP unparsed);
void SelectCaeModelPath(DialogItemMessage *dimP);

void ImportCaeInfDialog_AddResultFileHook(DialogItemMessage *dimP);
void ImportCaeInfDialog_TBSelImpDatHook(DialogItemMessage *dimP);
void ImportCaeInfDialog_DelResultFileHook(DialogItemMessage *dimP);
void ImportCaeInfDialog_mdlDialogHook(DialogMessage   *dmP);
void ImportCaeInfDialog_SelectCaeModelHook(DialogItemMessage *dmP);

