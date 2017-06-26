#pragma once

double OLEMethod(int nType, VARIANT *pvResult, IDispatch *pDisp,LPOLESTR ptName, int cArgs...);
double CoGetServerPID(IUnknown* punk, DWORD* pdwPID);
void	EndProccessPID(DWORD pdwPID);