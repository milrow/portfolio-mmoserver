#pragma once

#include "DbJob.h"

using namespace std;

class CreateCharacterDbJob : public DbJob
{
private:
	uint64_t _characterId = 0;
	SQLLEN _accountIdLen = 0;
	SQLLEN _nameLen = SQL_NTS;
	SQLLEN _slotLen = 0;

public:
	uint64_t _accountId;
	uint32_t _sessionId;
	short _slot;
	wstring _nickName;

public:
	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;

	virtual ~CreateCharacterDbJob() override {}
	virtual void Excute(DbConnection* conn) override;
};

