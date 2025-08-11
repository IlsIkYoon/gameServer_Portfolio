#include "Monitor.h"
#include "Contents/ContentsManager.h"

CMonitor_delete g_Monitor;
extern CContentsManager* g_ContentsManager;

void CMonitor_delete::ConsolPrintAll()
{
	ConsolPrintLoginCount();
	ConsolPrintUserCount();

}


void CMonitor_delete::ConsolPrintLoginCount()
{
}

void CMonitor_delete::ConsolPrintUserCount()
{
	printf("Current User : %d\n", g_ContentsManager->GetCurrentUser());

	return;
}