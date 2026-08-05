#pragma once

#include <shared_mutex>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <memory>
#include <mutex>


#include "GameObject.h"
#include "Player.h"


using namespace std;

struct PlayerPositionSnapshot
{
	uint64_t characterId;
	Position position;
};

class ObjectManager
{
private:
	atomic<uint64_t> _idGenerator{ 1 };
	unordered_map<uint64_t, shared_ptr<GameObject>> _objects;
	unordered_set<uint64_t> _dirtyPositionObjects;
	shared_mutex _mutex;

private:
	ObjectManager() {}
	ObjectManager(const ObjectManager& ref) {}
	ObjectManager& operator=(const ObjectManager& ref) {}
	~ObjectManager() {}

public:
	static ObjectManager& GetInstance()
	{
		static ObjectManager _instance;
		return _instance;
	}

public:
	shared_ptr<Player> SpawnPlayer(shared_ptr<Session> session);
	bool RemovePlayer(uint64_t playerId);
	void Despawn(uint64_t objectId);

	shared_ptr<GameObject> Find(uint64_t objectId);


	void UpdatePlayerPosition(uint64_t objectId, const Position& position);
	vector<PlayerPositionSnapshot> FlushDirtyPlayerPositions();
};

