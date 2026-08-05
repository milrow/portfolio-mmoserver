#include "GameService.h"

void GameService::LeaveGame(uint32_t sessionId)
{
	// 이미 나간 유저면 중복 처리 방지
	/*if(ObjectManager::GetInstance().RemovePlayer(sessionId) == false)
	  return;*/


	Protocol::S2C_LeaveGame packet;
	packet.set_sessionid(sessionId);
	SendPacketHandler::Broadcast(packet, Protocol::ID_S2C_LeaveGame);
}

void GameService::MovePlayer(uint32_t objectId, Position& position)
{
	Protocol::S2C_BroadcastMove packet;
	packet.set_posx(position.posx);
	packet.set_posy(position.posy);
	packet.set_posz(position.posz);
	packet.set_yaw(position.yaw);

	packet.set_objectid(objectId);

	SendPacketHandler::Broadcast(packet, Protocol::ID_S2C_BroadcastMove);

	//ObjectManager::GetInstance().UpdatePlayerPosition();
}

void GameService::JumpPlayer(uint32_t sessionId)
{
	Protocol::S2C_BroadcastJump packet;
	
	SendPacketHandler::Broadcast(packet, Protocol::ID_S2C_BroadcastJump);
}
