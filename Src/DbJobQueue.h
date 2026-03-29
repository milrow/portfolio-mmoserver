#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include "DbJob.h"

using namespace std;

class DbJobQueue
{
public:
	void Push(shared_ptr<DbJob> job);
	shared_ptr<DbJob> Pop();

private:
	queue<shared_ptr<DbJob>> _jobs;
	mutex _mutex;
	condition_variable _condVar;
};

