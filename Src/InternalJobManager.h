#pragma once

#include "JobQueue.h"
#include "InternalJob.h"
#include <thread>

using namespace std;

class InternalJobManager
{
public:
	static InternalJobManager& GetInstance()
	{
		static InternalJobManager _instance;
		return _instance;
	}
	
public:
	void Init();
	void PushJob(shared_ptr<IntenalJob> job) { return _queue.Push(job); }

private:
	JobQueue<IntenalJob> _queue;
};


