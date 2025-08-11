#include "ContentsManager.h"
#include "CommonProtocol.h"
#include "Monitor/Monitor.h"
#include "Monitor/MonitorManager.h"

CContentsManager* g_ContentsManager;
extern CMonitor g_MonitorManager;


extern CMonitor_delete g_Monitor;
extern CPdhManager g_Pdh;

CContentsManager::CContentsManager(CWanServer* pNetworkManager)
{
	unsigned short monitorPort;
	unsigned short portNum;
	int sessionCount;
	int workerThreadCount;
	int concurrentThreadCount;
	unsigned char localRedisAvailable;
	unsigned char networkCode;
	unsigned char stickyKey;
	CTextParser parser;


	networkManager = pNetworkManager;
	parser.GetData("LoginConfig.ini");
	parser.SearchData("ChatServerPort", &chatServerPort);
	parser.SearchData("GameServerPort", &gameServerPort);
	parser.SearchData("MonitorPort", &monitorPort);
	parser.SearchData("PortNum", &portNum);
	parser.SearchData("SessionCount", &sessionCount);
	parser.SearchData("WorkerThreadCount", &workerThreadCount);
	parser.SearchData("ConcurrentThreadCount", &concurrentThreadCount);
	parser.SearchData("ntProtocolCode", &networkCode);
	parser.SearchData("stickyKey", &stickyKey);
	parser.SearchData("redisAvailable", &localRedisAvailable);
	parser.CloseData();

	if (localRedisAvailable == 1)
	{
		redisAvailable = true;
	}
	else
	{
		redisAvailable = false;
	}

	networkManager->RegistPortNum(portNum);
	networkManager->RegistSessionMaxCount(sessionCount);
	networkManager->RegistConcurrentCount(concurrentThreadCount);
	networkManager->RegistWorkerThreadCount(workerThreadCount);
	networkManager->Regist_NetworkProtocolCode(networkCode);
	networkManager->Regist_PacketStickyKey(stickyKey);

	userManager = new CUserManager(pNetworkManager->_sessionMaxCount);
	networkManager->Start();

	g_MonitorManager.RegistMonitor(L"127.0.0.1", monitorPort);


	tickThread = std::thread([this]() {tickThreadFunc(); });
	//DBConnector = new CDBManager;
	redisPort = 0;
	/*
	if (redisAvailable == true)
	{
		RedisConnector = new CRedisConnector;
	}
	*/

	wcscpy_s(chatServerIP, IP_CHATSERVER);
	wcscpy_s(gameServerIP, IP_GAMESERVER);
}



void CContentsManager::tickThreadFunc()
{
	DWORD prevTime;
	DWORD currentTime;
	DWORD resultTime;
	
	g_Pdh.Start();
	prevTime = timeGetTime();

	while (1)
	{
		//틱 쓰레드 로직 진행
		g_Monitor.ConsolPrintAll();
		networkManager->EnqueSendRequest();

		g_MonitorManager.UpdateAllMonitorData();

		if (_kbhit())
		{
			char c = _getch();
			ULONG64 key;
			std::string value;
			if (c == 'q' || c == 'Q')
			{
				std::cout << "Set key: ";
				std::cin >> key;
				std::cout << "\nValue : ";
				std::cin >> value;

				g_ContentsManager->SetToken(value, key);

			}
		}

		currentTime = timeGetTime();
		resultTime = currentTime - prevTime;
		if (resultTime < 1000)
		{
			Sleep(1000 - resultTime);
		}
		prevTime = timeGetTime();

	}


}

bool CContentsManager::InitUser(unsigned long long sessionID)
{
	return userManager->InitUser(sessionID);
}
bool CContentsManager::DeleteUser(unsigned long long sessionID)
{
	return userManager->DeleteUser(sessionID);
}


bool CContentsManager::HandleContentsMessage(CPacket* message, ULONG64 ID)
{
	WORD messageType;

	if (message->GetDataSize() < sizeof(messageType))
	{
		networkManager->DisconnectSession(ID);
		return false;
	}
	*message >> messageType;

	switch (messageType)
	{
	case en_PACKET_CS_LOGIN_REQ_LOGIN:
		HandleLoginREQMsg(message, ID);
		break;

	default:
	{
		networkManager->DisconnectSession(ID);
		return false;
	}
	break;
	}

	return true;
}



bool CContentsManager::HandleLoginREQMsg(CPacket* message, ULONG64 ID)
{
	ULONG64 accountNo;
	char platformToken[65]; //토큰
	unsigned short userIndex;
	User* user;
	CPacket* sendMsg;

	userIndex = CWanServer::GetIndex(ID);
	user = userManager->GetUser(userIndex);

	if (user->alive == false)
	{
		__debugbreak();
	}
	if (user->sessionID != ID)
	{
		__debugbreak();
	}
		
	if (message->GetDataSize() < sizeof(accountNo) + sizeof(char) * 64)
	{
		networkManager->DisconnectSession(ID);
		return false;
	}

	*message >> accountNo;
	message->PopFrontData(64, platformToken);
	platformToken[64] = NULL;
	//로그인 과정
	WORD sendType;
	ULONG64 sendAccountNo;
	bool DBRetval;

	sendType = en_PACKET_CS_LOGIN_RES_LOGIN;
	sendAccountNo = accountNo;
	sendMsg = CPacket::Alloc();
	*sendMsg << sendType;
	*sendMsg << sendAccountNo;
	
	//현재의 플랫폼 확인 절차(현재는 모두 성공)
	DBRetval = DBConnector->LoginDataRequest(sendMsg, sendAccountNo);

	if (redisAvailable == true)
	{
		TLS_REDIS_CONNECTOR.SetToken(platformToken, sendAccountNo);
	}

	MsgSetServerAddr(sendMsg);

	networkManager->SendPacket(ID, sendMsg);

	InterlockedIncrement(&g_MonitorManager.loginSuccessCount);

	sendMsg->DecrementUseCount();
	return true;
}


bool CContentsManager::MsgSetServerAddr(CPacket* message)
{

	message->PutData((char*)gameServerIP, sizeof(WCHAR) * 16);
	*message << gameServerPort;
	message->PutData((char*)chatServerIP, sizeof(WCHAR) * 16);
	*message << chatServerPort;

	return true;
}

DWORD CContentsManager::GetCurrentUser()
{
	return userManager->currentUserCount;
}
bool CContentsManager::SetToken(std::string Value, ULONG64 key)
{
	TLS_REDIS_CONNECTOR.SetToken(Value, key);
	return true;
}