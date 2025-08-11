#include "ChatMultiServer.h"
#include "ContentsResource.h"
#include "ContentsThread/ContentsFunc.h"
#include "ContentsThread//ContentsThreadManager.h"
#include "Log/Monitoring.h"
#include "ContentsThread/MonitorManager.h"
CWanServer* networkServer;
CContentsThreadManager* contentsManager;
CMonitorManager g_MonitorManager;
CPdhManager g_PDH;

bool ChatMultiServer()
{
	CMonitor serverMornitor;
	procademy::CCrashDump dump;
	g_PDH.Start();
	networkServer = new CWanServer;
	contentsManager = new CContentsThreadManager(networkServer);
	contentsManager->Start();

	DWORD prevTime;
	DWORD currentTime;
	DWORD resultTime;

	prevTime = timeGetTime();

	while (1)
	{
		UpdateMonitorData();

		currentTime = timeGetTime();

		resultTime = currentTime - prevTime;

		Sleep(1000 - resultTime);
		
		prevTime += 1000;
	}

	contentsManager->End();

	return true;
}