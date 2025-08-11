#include "GameEchoServer.h"
#include "Thread/Auth.h"
#include "Thread/Game.h"
#include "Network/Network.h"
#include "Monitor/MonitorManager.h"
#include "CommonProtocol.h"
#include <set>

CWanServer* g_NetworkManager;

CGameThreadWork* g_GameThreadWork;
CAuthThreadWork* g_AuthThreadWork;

CMonitorManager g_Monitor;
CPdhManager g_PDH;
CCpuUsage CPUUsage;

void PrintConsolMenu();
void CheckInput();
void UpdateMonitorData();
void ExitAllProcess();

extern unsigned long long g_CPacketAllocCount;
unsigned int SendThreadFunc(void*);

extern unsigned long sendSuccessCount;

DWORD g_frame;

bool GameEchoServer()
{
	timeBeginPeriod(1);

	DWORD startTimeTick;
	DWORD endTimeTick;
	DWORD resultTimeTick;

	procademy::CCrashDump dump;

	unsigned short portNum;
	unsigned short monitorPortNum;
	int sessionMaxCount;
	int workerThreadCount;
	int concurrentThreadCount;
	unsigned char ntProtocol;
	unsigned char stickyKey;
	int sendBatchSize;

	txParser.GetData("GameEchoServer_config.ini");
	txParser.SearchData("PortNum", &portNum);
	txParser.SearchData("SessionMaxCount", &sessionMaxCount);
	txParser.SearchData("WorkerThreadCount", &workerThreadCount);
	txParser.SearchData("ConcurrentCount", &concurrentThreadCount);
	txParser.SearchData("MonitorPortNum", &monitorPortNum);
	txParser.SearchData("ntProtocolCode", &ntProtocol);
	txParser.SearchData("stickyKey", &stickyKey);
	txParser.SearchData("SendBatchSize", &sendBatchSize);
	txParser.CloseData();

	
	g_GameThreadWork = new CGameThreadWork;
	g_AuthThreadWork = new CAuthThreadWork;

	g_NetworkManager = new CWanServer;
	g_NetworkManager->RegistPortNum(portNum);
	g_NetworkManager->RegistConcurrentCount(concurrentThreadCount);
	g_NetworkManager->RegistWorkerThreadCount(workerThreadCount);
	g_NetworkManager->RegistSessionMaxCount(sessionMaxCount);
	g_NetworkManager->Regist_NetworkProtocolCode(ntProtocol);
	g_NetworkManager->Regist_PacketStickyKey(stickyKey);
	g_NetworkManager->playerManager = new CPlayerManager(sessionMaxCount);
	g_AuthThreadWork->playerManager = g_NetworkManager->playerManager;
	g_GameThreadWork->playerManager = g_NetworkManager->playerManager;

	g_NetworkManager->RegistWork(g_AuthThreadWork);
	g_NetworkManager->RegistWork(g_GameThreadWork);

	g_NetworkManager->Start();


	g_PDH.RegistEthernetMax(3);
	g_PDH.Start();
	g_Monitor.RegistMonitor(L"127.0.0.1", monitorPortNum);

	HANDLE sendThread = (HANDLE)_beginthreadex(NULL, 0, SendThreadFunc, NULL, NULL, NULL);

	startTimeTick = timeGetTime();

	while (1)
	{

		PrintConsolMenu();

		CheckInput();

		UpdateMonitorData();

		endTimeTick = timeGetTime();
		resultTimeTick = (endTimeTick - startTimeTick) / 1000;
		if (resultTimeTick < 1000)
		{
			Sleep(1000 - resultTimeTick);
		}
		startTimeTick += 1000;
	}
	return true;
}


void PrintConsolMenu()
{
	printf("Session Count : %d || ", g_NetworkManager->_sessionLoginCount);
	printf("Auth Count : %d || ", g_AuthThreadWork->workPlayerCount);
	printf("Game Count : %d\n", g_GameThreadWork->workPlayerCount);
	printf("Send Frame : %d || SendSuccess Count : %d\n", (int)InterlockedExchange(&g_frame, 0), (int)InterlockedExchange(&sendSuccessCount, 0));
}
void CheckInput()
{
	if (_kbhit())
	{
		char c = _getch();
		if (c == 'q' || c == 'Q')
		{
			__debugbreak(); //�޸� Dump�� Ȯ���ϱ� ���� debugBreak();
		}
		if (c == 'w' || c == 'W')
		{
			WriteAllProfileData();
		}
		if (c == 'E' || c == 'e')
		{
			ExitAllProcess();
		}
		if (c== 'r' || c == 'R')
		{
			ResetAllProfileDate();
		}
	}

}
void UpdateMonitorData()
{

	int gameEchoServer_ProcessTotal;
	double gameEchoServer_Memory;
	int acceptTps;
	int recvTps;
	int sendTps;
	int authThreadFrame;
	int gameThreadFrame;

	CPUUsage.UpdateCpuTime();
	gameEchoServer_ProcessTotal = (int)CPUUsage.ProcessTotal();

	g_PDH.GetMemoryData(&gameEchoServer_Memory, nullptr, nullptr, nullptr);
	
	acceptTps = (int)InterlockedExchange(&g_NetworkManager->acceptCount, 0);
	recvTps = (int)InterlockedExchange(&g_NetworkManager->recvCount, 0);
	sendTps = (int)InterlockedExchange(&g_NetworkManager->sendCount, 0);
	authThreadFrame = (int)InterlockedExchange(&g_AuthThreadWork->frame, 0);
	gameThreadFrame = (int)InterlockedExchange(&g_GameThreadWork->frame, 0);

	printf("\n");
	printf("Auth Thread Frame : %d || ", authThreadFrame);
	printf("Game Thread Frame : %d\n", gameThreadFrame);
	printf("Recv TPS :%d || Send TPS : %d\n", recvTps, sendTps);

	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_SERVER_RUN, 1);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_SERVER_CPU, gameEchoServer_ProcessTotal);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_SERVER_MEM, (int)gameEchoServer_Memory / 1024 / 1024);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_SESSION, g_NetworkManager->_sessionLoginCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_AUTH_PLAYER, g_AuthThreadWork->workPlayerCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_GAME_PLAYER, g_GameThreadWork->workPlayerCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_ACCEPT_TPS, acceptTps);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_PACKET_RECV_TPS, recvTps);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_PACKET_SEND_TPS, sendTps);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_AUTH_THREAD_FPS, authThreadFrame);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_GAME_THREAD_FPS, gameThreadFrame);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_PACKET_POOL, (int)g_CPacketAllocCount);

}

void ExitAllProcess()
{


}




unsigned int SendThreadFunc(void*)
{
	DWORD prevTime;
	DWORD currentTime;
	DWORD resultTime;

	DWORD sendTiming = 40;

	prevTime = timeGetTime();
	while (1)
	{
		InterlockedIncrement(&g_frame);
		g_NetworkManager->EnqueSendRequest();
		currentTime = timeGetTime();
		resultTime = currentTime - prevTime;
		if (sendTiming > resultTime)
		{
			Sleep(sendTiming - resultTime);
		}
		prevTime += sendTiming;
	}

	return 0;
}