#include "Player.h"
#include "Contents/ContentsPacket.h"
#include "Sector/Sector.h"
#include "Contents/ContentsFunc.h"
#include "Message/Message.h"
#include "Network/Network.h"
//-----------------------------------------
// �÷��̾� ī������ ���� ����
//-----------------------------------------
unsigned long long g_TotalPlayerCreate;
unsigned long long g_PlayerLogInCount;
unsigned long long g_PlayerLogOut;

long long g_playerCount = 0;
unsigned long long g_PlayerID = 0;

extern std::stack<int> g_playerIndexStack;
CPlayer* g_PlayerArr;

extern std::list<CPlayer*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
extern CWanServer* pLib;

bool CPlayer::Move(DWORD fixedDeltaTime) {
	if (_move == false)
	{
		return false;
	}

	if (_x >= dfRANGE_MOVE_RIGHT || _x < dfRANGE_MOVE_LEFT || _y >= dfRANGE_MOVE_BOTTOM || _y < dfRANGE_MOVE_TOP)
	{
		return false;
	}

	short deltaX;
	short deltaY;
	int oldX = _x;
	int oldY = _y;

	deltaX = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_X;
	deltaY = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_Y;

	switch (_direction) {
	case dfPACKET_MOVE_DIR_LL:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT) return false;
		_x -= deltaX;;
	}
	break;

	case dfPACKET_MOVE_DIR_LU:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT || _y - deltaY < dfRANGE_MOVE_TOP) return false;
		_x -= deltaX;
		_y -= deltaY;
	}
	break;

	case dfPACKET_MOVE_DIR_UU:
	{
		if (_y - deltaY < dfRANGE_MOVE_TOP) return false;
		_y -= deltaY;


	}

	break;

	case dfPACKET_MOVE_DIR_RU:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT || _y - deltaY < dfRANGE_MOVE_TOP) return false;
		_x += deltaX;
		_y -= deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_RR:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT) return false;
		_x += deltaX;


	}
	break;

	case dfPACKET_MOVE_DIR_RD:
	{
		if (_x + deltaX >= dfRANGE_MOVE_RIGHT || _y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_x += deltaX;
		_y += deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_DD:
	{
		if (_y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_y += deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_LD:
	{
		if (_x - deltaX < dfRANGE_MOVE_LEFT || _y + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		_x -= deltaX;
		_y += deltaY;


	}
	break;
	default:
		__debugbreak();
		break;
	}

	if ((_x / SECTOR_RATIO == oldX / SECTOR_RATIO) && (_y / SECTOR_RATIO == oldY / SECTOR_RATIO))
	{
		return true;
	}
	SyncSector(_ID, oldX, oldY);

	return true;
}

bool CPlayer::MoveStart(BYTE Direction, int x, int y) {

	if (x < 0 || y < 0)
	{
		__debugbreak();
	}

	int oldX = _x;
	int oldY = _y;

	int oldSectorX = oldX / SECTOR_RATIO;
	int oldSectorY = oldY / SECTOR_RATIO;

	_direction = Direction;
	_x = x;
	_y = y;
	_move = true;

	int newSectorX = x / SECTOR_RATIO;
	int newSectorY = y / SECTOR_RATIO;

	//���� ����ȭ
	if ((newSectorX != oldSectorX) || (newSectorY != oldSectorY))
	{
		SyncSector(_ID, oldX, oldY);
	}

	return true;
}

void CPlayer::MoveStop(BYTE Dir, int x, int y)
{
	if (x < 0 || y < 0)
	{
		__debugbreak();
	}
	int oldX = _x;
	int oldY = _y;

	int oldSectorX = oldX / SECTOR_RATIO;
	int oldSectorY = oldY / SECTOR_RATIO;

	_direction = Dir;
	_x = x;
	_y = y;
	_move = false;

	int newSectorX = x / SECTOR_RATIO;
	int newSectorY = y / SECTOR_RATIO;

	if ((newSectorX != oldSectorX) || (newSectorY != oldSectorY))
	{
		SyncSector(_ID, oldX, oldY);
	}
}
void CPlayer::Clear()
{
	_status = static_cast<BYTE>(STATUS::DELETED);
}
void CPlayer::Init(ULONG64 sessionID)
{
	_x = rand() % 6400;
	_y = rand() % 6400;
	_direction = (rand() % 2) * 4; //LL == 0, RR == 4

	_move = false;
	_ID = sessionID;
	_status = static_cast<BYTE>(STATUS::ALIVE);
	_timeOut = timeGetTime();

	Sector[_x / SECTOR_RATIO][_y / SECTOR_RATIO].push_back(this);
}

bool CPlayer::isAlive()
{
	if (_status == static_cast<BYTE>(STATUS::ALIVE))
	{
		return true;
	}
	return false;
}
unsigned long long CPlayer::GetID()
{
	return _ID;
}
