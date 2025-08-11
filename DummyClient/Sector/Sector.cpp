#include "Sector.h"
#include "Player/DummySession.h"

std::list<CDummySession*> g_Sector[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
std::mutex g_SectorLock[dfRANGE_MOVE_RIGHT / SECTOR_RATIO][dfRANGE_MOVE_BOTTOM / SECTOR_RATIO];
int sectorXRange = dfRANGE_MOVE_RIGHT / SECTOR_RATIO;
int sectorYRange = dfRANGE_MOVE_BOTTOM / SECTOR_RATIO;

extern CDummySession* g_DummySessionArr;