#pragma once

#include <cstdint>
#include <memory>

using namespace std;

struct Position
{
	float posx = 0.0f;
	float posy = 0.0f;
	float posz = 0.0f;
	float yaw = 0.0f;
};

enum class ObjectType : uint8_t
{
	None = 0,
	Player = 1,
	Monster = 2,
	NPC = 3,
	Projectile = 4,
	Item =5,
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
protected:
	uint64_t objectId = 0;
	Position position;
	ObjectType objectType = ObjectType::None;

public:
	GameObject(ObjectType type) : objectType(type) {}
	virtual ~GameObject() {}

	virtual void Update() {}

	uint64_t GetObjectId() const { return objectId; }
	void SetObjectId(uint64_t id) { objectId = id; }

	ObjectType GetObjectType() const { return objectType; }

	Position GetPosition() const { return position; }
	void SetPosition(const Position& pos) { position = pos; }
};