#pragma once
#include "Player/DummySession.h"

//--------------------------------------
// �޼��� üũ�� ���� �ʴ� ���� ����
//--------------------------------------
unsigned int DummyTrustThreadFunc(void*);
//--------------------------------------
// Trust�� ������ work(Action ������ ����)
//--------------------------------------
bool Trust_ContentsWork(CDummySession* sessionArr, DWORD mySessionCount, DWORD fixedDeltaTime);
//--------------------------------------
// Ȯ���� ���� movestart Ȥ�� chat Ȥ�� �ƹ��͵� ���� �ʴ� �Լ�
//--------------------------------------
bool DoChatOrMoveStart(CDummySession* _session);
//--------------------------------------
// ���� ä�� �޼����� �̾Ƽ� �����ִ� �Լ�
//--------------------------------------
bool SendRandomChat(CDummySession* _session);