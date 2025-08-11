#include "Network.h"
#include "Contents/ContentsPacket.h"

extern CLFree_Queue<Job> g_ContentsJobQ;
extern long long g_playerCount;
DWORD g_messageCount;

void CWanServer::_OnMessage(CPacket* message, ULONG64 ID)
{
	Job enqueMessage;
	enqueMessage.ID = ID;
	enqueMessage.packet = message;
	message->IncrementUseCount(); // �־��ٴ� �ǹ�
	g_ContentsJobQ.Enqueue(enqueMessage);
	InterlockedIncrement(&g_messageCount);
}

void CWanServer::_OnAccept(ULONG64 ID)
{
	Job enqueMessage;
	CPacket* CreatePlayerMsg;

	WORD type = stJob_CreatePlayer;

	CreatePlayerMsg = CPacket::Alloc();
	*CreatePlayerMsg << type;
	*CreatePlayerMsg << ID;

	enqueMessage.ID = SERVER_ID;
	enqueMessage.packet = CreatePlayerMsg;

	g_ContentsJobQ.Enqueue(enqueMessage);

	InterlockedIncrement64(&g_playerCount);
}

void CWanServer::_OnSend(ULONG64 ID)
{
	//�� �� ����
	return;
}
void CWanServer::_OnDisConnect(ULONG64 ID)
{
	Job enqueMessage;
	CPacket* DeletePlayerMsg;

	WORD type = stJob_DeletePlayer;

	DeletePlayerMsg = CPacket::Alloc();

	*DeletePlayerMsg << type;
	*DeletePlayerMsg << ID;

	enqueMessage.ID = SERVER_ID;
	enqueMessage.packet = DeletePlayerMsg;

	g_ContentsJobQ.Enqueue(enqueMessage);

	InterlockedDecrement64(&g_playerCount);
}
