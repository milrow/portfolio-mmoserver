#pragma once

#include <string>

#include "GameObject.h"
#include "Session.h"

using namespace std;

class Player : public GameObject
{
private:
	weak_ptr<Session> _session;

	//uint64_t accountId;
	uint64_t _characterId;
	wstring _name;

	uint32_t hp = 0;
	uint32_t maxHp = 0;
	uint32_t mp = 0;
	uint32_t maxMp = 0;
	uint16_t level = 0;

public:
	Player(uint64_t playerId, wstring name);
	Player(shared_ptr<Session> session, uint64_t playerId, wstring name = L"");
	Player(const Player& copy);
	virtual ~Player() {}

public:
	virtual void Update() override;
	void Send(shared_ptr<SendBuffer> sendBuffer);
	uint64_t GetCharacterID() { return _characterId; }
};

