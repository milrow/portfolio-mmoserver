#include "DbConnectionPool.h"

void DbConnectionPool::Push(shared_ptr<DbConnection> dbConn)
{
	lock_guard<mutex> lock(_mutex);
	_pool.push(dbConn);
	_condVar.notify_one();
}

shared_ptr<DbConnection> DbConnectionPool::Pop()
{
	unique_lock<mutex> lock(_mutex);
	_condVar.wait(lock, [this] {return !_pool.empty(); });

	shared_ptr<DbConnection> dbConn = _pool.front();
	_pool.pop();
	return dbConn;
}

void DbConnectionPool::Clear()
{
	lock_guard<mutex> lock(_mutex);
	while (!_pool.empty())
	{
		_pool.pop();
	}
	
}

//shared_ptr<DbConnection> DbConnectionPool::Borrow()
//{
//	auto conn = _pool.pop(); 
//
//	// 이 shared_ptr이 소멸될 때 다시 풀(this)에 Push하도록 설정
//	return shared_ptr<DbConnection>(conn.get(), [this](DbConnection* ptr) {
//		this->Push(shared_ptr<DbConnection>(ptr));
//		});
//}
