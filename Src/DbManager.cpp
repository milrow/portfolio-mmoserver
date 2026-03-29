#include "DbManager.h"

void DbManager::DbWorkerThread()
{
	while (true) {
		shared_ptr<DbJob> job = _queue.Pop();

		if (job == nullptr) {
			break;
		}

		shared_ptr<DbConnection> conn = _connectionPool.Pop();

		if (conn == nullptr) {
			continue;
		}

		conn->BeginTransaction();

		job->Excute(conn.get());

		//todo: true 대신 excute 결과에 따라 넣기
		conn->EndTransaction(true);

		_connectionPool.Push(conn);
	}
}

void DbManager::Init(int32_t connectionCnt)
{
	if (SQL_SUCCESS != ::SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv)) {
		// 에러 처리
	}

	// 2. ODBC 버전 설정 (보통 3버전 이상 사용)
	if (SQL_SUCCESS != ::SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) {
		// 에러 처리
	}

	for (int i = 0; i < connectionCnt; i++)
	{
		auto conn = make_shared<DbConnection>();
		if (conn->Connect(hEnv, L"Driver={ODBC Driver 17 for SQL Server};Server=localhost;Database=MMORPG;UID=sa3;PWD=8498417264;")) {
			_connectionPool.Push(conn);
		}
		else {
			cout << "DB Connection Failed!" << endl;
		}
	}

	for (int i = 0; i < connectionCnt; i++) {
		_threads.push_back(thread(&DbManager::DbWorkerThread, this));
	}
}
