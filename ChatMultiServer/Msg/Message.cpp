#include "ContentsResource.h"
#include "Message.h"
#include "Sector/Sector.h"
#include "ContentsPacket.h"
#include "Player/Player.h"
#include "ContentsThread/ContentsFunc.h"
#include "CommonProtocol.h"
#include "ContentsThread/ContentsThreadManager.h"
#include "Connector/RedisConnector.h"
//-----------------------------------------
// ����� ���� �޼��� ī����
//-----------------------------------------

extern unsigned long long g_loginMsgCnt;
extern unsigned long long g_sectorMoveMsgCnt;
extern unsigned long long g_chatMsgCnt;

extern std::stack<int> g_playerIndexStack;
extern long long g_playerCount;
extern unsigned long long g_PlayerLogOut;

extern std::list<CPlayer*> Sector[SECTOR_MAX][SECTOR_MAX];
extern std::recursive_mutex SectorLock[SECTOR_MAX][SECTOR_MAX];
extern int sectorXRange;
extern int sectorYRange;

extern unsigned long long g_PlayerLogInCount;
extern unsigned long long g_TotalPlayerCreate;

extern CWanServer* networkServer;
extern CContentsThreadManager contentsManager;

//-------------------------------------
// ���� �޼��� ���ῡ ���� ī��Ʈ
//-------------------------------------
extern unsigned long long g_ErrorSectorSize;
extern unsigned long long g_ErrorNetworkLen;
extern unsigned long long g_ErrorChatMsgLen;
extern unsigned long long g_ErrorPacketType;
extern unsigned long long g_ErrorSectorAccountNoMissmatch;
extern unsigned long long g_ErrorSectorPlayerStatus;
extern unsigned long long g_ErrorSectorIncompletePacket;
extern unsigned long long g_ErrorChatAccountNoMissmatch;
extern unsigned long long g_ErrorLoginPlayerStatus;
extern unsigned long long g_ErrorCharacterKeyInsert;
extern unsigned long long g_ErrorLoginNullChar;

void MsgSectorBroadCasting(void (*Func)(ULONG64 srcID, ULONG64 destID, CPacket* Packet), CPlayer* _src, CPacket* Packet, bool SendMe)
{
	
	int SectorX;
	int SectorY;

	SectorX = _src->sectorX;
	SectorY = _src->sectorY;

	//���� ���� ���� ��� ���� ����
	{
		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
				if (SectorY + j < 0 || SectorY + j >= sectorYRange) continue;

				
				SectorLock[SectorX + i][SectorY + j].lock();
			}
		}

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
				if (SectorY + j < 0 || SectorY + j >= sectorYRange) continue;

				for (auto pit = Sector[SectorX + i][SectorY + j].begin(); pit != Sector[SectorX + i][SectorY + j].end();)
				{
					if ((*pit)->_sessionID == _src->_sessionID && SendMe == false)
					{
						__debugbreak();
						continue;
					}
					if ((*pit)->isAlive() == false)
					{
						__debugbreak();
						continue;
					}
					if ((*pit)->_status < static_cast<BYTE>(CPlayer::enSTATUS::PLAYER))
					{
						__debugbreak();
					}


					auto next_pit = std::next(pit);

					Func(_src->_sessionID, (*pit)->_sessionID, Packet);

					pit = next_pit;

				}
			}
		}

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (SectorX + i < 0 || SectorX + i >= sectorXRange) continue;
				if (SectorY + j < 0 || SectorY + j >= sectorYRange) continue;

				SectorLock[SectorX + i][SectorY + j].unlock();
			}
		}
	}
}

void ContentsSendPacket(ULONG64 srcID, ULONG64 destID, CPacket* packet)
{
	if (srcID == destID)
	{
		InterlockedDecrement(&g_chatMsgCnt);
	}
	networkServer->SendPacket(destID, packet);
}

