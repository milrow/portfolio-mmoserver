#include "EnterWorldDbJob.h"

const wchar_t* EnterWorldDbJob::GetQuery()
{
	return L"{Call SP_EnterWorld(?, ?)}";
}

void EnterWorldDbJob::BindParameters(SQLHSTMT hStmt)
{
	::SQLBindParameter(
		hStmt,
		1,
		SQL_PARAM_INPUT,
		SQL_C_SBIGINT,
		SQL_BIGINT,
		0,
		0,
		&_accountId,
		0,
		&_accountIdLen
	);

	::SQLBindParameter(
		hStmt,
		2,
		SQL_PARAM_INPUT,
		SQL_C_SBIGINT,
		SQL_BIGINT,
		0,
		0,
		&_characterId,
		0,
		&_characterIdLen
	);
}

void EnterWorldDbJob::Excute(DbConnection* conn)
{
	BindParameters(conn->GetStmtHandle());

	Protocol::D2S_EnterWorld packet;

	if (conn->Prepare(GetQuery()) && conn->ExcutePrepared() && conn->Fetch())
	{
		//a.CharacterID, a.AccountID, a.Name, a.Level, b.MapID, b.PosX, b.PosY, b.PosZ, b.Yaw

		uint64_t characterId = conn->GetInt64Data(1);
		uint64_t accountId = conn->GetInt64Data(2);
		string name = conn->GetStringData(3);
		uint32_t level = conn->GetIntData(4);
		uint32_t mapID = conn->GetIntData(5);
		float posX = conn->GetFloatData(6);
		float posY = conn->GetFloatData(7);
		float posZ = conn->GetFloatData(8);
		float yaw = conn->GetFloatData(9);

		uint32_t result = 0;

		if (accountId == 0 || characterId == 0)
		{
			//no character matched accoundID
			result = 1;
		}
		
		packet.set_result(result);
		packet.set_mapid(mapID);
		packet.set_posx(posX);
		packet.set_posy(posY);
		packet.set_posz(posZ);
		packet.set_yaw(yaw);

		Protocol::WorldCharacter* character = packet.mutable_character();
		character->set_id(characterId);
		character->set_name(name);
		character->set_level(level);
	}
	else {
		PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
		//packet.set_result(100);
	}

	PushDbResultPacket(_sessionId, Protocol::ID_D2S_EnterWorld, packet);
}
