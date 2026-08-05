#pragma once
#include "DbJob.h"
#include "Protocol.pb.h"
#include <stdint.h>
#include <iostream>

using namespace std;

class SecurityManager;

class LoginDbJob : public DbJob {

private:
	uint64_t _accountId;
	string _storedPassword;

	SQLLEN _loginIdLen = SQL_NTS;
	SQLLEN _passwordLen = SQL_NTS;

public:
	uint32_t _sessionId;
	string _loginId;
	string _password;

public:
	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;

	virtual ~LoginDbJob() override { }
	virtual void Excute(DbConnection* conn) override;
};