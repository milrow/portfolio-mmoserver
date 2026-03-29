#pragma once
#include "DbJob.h"
#include "Protocol.pb.h"
#include <stdint.h>
#include <iostream>

using namespace std;

class LoginDbJob : public DbJob {
	uint64_t _accountId;

	SQLLEN _loginIdLen = SQL_NTS;
	SQLLEN _passwordLen = SQL_NTS;

public:
	wchar_t _loginId[11];
	wchar_t _password[21];

public:
	virtual ~LoginDbJob() override { }
	virtual void Excute(DbConnection* conn) override;
};