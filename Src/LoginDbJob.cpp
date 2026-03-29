#include "LoginDbJob.h"

void PrintOrderError(SQLHANDLE handle, SQLSMALLINT handleType) {
	SQLWCHAR sqlState[6];       // SQL 상태 코드 (5자 + null)
	SQLWCHAR message[256];      // 에러 메시지 본문
	SQLINTEGER nativeError;     // DB 엔진 고유 에러 번호
	SQLSMALLINT textLength;     // 실제 메시지 길이
	SQLSMALLINT recNumber = 1;  // 진단 기록 번호 (여러 개일 수 있음)

	// SQLGetDiagRecW를 루프 돌며 모든 에러 기록을 가져옵니다.
	while (SQLGetDiagRecW(handleType, handle, recNumber++,
		sqlState, &nativeError, message,
		sizeof(message) / sizeof(SQLWCHAR), &textLength) == SQL_SUCCESS)
	{
		wprintf(L"   [ODBC Error] State: %s, Native: %d, Msg: %s\n",
			sqlState, nativeError, message);
	}
}

void LoginDbJob::Excute(DbConnection* conn)
{
	const wchar_t* query = L"SELECT AccountID FROM dbo.Account WHERE LoginID = ? AND Password = ?";

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
		&_passwordLen);

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
		job->protocolId = Protocol::ID_D2S_Login;
		job->sessionId = _sessionId;

		Protocol::S2C_Login packet;
		packet.set_acountid(_accountId);
		packet.set_sessionid(_sessionId);
		packet.set_issuccess(isSuccess);
		
		const size_t size = packet.ByteSizeLong();
		job->rawData.resize(size);
		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
		job->len = static_cast<uint16_t>(size);

		JobManager::GetInstance().PushJob(job);

		if (isSuccess) {
			cout << "Loginin Success" << _accountId << endl;
		}

		return;
	}
	else {
		cout << "Excute Failed"  << endl;
		PrintOrderError(conn->GetStmtHandle(), SQL_HANDLE_STMT);
		printf("LoginId: %ls, password: %ls\n", _loginId, _password);
	}
}
