#include "PacketHandler.h"
#include "SessionManager.h"

PacketHandlerFunc DbPacketHandler::_handlers[65535];

void DbPacketHandler::Init()
{
	for (int i = 0; i < 65535; i++) {
		_handlers[i] = nullptr;
	}

	_handlers[Protocol::ID_D2S_Login] = Handle_D2S_Login;
	_handlers[Protocol::ID_D2S_CreateAccount] = Handle_D2S_CreateAccount;
	_handlers[Protocol::ID_D2S_CreateCharacter] = Handle_D2S_CreateCharacter;
	_handlers[Protocol::ID_D2S_EnterWorld] = Handle_D2S_EnterWorld;
}

bool DbPacketHandler::HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId)
{
	if (_handlers[protocolId] != nullptr) {
		return _handlers[protocolId](session, buffer, len);
	}

	return false;
}

bool DbPacketHandler::Handle_D2S_Login(SessionRef& s, BYTE* b, int32_t l)
{
	Protocol::D2S_Login dbPacket;
	if (dbPacket.ParseFromArray(b, l) == false) {
		return false;
	}

	s->_isLoggedIn = (dbPacket.result() == 0 && dbPacket.accountid() != 0);
	s->_accountId = s->_isLoggedIn ? dbPacket.accountid() : 0;

	Protocol::S2C_Login packet;
	packet.set_result(dbPacket.result());
	packet.set_accountid(dbPacket.accountid());
	packet.set_sessionid(s->_sessionId);

	auto sendBufferRef = MakeSendBuffer(packet, Protocol::ID_S2C_Login);
	s->Send(sendBufferRef);

	return true;
}

bool DbPacketHandler::Handle_D2S_CreateAccount(SessionRef& s, BYTE* b, int32_t l)
{
	auto sendBufferRef = AttachHeader(b, l, Protocol::ID_S2C_CreateAccount);
	s->Send(sendBufferRef);
	return true;
}

bool DbPacketHandler::Handle_D2S_CreateCharacter(SessionRef& s, BYTE* b, int32_t l)
{
	auto sendBufferRef = AttachHeader(b, l, Protocol::ID_S2C_CreateCharacter);
	s->Send(sendBufferRef);
	return true;
}

bool DbPacketHandler::Handle_D2S_EnterWorld(SessionRef& s, BYTE* b, int32_t l)
{
	Protocol::D2S_EnterWorld dbPacket;
	if (dbPacket.ParseFromArray(b, l) == false)
	{
		return false;
	}

	auto player =  ObjectManager::GetInstance().SpawnPlayer(s);
	if(player == nullptr)
	{
		return false;
	}
	player->SetPosition(Position(dbPacket.posx(), dbPacket.posy(), dbPacket.posz(), dbPacket.yaw()));

	s->_objectId = player->GetObjectId();

	Protocol::S2C_EnterWorld packet;
	packet.set_result(dbPacket.result());
	packet.set_objectid(player->GetObjectId());

	packet.set_posx(dbPacket.posx());
	packet.set_posy(dbPacket.posy());
	packet.set_posz(dbPacket.posz());
	packet.set_yaw(dbPacket.yaw());

	packet.mutable_character()->CopyFrom(dbPacket.character());

	auto sendBufferRef = MakeSendBuffer(packet, Protocol::ID_S2C_EnterWorld);
	s->Send(sendBufferRef);

	return true;
}
