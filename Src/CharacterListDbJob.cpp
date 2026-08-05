#include "CharacterListDbJob.h"

const wchar_t* CharacterListDbJob::GetQuery()
{
	return L"{Call SP_CharacterList(?)}";
}

void CharacterListDbJob::BindParameters(SQLHSTMT hStmt)
{
	SQLBindParameter(
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
}

void CharacterListDbJob::Excute(DbConnection* conn)
{
	BindParameters(conn->GetStmtHandle());
	if (conn->Prepare(GetQuery()) && conn->ExcutePrepared())
	{
		Protocol::S2C_CharacterList packet;
		
		while (conn->Fetch())
		{
			auto character = packet.add_characters();
			character->set_id(conn->GetInt64Data(1));
			character->set_name(conn->GetStringData(2));
			character->set_slot(conn->GetIntData(3));
			 conn->BindTimestampParameter(character->mutable_createdat(), conn->GetTimestampData(4));
		}
		PushSendPacket(_sessionId, Protocol::ID_S2C_CharacterList, packet);
	}
	else {
		PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
	}
}
