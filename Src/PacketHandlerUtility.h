#pragma once

#include <WinSock2.h>
#include <Windows.h>

#include <string>
#include <memory>
#include <functional>


#include "Protocol.pb.h"

using namespace std;

//template<typename T>
//SendBufferRef MakeSendBuffer(T& packet, uint16_t packetId) {
//	const uint16_t dataSize = static_cast<uint16_t>(packet.ByteSizeLong());
//	const uint16_t packetSize = dataSize + sizeof(PacketHeader);
//
//	SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
//
//	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
//	header->protocolId = packetId;
//	header->size = packetSize;
//
//	if (packet.SerializeToArray(header + 1, dataSize)) {
//		sendBuffer->Close(packetSize);
//		return sendBuffer;
//	}
//	return nullptr;
//}

wstring Utf8ToWstring(const string& str);


//SendBufferRef AttachHeader(BYTE* data, int32_t dataSize, uint16_t packetId) {
//	const uint16_t packetSize = dataSize + sizeof(PacketHeader);
//
//	SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
//
//	PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
//	header->protocolId = packetId;
//	header->size = packetSize;
//
//	memcpy(header + 1, data, dataSize);
//
//	sendBuffer->Close(packetSize);
//	return sendBuffer;
//}