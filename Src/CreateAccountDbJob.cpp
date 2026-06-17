#include "CreateAccountDbJob.h"

void CreateAccountDbJob::Excute(DbConnection* conn)
{
	const wchar_t* query = L"INSERT INTO dbo.Account (LoginID, Password) VALUES (?, ?); SELECT SCOPE_IDENTITY();";

	::SQLBindParameter(
		conn->GetStmtHandle(),
		1,
		SQL_PARAM_INPUT,
		SQL_C_WCHAR,
		SQL_WVARCHAR,
		10,
		0,
		_loginId,
		sizeof(_loginId),
		&_loginIdLen
	);

	::SQLBindParameter(
		conn->GetStmtHandle(),
		2,
		SQL_PARAM_INPUT,
		SQL_C_WCHAR,
		SQL_WVARCHAR,
		20,
		0,
		_password,
		sizeof(_password),
		&_passwordLen
	);

	if (conn->Excute(query)) {
		SQLLEN len = 0;
		::SQLBindCol(
			conn->GetStmtHandle(),
			1,
			SQL_C_UBIGINT,
			&_accountId,
			0,
			&len
		);

		bool isSuccess = conn->Fetch();
		shared_ptr<Job> job = make_shared<Job>();
		job->protocolId = Protocol::ID_D2S_CreateAccount;
		job->sessionId = _sessionId;

		Protocol::S2C_CreateAccount packet;
		packet.set_accountid(_accountId);
		packet.set_sessionid(_sessionId);
		packet.set_issuccess(isSuccess);

		const size_t size = packet.ByteSizeLong();
		job->rawData.resize(size);
		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
		job->len = static_cast<int16_t>(size);

		JobManager::GetInstance().PushJob(job);

		return;
	}


}
