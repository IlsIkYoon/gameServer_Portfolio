#include "ContentsResource.h"
#include "Sector.h"
#include "Msg/Message.h"
#include "ContentsThread/ContentsFunc.h"
#include "ContentsThread/ContentsThreadManager.h"

std::list<CPlayer*> Sector[SECTOR_MAX][SECTOR_MAX];
std::recursive_mutex SectorLock[SECTOR_MAX][SECTOR_MAX];

int sectorXRange = SECTOR_MAX;
int sectorYRange = SECTOR_MAX;

extern CWanServer* networkServer;
extern CContentsThreadManager contentsManager;

bool SyncSector(ULONG64 UserId, int oldSectorX, int oldSectorY)
{
	CPlayer* localPlayerList;
	int playerIndex;
	int currentSectorX;
	int currentSectorY;

	localPlayerList = contentsManager.playerList->playerArr;

	playerIndex = networkServer->GetIndex(UserId);
	currentSectorX = localPlayerList[playerIndex].sectorX;
	currentSectorY = localPlayerList[playerIndex].sectorY;

	if (currentSectorX == oldSectorX && currentSectorY == oldSectorY)
	{
		return false;
	}

	SectorLockByIndexOrder(oldSectorX, oldSectorY, currentSectorX, currentSectorY);

	Sector[oldSectorX][oldSectorY].remove(&localPlayerList[playerIndex]);

	Sector[currentSectorX][currentSectorY].push_back(&localPlayerList[playerIndex]);

	SectorUnlockByIndexOrder(oldSectorX, oldSectorY, currentSectorX, currentSectorY);

	return true;
}

bool CheckSector(ULONG64 UserId)
{
	//디버그 define에서만 동작. 
#ifdef __DEBUG__
	CPlayer* localPlayerList;
	int playerIndex;
	int localX;
	int localY;

	localPlayerList = contentsManager.playerList->playerArr;
	playerIndex = networkServer->GetIndex(UserId);
	localX = localPlayerList[playerIndex].sectorX;
	localY = localPlayerList[playerIndex].sectorY;
	{
		std::lock_guard guard(SectorLock[localX][localY]);
		auto searchIt = std::find(Sector[localX][localY].begin(),
			Sector[localX][localY].end(),
			&localPlayerList[playerIndex]);
		if (searchIt == Sector[localX][localY].end())
		{
			__debugbreak();
			return false;
		}
	}

#endif
	return true;
}



bool SectorLockByIndexOrder(int beforeX, int beforeY, int currentX, int currentY)
{
	//X를 기준으로 정렬
	int firstX;
	int firstY;
	int secondX;
	int secondY;

	if (beforeX < currentX)
	{
		firstX = beforeX;
		firstY = beforeY;
		secondX = currentX;
		secondY = currentY;
	}
	else if (beforeX == currentX)
	{
		if (beforeY < currentY)
		{
			firstX = beforeX;
			firstY = beforeY;
			secondX = currentX;
			secondY = currentY;
		}
		else
		{
			firstX = currentX;
			firstY = currentY;
			secondX = beforeX;
			secondY = beforeY;
		}
	}
	else {
		firstX = currentX;
		firstY = currentY;
		secondX = beforeX;
		secondY = beforeY;

	}

	SectorLock[firstX][firstY].lock();
	SectorLock[secondX][secondY].lock();
	return true;
}


bool SectorUnlockByIndexOrder(int beforeX, int beforeY, int currentX, int currentY)
{

	//X를 기준으로 먼저 정렬 후에 Y를 기준으로 정렬
	int firstX;
	int firstY;
	int secondX;
	int secondY;

	if (beforeX < currentX)
	{
		firstX = beforeX;
		firstY = beforeY;
		secondX = currentX;
		secondY = currentY;
	}
	else if (beforeX == currentX)
	{
		if (beforeY < currentY)
		{
			firstX = beforeX;
			firstY = beforeY;
			secondX = currentX;
			secondY = currentY;
		}
		else
		{
			firstX = currentX;
			firstY = currentY;
			secondX = beforeX;
			secondY = beforeY;
		}
	}
	else {
		firstX = currentX;
		firstY = currentY;
		secondX = beforeX;
		secondY = beforeY;

	}


	SectorLock[firstX][firstY].unlock();
	SectorLock[secondX][secondY].unlock();
	return true;
}