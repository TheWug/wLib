#pragma once

#include <pthread.h>

#include "wThread/include/runnable.h"

namespace w
{

class Thread : public Runnable
{
public:
	Thread();
	~Thread();

	virtual void Run();
	void Start(Runnable &task);
	void Start();
	
	void Join();
	bool IsRunning();
	
private:
	static void ThreadStart();

	pthread_t thread;
	bool is_running;
};

} // namespace w
