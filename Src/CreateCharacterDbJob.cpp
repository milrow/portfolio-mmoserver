#include "CreateCharacterDbJob.h"

namespace
{
	string WideToUtf8(const wstring& value)
	{
		if (value.empty()) {
			return "";
		}

		const int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
		string result(sizeNeeded, '\0');
		WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), result.data(), sizeNeeded, nullptr, nullptr);
		return result;
	}
}

//namespace
//{
//	void PushResult(uint32_t sessionId, int64_t accountId, int64_t characterId, uint32_t result)
//	{
//		shared_ptr<Job> job = make_shared<Job>();
//		job->protocolId = Protocol::ID_D2S_CreateCharacter;
//		job->sessionId = sessionId;
//
//		Protocol::S2C_CreateCharacter packet;
//		//packet.(characterId);
//		packet.set_result(result);
//
//		const size_t size = packet.ByteSizeLong();
//		job->rawData.resize(size);
//		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
//		job->len = static_cast<int16_t>(size);
//
//		JobManager::GetInstance().PushJob(job);
//	}
//}

const wchar_t* CreateCharacterDbJob::GetQuery()
{
	return L"{CALL SP_CreateCharacter(?, ?, ?)}";
}

void CreateCharacterDbJob::BindParameters(SQLHSTMT hStmt)
{
	_nameLen = SQL_NTS;

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
		SQL_C_SSHORT,
		SQL_SMALLINT,
		0,
		0,
		&_slot,
		0,
		&_slotLen
	);

	::SQLBindParameter(
		hStmt,
		3,
		SQL_PARAM_INPUT,
		SQL_C_WCHAR,
		SQL_WVARCHAR,
		100,
		0,
		(SQLPOINTER)_nickName.c_str(),
		static_cast<SQLLEN>((_nickName.length() + 1) * sizeof(wchar_t)),
		&_nameLen
	);
}

void CreateCharacterDbJob::Excute(DbConnection* conn)
{
	BindParameters(conn->GetStmtHandle());

	Protocol::S2C_CreateCharacter packet;
	
	if (conn->Prepare(GetQuery()) && conn->ExcutePrepared() && conn->Fetch())
	{
		int resultCode = conn->GetIntData(1);
		_characterId = conn->GetInt64Data(2);
		SQL_TIMESTAMP_STRUCT createdAt = conn->GetTimestampData(3);

		Protocol::Character* character = packet.mutable_character();
		character->set_id(_characterId);
		character->set_slot(_slot);
		character->set_name(WideToUtf8(_nickName));
		conn->BindTimestampParameter(character->mutable_createdat(), createdAt);

		packet.set_result(resultCode);
	}
	else {
		PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
		packet.set_result(100);
	}
	cout << "CreateCharacterDbJob::Excute: accountId=" << _accountId << ", characterId=" << _characterId << ", result=" << packet.result() << endl;
	PushSendPacket(_sessionId, Protocol::ID_S2C_CreateCharacter, packet);
}
