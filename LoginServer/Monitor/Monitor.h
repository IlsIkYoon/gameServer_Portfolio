#pragma once
#include "Resource/LoginServerResource.h"


class CMonitor_delete
{
public:
	unsigned long long loginSuccessCount;
	unsigned long long loginFailedCount;

	



public:
	//����Լ� 
	void ConsolPrintAll();

	void ConsolPrintLoginCount();
	void ConsolPrintUserCount();



};