#pragma once

#include "DbJob.h"

using namespace std;

class CharacterListDbJob : public DbJob
{
private:
	SQLLEN _accountIdLen = 0;
public:
	uint32_t _sessionId;
	int64_t _accountId;

public:
	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;

	virtual ~CharacterListDbJob() {}
	virtual void Excute(DbConnection* conn) override;

};

