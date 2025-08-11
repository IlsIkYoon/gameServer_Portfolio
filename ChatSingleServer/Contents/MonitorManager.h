#pragma once
#include "ContentsResource.h"

class CMonitorManager
{
	ULONG64 _mSessionID;
	int _myServerNo;

public:
	CMonitorManager();

	bool RegistMonitor(std::wstring ip, unsigned short portNum);
	bool UpdateMonitor(BYTE dataType, int dataValue);
};