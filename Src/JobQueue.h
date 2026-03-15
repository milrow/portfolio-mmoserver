#pragma once

#include <queue>
#include <vector>
#include <mutex>

using namespace std;

struct Job {
	uint32_t sessionId;
	uint16_t protocolId;
	vector<char> rawData;
	int16_t len;
};
	
class JobQueue
{
	public:
		void Push(shared_ptr<Job> job);
		shared_ptr<Job> Pop();
	private:
		queue<shared_ptr<Job>> _jobs;
		mutex _mutex;
		condition_variable _condVar;
};
