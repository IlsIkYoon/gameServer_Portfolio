#pragma once
#include "Player/DummySession.h"


class CAction
{
public:
	enum class Status
	{
		ACTION_CONNECT =  0,
		ACTION_SENDCHATMSG,
		ACTION_CHECKCHATMSG,
		ACTION_MOVE,
		ACTION_MOVESTOP,
		ACTION_CHECKMOVESTOPMSG,
		ACTION_CHECKDISCONNECT
	
	};
};

//----------------------------
// Connect�׼� �� ó���� ������ ����
//----------------------------
bool HandleConnectAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// Session�� ���� blocking Connect ����
//----------------------------
bool DoSessionConnect(CDummySession* session);
//----------------------------
// Move�׼� �� ó���� ������ ����
// MoveStart��Ŷ�� �����鼭 Move ���·� �ٲ��ְ� 
// MoveStop��Ŷ�� �����鼭 ���¸� MoveStop�Ϸ� ��Ŷ�� ��ٸ��� ���·� ����
//----------------------------
bool HandleMoveAction(CDummySession* mySessionArr, DWORD mySessionCount, DWORD fixedDeltaTime);
//----------------------------
// Msg�� �� �Դ��� üũ�ϴ� ����. ��� ������ �޼����� �� �Դ����� üũ
//----------------------------
bool HandleCheckMsgAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// ä�� �޼����� �������� ������ ������ �ڷᱸ���� �־��ִ� �Լ�
//----------------------------
bool HandleSendChatMsgAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// MoveStop�޼����� ��� �� �� ���� ��ٸ��� �Լ�
//----------------------------
bool HandleCheckMoveStopAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// DisConnect�Ǵ� �� ��ٸ��� DisConnect��Ű�� �Լ�
//----------------------------
bool HandleCheckDisconnectAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// MoveStop��Ŷ ������ ���� ��ٸ��� �Լ�
//----------------------------
bool HandleMoveStopAction(CDummySession* mySessionArr, DWORD mySessionCount);
//----------------------------
// Action�� �������ְ� g_ActionComplete�� �ʱ�ȭ����
//----------------------------
bool ChangeAction(char action);

