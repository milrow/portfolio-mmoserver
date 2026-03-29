#pragma once

#include "DbConnection.h"
#include "JobManager.h"

class DbJob
{
public:
	uint32_t _sessionId;
public:
	virtual ~DbJob() {}
	virtual void Excute(DbConnection* conn) = 0;
};



