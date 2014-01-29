#pragma once

#include <semaphore.h>

#include "wTime/include/interval.h"
#include "wTime/include/time.h"

namespace w
{

class Semaphore
{
	Semaphore();
	Semaphore(unsigned int initial);
	Semaphore(bool shared, unsigned int initial);
	~Semaphore();

	int GetValue();
	
	void Lock();
	bool TryLock();
	
	bool TimedLock(const w::Interval &duration);
	bool TimedLockUntil(const w::Time &wakeup);
	
	void Release();
	
	sem_t semaphore;
};

} // namespace w
