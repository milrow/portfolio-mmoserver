#pragma once 
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>

#include <functional>
#include <memory>

#include "Protocol.pb.h"
#include "DbManager.h"
#include "DbJob.h"
#include "LoginDbJob.h"
#include "SendBuffer.h"

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t size;
    uint16_t protocolId;
};
#pragma pack(pop)

// 전방 선언 (Session.h를 포함하지 않고도 SessionRef를 쓰기 위해)
class Session;
using SessionRef = std::shared_ptr<Session>;
using PacketHandlerFunc = std::function<bool(SessionRef&, BYTE*, int32_t)>;

template<typename T>
SendBufferRef MakeSendBuffer(T& packet, uint16_t packetId) {
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

inline wstring Utf8ToWstring(const string& str) {
    if (str.empty()) return wstring();

    // 필요한 크기 계산
    int len = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wstring res(len, 0);

    // 실제 변환
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &res[0], len);
    return res;
}

class ClientPacketHandler {
public:
    static void Init();
    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId);
    static void BroadcastLeaveGame(uint32_t sessionId);
private:
    static PacketHandlerFunc _handlers[65535];

    static bool Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l);

    static SendBufferRef Handle_S2C_BroadcastMove(float x, float y, float z, uint32_t sessionId);
    static SendBufferRef Handle_S2C_BroadcastJump(uint32_t sessionId);
    static SendBufferRef Handle_S2C_LeaveGame(uint32_t sessionId);
    
};

class DbPacketHandler {
public:
    static void Init();
    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId);
private:
    static PacketHandlerFunc _handlers[65535];

    static bool Handle_D2S_Login(SessionRef& s, BYTE* b, int32_t l);
};

