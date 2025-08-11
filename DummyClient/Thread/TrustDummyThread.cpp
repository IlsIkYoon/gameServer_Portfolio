#include "resource.h"
#include "TrustDummyThread.h"
#include "Network/ThreadNetworkManager.h"
#include "Msg/RandStringManager.h"
#include "Action/Action.h"
#include "DummyThread.h"
#include "Msg/ContentsPacket.h"

extern DWORD g_threadCount;
extern DWORD g_ThreadIndex;
extern DWORD g_clientCount;
extern DWORD g_trustMode;

extern CDummySession* g_DummySessionArr;


extern std::list<CDummySession*> g_Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern std::mutex g_SectorLock[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];

extern CRandStringManager g_RandStringManager;


extern thread_local DWORD g_prevFrameTime;
extern thread_local DWORD g_fixedDeltaTime;
extern thread_local DWORD g_frame;
extern thread_local DWORD g_sec;

extern thread_local DWORD tls_myThreadIndex;

//------------------------------------------
// 출력용 변수들
//------------------------------------------
extern unsigned long long g_moveStartPacket;
extern unsigned long long g_moveStopPacket;
extern unsigned long long g_recvdMoveStopPacket;

extern unsigned long long g_sendchatMsg;
extern unsigned long long g_recvdChatCompleteMsg;
extern unsigned long long g_sendLocalChatMsgTotal;
extern unsigned long long g_recvLocalChatMsgTotal;

extern unsigned long long g_sessionLoginCount;
extern unsigned long long g_sessionLogoutCount;

unsigned long long g_trustMsgProbability;

unsigned int DummyTrustThreadFunc(void*)
{
	CThreadNetworkManager ThreadNetworkManager;

	DWORD startTime = timeGetTime();

	DWORD dwUpdateTick = startTime - FrameSec;
	g_sec = startTime / 1000;

	g_prevFrameTime = startTime - FrameSec;// 초기 값 설정

	while (1)
	{
		DWORD currentTime = timeGetTime();
		DWORD deltaTime = currentTime - g_prevFrameTime;
		DWORD deltaCount = deltaTime / FrameSec;
		g_fixedDeltaTime = deltaCount * FrameSec;


		ThreadNetworkManager.HandleNetworkEvent();

		HandleRecvData_All(ThreadNetworkManager.mySessionArr, ThreadNetworkManager.dwMySessionCount);
		Trust_ContentsWork(ThreadNetworkManager.mySessionArr, ThreadNetworkManager.dwMySessionCount, g_fixedDeltaTime);

		DWORD logicTime = timeGetTime() - currentTime;

		if (logicTime < FrameSec)
		{

			Sleep(FrameSec - logicTime);
		}

		g_frame++;

		g_prevFrameTime += g_fixedDeltaTime;

	}

	return 0;
}

bool Trust_ContentsWork(CDummySession* mySessionArr, DWORD mySessionCount, DWORD fixedDeltaTime)
{
	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		mySessionArr[i].Move(fixedDeltaTime);

		long currentDummyStatus;
		currentDummyStatus = mySessionArr[i].dummyStatus;

		switch (currentDummyStatus)
		{
		case static_cast<long>(CDummySession::DummyStatus::Created):
			DoSessionConnect(&mySessionArr[i]);
			break;
		case static_cast<long>(CDummySession::DummyStatus::Connected):
			DoChatOrMoveStart(&mySessionArr[i]);
			break;
		case static_cast<long>(CDummySession::DummyStatus::SendingMove):
		{
			if (rand() % g_trustMsgProbability != 0)
			{
				break;
			}
			SendMoveStopMessage(&mySessionArr[i]);
			SendHeartBeatMessage(&mySessionArr[i]);
			mySessionArr[i].dummyStatus = static_cast<long>(CDummySession::DummyStatus::WaitingMoveEnd);
		}
			break;
		case static_cast<long>(CDummySession::DummyStatus::SendingChat):
		{
			if (rand() % g_trustMsgProbability != 0)
			{
				break;
			}
			SendChatEndMessage(&mySessionArr[i]);
			SendHeartBeatMessage(&mySessionArr[i]);
			mySessionArr[i].dummyStatus = static_cast<long>(CDummySession::DummyStatus::WaitingChatEnd);
		}
			break;
		case static_cast<long>(CDummySession::DummyStatus::RecvdChatComplete):
			DoChatOrMoveStart(&mySessionArr[i]);
			break;
		case static_cast<long>(CDummySession::DummyStatus::RecvdMoveEnd):
			DoChatOrMoveStart(&mySessionArr[i]);
			break;
		default:
			break;
		}

	}

	return true;
}

bool DoChatOrMoveStart(CDummySession* _session)
{
	if (rand() % g_trustMsgProbability != 0)
	{
		return false;
	}

	if(rand() % 2 == 0)
	{
		SendRandomChat(_session);
	}
	else 
	{
		SendMoveStartMessage(_session);
		_session->dummyStatus = static_cast<long>(CDummySession::DummyStatus::SendingMove);
	}
	return true;
}

bool SendRandomChat(CDummySession* _session)
{
	char* sendMsg;
	int sendLen;
	g_RandStringManager.GetRandString(&sendMsg, &sendLen);
	_session->dummyStatus = static_cast<long>(CDummySession::DummyStatus::SendingChat);

	SendLocalChatMessage(_session, sendMsg, sendLen);
	InterlockedIncrement(&g_sendchatMsg);

	if (g_trustMode == TRUST_MODE)
	{
		CheckReceiversInSector(_session, sendMsg, sendLen);
	}

	InterlockedIncrement(&g_sendLocalChatMsgTotal);

	return true;
}
