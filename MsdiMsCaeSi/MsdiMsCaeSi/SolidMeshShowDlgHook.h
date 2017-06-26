#pragma once
#include "MsdiMsCaeSipch.h"

void ShowCAESolidAllCmd(WCharCP unparsed);
void SolidMeshShowDlg_SolidAllShowHook(DialogMessage   *dmP);

class SolidMeshShowDlgHook
{
public:
	SolidMeshShowDlgHook();
	~SolidMeshShowDlgHook();
};

