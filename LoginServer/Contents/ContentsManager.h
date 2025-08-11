#pragma once

#include "Resource/LoginServerResource.h"
#include "User/User.h"
#include "Network/NetworkManager.h"
#include "Connector/DBConnector.h"
#include "Connector/RedisConnector.h"

class CContentsManager
{
public:
	CUserManager* userManager;
	CWanServer* networkManager;
	CDBManager* DBConnector;

	bool redisAvailable;

	std::thread tickThread;

	WCHAR gameServerIP[16];
	unsigned short gameServerPort;
	WCHAR chatServerIP[16];
	unsigned short chatServerPort;

	std::string redisIP;
	std::size_t redisPort;

public:
	CContentsManager() = delete;
	CContentsManager(CWanServer* pNetworkManager);


	void tickThreadFunc();
	
	//-------------------------------------------
	// User�� ���� �������̽�
	//-------------------------------------------
	bool InitUser(unsigned long long sessionID);
	bool DeleteUser(unsigned long long sessionID);

	DWORD GetCurrentUser();

	//-------------------------------------------
	// �޼��� ó�� �������̽�
	//-------------------------------------------
	bool HandleContentsMessage(CPacket* message, ULONG64 ID);

	//�޼��� ó����
	bool HandleLoginREQMsg(CPacket* message, ULONG64 ID);
	
	//�޼��� ���ۺ�
	bool MsgSetServerAddr(CPacket* message);


	bool SetToken(std::string Value, ULONG64 key);

};

