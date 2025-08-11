#pragma once
#include "ContentsResource.h"

class CCharacterKeyManager
{
	std::unordered_map<ULONG64, ULONG64> _Key_uMap;
	std::recursive_mutex _Key_uMapLock;
	DWORD _playerMaxCount;

public:
	CCharacterKeyManager() = delete;

	CCharacterKeyManager(DWORD playerCount)
	{
		_playerMaxCount = playerCount;
		_Key_uMap.rehash(playerCount);
	}
	
	bool InsertID(ULONG64 characterKey, ULONG64 sessionID);
	bool DeleteID(ULONG64 characterKey, ULONG64 sessionID);
};