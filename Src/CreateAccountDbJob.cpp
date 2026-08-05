#include "CreateAccountDbJob.h"

//namespace
//{
//	void PushCreateAccountResult(uint32_t sessionId, uint32_t result, uint32_t accountId)
//	{
//		shared_ptr<Job> job = make_shared<Job>();
//		job->kind = JobKind::SendPacket;
//		job->protocolId = Protocol::ID_D2S_CreateAccount;
//		job->sessionId = sessionId;
//
//		Protocol::S2C_CreateAccount packet;
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

void CreateAccountDbJob::Excute(DbConnection* conn)
{
	BindParameters(conn->GetStmtHandle());

	Protocol::S2C_CreateAccount packet;

	if (conn->Prepare(GetQuery()) && conn->ExcutePrepared() && conn->Fetch())
	{
		packet.set_result(conn->GetIntData(1));
	}
	else {
		PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
		packet.set_result(100);
	}

	PushSendPacket(_sessionId, Protocol::ID_S2C_CreateAccount, packet);
}

const wchar_t* CreateAccountDbJob::GetQuery()
{
	return L"{CALL SP_CreateAccount(?, ?)}";
}

void CreateAccountDbJob::BindParameters(SQLHSTMT hStmt)
{
	::SQLBindParameter(
		hStmt,
		1,
		SQL_PARAM_INPUT,
		SQL_C_CHAR,
		SQL_VARCHAR,
		10,
		0,
		(SQLPOINTER)_loginId.c_str(),
		_loginId.length(),
		&_loginIdLen
	);

	::SQLBindParameter(
		hStmt,
		2,
		SQL_PARAM_INPUT,
		SQL_C_CHAR,
		SQL_VARCHAR,
		255,
		0,
		(SQLPOINTER)_password.c_str(),
		_password.length(),
		&_passwordLen
	);
}
