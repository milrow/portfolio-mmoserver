// 자동 생성된 패킷 핸들러 정의부입니다. 수동 편집 금지!
#include "ClientPacketHandler.gen.h"
void ClientPacketHandler::Init()
{
    Register(Protocol::ID_C2S_CreateAccount, Handle_C2S_CreateAccount);
    Register(Protocol::ID_C2S_Login, Handle_C2S_Login);
    Register(Protocol::ID_C2S_Move, Handle_C2S_Move);
    Register(Protocol::ID_C2S_Jump, Handle_C2S_Jump);
    Register(Protocol::ID_C2S_LeaveGame, Handle_C2S_LeaveGame);
    Register(Protocol::ID_C2S_CreateCharacter, Handle_C2S_CreateCharacter);
    Register(Protocol::ID_C2S_CharacterList, Handle_C2S_CharacterList);
    Register(Protocol::ID_C2S_DeleteCharacter, Handle_C2S_DeleteCharacter);
    Register(Protocol::ID_C2S_EnterWorld, Handle_C2S_EnterWorld);
}
