#include "resource.h"
#include "Action.h"
#include "Thread/DummyThread.h"
#include "Msg/RandStringManager.h"
#include "Network/ThreadNetworkManager.h"

extern SOCKADDR_IN serverAddr;
extern char g_currentAction;

extern CRandStringManager g_RandStringManager;

extern char g_currentAction;
extern DWORD g_threadCount;
extern long* g_actionCompleteCount;
extern std::mutex actionCompleteLock;

extern thread_local DWORD tls_myThreadIndex;

//--------------------------------------------------
// 출력용 변수들
//--------------------------------------------------
extern unsigned long long g_moveStartPacket;
extern unsigned long long g_moveStopPacket;
extern unsigned long long g_recvdMoveStopPacket;

unsigned long long g_sendchatMsg;
unsigned long long g_recvdChatCompleteMsg;

unsigned long long g_sessionLoginCount;
unsigned long long g_sessionLogoutCount;

bool HandleConnectAction(CDummySession* mySessionArr, DWORD mySessionCount)
{
	bool allConnected = true;
	u_long nonBlockingMode = 1;

	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus == static_cast<long>(CDummySession::DummyStatus::Created))
		{
			DoSessionConnect(&mySessionArr[i]);
		}	

		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::Connected))
		{
			allConnected = false;
		}
	}

	if (allConnected == true)
	{
		bool completeRetval;
		completeRetval = ThreadActionComplete();

		if (completeRetval == true)
		{
			ChangeAction(static_cast<char>(CAction::Status::ACTION_MOVE));
		}

		return true;
	}

	return false;
}

bool HandleMoveAction(CDummySession* mySessionArr, DWORD mySessionCount, DWORD fixedDeltaTime)
{
	bool allActionComplete = true;

	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::Connected))
		{
			continue;
		}

		switch(mySessionArr[i].dummyStatus)
		{
		case static_cast<char>(CDummySession::DummyStatus::Connected):
			SendMoveStartMessage(&mySessionArr[i]);
			mySessionArr[i].dummyStatus = static_cast<char>(CDummySession::DummyStatus::SendingMove);
			allActionComplete = false;
			break;

		case static_cast<char>(CDummySession::DummyStatus::SendingMove): //moveStart를 보낸 상태
			break;

		default:
			__debugbreak();
			break;
		}
	}

	if (allActionComplete == true)
	{
		bool completeRetval;
		completeRetval = ThreadActionComplete();

		if (completeRetval == true)
		{
			ChangeAction(static_cast<char>(CAction::Status::ACTION_MOVESTOP));
		}

		return true;
	}

	return false;
}

bool HandleMoveStopAction(CDummySession* mySessionArr, DWORD mySessionCount)
{
	bool allActionComplete = true;

	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::Connected))
		{
			continue;
		}

		switch (mySessionArr[i].dummyStatus)
		{
		case static_cast<char>(CDummySession::DummyStatus::SendingMove):
		{
			SendMoveStopMessage(&mySessionArr[i]);
			SendHeartBeatMessage(&mySessionArr[i]);
			mySessionArr[i].dummyStatus = static_cast<char>(CDummySession::DummyStatus::WaitingMoveEnd);
			allActionComplete = false;
		}
			break;

		case static_cast<char>(CDummySession::DummyStatus::WaitingMoveEnd): //moveStart를 보낸 상태
			allActionComplete = false;
			break;

		case static_cast<char>(CDummySession::DummyStatus::RecvdMoveEnd):
			break;


		default:
			__debugbreak();
			break;
		}
	}

	if (allActionComplete == true)
	{
		bool completeRetval;
		completeRetval = ThreadActionComplete();

		if (completeRetval == true)
		{
			ChangeAction(static_cast<char>(CAction::Status::ACTION_SENDCHATMSG));
		}

		return true;
	}

	return false;
}

bool HandleCheckMsgAction(CDummySession* mySessionArr, DWORD mySessionCount)
{
	bool allRecvd = true;;
	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::SendingChat))
		{
			continue;
		}

		if (mySessionArr[i].allMsgRecvd == false)
		{
			allRecvd = false;
			break;
		}
	}

	if (allRecvd == true)
	{
		for (unsigned int i = 0; i < mySessionCount; i++)
		{
			if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::Connected))
			{
				continue;
			}

			mySessionArr[i].dummyStatus = static_cast<long>(CDummySession::DummyStatus::Connected);

		}

		bool completeRetval;
		completeRetval = ThreadActionComplete();
		if (completeRetval == true)
		{
			ChangeAction(static_cast<char>(CAction::Status::ACTION_CHECKDISCONNECT));
		}

		return true;
	}

	return false;
}

