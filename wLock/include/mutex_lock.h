#pragma once

#include "wLock/include/generic_lock.h"

#include <pthread.h>

namespace w
{

class LockArena;

class MutexLock : public GenericLock
{
private:
	pthread_mutex_t		m;
	pthread_cond_t		c;
	
	LockArena		*a;
	
	bool locked;
	
public:
	virtual void Lock();
	virtual void Unlock();
	
	MutexLock();
	MutexLock(LockArena &la);
	
	~MutexLock();
};

} // namespace w
