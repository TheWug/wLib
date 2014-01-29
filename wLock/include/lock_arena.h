#pragma once

#include <pthread.h>

namespace w
{

class LockArena
{
public:	
	static LockArena def;
	pthread_mutex_t mlock;
	
	LockArena();
	~LockArena();
};

} // namespace w
