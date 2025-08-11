#include "ContentsResource.h"
#include "ContentsFunc.h"
#include "Player/Player.h"
#include "Msg/ContentsPacket.h"
#include "Msg/Message.h"
#include "Sector/Sector.h"
#include "ContentsThread/ContentsThreadManager.h"
#include "Msg/CommonProtocol.h"
#include "MonitorManager.h"
//-------------------------------------
// 에러 메세지 종료에 대한 카운트
//-------------------------------------
extern unsigned long long g_ErrorSectorSize;
extern unsigned long long g_ErrorNetworkLen;
extern unsigned long long g_ErrorChatMsgLen;
extern unsigned long long g_ErrorPacketType;

extern unsigned long long g_AcceptTps;

CLFree_Queue<CPacket*> g_ContentsJobQ[CONTENTS_THREADCOUNT];

extern std::list<CPlayer*> Sector[SECTOR_MAX][SECTOR_MAX];
extern std::recursive_mutex SectorLock[SECTOR_MAX][SECTOR_MAX];

extern long long g_playerCount;

extern CWanServer* networkServer;

extern CContentsThreadManager* contentsManager;

extern CMonitorManager g_MonitorManager;
extern CPdhManager g_PDH;

//출력용 카운팅 변수들 DEBUG//
unsigned long long g_loginMsgCnt;
unsigned long long g_sectorMoveMsgCnt;
unsigned long long g_chatMsgCnt;

extern unsigned long long g_TotalPlayerCreate;
extern unsigned long long g_PlayerLogInCount;
extern unsigned long long g_PlayerLogOut;

extern unsigned long long g_CPacketAllocCount;
extern unsigned long long g_CPacketReleaseCount;

extern CCpuUsage g_CpuUsage;

unsigned long long g_UpdateMsgTps;

void CWanServer::_OnMessage(CPacket* message, ULONG64 sessionID)
{
	unsigned short playerIndex;
	WORD contentsType;
	CPlayer* localPlayerList;

	InterlockedIncrement(&g_UpdateMsgTps);

	if (message->GetDataSize() < sizeof(contentsType))
	{
		std::string error;
		error += "Incomplete Message || Contents Header size Error";
		
		EnqueLog(error);

		DisconnectSession(sessionID);
		return;
	}

	*message >> contentsType;
	playerIndex = GetIndex(sessionID);
	localPlayerList = contentsManager->playerList->playerArr;

	localPlayerList[playerIndex]._timeOut = timeGetTime();

	switch (contentsType)
	{
	case en_PACKET_CS_CHAT_REQ_LOGIN:
	{
		InterlockedIncrement(&g_loginMsgCnt);
		HandleLoginMessage(message, sessionID);
	}
		break;

	case en_PACKET_CS_CHAT_REQ_SECTOR_MOVE:
	{
		InterlockedIncrement(&g_sectorMoveMsgCnt);
		HandleSectorMoveMessage(message, sessionID);
	}
		break;

	case en_PACKET_CS_CHAT_REQ_MESSAGE:
	{
		InterlockedIncrement(&g_chatMsgCnt);
		HandleChatMessage(message, sessionID);
	}
		break;

	case en_PACKET_CS_CHAT_REQ_HEARTBEAT:
		break;

	default:
		InterlockedIncrement(&g_ErrorPacketType);
		networkServer->DisconnectSession(sessionID);
		break;
	}
}

