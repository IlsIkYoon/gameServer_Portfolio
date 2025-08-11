#include "ContentsResource.h"
#include "Player.h"
#include "Msg/ContentsPacket.h"
#include "Sector/Sector.h"
#include "ContentsThread/ContentsFunc.h"
#include "Msg/Message.h"
#include "ContentsThread/ContentsThreadManager.h"
//-----------------------------------------
// 플레이어 카운팅을 위한 변수
//-----------------------------------------
unsigned long long g_TotalPlayerCreate;
unsigned long long g_PlayerLogInCount;
unsigned long long g_PlayerLogOut;

long long g_playerCount;
unsigned long long g_PlayerID;

extern std::stack<int> g_playerIndexStack;

extern std::list<CPlayer*> Sector[SECTOR_MAX][SECTOR_MAX];
extern std::recursive_mutex SectorLock[SECTOR_MAX][SECTOR_MAX];

extern CWanServer* networkServer;
extern CContentsThreadManager contentsManager;

bool CPlayer::Move(DWORD fixedDeltaTime) {
	if (_move == false) return false;

	if (sectorX >= dfRANGE_MOVE_RIGHT || sectorX < dfRANGE_MOVE_LEFT || sectorY >= dfRANGE_MOVE_BOTTOM || sectorY < dfRANGE_MOVE_TOP) return false;


	short deltaX;
	short deltaY;
	int oldX = sectorX;
	int oldY = sectorY;

	deltaX = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_X;
	deltaY = ((short)fixedDeltaTime / FrameSec) * dfSPEED_PLAYER_Y;



	switch (_direction) {
	case dfPACKET_MOVE_DIR_LL:
	{
		if (sectorX - deltaX < dfRANGE_MOVE_LEFT) return false;
		sectorX -= deltaX;;

	}
	break;

	case dfPACKET_MOVE_DIR_LU:
	{
		if (sectorX - deltaX < dfRANGE_MOVE_LEFT || sectorY - deltaY < dfRANGE_MOVE_TOP) return false;
		sectorX -= deltaX;
		sectorY -= deltaY;

	}

	break;

	case dfPACKET_MOVE_DIR_UU:
	{
		if (sectorY - deltaY < dfRANGE_MOVE_TOP) return false;
		sectorY -= deltaY;


	}

	break;

	case dfPACKET_MOVE_DIR_RU:
	{
		if (sectorX + deltaX >= dfRANGE_MOVE_RIGHT || sectorY - deltaY < dfRANGE_MOVE_TOP) return false;
		sectorX += deltaX;
		sectorY -= deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_RR:
	{
		if (sectorX + deltaX >= dfRANGE_MOVE_RIGHT) return false;
		sectorX += deltaX;


	}
	break;

	case dfPACKET_MOVE_DIR_RD:
	{
		if (sectorX + deltaX >= dfRANGE_MOVE_RIGHT || sectorY + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		sectorX += deltaX;
		sectorY += deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_DD:
	{
		if (sectorY + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		sectorY += deltaY;


	}
	break;

	case dfPACKET_MOVE_DIR_LD:
	{
		if (sectorX - deltaX < dfRANGE_MOVE_LEFT || sectorY + deltaY >= dfRANGE_MOVE_BOTTOM) return false;
		sectorX -= deltaX;
		sectorY += deltaY;


	}
	break;
	default:
		__debugbreak();
		break;
	}



	if ((sectorX / SECTOR_RATIO == oldX / SECTOR_RATIO) && (sectorY / SECTOR_RATIO == oldY / SECTOR_RATIO))
	{
		return true;
	}


	SyncSector(accountNo, oldX, oldY);

	return true;
}



bool CPlayer::MoveStart(BYTE Direction, int x, int y) {

	int oldX = sectorX;
	int oldY = sectorY;

	int oldSectorX = oldX / SECTOR_RATIO;
	int oldSectorY = oldY / SECTOR_RATIO;

	_direction = Direction;
	sectorX = x;
	sectorY = y;
	_move = true;

	int newSectorX = x / SECTOR_RATIO;
	int newSectorY = y / SECTOR_RATIO;

	if ((newSectorX != oldSectorX) || (newSectorY != oldSectorY))
	{
		SyncSector(accountNo, oldX, oldY);
	}


	return true;
}



void CPlayer::MoveStop(BYTE Dir, int x, int y)
{
	int oldX = sectorX;
	int oldY = sectorY;

	int oldSectorX = oldX / SECTOR_RATIO;
	int oldSectorY = oldY / SECTOR_RATIO;



	_direction = Dir;
	sectorX = x;
	sectorY = y;

	_move = false;

	int newSectorX = x / SECTOR_RATIO;
	int newSectorY = y / SECTOR_RATIO;

	//섹터 동기화
	if ((newSectorX != oldSectorX) || (newSectorY != oldSectorY))
	{
		SyncSector(accountNo, oldX, oldY);
	}


}

void CPlayer::Clear()
{
	_status = static_cast<BYTE>(enSTATUS::IDLE);

}

void CPlayer::Init(ULONG64 sessionID)
{

	sectorX = 0;
	sectorY = 0;
	_direction = 0;


	_move = false;
	accountNo = sessionID;
	_status = static_cast<BYTE>(enSTATUS::PENDING_SECTOR);
	_timeOut = timeGetTime();
}

bool CPlayer::isAlive()
{
	if (_status != static_cast<BYTE>(enSTATUS::IDLE))
	{
		return true;
	}

	return false;
}
unsigned long long CPlayer::GetID()
{
	return accountNo;
}