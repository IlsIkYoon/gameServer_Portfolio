#pragma once
#include "GameEchoServerResource.h"


class CMonitorManager
{
	// ����͸� �������ݿ� ���缭 ������ �ְ� ������ 
	ULONG64 mSessionID;
	int myServerNo;

public:
	CMonitorManager();

	bool RegistMonitor(std::wstring ip, unsigned short portNum);
	bool UpdateMonitor(BYTE dataType, int dataValue);
};