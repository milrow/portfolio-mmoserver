#pragma once 
#include <functional>
#include <memory>
#include "Protocol.pb.h"

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

// "GPacketHandler라는 배열이 어딘가(cpp)에 진짜로 존재한다"고 예고만 함
extern PacketHandlerFunc GPacketHandler[65535];

void InitPacketHandler();
bool Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l);
bool Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l);
bool Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l);
bool Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l);

SendBufferRef Handle_S2C_BroadcastMove(float x, float y, float z, uint32_t sessionId);
SendBufferRef Handle_S2C_LeaveGame(uint32_t sessionId);
void BroadcastLeaveGame(uint32_t sessionId);

template<typename T>
SendBufferRef MakeSendBuffer(T& packet, uint16_t packetId) {
    const uint16_t dataSize = static_cast<uint16_t>(packet.ByteSizeLong());
    const uint16_t packetSize = dataSize + sizeof(PacketHeader);

    SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);

    PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
    header->protocolId = packetId;
    header->size = packetSize;

    //printf("[Debug] Header Addr: %p\n", header);
    //printf("[Debug] Data Addr:   %p\n", header + 1);

    // 실제 메모리 상의 바이트 차이 계산 (char* 로 캐스팅해서 빼야 1바이트 단위로 계산됨)
    intptr_t diff = reinterpret_cast<intptr_t>(header + 1) - reinterpret_cast<intptr_t>(header);
    //printf("[Debug] Address Diff: %lld bytes (Expected: 4)\n", diff);

    if (packet.SerializeToArray(header + 1, dataSize)) {
        sendBuffer->Close(packetSize);
        return sendBuffer;
    }
    return nullptr;
}