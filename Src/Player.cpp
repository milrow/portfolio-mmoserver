#include "Player.h"

Player::Player(uint64_t playerId, wstring name)
	: GameObject(ObjectType::Player), _characterId(playerId), _name(name)
{
}

Player::Player(shared_ptr<Session> session, uint64_t playerId, wstring name)
	: GameObject(ObjectType::Player), _session(session), _characterId(playerId), _name(name)
{
	SetObjectId(playerId);
}

Player::Player(const Player& copy)
	: GameObject(ObjectType::Player), _session(copy._session), _characterId(copy._characterId), _name(copy._name)
{
	SetPosition(copy.GetPosition());

	hp = copy.hp;
	maxHp = copy.maxHp;
	mp = copy.mp;
	maxMp = copy.maxMp;
	level = copy.level;
}

void Player::Update()
{

}

void Player::Send(shared_ptr<SendBuffer> sendBuffer)
{
	if(auto session = _session.lock())
	{
		session->Send(sendBuffer);
	}
}
