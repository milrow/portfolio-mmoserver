#pragma once

#include "JobQueue.h"

using namespace std;

class JobManager {
public:
	static JobManager& GetInstance()
	{
		static JobManager instance;
		return instance;
	}

	void PushJob(shared_ptr<Job> job) { return _queue.Push(job); }
	shared_ptr<Job> PopJob() { return _queue.Pop(); }
private:
	JobManager() = default;
	~JobManager() = default;
	JobManager(const JobManager&) = delete;

	JobQueue _queue;
};