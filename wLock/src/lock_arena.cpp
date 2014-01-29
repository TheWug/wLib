#include "wLock/include/lock_arena.h"

namespace w
{

LockArena LockArena::def;

LockArena::LockArena()
{
	pthread_mutex_init(&mlock, NULL);
}

LockArena::~LockArena()
{
	pthread_mutex_destroy(&mlock);
}

} // namespace w
