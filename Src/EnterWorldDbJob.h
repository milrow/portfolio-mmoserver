#pragma once

#include "DbJob.h"

class EnterWorldDbJob : public DbJob
{
public:
	uint64_t _sessionId;
	uint64_t _accountId;
	uint64_t _characterId;

private:
	SQLLEN _accountIdLen = SQL_NTS;
	SQLLEN _characterIdLen = SQL_NTS;

public:
	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;

	virtual ~EnterWorldDbJob() override {}
	virtual void Excute(DbConnection* conn) override;
};

