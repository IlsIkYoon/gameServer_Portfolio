#pragma once
#include "ContentsResource.h"
#include "Player/Player.h"

bool SyncSector(ULONG64 UserId, int oldSectorX, int oldSectorY);
//-------------------------------------------------
// ����׿�. ���� ���Ϳ� ���� ����ִ����� Ȯ�� 
//-------------------------------------------------
bool CheckSector(ULONG64 UserId);
//-------------------------------------------------
// ����� ������ ���� ���� �ε����� ������ ����ִ� �Լ�
//-------------------------------------------------
bool SectorLockByIndexOrder(int beforeX, int beforeY, int currentX, int currentY);
//-------------------------------------------------
// ���� �ε����� ������ Ǯ���ִ� �Լ�
//-------------------------------------------------
bool SectorUnlockByIndexOrder(int beforeX, int beforeY, int currentX, int currentY);