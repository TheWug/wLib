#include "wLock/include/semaphore.h"
#include "wTime/include/chrono.h"

#include <stdexcept>
#include <errno.h>

using namespace std;

namespace w
{

Semaphore::Semaphore()
{
	sem_init(&semaphore, 0, 1);
}

Semaphore::Semaphore(unsigned int initial)
{
	sem_init(&semaphore, 0, initial);
}

Semaphore::Semaphore(bool shared, unsigned int initial)
{
	sem_init(&semaphore, shared, initial);
}

Semaphore::~Semaphore()
{
	sem_destroy(&semaphore);
}

int Semaphore::GetValue()
{
	int value;
	int flags = sem_getvalue(&semaphore, &value);
	if (flags == 0) return (value > 0 ? value : 0);
	throw runtime_error("Semaphore::GetValue: Semaphore not initialized!"); // should be impossible
}

void Semaphore::Lock()
{
	while (true)
	{
		int flags = sem_wait(&semaphore);
		switch (flags)
		{
		case 0:
			return;
		case EDEADLK:
			throw runtime_error("Semaphore::Lock: Deadlock condition detected!");
		case EINTR:
			continue;
		case EINVAL:
			throw runtime_error("Semaphore::Lock: Semaphore not initialized!");
		}
	}
}

bool Semaphore::TryLock()
{
	while (true)
	{
		int flags = sem_wait(&semaphore);
		switch (flags)
		{
		case 0:
			return true;
		case EAGAIN:
			return false;
		case EDEADLK:
			throw runtime_error("Semaphore::Lock: Deadlock condition detected!");
		case EINTR:
			continue;
		case EINVAL:
			throw runtime_error("Semaphore::Lock: Semaphore not initialized!");
		}
	}
}

bool Semaphore::TimedLock(const w::Interval &duration)
{
	Time endtime = Chrono::Now() + duration;
	
	// if you hit this, it's either because you gave it a bad interval, or there's a problem with Time::operator+(Interval).
	if (endtime.tv_nsec < 0 || endtime.tv_sec > 999999999) throw runtime_error("Semaphore::TimedLock: Invalid timespec!");
	while (true)
	{
		int flags = sem_timedwait(&semaphore, &endtime);
		switch (flags)
		{
		case 0:
			return true;
		case ETIMEDOUT:
			return false;
		case EDEADLK:
			throw runtime_error("Semaphore::TimedLock: Deadlock condition detected!");
		case EINTR:
			continue;
		case EINVAL:
			throw runtime_error("Semaphore::TimedLock: Semaphore not initialized!");
		}
	}
}

bool Semaphore::TimedLockUntil(const w::Time &wakeup)
{
	// if you hit this, it's either because you gave it a bad time, or there's a problem with Time::operator+(Interval).
	if (wakeup.tv_nsec < 0 || wakeup.tv_sec > 999999999) throw runtime_error("Semaphore::TimedLockUntil: Invalid timespec!");
	while (true)
	{
		int flags = sem_timedwait(&semaphore, &wakeup);
		switch (flags)
		{
		case 0:
			return true;
		case ETIMEDOUT:
			return false;
		case EDEADLK:
			throw runtime_error("Semaphore::TimedLockUntil: Deadlock condition detected!");
		case EINTR:
			continue;
		case EINVAL:
			throw runtime_error("Semaphore::TimedLockUntil: Semaphore not initialized!");
		}
	}
}

void Semaphore::Release()
{
	int flags = sem_post(&semaphore);
	if (flags != 0) throw runtime_error("Semaphore::Release: Semaphore not initialized!");
}

}
