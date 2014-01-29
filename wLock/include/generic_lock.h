#pragma once

namespace w
{

class GenericLock
{
public:
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};

} // namespace w
