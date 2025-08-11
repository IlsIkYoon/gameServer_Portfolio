#pragma once
#include "resource.h"
#include "Player/DummySession.h"


//-----------------------------------------
// ����Ŭ���̾�Ʈ�� ��������� �� ��
//-----------------------------------------
unsigned int DummyClientThreadFunc(void*);
//--------------------------------
// ���� ������ ���� ���ư��� �Լ�.
// �����忡�� �ڱ� ���Ǹ� ������ ����
//--------------------------------
bool ContentsWork(CDummySession* mySessionArr, DWORD mySessionCount, DWORD fixedDeltaTime);
//--------------------------------
// �׼ǿ� ���� ���´ٴ� ī��Ʈ�� �ø��� ���� �������̸� return true;
//--------------------------------
bool ThreadActionComplete();
//--------------------------------
// MoveStart��Ŷ ���� sendBuf�� ��ť
//--------------------------------
bool SendMoveStartMessage(CDummySession* _session);
//--------------------------------
// MoveStop��Ŷ ���� sendBuf�� ��ť
//--------------------------------
bool SendMoveStopMessage(CDummySession* _session);
//--------------------------------
// ���� ���� ���� Ȱ��ȭ �Ǿ� ������ ��Ʈ��Ʈ �����ֱ�
//--------------------------------
bool SendHeartBeatMessage(CDummySession* _session);
//--------------------------------
// LocalChatMessage�� SendBuf�� Enque
//--------------------------------
bool SendLocalChatMessage(CDummySession* _session, char* chatMessage, int msgLen);
//--------------------------------
// ��� ���� ���鼭 Recv������ ó�����ִ� �Լ�
//--------------------------------
bool HandleRecvData_All(CDummySession* mySessionArr, DWORD dwMySessionCount);
//--------------------------------
// ������ recv�����͸� ó�����ִ� �Լ�
//--------------------------------
bool HandleRecvData_Session(CDummySession* _session);
//--------------------------------
// ��Ʈ��ũ ����� �� ����ȭ���ۿ� ��Ƽ� �������ִ� �Լ�
//--------------------------------
bool Decode_NetworkHeader(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// ������ ���̷ε带 ������ ���� ���� ����
//--------------------------------
bool Decode_ContentsPacket(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// ĳ���� ���� �޼����� ���� ó�� �Լ�
//--------------------------------
bool CreateCharacter(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// LocalChat�޼��� �ڵ鸵 �Լ�
//--------------------------------
bool HandleLocalChatMsg(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// MoveStopComplete�ڵ鸵 �Լ�
//--------------------------------
bool HandleMoveStopCompleteMsg(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// ChatComplete �ڵ鸵 �Լ�
//--------------------------------
bool HandleChatCompleteMsg(CDummySession* _session, CPacket* ContentsPacket);
//--------------------------------
// �޼����� �޾ƾ� �ϴ� ������ StringCount�� �÷��ִ� �Լ�
//--------------------------------
bool CheckReceiversInSector(CDummySession* _session, char* chatMsg, int msgLen);
//--------------------------------
// �� ActionCompleteī��Ʈ�� üũ
//--------------------------------
bool CheckMyActionComplete();
//--------------------------------
// ChatEnd�޼����� ������
//--------------------------------
bool SendChatEndMessage(CDummySession* _session);
//--------------------------------
// ��Ʈ��ũ ����� �ٽ� �־���
//--------------------------------
bool RestoreNetworkHeader(ClientHeader header, CDummySession* _session);