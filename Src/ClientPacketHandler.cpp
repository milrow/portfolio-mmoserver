
#include "SessionManager.h"
#include "PacketHandler.h"

PacketHandlerFunc ClientPacketHandler::_handlers[65535];

void ClientPacketHandler::Init()
{
	for (int i = 0; i < 65535; i++) {
		_handlers[i] = nullptr;
	}

	_handlers[Protocol::ID_C2S_Move] = Handle_C2S_Move;
	_handlers[Protocol::ID_C2S_Login] = Handle_C2S_Login;
	_handlers[Protocol::ID_C2S_Jump] = Handle_C2S_Jump;
	_handlers[Protocol::ID_C2S_LeaveGame] = Handle_C2S_LeaveGame;
}

bool ClientPacketHandler::HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId)
{
	if (_handlers[protocolId] != nullptr) {
		return _handlers[protocolId](session, buffer, len);
	}

	return false;
}

void ClientPacketHandler::BroadcastLeaveGame(uint32_t sessionId) {
	auto sendBufferRef = Handle_S2C_LeaveGame(sessionId);
	SessionManager::GetInstance().Broadcast(sendBufferRef);
	return;
}

#pragma region C2S_Region

bool ClientPacketHandler::Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Login packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	
	shared_ptr<LoginDbJob> dbJob = make_shared<LoginDbJob>();
	string loginId = packet.loginid();
	//printf(" loginId %s", loginId);
	int loginIdLen = MultiByteToWideChar(CP_UTF8, 0, loginId.c_str(), -1, dbJob->_loginId, 11);

	if (loginIdLen == 0) {
		cerr << "MultiByteToWideChar loginId Error" << GetLastError() << endl;
		return false;
	}

	string password = packet.password();
	int passwordLen = MultiByteToWideChar(CP_UTF8, 0, password.c_str(), -1, dbJob->_password, 21);
	dbJob->_sessionId = s->_sessionId;
	printf_s("LoginID:%ws, password:%ws", dbJob->_loginId, dbJob->_password);
	DbManager::GetInstance().PushJob(dbJob);

	return true;
}

bool ClientPacketHandler::Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Move packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}

	uint32_t sessionId = packet.sessionid();
	float posX = packet.posx();
	float posY = packet.posy();
	float posZ = packet.posz();

	auto sendBufferRef = Handle_S2C_BroadcastMove(posX, posY, posZ, sessionId);
	SessionManager::GetInstance().Broadcast(sendBufferRef);

	return true;
}

bool ClientPacketHandler::Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Jump packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	uint32_t sessionId = packet.sessionid();

	auto sendBufferRef = Handle_S2C_BroadcastJump(sessionId);
	SessionManager::GetInstance().Broadcast(sendBufferRef);

	return true;
}

bool ClientPacketHandler::Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_LeaveGame packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	uint32_t sessionId = packet.sessionid();

	BroadcastLeaveGame(sessionId);

	return true;
}
#pragma endregion

#pragma region S2C_Region
SendBufferRef ClientPacketHandler::Handle_S2C_BroadcastMove(float x, float y, float z, uint32_t sessionId) {
	Protocol::S2C_BroadcastMove packet;
	packet.set_posx(x);
	packet.set_posy(y);
	packet.set_posz(z);
	packet.set_sessionid(sessionId);

	return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastMove);
}

SendBufferRef ClientPacketHandler::Handle_S2C_BroadcastJump(uint32_t sessionId) {
	Protocol::S2C_BroadcastJump packet;
	packet.set_sessionid(sessionId);

	return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastJump);
}

SendBufferRef ClientPacketHandler::Handle_S2C_LeaveGame(uint32_t sessionId) {
	Protocol::S2C_LeaveGame packet;
	packet.set_sessionid(sessionId);

	return MakeSendBuffer(packet, Protocol::ID_S2C_LeaveGame);
}
#pragma endregion





























