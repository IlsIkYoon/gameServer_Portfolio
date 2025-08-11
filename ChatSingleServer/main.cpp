#include "Contents/ContentsResource.h"
#include "Contents/ContentsPacket.h"
#include "Contents/ContentsFunc.h"
#include "Player/Player.h"
#include "Network/Network.h"
#include "Contents/MonitorManager.h"
#include "CommonProtocol.h"

//----------------------------------------
//모니터링 출력을 위한 전역 변수들
//----------------------------------------
extern DWORD g_messageCount;
extern long long g_playerCount;

extern unsigned long long g_CPacketCreateCount;
extern unsigned long long g_CPacketDeleteCount;
extern unsigned long long g_CPacketReleaseCount;
extern unsigned long long g_CPacketAllocCount;

extern int g_concurrentCount;
extern int g_workerThreadCount;
extern int g_maxSessionCount;

extern unsigned long long g_LoginSessionCount;
extern unsigned long long g_LogoutSessionCount;

extern unsigned long long g_AcceptTps;
extern unsigned long long* g_pRecvTps;
extern unsigned long long* g_pSendTps;

extern unsigned long long g_CPacketAllocCount;

//-----------------------------------------
// 플레이어 카운팅을 위한 변수
//-----------------------------------------
extern unsigned long long g_TotalPlayerCreate;
extern unsigned long long g_PlayerLogInCount;
extern unsigned long long g_PlayerLogOut;


extern DWORD g_frame;

//----------------------------------------
//컨텐츠 쓰레드 핸들
//----------------------------------------
HANDLE g_ContentsThread;

HANDLE g_ExitEvent;

extern CLFree_Queue<Job> g_ContentsJobQ;

//----------------------------------------
// 출력 함수들
//----------------------------------------
void PrintConsolMenu();
void UpdateMonitorData();

//----------------------------------------
// 입력 값을 체크. 입력 값에 따라 출력 혹은 종료
//----------------------------------------
void CheckInput();

//----------------------------------------
// ESC를 눌렀을 때 서버 종료 절차가 진행되는 함수
//----------------------------------------
void ExitAllProcess();

CWanServer* pLib;
CCpuUsage CPUUsage;
CMonitorManager g_Monitor;
CPdhManager g_PDH;

int main()
{
	timeBeginPeriod(1);
	
	DWORD startTimeTick;
	DWORD endTimeTick;
	DWORD resultTimeTick;

	procademy::CCrashDump dump;

	int portNum;
	int MonitorPortNum;
	int SessionMaxCount;
	int WorkerThreadCount;
	int ConcurrentCount;
	int ntProtocol;
	int stickyKey;

	g_ExitEvent = CreateEvent(NULL, true, false, NULL);

	pLib = new CWanServer;
	g_ContentsThread = (HANDLE)_beginthreadex(NULL, 0, ContentsThreadFunc, NULL, NULL, NULL);

	txParser.GetData("Chat_Single_Config.ini");
	txParser.SearchData("PortNum", &portNum);
	txParser.SearchData("SessionMaxCount", &SessionMaxCount);
	txParser.SearchData("WorkerThreadCount", &WorkerThreadCount);
	txParser.SearchData("ConcurrentCount", &ConcurrentCount);
	txParser.SearchData("MonitorPortNum", &MonitorPortNum);
	txParser.SearchData("ntProtocolCode", &ntProtocol);
	txParser.SearchData("stickyKey", &stickyKey);
	txParser.CloseData();

	pLib->RegistPortNum(portNum);
	pLib->RegistSessionMaxCount(SessionMaxCount);
	pLib->RegistWorkerThreadCount(WorkerThreadCount);
	pLib->RegistConcurrentCount(ConcurrentCount);
	pLib->Regist_NetworkProtocolCode(ntProtocol);
	pLib->Regist_PacketStickyKey(stickyKey);

	pLib->Start();

	g_PDH.RegistEthernetMax(3);
	g_PDH.Start();
	g_Monitor.RegistMonitor(L"127.0.0.1", MonitorPortNum);

	
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

	return 0;
}


void PrintConsolMenu()
{
	printf("\n");
	printf("Q : DebugBreak() || W : WriteProfile() || E : Exit() \n");
	printf("\n");
	printf("-----------------------------------------------------------------------\n");
	printf("PortNum :%d || MaxSessionCount : %d || WORKERThread Count : %d || ConcurrentCount : %d\n",
		pLib->_portNum, pLib->_sessionMaxCount, pLib->_workerThreadCount, pLib->_concurrentCount);
	printf("-----------------------------------------------------------------------\n");
	return;
}

void CheckInput()
{
	if (_kbhit())
	{
		char c = _getch();
		if (c == 'q' || c == 'Q')
		{
			__debugbreak(); //메모리 Dump를 확인하기 위한 debugBreak();
		}
		if (c == 'w' || c == 'W')
		{
			WriteAllProfileData();
		}
		if (c == 'E' || c == 'e')
		{
			ExitAllProcess();
		}
	}
}




void ExitAllProcess()
{
	pLib->ExitNetWorkManager();
}

void UpdateMonitorData()
{
	long long localRECVTPS = 0;
	long long localSENDTPS = 0;
	int localFrame = 0;
	int localProcessorTotal = 0;
	double PrivateMem = 0;
	double ProcessNonPaged = 0;
	double TotalNonPaged = 0;
	double Available = 0;
	long long localAcceptTPS = InterlockedExchange(&g_AcceptTps, 0);
	int localPacketPoolTps = (int)g_CPacketAllocCount;
	
	DWORD localMessageCount = InterlockedExchange(&g_messageCount, 0);



	for (int i = 0; i < pLib->_workerThreadCount; i++)
	{
		localRECVTPS += InterlockedExchange(&g_pRecvTps[i], 0);
		localSENDTPS += InterlockedExchange(&g_pSendTps[i], 0);
	}
	localFrame = InterlockedExchange(&g_frame, 0);

	g_PDH.GetMemoryData(&PrivateMem, &ProcessNonPaged, &TotalNonPaged, &Available);
	
	CPUUsage.UpdateCpuTime();
	localProcessorTotal = (int)CPUUsage.ProcessTotal();
	
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_RUN, 1);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_CPU, localProcessorTotal);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_MEM, (int)(PrivateMem / 1024 / 1024));
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SESSION, pLib->_sessionLoginCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_PLAYER, (int)g_PlayerLogInCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_UPDATE_TPS, (int)localRECVTPS);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_PACKET_POOL, localPacketPoolTps);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_UPDATEMSG_POOL, localMessageCount);
	g_Monitor.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_ACCEPT_TPS, localAcceptTPS);	
}