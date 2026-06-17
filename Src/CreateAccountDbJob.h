#pragma once

#include <cstdint>
#include "DbJob.h"
#include "Protocol.pb.h"

using namespace std;

class CreateAccountDbJob : public DbJob
{
private:
	uint64_t _accountId;
	SQLLEN _loginIdLen = SQL_NTS;
	SQLLEN _passwordLen = SQL_NTS;
public:
	wchar_t _loginId[11];
	wchar_t _password[21];
public:
	virtual ~CreateAccountDbJob() override {}
	virtual void Excute(DbConnection* conn) override;
};
