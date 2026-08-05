#include "InternalJobManager.h"

void InternalJobManager::Init()
{
	for (int i = 0; i < 4; ++i)
	{
		thread t([this]() {
			while (true)
			{
				shared_ptr<IntenalJob> job = _queue.Pop();
				if (!job) break;
				//job->Execute();
			}
		});
		t.detach();
	}
}