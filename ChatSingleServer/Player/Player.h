#pragma once
#include "Contents/ContentsResource.h"
extern unsigned long long g_PlayerID;
extern std::stack<int> g_playerIndexStack;

class CPlayer;

extern CPlayer* PlayerArr;

class CPlayer {
public:

	enum class STATUS
	{
		DELETED = 0, WAIT_CREATE, ALIVE
	};

	DWORD _timeOut;
	BYTE _status;

private:
	bool _move;
	short _x;
	short _y;
	BYTE _direction;
	unsigned long long _ID;
public:
	CPlayer()
	{
		_move = false;
		_status = static_cast<BYTE>(STATUS::DELETED);
		_direction = 0;
		_x = 0;
		_y = 0;
		_ID = 0;
		_timeOut = 0;
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
		_ID = id;
	}
	inline short GetX()
	{
		return _x;
	}
	inline short GetY()
	{
		return _y;
	}
};