void SendChatResPacket(ULONG64 srcID, ULONG64 destID, CPacket* packet) 
{
	CPacket* sendMsg;
	unsigned short playerIndex;
	CPlayer* localPlayerList;

	if (srcID == destID)
	{
		InterlockedDecrement(&g_chatMsgCnt);
	}

	//-----------------------------
	// payLoad �� �ܵ� ������
	//-----------------------------
	WORD Type;
	INT64 AccountNo;

	playerIndex = CWanManager::GetIndex(destID);
	localPlayerList = contentsManager.playerList->playerArr;

	Type = en_PACKET_CS_CHAT_RES_MESSAGE;
	AccountNo = localPlayerList[playerIndex].accountNo;

	sendMsg = CPacket::Alloc();
	*sendMsg << Type;
	sendMsg->PutData(packet->GetDataPtr(), packet->GetDataSize());

	networkServer->SendPacket(destID, sendMsg);
	sendMsg->DecrementUseCount();
}

void SendLoginResPacket(ULONG64 sessionID)
{
	int playerIndex;
	CPlayer* localPlayerList;
	CPacket* sendMsg;

	WORD Type;
	BYTE Status;	//0 : ���� 1: ����
	INT64 AccountNo;

	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = networkServer->GetIndex(sessionID);

	Type = en_PACKET_CS_CHAT_RES_LOGIN;
	Status = 1;
	AccountNo = localPlayerList[playerIndex].accountNo;

	sendMsg = CPacket::Alloc();
	*sendMsg << Type;
	*sendMsg << Status;
	*sendMsg << AccountNo;

	networkServer->SendPacket(sessionID, sendMsg);

	sendMsg->DecrementUseCount();

	InterlockedIncrement(&g_PlayerLogInCount);
	InterlockedIncrement(&g_TotalPlayerCreate);
	InterlockedDecrement(&g_loginMsgCnt);
}

bool HandleLoginMessage(CPacket* message, ULONG64 sessionID)
{
	DWORD playerIndex;
	CPlayer* localPlayerList;

	//------------------------------------
	// �޼��� ���̷ε�
	//------------------------------------
	INT64 AccountNo; //ĳ���� Ű
	WCHAR ID[20]; //null����
	WCHAR Nickname[20]; //null����
	char SessionKey[65]; //���� ��ū
	
	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = CWanManager::GetIndex(sessionID);
	//------------------------------------
	// ���� üũ
	//------------------------------------
	if (localPlayerList[playerIndex]._status != static_cast<BYTE>(CPlayer::enSTATUS::SESSION))
	{
		InterlockedIncrement(&g_ErrorLoginPlayerStatus);
		networkServer->DisconnectSession(sessionID);
		return false;
	}

	if (message->GetDataSize() != sizeof(AccountNo) + sizeof(WCHAR) * 20 + sizeof(WCHAR) * 20 + 64)
	{
		std::string error;
		error = "Login Message Error || data Size error || size : ";
		error += std::to_string(message->GetDataSize());
		error += " || RequireSize : ";
		error += std::to_string(sizeof(AccountNo) + sizeof(WCHAR) * 20 + sizeof(WCHAR) * 20 + 64);

		networkServer->DisconnectSession(sessionID);
		return false;
	}

	*message >> AccountNo;
	message->PopFrontData(sizeof(WCHAR) * 20, (char*)ID);
	message->PopFrontData(sizeof(WCHAR) * 20, (char*)Nickname);
	message->PopFrontData(sizeof(char) * 64, SessionKey);
	SessionKey[64] = NULL;

	std::string redisKey = std::to_string(AccountNo);
	std::string redisToken = SessionKey;
	bool tokenRetval = true;

	tokenRetval = TLS_REDIS_CONNECTOR.CheckToken(redisKey, redisToken);

	if (tokenRetval == false)
	{
		__debugbreak();//������ ��
		networkServer->DisconnectSession(sessionID);
		//���⼭ �� �����͸� ������� �߳� ?
		return false;
	}

	//LoadCharacterDataOnLogin(AccountNo, userId); 
	//��ū�� ��ȿ�ϴٸ� DB���� ĳ���� ������ �ܾ���� ��û �� �ܾ�� �ڿ� ����� ���� Login_res �޼��� ����

	if (contentsManager.keyList->InsertID(AccountNo, sessionID) == false)
	{
		networkServer->DisconnectSession(sessionID);
		return false;
	}
	if (CheckNullChar(ID, 20) == false || CheckNullChar(Nickname, 20) == false)
	{
		InterlockedIncrement(&g_ErrorLoginNullChar);
		networkServer->DisconnectSession(sessionID);
		return false;
	}

	localPlayerList[playerIndex].Init(sessionID);
	localPlayerList[playerIndex].accountNo = AccountNo;
	wcsncpy_s(localPlayerList[playerIndex].ID, ID, 20);
	wcsncpy_s(localPlayerList[playerIndex].nickname, Nickname, 20);

	SendLoginResPacket(sessionID);

	return true;
}

