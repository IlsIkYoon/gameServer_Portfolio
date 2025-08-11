#pragma once
#include "ContentsResource.h"
#include "Player/Player.h"


//-----------------------------------------------------
// sendpacket�� ȣ�����ִ� �Լ� (BroadCasting�Լ��� ���� ����)
//-----------------------------------------------------
void ContentsSendPacket(ULONG64 srcID, ULONG64 destID, CPacket* packet);

//-----------------------------------------------------
// �޼��� ���ۿ� �Լ���
//-----------------------------------------------------
void SendLoginResPacket(ULONG64 sessionID);
void SendSectorMoveResPacket(ULONG64 sessionID);
//-----------------------------------------------------
// �ֺ� ��ο��� �Լ��� ȣ�����ִ� �Լ� �ѷ��ִ� �Լ�
//-----------------------------------------------------
void MsgSectorBroadCasting(void (*Func)(ULONG64 srcID, ULONG64 destID, CPacket* packet), CPlayer* _src, CPacket* Packet, bool SendMe);
//------------------------------------------------------
// �޼��� �ڵ鸵 �Լ���
//------------------------------------------------------
bool HandleLoginMessage(CPacket* message, ULONG64 sessionID);
bool HandleSectorMoveMessage(CPacket* message, ULONG64 sessionID);
bool HandleChatMessage(CPacket* message, ULONG64 sessionID);
//------------------------------------------------------
// dest���� MoveStop�� �Ϸ��ߴٴ� �޼����� �����ִ� �Լ�
//------------------------------------------------------
void SendMoveStopCompleteMessage(ULONG64 destID);
//------------------------------------------------------
// ���ڿ��� NULL���ڰ� ���ԵǾ� �ִ��� üũ���ִ� �Լ�
//------------------------------------------------------
bool CheckNullChar(WCHAR* message, DWORD len);
