#include "PacketHandler.h"
#include "SessionManager.h"

bool SendPacketHandler::HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId)
{
	auto sendBufferRef = AttachHeader(buffer, len, protocolId);
	session->Send(sendBufferRef);
	return true;
}