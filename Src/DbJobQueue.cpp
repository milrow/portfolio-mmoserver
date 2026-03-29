#include "DbJobQueue.h"

void DbJobQueue::Push(shared_ptr<DbJob> job)
{
	lock_guard<mutex> lock(_mutex);
	_jobs.push(job);
	_condVar.notify_one();
	return;
}

shared_ptr<DbJob> DbJobQueue::Pop()
{
	unique_lock<mutex> lock(_mutex);
	_condVar.wait(lock, [this] {return !_jobs.empty(); });

	shared_ptr<DbJob> job = _jobs.front();
	_jobs.pop();
	return job;
}

