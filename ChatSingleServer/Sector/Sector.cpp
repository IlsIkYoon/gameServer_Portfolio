#include "Sector.h"
#include "Message/Message.h"
#include "Contents/ContentsFunc.h"
#include "Network/Network.h"

std::list<CPlayer*> Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
int sectorXRange = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
int sectorYRange = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

extern CPlayer* g_PlayerArr;
extern CWanServer* pLib;

bool SyncSector(ULONG64 UserId, int beforeX, int beforeY)
{
	int playerIndex = pLib->GetIndex(UserId);
	int playerX = g_PlayerArr[playerIndex].GetX();
	int playerY = g_PlayerArr[playerIndex].GetY();

	int beforeSectorX = beforeX / SECTOR_RATIO;
	int beforeSectorY = beforeY / SECTOR_RATIO;

	int SectorX = playerX / SECTOR_RATIO;
	int SectorY = playerY / SECTOR_RATIO;
	
	size_t debugSize = Sector[beforeSectorX][beforeSectorY].size();
	Sector[beforeSectorX][beforeSectorY].remove(&g_PlayerArr[playerIndex]);

	if (debugSize == Sector[beforeSectorX][beforeSectorY].size())
	{
		__debugbreak();
		return false;
	}
	Sector[SectorX][SectorY].push_back(&g_PlayerArr[playerIndex]);

	return true;
}






bool CheckSector(ULONG64 UserId)
{
#ifdef __DEBUG__
	int playerIndex = pLib->GetIndex(UserId);
	int localX;
	int localY;

	localX = g_PlayerArr[playerIndex].GetX();
	localY = g_PlayerArr[playerIndex].GetY();
	auto searchIt = std::find(Sector[localX / SECTOR_RATIO][localY / SECTOR_RATIO].begin(),
		Sector[localX / SECTOR_RATIO][localY / SECTOR_RATIO].end(),
		&g_PlayerArr[playerIndex]);
	if (searchIt == Sector[localX / SECTOR_RATIO][localY / SECTOR_RATIO].end())
	{
		__debugbreak();
		return false;
	}
#endif
	return true;
}