bool HandleSendChatMsgAction(CDummySession* mySessionArr, DWORD mySessionCount)
{

	bool allActionComplete = true;

	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::RecvdMoveEnd))
		{
			continue;
		}

		if (mySessionArr[i].dummyStatus == static_cast<long>(CDummySession::DummyStatus::RecvdMoveEnd))
		{
			char* sendMsg;
			int sendLen;
			g_RandStringManager.GetRandString(&sendMsg, &sendLen);
			mySessionArr[i].dummyStatus = static_cast<long>(CDummySession::DummyStatus::SendingChat);

			SendLocalChatMessage(&mySessionArr[i], sendMsg, sendLen);
			InterlockedIncrement(&g_sendchatMsg);
			CheckReceiversInSector(&mySessionArr[i], sendMsg, sendLen);
		}
		else if (mySessionArr[i].dummyStatus == static_cast<long>(CDummySession::DummyStatus::SendingChat))
		{
			mySessionArr[i].dummyStatus = static_cast<long>(CDummySession::DummyStatus::WaitingChatEnd);
			SendChatEndMessage(&mySessionArr[i]);
		}

		if (mySessionArr[i].dummyStatus != static_cast<long>(CDummySession::DummyStatus::RecvdChatComplete))
		{
			allActionComplete = false;
		}
	}

	if (allActionComplete == true)
	{
		
		bool completeRetval;
		completeRetval = ThreadActionComplete();

		if (completeRetval == true)
		{
			ChangeAction(static_cast<char>(CAction::Status::ACTION_CHECKCHATMSG));
		}
		return true;
	}
	
	return false;
}

bool HandleCheckMoveStopAction(CDummySession* mySessionArr, DWORD mySessionCount)
{
	bool allActionComplete = true;

	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus == static_cast<long>(CDummySession::DummyStatus::SendingMove))
		{
			allActionComplete = false;
			break;
		}
	}

	return allActionComplete;
}

bool HandleCheckDisconnectAction(CDummySession* mySessionArr, DWORD mySessionCount)
{
	for (unsigned int i = 0; i < mySessionCount; i++)
	{
		if (mySessionArr[i].dummyStatus < static_cast<long>(CDummySession::DummyStatus::Connected))
		{
			continue;
		}

		if (rand() % 20 == 0)
		{
			mySessionArr[i].Dummy_Clear();
			InterlockedIncrement(&g_sessionLogoutCount);
			InterlockedDecrement(&g_sessionLoginCount);
		}
	}

	bool completeRetval;
	completeRetval = ThreadActionComplete();

	if (completeRetval == true)
	{
		ChangeAction(static_cast<char>(CAction::Status::ACTION_CONNECT));
	}

	return true;
}


bool ChangeAction(char action)
{
	g_currentAction = action;

	actionCompleteLock.lock();
	if (g_actionCompleteCount[tls_myThreadIndex] != THREAD_ACTION_COMPLETE)
	{
		actionCompleteLock.unlock();
		return false;
	}

	for (unsigned int i = 0; i < g_threadCount; i++)
	{
		g_actionCompleteCount[i] = 0;
	}
	actionCompleteLock.unlock();

	return true;
}

bool DoSessionConnect(CDummySession* session)
{
	int connectRetval;
	session->_socket = socket(AF_INET, SOCK_STREAM, NULL);

	connectRetval = connect(session->_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (connectRetval == SOCKET_ERROR)
	{
		//블로킹 소켓이므로 getlastError코드 확인이 필요
		if (GetLastError() == 10061)
		{
			closesocket(session->_socket);
			return false;
		}
		else if (GetLastError() == 10054)
		{
			__debugbreak(); //여기서 이 값이 나오는지 확인이 필요.
		}
		__debugbreak();
	}
	CThreadNetworkManager::SetSocketOption(&session->_socket);

	session->dummyStatus = static_cast<long>(CDummySession::DummyStatus::Connecting);

	return true;
}