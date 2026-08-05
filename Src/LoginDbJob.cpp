#include "LoginDbJob.h"
#include "SecurityManager.h"

namespace
{
	std::string WcharToString(const wchar_t* wstr) {
		if (wstr == nullptr) return "";

		int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		std::vector<char> buffer(size_needed);

		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &buffer[0], size_needed, NULL, NULL);
		return std::string(&buffer[0]);
	}

	/*void PushResult(uint32_t sessionId, int64_t accountId, uint32_t result)
	{
		shared_ptr<Job> job = make_shared<Job>();
		job->kind = JobKind::SendPacket;
		job->protocolId = Protocol::ID_S2C_Login;
		job->sessionId = sessionId;

		Protocol::S2C_Login packet;
		packet.set_result(result);

		const size_t size = packet.ByteSizeLong();
		job->rawData.resize(size);
		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
		job->len = static_cast<uint16_t>(size);

		JobManager::GetInstance().PushJob(job);
	}*/
}




const wchar_t* LoginDbJob::GetQuery()
{
	return L"{CALL SP_Login(?)}";
}

void LoginDbJob::BindParameters(SQLHSTMT hStmt)
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
}

void LoginDbJob::Excute(DbConnection* conn)
{
	Protocol::D2S_Login packet;

	try
	{
		BindParameters(conn->GetStmtHandle());

		if (conn->Prepare(GetQuery()) && conn->ExcutePrepared() && conn->Fetch())
		{
			int result = conn->GetIntData(1);
			_accountId = conn->GetInt64Data(2);
			_storedPassword = conn->GetStringData(3);

			if (result == 1)
			{
				//account not found
				throw 1;
			}

			if (!SecurityManager::GetInstance().VerifyPassword(_password, _storedPassword))
			{
				//failed to verify password
				throw 2;
			}

			packet.set_result(result);
			packet.set_accountid(result == 0 ? _accountId : 0);
			packet.set_sessionid(_sessionId);
		}
		else
		{
			PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
			throw 100;
		}
	}
	catch (const int result)
	{
		packet.set_result(result);
	}

	PushDbResultPacket(_sessionId, Protocol::ID_D2S_Login, packet);
}
