#include "ObjectManager.h"

shared_ptr<Player> ObjectManager::SpawnPlayer(shared_ptr<Session> session)
{
	unique_lock<shared_mutex> lock(_mutex);
	const uint64_t playerId = _idGenerator.fetch_add(1);
	shared_ptr<Player> player = make_shared<Player>(session, playerId);
	_objects[playerId] = player;

    return player;
}

bool ObjectManager::RemovePlayer(uint64_t playerId)
{
	unique_lock<shared_mutex> lock(_mutex);

	_objects.erase(playerId);

	return true;
}

void ObjectManager::Despawn(uint64_t objectId)
{
	unique_lock<shared_mutex> lock(_mutex);
	if(_objects.find(objectId) != _objects.end()) {
		_objects.erase(objectId);
	}
}

shared_ptr<GameObject> ObjectManager::Find(uint64_t objectId)
{
	shared_lock<shared_mutex> lock(_mutex);
	auto it = _objects.find(objectId);
	if (it != _objects.end()) {
		return it->second;
	}
	return nullptr;
}

void ObjectManager::UpdatePlayerPosition(uint64_t objectId, const Position& position)
{
	unique_lock<shared_mutex> lock(_mutex);
	try
	{
		auto it = _objects.find(objectId);
		if (it == _objects.end())
		{
			return;
		}

		if (it->second->GetObjectType() != ObjectType::Player)
		{
			return;
		}

		it->second->SetPosition(position);

		_dirtyPositionObjects.insert(objectId);

		return;
	}
	catch (const std::exception&)
	{

	}
	


}

vector<PlayerPositionSnapshot> ObjectManager::FlushDirtyPlayerPositions()
{
	vector<PlayerPositionSnapshot> snapshots;
	unique_lock<shared_mutex> lock(_mutex);

	for (uint64_t objectID : _dirtyPositionObjects)
	{
		auto it = _objects.find(objectID);
		if (it == _objects.end())
		{
			continue;
		}

		shared_ptr<Player> player = dynamic_pointer_cast<Player>(it->second);
		if (player == nullptr)
		{
			continue;
		}

		PlayerPositionSnapshot snapshot;
		snapshot.characterId = player->GetCharacterID();
		snapshot.position = player->GetPosition();
		snapshots.push_back(snapshot);
	}

	_dirtyPositionObjects.clear();

	return snapshots;
}
