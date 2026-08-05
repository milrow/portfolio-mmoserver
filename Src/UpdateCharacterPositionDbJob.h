#pragma once

#include "DbJob.h"
#include "GameObject.h"

using namespace std;

class UpdateCharacterPositionDbJob : public DbJob
{
public:
	uint64_t _characterId;
	uint32_t _mapId;
	Position _position;

private:
	SQLLEN _characterIdLen;
	SQLLEN _mapIdLen;
	SQLLEN _posXLen;
	SQLLEN _posYLen;
	SQLLEN _posZLen;
	SQLLEN _yawLen;

public:
	virtual const wchar_t* GetQuery() override;
	virtual void BindParameters(SQLHSTMT hStmt) override;
	virtual ~UpdateCharacterPositionDbJob() override {};
	virtual void Excute(DbConnection* conn) override;

};

