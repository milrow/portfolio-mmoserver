#pragma once

#include "DbConnection.h"
#include "JobManager.h"
#include "Protocol.pb.h"

class DbJob
{

public:
	virtual ~DbJob() {}
	virtual void Excute(DbConnection* conn) = 0;

protected:
	virtual const wchar_t* GetQuery() = 0;
	virtual void BindParameters(SQLHSTMT hStmt) = 0;

	template<typename Packet>
	void PushSendPacket(uint32_t sessionId, uint16_t protocolId, Packet& packet) {
		shared_ptr<Job> job = make_shared<Job>();
		job->kind = JobKind::SendPacket;
		job->protocolId = protocolId;
		job->sessionId = sessionId;

		const size_t size = packet.ByteSizeLong();
		job->rawData.resize(size);
		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
		job->len = static_cast<int16_t>(size);
		
		JobManager::GetInstance().PushJob(job);
		cout << "PushSendPacket: sessionId=" << sessionId << ", protocolId=" << protocolId << ", size=" << size << endl;
	}

	template<typename Packet>
	void PushDbResultPacket(uint32_t sessionId, uint16_t protocolId, Packet& packet) {
		shared_ptr<Job> job = make_shared<Job>();
		job->kind = JobKind::DbResult;
		job->protocolId = protocolId;
		job->sessionId = sessionId;

		const size_t size = packet.ByteSizeLong();
		job->rawData.resize(size);
		packet.SerializeToArray(job->rawData.data(), static_cast<int>(size));
		job->len = static_cast<int16_t>(size);

		JobManager::GetInstance().PushJob(job);
	}

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
};



