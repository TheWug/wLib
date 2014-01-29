#include "wLock/include/mutex_lock.h"
#include "wLock/include/lock_arena.h"

#include <stdexcept>

namespace w
{

void MutexLock::Lock()
{
	pthread_mutex_lock(&a->mlock);
	while (pthread_mutex_trylock(&m))
		pthread_cond_wait(&c, &a->mlock);
	locked = true;
	pthread_mutex_unlock(&a->mlock);
}

void MutexLock::Unlock()
{
	pthread_mutex_lock(&a->mlock);
	if (!locked) throw std::runtime_error("wLock::Unlock: mutex is already unlocked");
	pthread_mutex_unlock(&m);
	pthread_cond_signal(&c);
	locked = false;
	pthread_mutex_unlock(&a->mlock);
}
	
MutexLock::MutexLock()
{
	locked = false;
	a = &LockArena::def;
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c, NULL);
}
	
MutexLock::MutexLock(LockArena &la)
{
	locked = false;
	a = &la;
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c, NULL);
}

MutexLock::~MutexLock()
{
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&c);
}

} // namespace w
