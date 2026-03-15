#include "JobQueue.h"

void JobQueue::Push(shared_ptr<Job> job)
{
    lock_guard<mutex> lock(_mutex);
    //printf("Protocol ID: %d\n", job->protocolId);
    //printf("_sessionId ID: %d", job->sessionId);
    _jobs.push(job);
    //printf("_jobs size : %d", _jobs.size());
    _condVar.notify_one();
}

shared_ptr<Job> JobQueue::Pop()
{
    unique_lock<mutex> lock(_mutex);
    _condVar.wait(lock, [this] {return !_jobs.empty(); });

    auto job = _jobs.front();
    _jobs.pop();
    return job;
}
