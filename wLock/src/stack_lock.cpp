#include "wLock/include/stack_lock.h"

namespace w
{

StackLock::StackLock(GenericLock &gl) : g(gl)
{
	g.Lock();
}

StackLock::~StackLock()
{
	g.Unlock();
}

} // namespace w
