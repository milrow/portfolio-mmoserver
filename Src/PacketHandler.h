#pragma once 
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WinSock2.h>
#include <Windows.h>

#include <functional>
#include <memory>

#include "Protocol.pb.h"
#include "DbManager.h"
#include "DbJob.h"

#include "LoginDbJob.h"
#include "CreateAccountDbJob.h"
#include "CreateCharacterDbJob.h"
#include "CharacterListDbJob.h"
#include "EnterWorldDbJob.h"

#include "SendBuffer.h"
#include "SessionManager.h"
#include "GameService.h"

//#include "PacketHandlerUtility.h"

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;
    uint16_t protocolId;
};
#pragma pack(pop)

class SecurityManager;

class Session;
using SessionRef = std::shared_ptr<Session>;
using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32_t)>;


template<typename Packet>
SendBufferRef MakeSendBuffer(Packet& packet, uint16_t packetId) {
	const uint16_t dataSize = static_cast<uint16_t>(packet.ByteSizeLong());
	const uint16_t packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);

	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->protocolId = packetId;
	header->size = packetSize;

	if (packet.SerializeToArray(header + 1, dataSize)) {
		sendBuffer->Close(packetSize);
		return sendBuffer;
	}
	return nullptr;
}

inline SendBufferRef AttachHeader(BYTE* data, int32_t dataSize, uint16_t packetId) {
	const uint16_t packetSize = dataSize + sizeof(PacketHeader);

	SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);

	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
	header->protocolId = packetId;
	header->size = packetSize;

	memcpy(header + 1, data, dataSize);

	sendBuffer->Close(packetSize);
	return sendBuffer;
}

template<typename Derived>
class PacketHandlerBase {
public:
	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId)
	{
		if(protocolId >= _handlers.size()) {
			return false;
		}

		PacketHandlerFunc handler = _handlers[protocolId];
		if (handler != nullptr) {
			return handler(session, buffer, len);
		}

		return false;
	}
protected:
	static void Register(uint16_t protocolId, PacketHandlerFunc handler) {
		if(protocolId >= _handlers.size()) {
			return;
		}
		_handlers[protocolId] = handler;
	}

	inline static array<PacketHandlerFunc, 65535> _handlers = {};
};


class DbPacketHandler {
public:
    static void Init();
    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId);
private:
    static PacketHandlerFunc _handlers[65535];

    static bool Handle_D2S_Login(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_D2S_CreateAccount(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_D2S_CreateCharacter(SessionRef& s, BYTE* b, int32_t l);
	static bool Handle_D2S_EnterWorld(SessionRef& s, BYTE* b, int32_t l);
};

class SendPacketHandler
{
public:
	static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId);

	template<typename Packet>
	static void Broadcast(const Packet& packet, uint16_t protocolId)
	{
		SendBufferRef sendBuffer = ::MakeSendBuffer(packet, protocolId);
		if (sendBuffer == nullptr) {
			return;
		}
		
		SessionManager::GetInstance().Broadcast(sendBuffer);
	}
};
