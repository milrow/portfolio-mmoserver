#include "PacketHandler.h"
#include "SessionManager.h"

PacketHandlerFunc DbPacketHandler::_handlers[65535];

void DbPacketHandler::Init()
{
	for (int i = 0; i < 65535; i++) {
		_handlers[i] = nullptr;
	}

	_handlers[Protocol::ID_D2S_Login] = Handle_D2S_Login;
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
	auto sendBufferRef = AttachHeader(b, l, Protocol::ID_S2C_Login);
	s->Send(sendBufferRef);
	return true;
}