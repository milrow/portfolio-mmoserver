

#include "ClientPacketHandler.gen.h"
#include "SecurityManager.h"


//void ClientPacketHandler::BroadcastLeaveGame(uint32_t sessionId) {
//	auto sendBufferRef = Handle_S2C_LeaveGame(sessionId);
//	SessionManager::GetInstance().Broadcast(sendBufferRef);
//	return;
//}

#pragma region C2S_Region

bool ClientPacketHandler::Handle_C2S_Login(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Login packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}

	shared_ptr<LoginDbJob> dbJob = make_shared<LoginDbJob>();
	dbJob->_loginId = packet.loginid();
	dbJob->_password = packet.password();
	dbJob->_sessionId = s->_sessionId;

	DbManager::GetInstance().PushJob(dbJob);

	return true;
}

bool ClientPacketHandler::Handle_C2S_Move(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Move packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	
	uint32_t objectId = s->_objectId;

	Position position;
	position.posx = packet.posx();
	position.posy = packet.posy();
	position.posz = packet.posz();
	position.yaw = packet.yaw();

	GameService::GetInstance().MovePlayer(objectId, position);

	return true;
}

bool ClientPacketHandler::Handle_C2S_Jump(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_Jump packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	uint32_t objectId = s->_objectId;
	
	GameService::GetInstance().JumpPlayer(objectId);

	return true;
}

bool ClientPacketHandler::Handle_C2S_LeaveGame(SessionRef& s, BYTE* b, int32_t l) {
	Protocol::C2S_LeaveGame packet;
	if (packet.ParseFromArray(b, l) == false) {
		return false;
	}
	uint32_t sessionId = packet.sessionid();

	GameService::GetInstance().LeaveGame(sessionId);

	return true;
}
bool ClientPacketHandler::Handle_C2S_CreateAccount(SessionRef& s, BYTE* b, int32_t l)
{
	try
	{
		Protocol::C2S_CreateAccount packet;
		if (packet.ParseFromArray(b, l) == false) {
			return false;
		}

		shared_ptr<CreateAccountDbJob> dbJob = make_shared<CreateAccountDbJob>();
		dbJob->_loginId = packet.loginid();

		string hashedPassword = SecurityManager::GetInstance().HashPassword(packet.password());
		dbJob->_password = hashedPassword;

		dbJob->_sessionId = s->_sessionId;
		cout << "Handle_C2S_CreateAccount" << endl;
		DbManager::GetInstance().PushJob(dbJob);

		return true;
	}
	catch (const std::exception&)
	{

	}
	return false;
}
bool ClientPacketHandler::Handle_C2S_CreateCharacter(SessionRef& s, BYTE* b, int32_t l)
{
	try
	{
		Protocol::C2S_CreateCharacter packet;
		if (packet.ParseFromArray(b, l) == false) {
			return false;
		}

		shared_ptr<CreateCharacterDbJob> dbJob = make_shared<CreateCharacterDbJob>();
		dbJob->_accountId = s->_accountId;
		int sizeNeed = MultiByteToWideChar(CP_UTF8, 0, packet.name().c_str(), packet.name().size(), NULL, 0);
		dbJob->_nickName.resize(sizeNeed);
		MultiByteToWideChar(CP_UTF8, 0, packet.name().c_str(), packet.name().size(), dbJob->_nickName.data(), sizeNeed);
		dbJob->_slot = packet.slotid();
		dbJob->_sessionId = s->_sessionId;

		DbManager::GetInstance().PushJob(dbJob);

		return true;
	}
	catch (const std::exception&)
	{

	}
	
}
bool ClientPacketHandler::Handle_C2S_CharacterList(SessionRef& s, BYTE* b, int32_t l)
{
	try
	{
		if (s->_isLoggedIn == false || s->_accountId == 0)
		{
			throw 2;
		}

		Protocol::C2S_CharacterList packet;
		if (packet.ParseFromArray(b, l) == false) {
			return false;
		}

		shared_ptr<CharacterListDbJob> dbJob = make_shared<CharacterListDbJob>();
		dbJob->_accountId = s->_accountId;
		dbJob->_sessionId = s->_sessionId;

		DbManager::GetInstance().PushJob(dbJob);

		return true;
	}
	catch (const int errorCode)
	{
		Protocol::S2C_CharacterList packet;
		packet.set_result(500);
		auto sendBuffer = MakeSendBuffer(packet, Protocol::ID_S2C_CharacterList);
		s->Send(sendBuffer);

		return false;
	}
	
	return false;
}
bool ClientPacketHandler::Handle_C2S_DeleteCharacter(SessionRef& s, BYTE* b, int32_t l)
{
	return false;
}
bool ClientPacketHandler::Handle_C2S_EnterWorld(SessionRef& s, BYTE* b, int32_t l)
{
	try
	{
		Protocol::C2S_EnterWorld packet;
		if (packet.ParseFromArray(b, l) == false) {
			return false;
		}

		shared_ptr<EnterWorldDbJob> job = make_shared<EnterWorldDbJob>();
		job->_accountId = s->_accountId;
		job->_sessionId = s->_sessionId;
		job->_characterId = packet.characterid();


		DbManager::GetInstance().PushJob(job);
	}
	catch (const std::exception&)
	{

	}
	
	


	

	return false;
}
#pragma endregion
//
//#pragma region S2C_Region
//SendBufferRef ClientPacketHandler::Handle_S2C_BroadcastMove(float x, float y, float z, uint32_t sessionId) {
//	Protocol::S2C_BroadcastMove packet;
//	packet.set_posx(x);
//	packet.set_posy(y);
//	packet.set_posz(z);
//	packet.set_sessionid(sessionId);
//
//	return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastMove);
//}
//
//SendBufferRef ClientPacketHandler::Handle_S2C_BroadcastJump(uint32_t sessionId) {
//	Protocol::S2C_BroadcastJump packet;
//	packet.set_sessionid(sessionId);
//
//	return MakeSendBuffer(packet, Protocol::ID_S2C_BroadcastJump);
//}
//
//SendBufferRef ClientPacketHandler::Handle_S2C_LeaveGame(uint32_t sessionId) {
//	Protocol::S2C_LeaveGame packet;
//	packet.set_sessionid(sessionId);
//
//	return MakeSendBuffer(packet, Protocol::ID_S2C_LeaveGame);
//}
//
//#pragma endregion





