bool HandleSectorMoveMessage(CPacket* message, ULONG64 sessionID)
{
	//�÷��̾��� ���� ������ ���� �����ְ� 
	//���� �̵��� ���� ���� ó���� ��
	CPlayer* localPlayerList;
	DWORD playerIndex;
	WORD oldSectorX;
	WORD oldSectorY;
	//-------------------------------
	// �޼��� ���̷ε�
	//-------------------------------
	INT64 AccountNo;
	WORD SectorX;
	WORD SectorY;

	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = CWanManager::GetIndex(sessionID);

	if (message->GetDataSize() != sizeof(AccountNo) + sizeof(SectorX) + sizeof(SectorY))
	{
		InterlockedIncrement(&g_ErrorSectorIncompletePacket);
		networkServer->DisconnectSession(sessionID);
		return false;
	}

	*message >> AccountNo;
	*message >> SectorX;
	*message >> SectorY;


	//-----------------------------------------
	// ���� üũ
	//-----------------------------------------
	if (localPlayerList[playerIndex]._status < static_cast<BYTE>(CPlayer::enSTATUS::PENDING_SECTOR))
	{
		std::string error;
		error = "Error|| SectorMsg || PendingSector";

		networkServer->EnqueLog(error);
		InterlockedIncrement(&g_ErrorSectorPlayerStatus);
		networkServer->DisconnectSession(sessionID);
		return false;
	}
	if (SectorX >= SECTOR_MAX || SectorY >= SECTOR_MAX)
	{
		InterlockedIncrement(&g_ErrorSectorSize);
		networkServer->DisconnectSession(sessionID);
		return false;
	}
	if (AccountNo != localPlayerList[playerIndex].accountNo)
	{
		InterlockedIncrement(&g_ErrorSectorAccountNoMissmatch);
		networkServer->DisconnectSession(sessionID);
		return false;
	}
	//-----------------------------------------

	if (localPlayerList[playerIndex]._status == static_cast<BYTE>(CPlayer::enSTATUS::PENDING_SECTOR))
	{
		//���� ù ��ġ �۾�
		localPlayerList[playerIndex]._status = static_cast<BYTE>(CPlayer::enSTATUS::PLAYER); //���� ��ġ�� �÷��̾� �°�
		localPlayerList[playerIndex].sectorX = SectorX;
		localPlayerList[playerIndex].sectorY = SectorY;

		{
			std::lock_guard guard(SectorLock[SectorX][SectorY]);
			Sector[SectorX][SectorY].push_back(&localPlayerList[playerIndex]);
		}

	}
	else 
	{
		oldSectorX = localPlayerList[playerIndex].sectorX;
		oldSectorY = localPlayerList[playerIndex].sectorY;

		localPlayerList[playerIndex].sectorX = SectorX;
		localPlayerList[playerIndex].sectorY = SectorY;

		SyncSector(sessionID, oldSectorX, oldSectorY);
	}
	SendSectorMoveResPacket(sessionID);

	return true;
}

