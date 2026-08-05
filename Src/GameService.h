#pragma once

#include <cstdint>

#include "PacketHandler.h"
#include "ObjectManager.h"

class GameService
{
public:
	static GameService& GetInstance() {
		static GameService _instance;
		return _instance;
	}

public:
	void LeaveGame(uint32_t sessionId);
	void MovePlayer(uint32_t objectId, Position& position);
	void JumpPlayer(uint32_t sessionId);
};

