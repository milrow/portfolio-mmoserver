
#include "SessionManager.h"
#include "PacketHandler.h"

PacketHandlerFunc GPacketHandler[65535];




#pragma region S2C_Region
SendBufferRef Handle_S2C_BroadcastMove(float x, float y, float z, uint32_t sessionId) {
    Protocol::S2C_BroadcastMove packet;
    packet.set_posx(x);
    packet.set_posy(y);
    packet.set_posz(z);
    packet.set_sessionid(sessionId);

    return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastMove);
}

SendBufferRef Handle_S2C_BroadcastJump(uint32_t sessionId) {
    Protocol::S2C_BroadcastJump packet;
    packet.set_sessionid(sessionId);

    return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastJump);
}
 
SendBufferRef Handle_S2C_LeaveGame(uint32_t sessionId) {
    Protocol::S2C_LeaveGame packet;
    packet.set_sessionid(sessionId);

    return MakeSendBuffer(packet, Protocol::ID_S2C_LeaveGame);
}
#pragma endregion

void BroadcastLeaveGame(uint32_t sessionId) {
    auto sendBufferRef = Handle_S2C_LeaveGame(sessionId);
    SessionManager::GetInstance().Broadcast(sendBufferRef);
    return;
}



#pragma region C2S_Region

bool Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l) {
    Protocol::C2S_Login packet;
    if (packet.ParseFromArray(b, l) == false) {
        return false;
    }

    Protocol::S2C_Login response;
    response.set_sessionid(s->_sessionId);
    response.set_userid(packet.userid());

    auto sendBufferRef = MakeSendBuffer(response, Protocol::ID_S2C_Login);
    s->Send(sendBufferRef);
    //SessionManager::GetInstance().Send(sessionBuf, s->_sessionId);
    return true;
}

bool Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l) {
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

bool Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l) {
    Protocol::C2S_Jump packet;
    if (packet.ParseFromArray(b, l) == false) {
        return false;
    }
    uint32_t sessionId = packet.sessionid();

    auto sendBufferRef = Handle_S2C_BroadcastJump(sessionId);
    SessionManager::GetInstance().Broadcast(sendBufferRef);

    return true;
}

bool Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l) {
    Protocol::C2S_LeaveGame packet;
    if (packet.ParseFromArray(b, l) == false) {
        return false;
    }
    uint32_t sessionId = packet.sessionid();

    BroadcastLeaveGame(sessionId);

    return true;
}

#pragma endregion




void InitPacketHandler()
{
    GPacketHandler[Protocol::ID_C2S_Move] = Handle_C2S_Move;
    GPacketHandler[Protocol::ID_C2S_Login] = Handle_C2S_Login;
    GPacketHandler[Protocol::ID_C2S_Jump] = Handle_C2S_Jump;
    GPacketHandler[Protocol::ID_C2S_LeaveGame] = Handle_C2S_LeaveGame;
}