bool HandleChatMessage(CPacket* message, ULONG64 sessionID)
{
	DWORD playerIndex;

	INT64 AccountNo;
	WORD MessageLen;
	WCHAR* Message; //null ������ // messageLen / 2����
	CPlayer* localPlayerList;

	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = CWanManager::GetIndex(sessionID);

	if (message->GetDataSize() < sizeof(AccountNo) + sizeof(MessageLen))
	{
		std::string error;
		error = "Chat Message Error || Data Size Error : ";
		error += std::to_string(message->GetDataSize());

		networkServer->EnqueLog(error);

		networkServer->DisconnectSession(sessionID);
		return false;
	}

	*message >> AccountNo;
	*message >> MessageLen;


	//-----------------------------------------
	//����üũ
	//-----------------------------------------
	if (AccountNo != localPlayerList[playerIndex].accountNo)
	{
		std::string error;
		error += "Chat Message Error || AccountNo : ";
		error += std::to_string(AccountNo);

		networkServer->EnqueLog(error);

		networkServer->DisconnectSession(sessionID);
		return false;
	}
	if (message->GetDataSize() != MessageLen)
	{
		std::string error;
		error += "Chat Message Error || MessageLen : ";
		error += std::to_string(MessageLen);

		networkServer->EnqueLog(error);

		networkServer->DisconnectSession(sessionID);
		return false;
	}
	//-----------------------------------------

	Message = new WCHAR[MessageLen / 2];

	message->PopFrontData(MessageLen, (char*)Message);

	WORD Type;
	WCHAR ID[20] = { 0 };
	WCHAR Nickname[20] = { 0 };

	Type = en_PACKET_CS_CHAT_RES_MESSAGE;
	wcsncpy_s(ID, localPlayerList[playerIndex].ID, 20);
	wcsncpy_s(Nickname, localPlayerList[playerIndex].nickname, 20);

	CPacket* sendMsg; //���� �����Ϳ� ���� ���
	sendMsg = CPacket::Alloc();

	*sendMsg << Type;
	*sendMsg << AccountNo;
	sendMsg->PutData((char*)ID, sizeof(WCHAR) * 20);
	sendMsg->PutData((char*)Nickname, sizeof(WCHAR) * 20);
	*sendMsg << MessageLen;
	sendMsg->PutData((char*)Message, MessageLen);

	MsgSectorBroadCasting(ContentsSendPacket, &localPlayerList[playerIndex], sendMsg, true);

	sendMsg->DecrementUseCount();

	delete[] Message;

	return true;
}

void SendSectorMoveResPacket(ULONG64 sessionID)
{
	int playerIndex;
	CPlayer* localPlayerList;
	CPacket* sendMsg;

	WORD Type;
	INT64 AccountNo;
	WORD SectorX;
	WORD SectorY;

	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = CWanManager::GetIndex(sessionID);

	Type = en_PACKET_CS_CHAT_RES_SECTOR_MOVE;
	AccountNo = localPlayerList[playerIndex].accountNo;
	SectorX = localPlayerList[playerIndex].sectorX;
	SectorY = localPlayerList[playerIndex].sectorY;

	sendMsg = CPacket::Alloc();
	*sendMsg << Type;
	*sendMsg << AccountNo;
	*sendMsg << SectorX;
	*sendMsg << SectorY;

	networkServer->SendPacket(sessionID, sendMsg);

	sendMsg->DecrementUseCount();

	InterlockedDecrement(&g_sectorMoveMsgCnt);
}


bool CheckNullChar(WCHAR* message, DWORD len)
{
	bool retval = false;

	for (unsigned int i = 0; i < len; i++)
	{
		if (message[i] == NULL)
		{
			retval = true;
			break;
		}
	}

	return retval;
}