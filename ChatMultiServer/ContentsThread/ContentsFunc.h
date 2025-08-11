#pragma once
#include "ContentsResource.h"
#include "Player/Player.h"
#include "Msg/ContentsPacket.h"

class CWanServer : public CWanManager
{
public:
	CWanServer();

	void _OnMessage(CPacket* message, ULONG64 sessionID) override final;
	void _OnAccept(ULONG64 sessionID) override final;
	void _OnDisConnect(ULONG64 sessionID) override final;
	void _OnSend(ULONG64 sessionID) override final;
};

//----------------------------------------------
// �÷��̾� ���� ���鼭 ��Ʈ��Ʈ �ð� üũ�ϴ� �Լ�
//----------------------------------------------
void TimeOutCheck();

bool UpdateMonitorData();