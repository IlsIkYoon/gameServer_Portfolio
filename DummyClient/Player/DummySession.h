#pragma once

#include "resource.h"


class CDummySession : public CSession
{
public:
	enum class DummyStatus
	{
        Idle = 0,            // �ƹ��͵� �� �ϰ� �ִ� �ʱ� ���� �������� ���� �ɷ� ħ
        Created,
        Disconnected,
        Connecting,          // ������ ���� �õ� ��
        Connected,           // ���� �Ϸ�, �ʱ� ��� �غ�
        ReadyToMoveStart, //MoveStart ��Ŷ ���� �غ�
        SendingMove,         // �̵� ��� ���� ��
        WaitingMoveEnd,      // �̵� ���� ��ٸ��� ��
        RecvdMoveEnd,
        SendingChat,         // ä�� ���� ��
        WaitingChatEnd,      // ä�� ���� ��ٸ��� ��
        RecvdChatComplete,
        Disconnecting,       // ���� �õ� ��
        WaitingDisconnect   // ������ ���� ��ٸ��� ��
	};

    int _x;
    int _y;
    char _direction;
    bool _created;
    
    std::mutex _sessionLock;

    bool _move;

	long dummyStatus;
    RingBuffer _recvQ{ 4 * 1024 };
    RingBuffer _sendQ{ 4 * 1024 };

    std::map<std::string, long> stringCounter;
    bool allMsgRecvd;
    
    //------------------------------------
    // �����ڿ��� Status�� Idle�� �ʱ�ȭ ��
    //------------------------------------
    CDummySession();

    void Dummy_Clear();
    bool Move(DWORD deltaTime);
    //------------------------------------
    // SendBuffer�� enque�� �ϰ� enque�� �����ϸ� debugbreak();
    //------------------------------------
    bool SendBufferEnque(char* buffer, int size, unsigned int* enqueResult);
    //------------------------------------
    // RecvBuffer�� enque�� �ϰ� enque�� �����ϸ� debugbreak();
    //------------------------------------
    bool RecvBufferEnque(char* buffer, int size, unsigned int* enqueResult);
    //------------------------------------
    // SendBuffer Deque ���� ����ó�� ���� �Լ�
    //------------------------------------
    bool SendBufferDeque(char* buffer, int size, unsigned int* dequeResult);
    //------------------------------------
    // RecvBuffer Deque ���� ����ó�� ���� �Լ�
    //------------------------------------
    bool RecvBufferDeque(char* buffer, int size, unsigned int* dequeResult);
};
//-------------------------------------------------------
// ���� ���� �ÿ� ���� �̵����ִ� �Լ�
//-------------------------------------------------------
bool SyncSector(CDummySession* session, int beforeX, int beforeY);