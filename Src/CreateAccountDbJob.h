#pragma once

#include <cstdint>
#include "DbJob.h"


using namespace std;

class CreateAccountDbJob : public DbJob
{
private:
	SQLLEN _loginIdLen = SQL_NTS;
	SQLLEN _passwordLen = SQL_NTS;
public:
	uint32_t _sessionId;
	string _loginId;
	string _password;

public:
	virtual ~CreateAccountDbJob() override {}
	virtual void Excute(DbConnection* conn) override;

	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;
};
