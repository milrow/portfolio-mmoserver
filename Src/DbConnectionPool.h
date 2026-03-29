#pragma once

#include <mutex>
#include <queue>
#include <vector>

class DbConnection;

using namespace std;

class DbConnectionPool {
public:
	void Push(shared_ptr<DbConnection> dbConn);
	shared_ptr<DbConnection> Pop();
	shared_ptr<DbConnection> Borrow();
	void Clear();

private:
	queue<shared_ptr<DbConnection>> _pool;
	mutex _mutex;
	condition_variable _condVar;
};