#pragma once

#include "wLock/include/mutex_lock.h"

namespace w
{

class StackLock
{
private:
	GenericLock		&g;
	
public:
	StackLock(GenericLock &gl);
	~StackLock();
};

} // namespace w
