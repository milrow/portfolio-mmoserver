// 자동 생성된 패킷 핸들러 선언부입니다. 수동 편집 금지!
#pragma once

#include "PacketHandler.h"
class ClientPacketHandler {
public:
    static void Init();
    static bool HandlePacket(SessionRef& session, BYTE* buffer, int32_t len, uint16_t protocolId);
static void BroadcastLeaveGame(uint32_t sessionId);
private:
    static PacketHandlerFunc _handlers[65535];
    static bool Handle_C2S_CreateAccount(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_CreateCharacter(SessionRef& s, BYTE* b, int32_t l);
    static bool Handle_C2S_CharacterList(SessionRef& s, BYTE* b, int32_t l);
};

