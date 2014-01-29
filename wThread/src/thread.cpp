#include "wThread/include/thread.h"

namespace w
{

Thread::Thread() : is_running(false)
{
}

Thread::~Thread()
{
	Join();
}

void Thread::Run()
{
	// does nothing.  Since Thread is a Runnable, you can subclass it and override this function
}

void Thread::Start(Runnable &task)
{
	is_running = true;
	pthread_create(&thread, NULL, (void *(*)(void *)) Runnable::Start, &task);
}

void Thread::Start()
{
	is_running = true;
	pthread_create(&thread, NULL, (void *(*)(void *)) Runnable::Start, this);
}

void Thread::Join()
{
	if (IsRunning())
	pthread_join(thread, NULL);
	is_running = false;
}

bool Thread::IsRunning()
{
	return is_running;
}

} // namespace w