void CWanServer::_OnAccept(ULONG64 sessionID)
{
	unsigned short playerIndex = GetIndex(sessionID);

	CPlayer* localPlayerList = contentsManager->playerList->playerArr;

	if (localPlayerList[playerIndex]._status != static_cast<BYTE>(CPlayer::enSTATUS::IDLE))
	{
		__debugbreak();
	}

	localPlayerList[playerIndex]._status = static_cast<BYTE>(CPlayer::enSTATUS::SESSION);
	localPlayerList[playerIndex]._sessionID = sessionID;
	localPlayerList[playerIndex]._timeOut = timeGetTime();
	localPlayerList[playerIndex].sectorX = 0;
	localPlayerList[playerIndex].sectorY = 0;
	localPlayerList[playerIndex].accountNo = 0;
	localPlayerList[playerIndex]._move = false;

}
void CWanServer::_OnSend(ULONG64 ID)
{
	//할 일 없음
	return;
}
void CWanServer::_OnDisConnect(ULONG64 sessionID)
{
	unsigned short playerIndex = GetIndex(sessionID);
	CPlayer* localPlayerList = contentsManager->playerList->playerArr;

	if (localPlayerList[playerIndex]._sessionID != sessionID)
	{
		std::string error;
		error = "OnDisConnect_Session not Match";
		EnqueLog(error);
		return;
	}

	if (localPlayerList[playerIndex]._status < static_cast<BYTE>(CPlayer::enSTATUS::PLAYER))
	{
		if (localPlayerList[playerIndex]._status == static_cast<BYTE>(CPlayer::enSTATUS::PENDING_SECTOR))
		{
			InterlockedDecrement(&g_PlayerLogInCount);
			InterlockedIncrement(&g_PlayerLogOut);
		}

		localPlayerList[playerIndex]._status = static_cast<BYTE>(CPlayer::enSTATUS::IDLE);
		return;
	}

	InterlockedDecrement(&g_PlayerLogInCount);
	InterlockedIncrement(&g_PlayerLogOut);

	CheckSector(sessionID);

	int SectorX = localPlayerList[playerIndex].sectorX;
	int SectorY = localPlayerList[playerIndex].sectorY;

	{
		std::lock_guard guard(SectorLock[SectorX][SectorY]);
		Sector[SectorX][SectorY].remove(&localPlayerList[playerIndex]);
	}

	contentsManager->keyList->DeleteID(localPlayerList[playerIndex].accountNo, sessionID);
	localPlayerList[playerIndex].Clear();
}

CWanServer::CWanServer()
{
}

void TimeOutCheck()
{
	return; // 현재 타임아웃 검사를 하지 않음


	DWORD deadLine = timeGetTime() - dfNETWORK_PACKET_RECV_TIMEOUT;

	int sessionCount;
	sessionCount = networkServer->GetSessionMaxCount();
	CPlayer* localPlayerList;

	localPlayerList = contentsManager->playerList->playerArr;

	for (int i = 0; i < sessionCount; i++)
	{
		if (localPlayerList[i]._status == static_cast<BYTE>(CPlayer::enSTATUS::IDLE))
		{
			continue;
		}

		if (localPlayerList[i]._timeOut < deadLine)
		{
			if (localPlayerList[i]._timeOut == 0)
			{
				continue;
			}
			networkServer->DisconnectSession(localPlayerList[i].GetID());	
		}
	}
}

bool UpdateMonitorData()
{
	//데이터 정산 후에 모니터 데이터로 보내주는 함수
	int localPlayerCount = (int)g_PlayerLogInCount;
	int localSessionCount = (int)g_LoginSessionCount;
	int localProcessTotal;
	int localUpdateMsgTps;
	double privateMem;
	g_PDH.GetMemoryData(&privateMem, nullptr, nullptr, nullptr);
	g_CpuUsage.UpdateCpuTime();
	localProcessTotal = (int)g_CpuUsage.ProcessTotal();
	long long localAcceptTPS = InterlockedExchange(&g_AcceptTps, 0);

	localUpdateMsgTps = (int)InterlockedExchange(&g_UpdateMsgTps, 0);

	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_RUN, 1);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_CPU, localProcessTotal);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SESSION, localSessionCount);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_PLAYER, localPlayerCount);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_SERVER_MEM, (int)privateMem / 1024 / 1024);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_UPDATE_TPS, localUpdateMsgTps);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_CHAT_PACKET_POOL, (int)g_CPacketAllocCount);
	g_MonitorManager.UpdateMonitor(dfMONITOR_DATA_TYPE_GAME_ACCEPT_TPS, (int)localAcceptTPS);

	return true;
}