#pragma once

#include <queue>
#include <vector>
#include <mutex>

using namespace std;

enum class JobKind : uint8_t {
	CientPacket,
	SendPacket,
	DbResult,
};

struct Job {
	JobKind kind;
	uint32_t sessionId;
	uint16_t protocolId;
	vector<char> rawData;
	int16_t len;
};

template<typename TJob>
class JobQueue
{
private:
	queue<shared_ptr<TJob>> _jobs;
	mutex _mutex;
	condition_variable _condVar;

public:
	void Push(shared_ptr<TJob> job)
	{
		lock_guard<mutex> lock(_mutex);
		_jobs.push(job);
		_condVar.notify_one();
		return;
	}
	shared_ptr<TJob> Pop()
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return !_jobs.empty(); });

		shared_ptr<TJob> job = _jobs.front();
		_jobs.pop();
		return job;
	}

public:
	JobQueue() {}
};
