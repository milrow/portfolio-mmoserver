#pragma once

#include <iostream>
#include <thread>

#include "DBJobQueue.h"
#include "DbConnectionPool.h"
#include "DbConnection.h"

using namespace std;

class DbManager
{
	DbManager() { }
	~DbManager() {
		for (uint32_t i = 0; i < _threads.size(); i++) {
			_queue.Push(nullptr);
		}

		for (thread& t : _threads) {
			if (t.joinable()) t.join();
		}

		_connectionPool.Clear();
		if (hEnv != SQL_NULL_HENV) {
			::SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
			hEnv = SQL_NULL_HENV;
		}
	}

public:
	static DbManager& GetInstance() {
		static DbManager _instance;
		return _instance;
	}
	void Init(int32_t connectionCnt);
	void PushJob(shared_ptr<DbJob> job) { return _queue.Push(job); }
private:
	void DbWorkerThread();

private:
	SQLHENV hEnv = SQL_NULL_HENV;

	DbJobQueue _queue;
	DbConnectionPool _connectionPool;
	vector<thread> _threads;

};

