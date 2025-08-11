#pragma once

#include "ContentsResource.h"
#include "Player.h"

class CPlayerManager
{
public:
	CPlayer* playerArr;
	CPlayerManager(int playerCount);

	CPlayer& operator[](int iDex)
	{
		return playerArr[iDex];
	}
};