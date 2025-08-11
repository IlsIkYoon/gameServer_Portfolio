#pragma once
#include "ContentsResource.h"

extern unsigned long long g_PlayerID;
extern std::stack<int> g_playerIndexStack;

class CPlayer {
public:

	enum class enSTATUS
	{
		IDLE = 0, SESSION, PENDING_SECTOR , PLAYER
	};

	DWORD _timeOut;
	BYTE _status;
public:
	bool _move;
	short sectorX;
	short sectorY;
	BYTE _direction;
	unsigned long long accountNo;
	unsigned long long _sessionID;
	WCHAR nickname[20];
	WCHAR ID[20];

public:
	CPlayer()
	{
		_move = false;
		_status = static_cast<BYTE>(enSTATUS::IDLE);
		_direction = 0;
		sectorX = 0;
		sectorY = 0;
		accountNo = 0;
		_timeOut = 0;
		_sessionID = 0;
		ZeroMemory((char*)ID, sizeof(WCHAR) * 20);
		ZeroMemory((char*)nickname, sizeof(WCHAR) * 20);
	}
	//------------------------------------------------------
	// ���� �÷��� �Ѱ�Ŭ���̾�Ʈ���� �� ��ǥ ���� �״�� ����
	//------------------------------------------------------
	bool MoveStart(BYTE Direction, int x, int y);
	bool Move(DWORD deltaTime);
	//------------------------------------------------------
	// ���� �÷��� ���� Ŭ���̾�Ʈ������ ���� ��ǥ �״�� ����
	//------------------------------------------------------
	void MoveStop(BYTE Direction, int x, int y);
	//------------------------------------------------------
	// ���� �迭�̶� ������ ���ϴ� �Լ�
	//------------------------------------------------------
	void Clear();
	//------------------------------------------------------
	// ���� �迭�̶� �����ڿ� ���ϴ� �Լ�
	//------------------------------------------------------
	void Init() = delete;
	void Init(ULONG64 sessionID);
	//------------------------------------------------------
	// Player�� ��ȿ������ ����
	//------------------------------------------------------
	bool isAlive();

	unsigned long long GetID();
	void SetID(ULONG64 id)
	{
		accountNo = id;
	}
	inline short GetX()
	{
		return sectorX;
	}
	inline short GetY()
	{
		return sectorY;
	}
};