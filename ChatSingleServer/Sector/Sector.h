#pragma once
#include "Contents/ContentsResource.h"
#include "Player/Player.h"

bool SyncSector(ULONG64 UserId, int oldX, int oldY);
//-------------------------------------------------
// ����׿�. ���� ���Ϳ� ���� ����ִ����� Ȯ��
//-------------------------------------------------
bool CheckSector(ULONG64 UserId);